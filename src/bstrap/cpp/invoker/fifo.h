#ifndef FIFO_H_
#define FIFO_H_

#include "mutex.h"
#include "sema.h"

template <class TYPE,
    class ReadingMutex=MutexType,
    class WritingMutex=MutexType>
class FifoQueue {
 public:
  inline FifoQueue(size_t capacity):
      capacity_(capacity),
      sema_writing_(capacity, capacity),
      sema_reading_(capacity, 0) {
    container_ = new TYPE[capacity + 1];
    id_writing_ = id_reading_ = 0;
  }

  inline ~FifoQueue() {
    delete[] container_;
    container_ = NULL;
  }

  bool push_back(const TYPE &one) {
    if (sema_writing_.trywait()) {
      do {
        ScopedMutex<WritingMutex> lock(mutex_writing_);
        uint32_t w = id_writing_;
        container_[w] = one;
        id_writing_ = (w == capacity_ ? 0 : w+1);
      } while(0);
      sema_reading_.post();
      return true;
    }
    return false;
  }

  bool pop_front(TYPE *one) {
    if (sema_reading_.trywait()) {
      do {
        ScopedMutex<ReadingMutex> lock(mutex_reading_);
        uint32_t r = id_reading_;
        *one = container_[r];
        id_reading_ = (r == capacity_ ? 0 : r+1);
      } while(0);
      sema_writing_.post();
      return true;
    }
    return false;
  }


  bool pop_front(double sec, TYPE &one) {
    if (sema_reading_.wait(sec)) {
      do {
        ScopedMutex<ReadingMutex> lock(mutex_reading_);
        uint32_t r = id_reading_;
        one = container_[r];
        id_reading_ = (r == capacity_ ? 0 : r+1);
      } while(0);
      sema_writing_.post();
      return true;
    }
    return false;
  }

  inline uint32_t get_used() const {
    int32_t delta = id_writing_ - id_reading_;
    return delta >= 0 ? delta : (capacity_ + 1 + delta);
  }

  inline size_t get_capacity() const {
    return capacity_;
  }

  bool push_front(const TYPE &one) {
    if (sema_writing_.trywait()) {
      do {
        ScopedMutex<ReadingMutex> lock(mutex_reading_);
        uint32_t r = id_reading_;
        id_reading_ = (r == 0 ? capacity_ : r-1);
        container_[id_reading_] = one;
      } while (0);
      sema_reading_.post();
      return true;
    }
    return false;
  }

 protected:
  TYPE *container_;
  size_t capacity_;

  uint32_t id_writing_; ///> cursor for writing
  char padding_1[64-sizeof(uint32_t)];
  uint32_t id_reading_; ///> cursor for reading
  char padding_2[64-sizeof(uint32_t)];

 private:
  WritingMutex mutex_writing_;

  ReadingMutex mutex_reading_;

  SemaType sema_writing_;
  SemaType sema_reading_;

  FifoQueue(const FifoQueue&);
  FifoQueue& operator=(const FifoQueue&);
};


#endif//FIFO_H_
