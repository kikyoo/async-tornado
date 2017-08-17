#include <boost/python.hpp>
#include "invoker.hpp"
using namespace boost::python;

BOOST_PYTHON_MODULE(kikyoo)
{
  class_<Invoker>("Invoker", init<PyObject*>())
    .def("module_call",
      &Invoker::module_call,
      boost::python::args("pobj", "server", "name", "msg"))

    .def("hash_call",   
      &Invoker::hash_call,
      boost::python::args("pobj", "server", "key", "name", "msg"))

    .def("module_msg",
      &Invoker::module_msg,
      boost::python::args("server", "name", "msg"))

    .def("hash_msg",
      &Invoker::hash_msg,
      boost::python::args("server", "key", "name", "msg"))

    .def("stop",
      &Invoker::stop)
    ;
}

