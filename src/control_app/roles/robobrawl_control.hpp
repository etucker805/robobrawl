#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_control.hpp"

namespace robobrawl {

  

  class Control : public Robobrawl_Control {
  public:
    void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);

    void init_device_info() override;  // called by init() to configure device specific DeviceInfo

    void update_state() override;

    // bool authorization_required(const robobrawl::EnergyStartStopRequest& ess) override;

    // void initiate_energy_start_stop(const robobrawl::EnergyStartStopRequest& ess) override;

    void do_continuous() override;
  protected:
    robobrawl::String32 serial_num_{};

    Timer   update_timer_;
    Timer   transition_timer_;
  };

};


