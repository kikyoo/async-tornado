#ifndef FIFO_QUEUE_HPP_
#define FIFO_QUEUE_HPP_

#include "sema_type.hpp"
#include "null_mutex.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

using namespace apache::thrift::concurrency;

namespace kikyoo {

template <class TYPE,
    class ReadingMutex=NullMutex,
    class WritingMutex=NullMutex>
class FifoQueue: boost::noncopyable {
 public:
  inline FifoQueue(size_t capacity):
      capacity_(capacity),
      sema_writing_(capacity),
      sema_reading_(0) {
    container_ = boost::make_shared<TYPE[]>(capacity);
    id_writing_ = id_reading_ = 0;
  }

  bool push_back(const TYPE &one) {
    if (sema_writing_.trywait()) {
      do {
        SuperGuard<WritingMutex> g(mutex_writing_);
        container_[id_writing_++] = one;
        id_writing_ %= capacity_;
      } while(0);
      sema_reading_.post();
      return true;
    }
    return false;
  }

  bool pop_front(TYPE& one) {
    if (sema_reading_.trywait()) {
      do {
        SuperGuard<ReadingMutex> g(mutex_reading_);
        one = container_[id_reading_++];
        id_reading_ %= capacity_;
      } while(0);
      sema_writing_.post();
      return true;
    }
    return false;
  }


  bool pop_front(int64_t ms, TYPE &one) {
    if (sema_reading_.wait(ms)) {
      do {
        SuperGuard<ReadingMutex> g(mutex_reading_);
        one = container_[id_reading_++];
        id_reading_ %= capacity_;
      } while(0);
      sema_writing_.post();
      return true;
    }
    return false;
  }

  size_t capacity() {
    return capacity_;
  }

private:
  boost::shared_ptr<TYPE[]>container_;
  size_t capacity_;

  uint32_t id_writing_;
  uint32_t id_reading_;

private:
  const WritingMutex mutex_writing_;
  const ReadingMutex mutex_reading_;

  SemaType sema_writing_;
  SemaType sema_reading_;
};

}

#endif//FIFO_H_

