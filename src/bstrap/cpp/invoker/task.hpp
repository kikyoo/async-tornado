#ifndef TASK_HPP_
#define TASK_HPP_

#include <thrift/concurrency/Thread.h>
using namespace apache::thrift::concurrency;

namespace kikyoo {

class Task: public Runnable {
public:
  Task():m_stop_(false) {}

  virtual void run() {
    while(!m_stop_) {

      event_handle();

      work_handle();
    }
  }

  virtual void work_handle() = 0;

  virtual void event_handle() = 0;

protected:
  bool m_stop_;

};

}

#endif

