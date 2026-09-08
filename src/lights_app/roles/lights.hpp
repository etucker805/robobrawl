#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_lights.hpp"

namespace robobrawl {

  

  class Lights : public Robobrawl_Lights {
  public:
    void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);

    void init_device_info() override;  // called by init() to configure device specific DeviceInfo

    void update_state() override;

    void handle_clock_command(const robobrawl::ClockCommand& comm) override;

    void handle_claim_system(const robobrawl::ClaimSystem& claim) override;
      
    void handle_state_change(const robobrawl::StateChange& state) override;

    void do_continuous() override;
  protected:
    robobrawl::String32 serial_num_{};

    Timer   update_timer_;
    Timer   transition_timer_;
  };

};


