#ifndef TIMESPEC_H_
#define TIMESPEC_H_

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>


inline void TimespecConvert(double seconds, struct timespec *ts) {
  struct timeval tv;
  if (0 == gettimeofday(&tv, NULL)) {
    time_t inte = (time_t) seconds;
    double frac = (seconds - inte);
    ts->tv_sec = tv.tv_sec + inte;
    ts->tv_nsec = (suseconds_t) (tv.tv_usec*1000 + frac*1000000000);
    if (ts->tv_nsec >= 1000000000) {
      ts->tv_sec++;
      ts->tv_nsec -= 1000000000;
    }
  } else {
    time_t delta = (time_t)(seconds + 0.5);
    if (delta == 0) delta = 1;
    ts->tv_sec = time(NULL) + delta;
    ts->tv_nsec = 0;
  }
}

inline uint64_t GetNowMsec() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return (uint64_t)tp.tv_sec * 1000 + tp.tv_usec/1000;
}


#endif//TIMESPEC_H_

