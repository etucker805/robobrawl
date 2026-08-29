

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_device.hpp"
#include "robobrawl/device/known_devices.hpp"

//include other messages here
#include "robobrawl/msg/state_change.hpp"
#include "robobrawl/msg/button_data.hpp"
#include "robobrawl/msg/claim_system.hpp"


namespace robobrawl {

  class Robobrawl_Control : public Robobrawl_BaseDevice, public KnownDevices_Client, public ButtonData_Client /* add any client classes to extend here */ {
    public:
      void init(RobobrawlContext& context, const robobrawl::Identity& dev_id) override;

      void clear() override;

      void do_startup() override;

      void do_continuous() override;

      void do_medium() override;

      void do_slow() override;

      // -------------------------------------------------------
      // virtual bool authorization_required(const robobrawl::EnergyStartStopRequest& ess) = 0; // left as tempolate 


      // -------------------------------------------------------
      void handle_button_data(const robobrawl::ButtonData& data) override; // template 

      void handle_new_device(const robobrawl::DeviceInfo& dev_info) override;


    protected:
      //write / listener / clients here... 
      StateChangeWriter sc_writer_;
      // overide state writers
      ClaimSystemWriter cs_writer_;
      ButtonDataListener bd_listener_;
      // clock / button  data readers 
      // sync time writer
      // 


      //data
      robobrawl::States state_;
      

  };

};
