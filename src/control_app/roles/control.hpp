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

    void handle_new_device(const robobrawl::DeviceInfo& dev_info) override;
    
    void do_continuous() override;

    void do_medium() override; // for when we are running clock skey control... at 1hz, or I could do more theoreticalu, but then the point would be to have the mytime - theirTime < small, so latancyu becomes a ig condidreation, at leat ao 100hz... 
    
  protected:
    robobrawl::String32 serial_num_{};


    

    Timer   update_timer_;
    Timer   transition_timer_;
  };

};


