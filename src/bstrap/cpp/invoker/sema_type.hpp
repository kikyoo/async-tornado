#ifndef SEMA_TYPE_HPP_
#define SEMA_TYPE_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <semaphore.h>
#include <boost/noncopyable.hpp>
#include <thrift/concurrency/Util.h>

namespace apache {
namespace thrift {
namespace concurrency {
class Util_: public Util {
  static const int64_t NS_PER_S = 1000000000LL;
  static const int64_t US_PER_S = 1000000LL;
  static const int64_t MS_PER_S = 1000LL;

  static const int64_t NS_PER_MS = NS_PER_S / MS_PER_S;
  static const int64_t NS_PER_US = NS_PER_S / US_PER_S;
  static const int64_t US_PER_MS = US_PER_S / MS_PER_S;

public:
  static void toAbsTimespec(struct THRIFT_TIMESPEC& result, int64_t ms) {
    struct timeval now;
    int ret = THRIFT_GETTIMEOFDAY(&now, NULL);
    assert(ret == 0);
    THRIFT_UNUSED_VARIABLE(ret); 
    result.tv_sec = now.tv_sec;
    result.tv_nsec = now.tv_usec * NS_PER_US + ms * NS_PER_MS;
    result.tv_sec += result.tv_nsec / NS_PER_S;
    result.tv_nsec %= NS_PER_S;
  }
};
}
}
}

class SemaType: boost::noncopyable {
 public:
  SemaType(uint32_t capacity, int flag=0) {
    int ret = sem_init(&sema_, flag, capacity);
    ret? abort() : void();
  }

  ~SemaType() {
    sem_destroy(&sema_);
  }

  bool post() {
    return (sem_post(&sema_) == 0);
  }

  bool wait(int64_t ms) {
    if (ms < 0) {
      return (sem_wait(&sema_) == 0);
    }
    if (ms > 0) {
      struct timespec ts;
      ::apache::thrift::concurrency::Util_::toAbsTimespec(ts, ms);
      return (sem_timedwait(&sema_, &ts) == 0);
    }
    return (sem_trywait(&sema_) == 0);
  }

  bool trywait() {
    return sem_trywait(&sema_) == 0;
  }

 private:
  sem_t sema_;
};

#endif//SEMA_H_

