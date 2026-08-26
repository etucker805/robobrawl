

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

#include "robobrawl/device/robobrawl_base.hpp"

#include "robobrawl/msg/device_info.hpp"
#include "robobrawl/msg/heartbeat.hpp"
#include "robobrawl/msg/reply.hpp"
#include "robobrawl/msg/active_controller_state.hpp"
#include "robobrawl/msg/active_diagnostic_state.hpp"

namespace robobrawl {

  // adds in the ActiveControllerStateWriter (needed by all 'device' types except CONTROLLER)
  
  class TMS_Device : public TMS_BaseDevice {
    public:
      TMS_Device() = default;
      virtual ~TMS_Device() = default;
    
      virtual void init(TmsContext& context, const robobrawl::Identity& dev_id);
      virtual void clear();
    
      void set_active_controller(robobrawl::Identity active_controller) override;
      void publish_controller_state() override;
  
    protected:

      ActiveControllerStateWriter ac_state_;
  };

};


