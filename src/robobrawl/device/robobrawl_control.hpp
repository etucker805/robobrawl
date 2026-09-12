

#pragma once



#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/components/countdown_timer.hpp"
#include "robobrawl/device/robobrawl_device.hpp"
#include "robobrawl/device/known_devices.hpp"

//include other messages here
#include "robobrawl/msg/state_change.hpp"
#include "robobrawl/msg/button_data.hpp"
#include "robobrawl/msg/clock_data.hpp"
#include "robobrawl/msg/button_command.hpp"
#include "robobrawl/msg/clock_command.hpp"
#include "robobrawl/msg/lights_command.hpp"
#include "robobrawl/msg/claim_system.hpp"


namespace robobrawl {

  class Robobrawl_Control : public Robobrawl_BaseDevice, public KnownDevices_Client, public ButtonData_Client, public ClockData_Client /* add any client classes to extend here */ {
    public:
      void init(RobobrawlContext& context, const robobrawl::Identity& dev_id) override;

      void clear() override;

      void do_startup() override;

      void do_continuous() override;

      void do_medium() override;

      void do_slow() override;

      // -------------------------------------------------------
      void send_state_change(SystemId target, States state, TimeData time);

      void send_claim_system(SystemId target);

      void send_button_command(Identity target, SystemId new_system, ButtonColor new_color);

      void send_lights_command(Identity target, SystemId new_system);

      void send_clock_command(Identity target, SystemId new_system, Colors display_color, TimeData display_time);

      inline void update_state_of_system(SystemId target, States new_state, TimeData time){
        states_[target] = new_state;
        send_state_change(target, new_state, time);
      }



      // -------------------------------------------------------
      virtual void handle_button_data(const robobrawl::ButtonData& data)=0; // template 

      virtual void handle_clock_data(const robobrawl::ClockData& data)=0;

      virtual void handle_new_device(const robobrawl::DeviceInfo& dev_info)=0;

      virtual void handle_timer_expired(const  CountdownTimer::Id& id)=0;

      
    

      std::vector<robobrawl::SystemId> claimed_systems_;

      std::map<robobrawl::SystemId, robobrawl::States>        states_{};


      struct min_sec_time{
        int min;
        int sec;
      };


      std::map<robobrawl::SystemId, min_sec_time>      system_match_times_{};

      CountdownTimerManager timer_manager_;


    protected:
      //write / listener / clients here... 
      StateChangeWriter sc_writer_;
      // overide state writers
      ClaimSystemWriter cs_writer_;

      // clock / button  data readers 
      ButtonDataListener bd_listener_;
      ClockDataListener  cd_listener_;

      ButtonCommandWriter bc_writer_;
      ClockCommandWriter  cc_writer_;
      LightsCommandWriter lc_writer_;
      // sync time writer
      // 


      //data
      

  };

};
