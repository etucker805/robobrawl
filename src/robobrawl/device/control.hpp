

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/device.hpp"

//include other messages here
#include "robobrawl/msg/state_change.hpp"

namespace robobrawl {

  class Robobrawl_Control : public Robobrawl_BaseDevice /* add any client classes to extend here */ {
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
      // void handle_ate_reply(const robobrawl::AuthorizationToEnergizeReply& reply) override; // template 


    protected:
      //write / listener / clients here... 
      // StateChangeWriter sc_writer_;
      // state chege writer
      // overide state writers
      // claim system writer
      // clock / button  data readers 
      // sync time writer
      // 

      

  };

};
