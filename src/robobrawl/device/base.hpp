
#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

#include "robobrawl/msg/device_info.hpp"
#include "robobrawl/msg/heartbeat.hpp"
#include "robobrawl/msg/reply.hpp"
#include "robobrawl/msg/active_controller_state.hpp"
// #include "robobrawl/msg/active_diagnostic_state.hpp"

namespace robobrawl {

  class Robobrawl_BaseDevice {
    public:
      virtual ~Robobrawl_BaseDevice() = default;

      virtual void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);

      virtual void clear();

      virtual void init_device_info() = 0;  // called by init() to configure device specific DeviceInfo

      virtual void do_startup() = 0;    // invoked at startup (send 'initial' msgs)
      virtual void do_continuous() = 0; // invoked at 10Hz
      virtual void do_medium() = 0;     // invoked at  1Hz
      virtual void do_slow() = 0;       // invoked at  0.1Hz
      virtual void update_state() = 0;


      void publish_device_info();
      virtual void set_active_controller(robobrawl::Identity active_controller) {}
      virtual void publish_controller_state() {}

    protected:

      robobrawl::Identity device_id_;
      robobrawl::DeviceInfo device_info_;

      robobrawl::Identity controller_id_;

      // DeviceInfoWriter device_;
      HeartbeatWriter heartbeat_;
      ReplyWriter reply_;

      // ActiveDiagnosticStateWriter diagnostics_;
  };

};


