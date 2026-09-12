

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/components/countdown_timer.hpp"
#include "robobrawl/device/robobrawl_device.hpp"

//include other messages here
#include "robobrawl/msg/state_change.hpp"
#include "robobrawl/msg/claim_system.hpp"
#include "robobrawl/msg/button_data.hpp"
#include "robobrawl/msg/button_command.hpp"


namespace robobrawl {

  class Robobrawl_Button : public Robobrawl_Device, public ButtonCommand_Client, public StateChange_Client {
    public:
      void init(RobobrawlContext& context, const robobrawl::Identity& dev_id) override;

      void clear() override;

      void do_startup() override;

      void do_continuous() override;

      void do_medium() override;

      void do_slow() override;

      // -------------------------------------------------------
      // virtual bool authorization_required(const robobrawl::EnergyStartStopRequest& ess) = 0; // left as tempolate 
      virtual void handle_state_change(const robobrawl::StateChange& req)=0;

      virtual void handle_claim_system(const robobrawl::ClaimSystem& claim)=0;

      virtual void handle_button_command(const robobrawl::ButtonCommand& comm)=0;

      virtual void handle_timer_expired(const CountdownTimer::Id& id)=0;



      CountdownTimer timer_;

    protected:
      //write / listener / clients here... 
      // claim reader
      // state change reader
      StateChangeListener   sc_listener_;
      // button override reader 
      ButtonCommandListener bc_listener_;
      // data writer
      ButtonDataWriter      bd_writer_;


      // TImer

  };

};
