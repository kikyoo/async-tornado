#ifndef MUTEX_H_
#define MUTEX_H_

#include <stdlib.h>
#include <pthread.h>


class NullMutex {
 public:
  inline bool Acquire() {
    return true;
  }

  inline bool TryLock() {
    return true;
  }

  inline bool Release() {
    return true;
  }
};


class MutexType {
 public:
  MutexType() {
    pthread_mutex_init(&mutex_, NULL);
  }

  ~MutexType() {
    pthread_mutex_destroy(&mutex_);
  }

  /// @brief Attempts to get the mutex.
  inline bool Acquire() {
    return (0 == pthread_mutex_lock(&mutex_));
  }

  /// @brief Try to lock the mutex.
  inline bool TryLock() {
    return (0 == pthread_mutex_trylock(&mutex_));
  }

  /// @brief Attempts to release the mutex.
  inline bool Release() {
    return (0 == pthread_mutex_unlock(&mutex_));
  }

 private:
  MutexType(const MutexType &);
  const MutexType& operator = (const MutexType &);

  pthread_mutex_t mutex_;
};


template<class Mutex>
class ScopedMutex {
 public:
  ScopedMutex(Mutex& lock): lock_(lock) {
    lock_.Acquire();
  }
  ~ScopedMutex() {
    lock_.Release();
  }

 private:
  Mutex& lock_;
};


#endif //MUTEX_H_
