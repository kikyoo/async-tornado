#include <list>
#include "cpython.hpp"

namespace kikyoo {

struct CPython::RefManager {
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

CPython::CPython(): m_rmgr_(new CPython::RefManager) {
}

bool CPython::import(int port,
  const std::string& cpy_path,
  const std::string& srv_path,
  const std::string& pylib_path,
  const std::string& conf_file) {

  std::string cpath, spath, ppath;
  cpath.append("sys.path.append('")
    .append(cpy_path)
    .append("')");
  spath.append("sys.path.append('")
    .append(srv_path)
    .append("')");
  ppath.append("sys.path.append('")
    .append(pylib_path)
    .append("')");
  PyRun_SimpleString("import sys");  
  PyRun_SimpleString(cpath.c_str()); 
  PyRun_SimpleString(spath.c_str()); 
  PyRun_SimpleString(ppath.c_str()); 

  auto* pModule = PyImport_ImportModule("server");
  m_rmgr_->add_ref(pModule);
  if (!pModule) {
    return false;
  }

  auto* pDict = PyModule_GetDict(pModule);
  m_rmgr_->add_ref(pDict);
  if (!pDict) {
    return false;
  }

  auto* pClass = PyDict_GetItemString(pDict, "Server");
  m_rmgr_->add_ref(pClass);
  if (!pClass) {
    return false; 
  }

  auto* pargs = PyTuple_New(2);
  auto* item0 = Py_BuildValue("i", port);
  auto* item1 = Py_BuildValue("s", conf_file.c_str());
  m_rmgr_->add_ref(pargs);
  if (!pargs || !item0 || !item1) {
    return false;
  }
  PyTuple_SetItem(pargs, 0, item0);
  PyTuple_SetItem(pargs, 1, item1);

  m_inst_ = PyInstance_New(pClass, pargs, NULL);
  if (!m_inst_) {
    return false;
  }
  m_rmgr_->add_ref(m_inst_);

  return true;
}

int CPython::hash_call(std::string& name_,
  std::string& msg_,
  const std::string& key,
  const std::string& name,
  const std::string& msg) {

  auto* pret = PyObject_CallMethod(m_inst_, 
    "hash_call", 
    "sss", 
    key.c_str(),
    name.c_str(),
    msg.c_str());

  int status = PyInt_AsLong(PyTuple_GetItem(pret, 0));
  msg_.assign(PyString_AsString(PyTuple_GetItem(pret, 2)));
  name_.assign(PyString_AsString(PyTuple_GetItem(pret, 1)));

  Py_DECREF(pret);
  
  return status;

}

int CPython::module_call(std::string& name_,
  std::string& msg_,
  const std::string& name,
  const std::string& msg) {

  auto* pret = PyObject_CallMethod(m_inst_, 
    "module_call", 
    "ss", 
    name.c_str(),
    msg.c_str());

  int status = PyInt_AsLong(PyTuple_GetItem(pret, 0));
  msg_.assign(PyString_AsString(PyTuple_GetItem(pret, 2)));
  name_.assign(PyString_AsString(PyTuple_GetItem(pret, 1)));

  Py_DECREF(pret);

  return status;
}

void CPython::hash_msg(const std::string& key,
  const std::string& name,
  const std::string& msg) {

  PyObject_CallMethod(m_inst_, 
    "hash_msg", 
    "sss", 
    key.c_str(),
    name.c_str(),
    msg.c_str());
}

void CPython::module_msg(const std::string& name,
  const std::string& msg) {

  PyObject_CallMethod(m_inst_, 
    "module_msg", 
    "ss", 
    name.c_str(),
    msg.c_str());
}

}

