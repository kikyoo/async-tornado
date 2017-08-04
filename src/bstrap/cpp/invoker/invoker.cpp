#include <thread>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <boost/make_shared.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "RouteService.h"  

#include "invoker.hpp"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace kikyoo;

struct Message {
  PyObject* pobj;
  int64_t birth_time;
  kikyoo::MsgType::type type;
  std::string server;
  std::string key;
  std::string name;
  std::string msg;
};

struct Callback {
  PyObject* pobj;
  kikyoo::RsltType::type type;
  std::string name;
  std::string msg;
};

static MessagePtr SillyMan(PyObject* pobj,
  const std::string& server,
  const std::string& key,
  const std::string& name,
  const std::string& msg,
  const kikyoo::MsgType::type& type) {

  auto msg_ptr = boost::make_shared<Message>();
  msg_ptr->pobj = pobj;
  msg_ptr->birth_time = GetNowMsec();
  msg_ptr->type = type;
  msg_ptr->server.assign(server);
  msg_ptr->key.assign(key);
  msg_ptr->name.assign(name);
  msg_ptr->msg.assign(msg);

  return msg_ptr;
}

FifoQueue<MessagePtr> Invoker::in_queue(100000);
FifoQueue<CallbackPtr> Invoker::out_queue(100000);
std::atomic<bool> Invoker::m_stop(false);

Invoker::Invoker(int i){

  PyEval_InitThreads();

  std::thread tcb(call_back);
  tcb.detach();

  std::thread trc(rpc_call);
  trc.detach();

}

void Invoker::hash_call(PyObject* pobj,
  const std::string& server,
  const std::string& key,
  const std::string& name,
  const std::string& msg) {
  auto msg_ptr = SillyMan(pobj,
    server,
    key,
    name,
    msg,
    kikyoo::MsgType::H_CALL);
  while(!in_queue.push_back(msg_ptr));
}

void Invoker::module_call(PyObject* pobj,
  const std::string& server,
  const std::string& name,
  const std::string& msg) {
  auto msg_ptr = SillyMan(pobj,
    server,
    "",
    name,
    msg,
    kikyoo::MsgType::M_CALL);
  while(!in_queue.push_back(msg_ptr));
}

void Invoker::hash_msg(const std::string& server,
  const std::string& key,
  const std::string& name,
  const std::string& msg) {
  auto msg_ptr = SillyMan(NULL,
    server,
    key,
    name,
    msg,
    kikyoo::MsgType::H_MSG);
}

void Invoker::module_msg(const std::string& server,
  const std::string& name,
  const std::string& msg)
{
  auto msg_ptr = SillyMan(NULL,
    server,
    "",
    name,
    msg,
    kikyoo::MsgType::M_MSG);
  while(!in_queue.push_back(msg_ptr));
}

void Invoker::rpc_call() {
  const int port =  9090;
  boost::shared_ptr<TTransport> socket(new TSocket("localhost", port));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  RouteServiceClient client(protocol);

  try {
    transport->open();

    boost::shared_ptr<Message> msg_ptr;
    while (!m_stop) {
      while (in_queue.pop_front(0.1, msg_ptr)) {
        Msg msg;
        msg.type = msg_ptr->type;
        msg.name = msg_ptr->name;
        msg.msg = msg_ptr->msg;
        msg.key = msg_ptr->key;
        msg.__isset.key = true;
        if (msg_ptr->type & (kikyoo::MsgType::M_CALL|kikyoo::MsgType::H_CALL)) {
          Rslt rslt;
          client.call(rslt, msg);
          auto cb_ptr = boost::make_shared<Callback>();
          cb_ptr->pobj = msg_ptr->pobj;
          cb_ptr->type = rslt.status;
          cb_ptr->name.assign(rslt.name);
          cb_ptr->msg.assign(rslt.msg);
          while(!out_queue.push_back(cb_ptr));
        } else {
          client.message(msg);
        }
      }
    }

    transport->close();
  } catch (TException& tx) {
    std::cout << "ERROR: " << tx.what() << std::endl;
  }
}

void Invoker::call_back()
{

  struct ScopeLock{
    ScopeLock() {
      state = PyGILState_Ensure();
    }
    ~ScopeLock() {
      PyGILState_Release(state);
    }
    private: 
      PyGILState_STATE state;
  };

  namespace python = boost::python;

  boost::shared_ptr<Callback> cb_ptr;
  while (!m_stop) {
    while (out_queue.pop_front(0.1, cb_ptr)) {
      ScopeLock lock;
      python::handle<> handle(python::borrowed(cb_ptr->pobj));
      python::call_method<void>(handle.get(), 
        "cb", 
        (int)cb_ptr->type,
        cb_ptr->name,
        cb_ptr->msg);
    }
  }
}

