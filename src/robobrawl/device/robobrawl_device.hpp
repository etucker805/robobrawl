

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

#include "robobrawl/device/robobrawl_base.hpp"

#include "robobrawl/msg/device_info.hpp"
#include "robobrawl/msg/heartbeat.hpp"
#include "robobrawl/msg/reply.hpp"
#include "robobrawl/msg/active_controller_state.hpp"
#include "robobrawl/msg/claim_system.hpp"

namespace robobrawl {

  // adds in the ActiveControllerStateWriter (needed by all 'device' types except CONTROLLER)
  
  class Robobrawl_Device : public Robobrawl_BaseDevice, public ClaimSystem_Client {
    public:
      Robobrawl_Device() = default;
      virtual ~Robobrawl_Device() = default;
    
      virtual void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);
      virtual void clear();

      void handle_claim_system(const robobrawl::ClaimSystem& claim) override;
    
      void set_active_controller(robobrawl::Identity active_controller) override;
      void publish_controller_state() override;
      
      States                      current_state;
  
    protected:

      ActiveControllerStateWriter ac_state_writer_;
      ClaimSystemListener         cs_reader_;
      Identity                    system_controller_;

  };

};


