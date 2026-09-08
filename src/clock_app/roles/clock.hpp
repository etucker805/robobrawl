#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_clock.hpp"

namespace robobrawl {

  

  class Clock : public Robobrawl_Clock {
  public:
    void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);

    void init_device_info() override;  // called by init() to configure device specific DeviceInfo

    void update_state() override;

    // bool authorization_required(const robobrawl::EnergyStartStopRequest& ess) override;

    void handle_new_device(const robobrawl::DeviceInfo& info) override; 

    void handle_clock_command(const robobrawl::ClockCommand& comm) override;
    void handle_claim_system(const robobrawl::ClaimSystem& claim) override;
    void handle_state_change(const robobrawl::StateChange& state) override;
    void handle_timer_expired(const CountdownTimer::Id& id) override;



    void do_continuous() override;
    void do_medium() override;

  protected:
    robobrawl::String32 serial_num_{};
    

    Timer   update_timer_;
    Timer   transition_timer_;
  };

};


