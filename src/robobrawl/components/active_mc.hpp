

#pragma once

#include "robobrawl.hh"
#include "timer.hpp"
#include "robobrawl_context.hpp"
#include "robobrawl/device/known_devices.hpp"

#include <mutex>
#include <set>

namespace robobrawl {

  class ActiveMC : public dds::sub::NoOpDataReaderListener<robobrawl::Heartbeat> {
    public:
      struct MC {
        robobrawl::Identity id;
        Timer last_update;
      };

    public:
      explicit ActiveMC(const KnownDevices& known_devices);

      void init(TmsContext& context);

      void clear();

      // ---------------------------------------------------------------
      // call periodically ( >= 1 Hz ) to see if we need to trigger the
      //  MicrogridController selection algorithm.
      //
      // returns 'true' if active controller changed, false otherwise
      bool check();

      // ---------------------------------------------------------------
      robobrawl::Identity get_active_controller();

      // ---------------------------------------------------------------
      bool is_active_controller(const robobrawl::Identity& id);

      // ---------------------------------------------------------------
//      void publish_active_controller(robobrawl::Identity& from_id);

      // -------------------------------------------------------
      void handle_heartbeat(const robobrawl::Heartbeat& hb);

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::Heartbeat>& reader) override;

    protected:
      // ---------------------------------------------------------------
      bool select_controller();

      void clear_active_controller();

      void remove_expired_controllers();

    protected:
      const KnownDevices& known_devices_;
      Timer no_mc_timer_{};
      Timer new_mc_timer_{};
      Timer lost_active_timer_{};

      std::mutex mtx_{};
      MC active_mc_{};
      std::map<robobrawl::Identity, Timer> available_mcs_{};
      bool new_controller_available_ = false;

      dds::sub::DataReader<robobrawl::Heartbeat> dr_ = dds::core::null;
      dds::pub::DataWriter<robobrawl::ActiveMicrogridControllerState> active_mc_dw_ = dds::core::null;

  };

};

