#ifndef CPYTHON_HPP_
#define CPYTHON_HPP_

#include <string>
#include <Python.h>
#include <boost/shared_ptr.hpp>

namespace kikyoo {

class CPython {
public:
  CPython();

  bool import(const std::string&,
    const std::string&,
    const std::string&);

  int hash_call(std::string& name_, 
    std::string& msg_,
    const std::string& key,
    const std::string& name,
    const std::string& msg);

  int module_call(std::string& name_, 
    std::string& msg_,
    const std::string& name,
    const std::string& msg);

  void hash_msg(const std::string& key,
    const std::string& name,
    const std::string& msg);

  void module_msg(const std::string& name,
    const std::string& msg);

private:
  PyObject*  m_inst_;

  struct RefManager;
  boost::shared_ptr<RefManager> m_rmgr_;
};

}

#endif

