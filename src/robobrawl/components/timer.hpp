
#pragma once

#include <chrono>

class Timer {
  public:
    using Timestamp = std::chrono::steady_clock::time_point;

    Timer()
        :start_(std::chrono::steady_clock::now()), running_(false) { }

    void start()
    {
      start_ = std::chrono::steady_clock::now();
      running_ = true;
    }

    void stop() { running_ = false; }

    [[nodiscard]] bool is_running() const { return running_; }

    [[nodiscard]] std::chrono::milliseconds elapsed() const
    {
      std::chrono::steady_clock::time_point _end;
      _end = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::milliseconds>(_end - start_);
    }

    [[nodiscard]] int64_t elapsed_msec() const
    {
      return elapsed().count();
    }

  private:
    std::chrono::steady_clock::time_point start_; //  = std::chrono::steady_clock::now();
    bool running_;

};
