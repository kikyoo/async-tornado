#include <atomic>
#include <string>
#include <Python.h>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include "fifo.h"

namespace python = boost::python;

struct Message;
typedef boost::shared_ptr<Message> MessagePtr;

struct Callback;
typedef boost::shared_ptr<Callback> CallbackPtr;

class Invoker {
public:
  Invoker(int i);

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


  static void rpc_call();
  static void call_back();

  void stop() {
    m_stop = true;
  }

private:
  static std::atomic<bool> m_stop;
  static FifoQueue<MessagePtr> in_queue;
  static FifoQueue<CallbackPtr> out_queue;
};

