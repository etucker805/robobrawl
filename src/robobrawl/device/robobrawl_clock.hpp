

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_device.hpp"

//include other messages here
#include "known_devices.hpp"
#include "robobrawl/msg/state_change.hpp"
#include "robobrawl/msg/claim_system.hpp"
#include "robobrawl/msg/clock_command.hpp"
#include "robobrawl/msg/clock_data.hpp"


namespace robobrawl {

  class Robobrawl_Clock : public Robobrawl_Device, public KnownDevices_Client, public ClockCommand_Client, public StateChange_Client  /* add any client classes to extend here */ {
    public:
      void init(RobobrawlContext& context, const robobrawl::Identity& dev_id) override;

      void clear() override;

      void do_startup() override;

      void do_continuous() override;

      void do_medium() override;

      void do_slow() override;

      // -------------------------------------------------------
      virtual void handle_clock_command(const robobrawl::ClockCommand& comm)=0;

      virtual void handle_claim_system(const robobrawl::ClaimSystem& claim)=0;
      
      virtual void handle_state_change(const robobrawl::StateChange& state)=0;

      virtual void handle_new_device(const robobrawl::DeviceInfo& info)=0; 



    protected:
      //write / listener / clients here... 
     robobrawl::ClockDataWriter cd_writer_;

     robobrawl::ClockCommandListener cc_listener_;
     robobrawl::StateChangeListener  sc_listener_;



  };

};
