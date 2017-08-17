#ifndef INVOKER_THREAD_HPP_
#define INVOKER_THREAD_HPP_

#include <string>
#include <unordered_map>
#include "task.hpp"
#include "task_header.hpp"

using namespace apache::thrift::concurrency;

namespace kikyoo {
class InvokerTask: public Task {
public:
  InvokerTask(EventListType even_list, 
    RequestQueueType rpc_queue,
    CbQueueType cb_queue,
    int timeout);

  void add_node(NodePtr node) {
    node_list_.push_back(node);
  }

  void work_handle();

  void event_handle();

private:
  bool connect(NodePtr node);

private:
  std::list<NodePtr> node_list_;
  std::unordered_map<std::string, NodePtr> clients_;

  EventListType event_list_;
  RequestQueueType rpc_queue_; 
  CbQueueType cb_queue_; 

  int timeout_;
  const static int POP_WAIT  = 10;   //10ms
  const static int CONN_INVL = 5000;  //5000ms
  const static int INVL_MAX  = CONN_INVL * 60; //5min
  const static int LOOP_MAX  = CONN_INVL / POP_WAIT;

};

}

#endif

