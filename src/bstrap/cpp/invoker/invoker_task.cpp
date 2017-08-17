#include <boost/make_shared.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/concurrency/Util.h>
#include "RouteService.h"  
#include "invoker_task.hpp"

#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::concurrency;

namespace kikyoo {

InvokerTask::InvokerTask(EventListType event_list, 
  RequestQueueType rpc_queue,
  CbQueueType cb_queue,
  int timeout) 
  : event_list_(event_list)
  , rpc_queue_(rpc_queue)
  , cb_queue_(cb_queue)
  , timeout_(timeout) {
}

bool InvokerTask::connect(NodePtr node) {
  try {
    boost::shared_ptr<TTransport> socket(new TSocket(node->host, node->port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    node->client_ptr = boost::make_shared<RouteServiceClient>(protocol);
    transport->open();
  } catch (...) {
    //std::cout << __FILE__ << ':' << __LINE__ << ' ' << node << std::endl;
    return false;
  }
  return true;
}

void InvokerTask::event_handle() {
  auto it = node_list_.begin();
  while(node_list_.cend() != it) {
    if (Node::CORRUPTED == (*it)->state) {
      bool should_try = false;
      auto invl = (*it)->reconnect_times * InvokerTask::CONN_INVL;
      if (invl > InvokerTask::INVL_MAX) {
        should_try = Util::currentTime()-(*it)->last_reconnect_time > InvokerTask::INVL_MAX;
      } else {
        should_try = Util::currentTime()-(*it)->last_reconnect_time > invl;
      }
      if (should_try) {
        if (connect(*it)) {
          (*it)->reconnect_times = 0;
          (*it)->last_reconnect_time = 0;
          (*it)->state = Node::CONNECTED;
          clients_[(*it)->server] = *it;
          event_list_->push_back(*it);
          //std::cout << __FILE__ << ':' << __LINE__ << ' ' << *it << std::endl;
          it = node_list_.erase(it);
        } else {
          (*it)->reconnect_times += 1;
          (*it)->last_reconnect_time = Util::currentTime();
          //std::cout << __FILE__ << ':' << __LINE__ << ' ' << *it << std::endl;
          it ++;
        }
      } else {
        break;
      }
    }else if (Node::UNINITIALIZED == (*it)->state) {
      if (connect(*it)) {
        (*it)->reconnect_times = 0;
        (*it)->last_reconnect_time = 0;
        (*it)->state = Node::CONNECTED;
        clients_[(*it)->server] = *it;
        event_list_->push_back(*it);
        //std::cout << __FILE__ << ':' << __LINE__ << ' ' << *it << std::endl;
        it = node_list_.erase(it);
      } else {
        (*it)->state = Node::CORRUPTED;
        (*it)->reconnect_times += 1;
        (*it)->last_reconnect_time = Util::currentTime();
        node_list_.push_back(*it);
        //std::cout << __FILE__ << ':' << __LINE__ << ' ' << *it << std::endl;
        it = node_list_.erase(it);
      }
    }
  }
}

void InvokerTask::work_handle() {
  auto default_cb = [](PyObject* pobj, RsltType::type type = RsltType::RPC_TIMEOUT){
    auto cb = boost::make_shared<Callback>();
    cb->pobj = pobj;
    cb->type = RsltType::RPC_TIMEOUT;
    return cb;
  };

  RpcPtr rpc;
  size_t loop = 0;
  while(loop++ < InvokerTask::LOOP_MAX
    && rpc_queue_->pop_front(InvokerTask::POP_WAIT, rpc)) {

    //std::cout << __FILE__ << ':' << __LINE__ << ' ' << rpc << std::endl;
    //all stop message recved

    //stop message recved
    if (MsgType::M_STOP & rpc->type) {
      //std::cout << __FILE__ << ':' << __LINE__ << ' ' << rpc << std::endl;
      auto cb = default_cb(rpc->pobj, RsltType::M_STOP);
      while(!cb_queue_->push_back(cb));
      m_stop_ = true;
      break;
    }

    //timeout check
    if (Util::currentTime()-rpc->birth_time > timeout_) {
      //std::cout << __FILE__ << ':' << __LINE__ << ' ' << rpc << std::endl;
      if (rpc->type & (MsgType::M_CALL|MsgType::H_CALL)) {
        auto cb = default_cb(rpc->pobj);
        while(!cb_queue_->push_back(cb));
      }
      continue;
    }

    //not find, client corrupted
    auto client = clients_.find(rpc->server);
    if (clients_.cend() == client) {
      if (rpc->type & (MsgType::M_CALL|MsgType::H_CALL)) {
        auto cb = default_cb(rpc->pobj);
        //std::cout << __FILE__ << ':' << __LINE__ << ' ' << cb << std::endl;
        while(!cb_queue_->push_back(cb));
      }
      continue;
    }

    auto client_ptr = client->second->client_ptr;
    //call | msg
    try {
      Msg msg;
      msg.type = rpc->type;
      msg.name = rpc->name;
      msg.msg = rpc->request;
      if (rpc->type & (MsgType::M_CALL|MsgType::H_CALL)) {
        msg.key = rpc->key;
        msg.__isset.key = true;
        Rslt rslt;
        client_ptr->call(rslt, msg);
        auto cb = boost::make_shared<Callback>();
        cb->pobj = rpc->pobj;
        cb->type = rslt.status;
        cb->name = rslt.name;
        cb->response.assign(rslt.msg);
        //std::cout << __FILE__ << ':' << __LINE__ << ' ' << cb << std::endl;
        while(!cb_queue_->push_back(cb));
      } else {
        client_ptr->message(msg);
      }
    } catch (...) {
      if (rpc->type & (MsgType::M_CALL|MsgType::H_CALL)) {
        auto cb = default_cb(rpc->pobj);
        //std::cout << __FILE__ << ':' << __LINE__ << ' ' << cb << std::endl;
        while(!cb_queue_->push_back(cb));
      }
      clients_.erase(client->second->server);
      client->second->state = Node::CORRUPTED;
      event_list_->push_back(client->second);
      node_list_.push_back(client->second);
      break;
    }
  }
}

}

