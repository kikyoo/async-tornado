#include "config.hpp"
#include "invoker.hpp"
#include "invoker_task.hpp"
#include "dispatch_task.hpp"
#include "callback_task.hpp"
#include <thrift/concurrency/Util.h>
#include <thrift/concurrency/PlatformThreadFactory.h>

#include <iostream>
using namespace kikyoo;
using namespace apache::thrift::concurrency;


static RpcPtr silly_man(PyObject* pobj,
  const std::string& server,
  const std::string& key,
  const std::string& name,
  const std::string& msg,
  const MsgType::type& type) {

  auto rpc = boost::make_shared<Rpc>();
  rpc->pobj = pobj;
  rpc->birth_time = Util::currentTime();
  rpc->type = type;
  rpc->server.assign(server);
  rpc->key.assign(key);
  rpc->name.assign(name);
  rpc->request.assign(msg);

  return rpc;
}

Invoker::Invoker(PyObject* conf)
  : timeout_(10)
  , queue_size_(10000)
  , event_list_(boost::make_shared<EventBuffer<NodePtr>>()) {
  PyEval_InitThreads();
  init_invoker(conf);
}

void Invoker::init_invoker(PyObject* conf) {
  Config config(conf);
  timeout_ = config.timeout();
  queue_size_ = config.queue_size();

  if (timeout_ <= 0) {
    timeout_ = 10;
  }
  if (config.queue_size() <=0 || config.host_max() <= 0) {
    return;
  }
  rpc_queue_ = boost::make_shared<FifoQueue<RpcPtr>>(queue_size_);
  std::cout << rpc_queue_->capacity() << std::endl;
  cb_queue_  = boost::make_shared<FifoQueue<CallbackPtr, NullMutex, Mutex>>(queue_size_);
  std::cout << cb_queue_->capacity() << std::endl;
  invoker_queues_ = boost::make_shared<std::vector<RequestQueueType>>(config.host_max());
  for (size_t i=0; i<config.host_max(); i++) {
    (*invoker_queues_)[i] = boost::make_shared<FifoQueue<RpcPtr>>(queue_size_);
  }

  //tmp invoker task vec
  std::vector<boost::shared_ptr<InvokerTask>> tasks;
  for (size_t i=0; i<config.host_max(); i++) {
    tasks.push_back(boost::shared_ptr<InvokerTask>(new InvokerTask(event_list_, 
          (*invoker_queues_)[i],
          cb_queue_,
          timeout_)));
  }
  //assign node
  size_t node_sum = 0;
  for (auto& server: config.route_map()) {
    for (auto& host: server.second) {
      auto node = boost::make_shared<Node>();
      node->seq_id = node_sum ++;
      node->server = server.first;
      node->host   = std::get<0>(host);
      node->port   = std::get<1>(host);
      tasks[node->seq_id%tasks.size()]->add_node(node);
    }
  }

  //add invoker threads
  boost::shared_ptr<PlatformThreadFactory> factory
    = boost::shared_ptr<PlatformThreadFactory>(new PlatformThreadFactory());
  for (auto& task: tasks) {
    client_threads_.insert(factory->newThread(task));
  }

  //add dispatch task thread
  client_threads_.insert(factory->newThread(
      boost::shared_ptr<DispatchTask>(new DispatchTask(
          event_list_,
          rpc_queue_,
          invoker_queues_,
          cb_queue_,
          timeout_))));

  //add callback task thread
  client_threads_.insert(factory->newThread(
      boost::shared_ptr<CallbackTask>(new CallbackTask(
          cb_queue_,
          tasks.size()))));

  //start all task threads
  for (auto& thread: client_threads_) {
    thread->start();
  }
}

void Invoker::hash_call(PyObject* pobj,
  const std::string& server,
  const std::string& key,
  const std::string& name,
  const std::string& msg) {
  auto rpc = silly_man(pobj,
    server,
    key,
    name,
    msg,
    MsgType::H_CALL);
  while(!rpc_queue_->push_back(rpc));
}

void Invoker::module_call(PyObject* pobj,
  const std::string& server,
  const std::string& name,
  const std::string& msg) {
  auto rpc = silly_man(pobj,
    server,
    "",
    name,
    msg,
    MsgType::M_CALL);
  while(!rpc_queue_->push_back(rpc));
}

void Invoker::hash_msg(const std::string& server,
  const std::string& key,
  const std::string& name,
  const std::string& msg) {
  auto rpc = silly_man(NULL,
    server,
    key,
    name,
    msg,
    MsgType::H_MSG);
  while(!rpc_queue_->push_back(rpc));
}

void Invoker::module_msg(const std::string& server,
  const std::string& name,
  const std::string& msg) {
  auto rpc = silly_man(NULL,
    server,
    "",
    name,
    msg,
    MsgType::M_MSG);
  while(!rpc_queue_->push_back(rpc));
}

void Invoker::stop() {
  auto rpc  = boost::make_shared<Rpc>();
  rpc->type = MsgType::M_STOP;
  while(!rpc_queue_->push_back(rpc));
}


