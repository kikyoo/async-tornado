#ifndef EVENT_BUFFER_HPP_
#define EVENT_BUFFER_HPP_

#include <list>
#include <boost/noncopyable.hpp>
#include <thrift/concurrency/Mutex.h>

namespace kikyoo {

template<typename T>
class EventBuffer: boost::noncopyable {
public:
  void push_back(const T& t) {
    ::apache::thrift::concurrency::Guard g(mutex_);
    m_list_.push_back(t);
  }

  void try_get(std::list<T>& list) {
    if (mutex_.trylock()) {
      list = std::move(m_list_);
      mutex_.unlock();
    }
  }

private:
  std::list<T> m_list_;

  ::apache::thrift::concurrency::Mutex mutex_;
};

}

#endif

