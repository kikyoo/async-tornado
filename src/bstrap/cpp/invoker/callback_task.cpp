#include <Python.h>
#include <boost/python.hpp>
#include "callback_task.hpp"
#include <iostream>
namespace python = boost::python;

namespace kikyoo {

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

CallbackTask::CallbackTask(CbQueueType cb_queue,
  size_t stop_count)
  : cb_queue_(cb_queue) 
  , m_stop_count_(stop_count){
}

void CallbackTask::work_handle() {
  CallbackPtr cb;
  size_t loop = 0;
  while(loop++ < CallbackTask::LOOP_MAX
    && cb_queue_->pop_front(CallbackTask::POP_WAIT, cb)) {

    //std::cout << __FILE__ << ':' << __LINE__ << ' ' << cb << std::endl;
    //all stop message recved
    //all invoker threads exited
    if (RsltType::M_STOP & cb->type 
        && !(--m_stop_count_)) {
      m_stop_ = true;
      break;
    }

    ScopeLock lock;
    python::handle<> handle(python::borrowed(cb->pobj));
    python::call_method<void>(handle.get(), 
      "cb", 
      (int)cb->type,
      cb->name,
      cb->response);
  }
}

}

