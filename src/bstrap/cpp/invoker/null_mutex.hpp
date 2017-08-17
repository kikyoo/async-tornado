#ifndef NULL_MUTEX_H_
#define NULL_MUTEX_H_

#include <boost/noncopyable.hpp>
#include <thrift/concurrency/Mutex.h>

namespace apache {
namespace thrift {
namespace concurrency {

class NullMutex: boost::noncopyable {
 public:
  inline bool lock() const {
    return true;
  }

  inline bool try_lock() const {
    return true;
  }

  inline bool unlock() const {
    return true;
  }
};

template<class T=NullMutex>
class SuperGuard: boost::noncopyable {
public:
  SuperGuard(const T& value): mutex_(&value) {
    value.lock();
  }

  ~SuperGuard() {
    mutex_->unlock();
  }

private:
  const T* mutex_;

};

}
}
}

#endif //MUTEX_H_

