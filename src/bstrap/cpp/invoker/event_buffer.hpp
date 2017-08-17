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
    cur_.push_back(t);
  }

  const std::list<T>& try_get() {
    swp_.clear();
    if (mutex_.trylock()) {
      cur_.swap(swp_);
      mutex_.unlock();
    }
    return swp_;
  }

private:
  std::list<T> cur_;
  std::list<T> swp_;

  ::apache::thrift::concurrency::Mutex mutex_;
};

}

#endif

