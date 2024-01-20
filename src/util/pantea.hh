#pragma once

#include <iostream>
#include <sys/time.h>

namespace pantea_cc {

namespace {

time_t now_ms() {
  struct timeval time_now {};
  gettimeofday(&time_now, nullptr);
  return (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
}

time_t now_us() {
  struct timeval time_now {};
  gettimeofday(&time_now, nullptr);
  return (time_now.tv_sec * 1'000'000) + (time_now.tv_usec);
}

}

template <typename T>
void log_event(const char* what, const T& data, const char* unit = nullptr, const char * extra = nullptr) {
  const auto now = now_ms();
  static time_t job_start_time_ms = now;
  std::cerr << "(pantea.h:35): [pantea] " << what << " " << data << (unit ? " " : "")
                   << (unit ? unit : "") << " at time (ms) " << now
                   << (extra ? " " : "") << (extra ? extra : "")
                   << " [t=" << (now - job_start_time_ms) << "]" << std::endl;
}

class Duration {
 public:
  Duration(const char * name) : name_(name), start_(now_us()) {}
  void lap(const char * event) const
  {
    const auto now = now_us();
    log_event("Duration", now - start_, name_, event);
    start_ = now;
  }

 private:
  const char * name_;
  mutable time_t start_;
};

}  // namespace pantea_cc
