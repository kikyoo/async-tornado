#include <sstream>
#include <iostream>
#include "config.hpp"

namespace kikyoo{

template<typename K>
std::string to_string(const K& k) {
  std::ostringstream ostr;
  ostr << k;
  return ostr.str();
}

template <typename K, typename V>
std::string to_string(const std::tuple<K, V>& t);
template <typename K, typename V>
std::string to_string(const std::pair<K, V>& p);
template <typename K, typename V>
std::string to_string(const std::map<K, V>& m);
template <typename T>
std::string to_string(const T& beg, const T& end);
template <typename T>
std::string to_string(const std::vector<T>& t);

template <typename K, typename V>
std::string to_string(const std::tuple<K, V>& t) {
  std::ostringstream ostr;
  ostr << '(' << to_string(std::get<0>(t)) << ", " << to_string(std::get<1>(t)) << ')';
  return ostr.str();
}
template <typename K, typename V>
std::string to_string(const std::pair<K, V>& t) {
  std::ostringstream ostr;
  ostr << to_string(t.first) << ": " << to_string(t.second);
  return ostr.str();
}
template <typename K, typename V>
std::string to_string(const std::map<K, V>& m) {
  std::ostringstream ostr;
  ostr << "{" << to_string(m.begin(), m.end()) << "}";
  return ostr.str();
}
template <typename T>
std::string to_string(const T& beg, const T& end) {
  size_t idx = 0;
  std::ostringstream ostr;
  for (T it = beg; it != end; ++it, ++idx) {
    if (it != beg)
      ostr << ", ";
    ostr << to_string(*it);
  }
  return ostr.str();
}
template<typename T>
std::string to_string (const std::vector<T>& t) {
  std::ostringstream ostr;
  ostr << "[" << to_string(t.begin(), t.end()) << "]";
  return ostr.str();
}

Config::Config(PyObject* conf)
  : m_timeout_(10)
  , m_queue_size_(10000)
  , m_host_max_(0) {

  parse_config(conf);
}

void Config::parse_config(PyObject* conf) {
  PyObject *key, *value;
  Py_ssize_t pos = 0;
  while (PyDict_Next(conf, &pos, &key, &value)) {
    const char* key_ = PyString_AsString(key);
    if (!strcmp(key_, "timeout")) {
      m_timeout_ = PyInt_AS_LONG(value);
    } else if (!strcmp(key_, "queue_size")) {
      m_queue_size_ = PyInt_AS_LONG(value);
    } else {
      for (size_t idx=0; idx<PyList_Size(value); idx++) {
        auto host = std::string(PyString_AsString(PyList_GetItem(value, idx)));
        auto pos = host.find(':');
        if (std::string::npos != pos) {
          std::string ip = host.substr(0, pos);
          int port = (int)strtoul(host.substr(pos+1).c_str(), NULL, 0);
          if (!ip.empty() && port > 0 && port < 0xFFFF) {
            auto& server = m_route_[std::string(key_)];
            server.push_back(std::make_tuple(ip, port));
            if (server.size() > m_host_max_) {
              m_host_max_ = server.size();
            }
          }
        }
      }
    }
  }

  //std::cout << to_string(m_route_) << std::endl;
}

}

