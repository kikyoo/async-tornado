#ifndef CPYTHON_H
#define CPYTHON_

#include <string>
#include <Python.h>

#include <iostream>

namespace kikyoo {

class CPython {
public:
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
  struct RefManager {
    struct RefManager& add_ref(PyObject* pobj) {
      if (pobj) {
        m_pobj_ref_.push_back(pobj);
      }
      return *this;
    }
    void release_ref(void) {
      for (const auto pobj: m_pobj_ref_) {
        Py_DECREF(pobj);
      }
    }
    ~RefManager() {
      release_ref();
    }
    std::list<PyObject*> m_pobj_ref_;
  };
private:
  PyObject*  m_inst_;
  RefManager m_rmgr_;
};


#endif

}
