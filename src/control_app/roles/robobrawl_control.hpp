#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/control.hpp"

namespace robobrawl {

  

  class Control : public Robobrawl_Control {
  public:
    void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);

    void init_device_info() override;  // called by init() to configure device specific DeviceInfo

    void update_state() override;

    // bool authorization_required(const robobrawl::EnergyStartStopRequest& ess) override;

    // void initiate_energy_start_stop(const robobrawl::EnergyStartStopRequest& ess) override;

    // may override the data luisteners. 

    void do_continuous() override;

    void do_medium() override; // for when we are running clock skey control... at 1hz, or I could do more theoreticalu, but then the point would be to have the mytime - theirTime < small, so latancyu becomes a ig condidreation, at leat ao 100hz... 
  protected:
    robobrawl::String32 serial_num_{};

    Timer   update_timer_;
    Timer   transition_timer_;
  };

};


