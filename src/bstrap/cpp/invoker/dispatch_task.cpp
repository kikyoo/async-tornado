#include <algorithm>
#include <functional>
#include <thrift/concurrency/Util.h>
#include "dispatch_task.hpp"
#include <iostream>

namespace kikyoo {

DispatchTask::DispatchTask(EventListType event_list,
  RequestQueueType rpc_queue,
  InvokerQueueType invoker_queues,
  CbQueueType cb_queue,
  int timeout)
  : event_list_(event_list)
  , rpc_queue_(rpc_queue)
  , invoker_queues_(invoker_queues)
  , cb_queue_(cb_queue)
  , timeout_(timeout) {
}

void DispatchTask::event_handle() {
  const auto& list = event_list_->try_get();
  for (const auto& node: list) {
    //std::cout << __FILE__ << ':' << __LINE__ << ' ' << node << std::endl;
    auto& server = servers_[node->server];
    if (Node::CONNECTED == node->state) {
      server.hosts.push_back(node);
    } else { //corrupted
      auto it = std::find(server.hosts.begin(), server.hosts.end(), node);
      if (server.hosts.cend() != it) {
        server.hosts.erase(it);
      }
    }
  }
}

void DispatchTask::work_handle() {
  RpcPtr rpc;
  size_t loop = 0;
  while(loop++ < DispatchTask::LOOP_MAX
    && rpc_queue_->pop_front(DispatchTask::POP_WAIT, rpc)) {

    //std::cout << __FILE__ << ':' << __LINE__ << ' ' << rpc << std::endl;

    //stop message recved
    if (MsgType::M_STOP & rpc->type) {
      //send stop message to all invoker threads
      for (auto& queue: *invoker_queues_) {
        //std::cout << __FILE__ << ':' << __LINE__ << ' ' << rpc << std::endl;
        while(!queue->push_back(rpc));
      }
      m_stop_ = true;
      break;
    }

    //timeout check
    if (Util::currentTime()-rpc->birth_time > timeout_) {
      auto cb = boost::make_shared<Callback>();
      cb->pobj = rpc->pobj;
      cb->type = RsltType::RPC_TIMEOUT ;
      //std::cout << __FILE__ << ':' << __LINE__ << ' ' << cb << std::endl;
      while(!cb_queue_->push_back(cb));
      continue;
    }

    //find server, hash request key to host 
    auto it = servers_.find(rpc->server);
    if (servers_.cend() == it
      || it->second.hosts.empty()) { //all corrupted?
      auto cb = boost::make_shared<Callback>();
      cb->pobj = rpc->pobj;
      cb->type = RsltType::RPC_TIMEOUT ;
      //std::cout << __FILE__ << ':' << __LINE__ << ' ' << cb << std::endl;
      while(!cb_queue_->push_back(cb));
      continue;
    }

    std::hash<std::string> hash;
    if (rpc->type & (MsgType::M_CALL|MsgType::H_CALL)) {
      rpc->hash_key = hash(rpc->key);  
    } else {
      rpc->hash_key = it->second.request_sum ++;
    }
    //std::cout << __FILE__ << ':' << __LINE__ << ' ' << rpc << std::endl;
    auto& node = it->second.hosts[rpc->hash_key % it->second.hosts.size()];
    //std::cout << __FILE__ << ':' << __LINE__ << ' ' << node << std::endl;

    auto& queue = (*invoker_queues_)[node->seq_id % invoker_queues_->size()];
    while(!queue->push_back(rpc));
  }
}

};

