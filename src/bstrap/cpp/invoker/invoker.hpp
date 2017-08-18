#ifndef INVOKER_HPP_
#define INVOKER_HPP_

#include <set>
#include <string>
#include <Python.h>
#include <boost/shared_ptr.hpp>
#include <thrift/concurrency/Thread.h>
#include "task_header.hpp"
#include "event_buffer.hpp"
using namespace kikyoo;
using namespace apache::thrift::concurrency;

class Invoker {
public:
  Invoker(PyObject*);

  void hash_call(PyObject* pobj,
    const std::string& server,
    const std::string& key,
    const std::string& name,
    const std::string& msg);

  void module_call(PyObject* pobj,
    const std::string& server,
    const std::string& name,
    const std::string& msg);

  void hash_msg(const std::string& server,
    const std::string& key,
    const std::string& name,
    const std::string& msg);

  void module_msg(const std::string& server,
    const std::string& name,
    const std::string& msg);

  void stop();

private:
  void init_invoker(PyObject*);

private:
  EventListType event_list_;
  RequestQueueType rpc_queue_; 
  MulRequestQueueType rpc_queues_;
  CbQueueType cb_queue_;

private:
  int timeout_;
  int queue_size_;
  std::set<boost::shared_ptr<Thread>> task_threads_;
};

#endif

