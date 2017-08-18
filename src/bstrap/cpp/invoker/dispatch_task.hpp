#ifndef DISPATCH_THREAD_HPP_
#define DISPATCH_THREAD_HPP_

#include <vector>
#include <unordered_map>
#include "task.hpp"
#include "task_header.hpp"

namespace kikyoo {

struct Server {
  std::vector<NodePtr> hosts;
  int64_t request_sum;
  Server(): request_sum(0) {}
};

class DispatchTask: public Task {
public:
  typedef std::unordered_map<std::string, Server> ServerMap;

  DispatchTask(EventListType event_list,
    RequestQueueType rpc_queue,
    MulRequestQueueType rpc_queues,
    CbQueueType cb_queue,
    int timeout);

  virtual void event_handle();

  virtual void work_handle();

private:
  int timeout_;
  ServerMap servers_;

private:
  EventListType event_list_;
  RequestQueueType rpc_queue_; 
  MulRequestQueueType rpc_queues_;
  CbQueueType cb_queue_;

private:
  const static int POP_WAIT = 10; //10ms
  const static int LOOP_MAX = 1000;//break and event check

};

}

#endif

