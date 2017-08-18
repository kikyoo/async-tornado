#include <Python.h>
#include <unistd.h>
#include "invoker.hpp"
#include <iostream>

int main() {
  Py_Initialize();

  auto* dict = PyDict_New();
  PyDict_SetItem(dict, Py_BuildValue("s","timeout"), Py_BuildValue("i", 10));
  PyDict_SetItem(dict, Py_BuildValue("s", "queue_size"), Py_BuildValue("i", 10000));

  auto* user = PyList_New(2);
  auto* s1 = Py_BuildValue("s", "localhost:9090");
  auto* s2 = Py_BuildValue("s", "localhost:9091");
  PyList_SetItem(user, 0, s1);
  PyList_SetItem(user, 1, s2);
  auto* xxx = PyList_New(1);
  auto* s3 = Py_BuildValue("s", "localhost:9092");
  PyList_SetItem(xxx, 0, s3);

  PyDict_SetItem(dict, Py_BuildValue("s", "user"), user);
  PyDict_SetItem(dict, Py_BuildValue("s", "xxx"),  xxx);

  Invoker invo(dict);

  int i = 10;
  while(i--) {
    sleep(1);
  }

  invo.stop();

  i = 10;
  while(i--) {
    sleep(1);
    std::cout << "stop!" << std::endl;
  }

  Py_DECREF(dict);
  Py_Finalize();

  return 0;
}
