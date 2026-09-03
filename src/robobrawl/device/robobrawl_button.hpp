

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_device.hpp"

//include other messages here
#include "robobrawl/msg/state_change.hpp"
#include "robobrawl/msg/claim_system.hpp"
#include "robobrawl/msg/button_data.hpp"


namespace robobrawl {

  class Robobrawl_Button : public Robobrawl_Device, public ClaimSystem_Client {
    public:
      void init(RobobrawlContext& context, const robobrawl::Identity& dev_id) override;

      void clear() override;

      void do_startup() override;

      void do_continuous() override;

      void do_medium() override;

      void do_slow() override;

      // -------------------------------------------------------
      // virtual bool authorization_required(const robobrawl::EnergyStartStopRequest& ess) = 0; // left as tempolate 
      virtual void handle_change_state(const robobrawl::StateChange& req)=0;

      virtual void handle_claim_system(const robobrawl::ClaimSystem& claim)=0;



      // -------------------------------------------------------
      // void handle_ate_reply(const robobrawl::AuthorizationToEnergizeReply& reply) override; // template 


    protected:
      //write / listener / clients here... 
      // claim reader
      ClaimSystemListener cs_reader_;
      // state change reader
      StateChangeListener sc_reader_;
      // data writer
      ButtonDataWriter    bd_writer_;
      // button override reader 



  };

};
