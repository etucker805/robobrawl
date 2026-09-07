#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_control.hpp"

namespace robobrawl {

  extern KnownDevices known_devices;

  class Control : public Robobrawl_Control {
  public:
    void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);

    void init_device_info() override;  // called by init() to configure device specific DeviceInfo

    void update_state() override;

    void handle_button_data(const robobrawl::ButtonData& data) override; // template 

    void handle_clock_data(const robobrawl::ClockData& data) override;

    void handle_new_device(const robobrawl::DeviceInfo& dev_info) override;

    void handle_timer_expired(const CountdownTimer::Id& id) override;

    int need_override_authority(SystemId target_system);

    void send_override_button(Identity target, ButtonColor new_color, SystemId new_system); 
    
    void send_override_clock(Identity target, Colors display_color, TimeData display_time, SystemId new_system); 
    
    void do_continuous() override;

    void do_medium() override; // for when we are running clock skey control... at 1hz, or I could do more theoreticalu, but then the point would be to have the mytime - theirTime < small, so latancyu becomes a ig condidreation, at leat ao 100hz... 
    
  protected:
    robobrawl::String32 serial_num_{};


    

    Timer   update_timer_;
    Timer   transition_timer_;
  };

};


