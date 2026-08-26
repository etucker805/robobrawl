

#pragma once

#include "robobrawl.hh"

namespace robobrawl {

  inline robobrawl::ClockMonotonic now_monotonic()
  {
    struct timespec now{};
    clock_gettime(CLOCK_MONOTONIC, &now);
    robobrawl::ClockMonotonic rval;
    rval.seconds(now.tv_sec);
    rval.nanoseconds(now.tv_nsec);
    return rval;
  }

  inline robobrawl::ClockRealtime now_realtime()
  {
    struct timespec now{};
    clock_gettime(CLOCK_REALTIME, &now);
    robobrawl::ClockRealtime rval;
    rval.epoch(0); // not sure where to get this, what it should be.
    rval.seconds(now.tv_sec);
    rval.nanoseconds(now.tv_nsec);
    return rval;
  }

};

