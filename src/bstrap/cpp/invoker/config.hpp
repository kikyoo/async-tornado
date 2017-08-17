#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <Python.h>

namespace kikyoo {
class Config {
public:
  typedef std::tuple<std::string, int> HostType;
  typedef std::vector<HostType> ServerType;
  typedef std::map<std::string, ServerType> RouteType;

  Config(PyObject*);

  int queue_size() {
    return m_queue_size_;
  }

  void queue_size(int size) {
    m_queue_size_ = size;
  }

  int timeout() {
    return m_timeout_;
  }

  void timeout(int to) {
    m_timeout_ = to;
  }

  int host_max() {
    return m_host_max_;
  }

  RouteType& route_map() {
    return m_route_;
  }

private:
  void parse_config(PyObject*);

private:
  int m_timeout_;
  int m_queue_size_;
  RouteType m_route_;

private:
  int m_host_max_;

};
}

#endif

