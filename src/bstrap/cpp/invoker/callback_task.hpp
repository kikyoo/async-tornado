#ifndef CALLBACK_THREAD_HPP_
#define CALLBACK_THREAD_HPP_

#include "task.hpp"
#include "task_header.hpp"

namespace kikyoo {

class CallbackTask: public Task {
public:
  CallbackTask(CbQueueType cb_queue,
    size_t stop_count);

  virtual void event_handle() {}

  virtual void work_handle();

private:
  CbQueueType cb_queue_;
  size_t m_stop_count_;

private:
  const static int POP_WAIT = 10; //10ms
  const static int LOOP_MAX = 1000;//break and event check

};

}

#endif

