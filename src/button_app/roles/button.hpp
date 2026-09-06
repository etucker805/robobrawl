#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_button.hpp"

namespace robobrawl {

  

  class Button : public Robobrawl_Button {
  public:
    void init(RobobrawlContext& context, const robobrawl::Identity& dev_id);

    void init_device_info() override;  // called by init() to configure device specific DeviceInfo

    void update_state() override;

    void handle_claim_system(const robobrawl::ClaimSystem& claim);

    void handle_change_state(const robobrawl::StateChange& req);

    void handle_button_command(const robobrawl::ButtonCommand& comm);

    void send_data_message( SystemId system, 
                            ButtonColor color,
                            bool readyPress, 
                            bool tapoutPress, 
                            bool unstickPress); // called in gui or wherever polling happens... idealy. 

    void do_continuous() override;
  protected:
    robobrawl::String32 serial_num_{};

    

    Timer   update_timer_;
    Timer   transition_timer_;
  };

};


