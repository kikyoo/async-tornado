#ifndef SEMA_H_
#define SEMA_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdexcept>
#include <semaphore.h>

#include "timespec.h"

class SemaType {
 public:
  SemaType(uint32_t /*capacity*/, uint32_t count) {
    int ret = sem_init(&sema_, 0, count);
    if (ret != 0) throw std::runtime_error("failed: sem_init()");
  }

  ~SemaType() {
    sem_destroy(&sema_);
  }

  bool post() {
    return (sem_post(&sema_) == 0);
  }

  bool wait(double seconds) {
    if (seconds < 0) {
      return (sem_wait(&sema_) == 0);
    }
    if (seconds > 0) {
      struct timespec ts;
      TimespecConvert(seconds, &ts);
      return (sem_timedwait(&sema_, &ts) == 0);
    }
    return (sem_trywait(&sema_) == 0);
  }

  bool trywait() {
    return sem_trywait(&sema_) == 0;
  }

 private:
  sem_t sema_;

  SemaType(const SemaType&);
  SemaType& operator=(const SemaType&);
};


#endif//SEMA_H_
