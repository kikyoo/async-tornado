#include <boost/python.hpp>
#include "invoker.hpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(kikyoo)
{
  class_<Invoker>("Invoker", init<int>())
    .def("module_call",
      &Invoker::module_call,
      python::args("pobj", "server", "name", "msg"))

    .def("hash_call",   
      &Invoker::hash_call,
      python::args("pobj", "server", "key", "name", "msg"))

    .def("module_msg",
      &Invoker::module_msg,
      python::args("server", "name", "msg"))

    .def("hash_msg",
      &Invoker::hash_msg,
      python::args("server", "key", "name", "msg"))
    ;
}

