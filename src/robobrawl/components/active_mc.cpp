

#include "active_mc.hpp"

#include "robobrawl.hh"
#include "robobrawl/device/known_devices.hpp"

namespace robobrawl {

  ActiveMC::ActiveMC(const KnownDevices& known_devices)
      :known_devices_(known_devices)
  {

  }

  // ---------------------------------------------------------------
  void
  ActiveMC::init(RobobrawlContext& context)
  {
    auto topic = dds::topic::Topic<robobrawl::Heartbeat>(context.participant(),
        robobrawl::topic::TOPIC_HEARTBEAT);
    dr_ = dds::sub::DataReader<robobrawl::Heartbeat>(context.subscriber(),
        topic);

    dds::sub::qos::DataReaderQos dr_qos = dr_.qos();
    dr_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);
    dr_.qos(dr_qos);  

    dr_.listener(this, dds::core::status::StatusMask::all());
  }

  // ---------------------------------------------------------------
  // Call when we receive a Heartbeat from a 'control' device.
  void
  ActiveMC::handle_heartbeat(const robobrawl::Heartbeat& hb)
  {
    //  check if this 'hb' is from a device with 'controller' role
    robobrawl::DeviceRole role = known_devices_.get_device_role(hb.deviceId());
    if (role == robobrawl::DeviceRole::ROLE_CONTROL) {
      mtx_.lock();

      if (available_mcs_.count(hb.deviceId()) == 0)
        new_controller_available_ = true;

      // update the last update 'timer' associated with this id (we add or update here):
      available_mcs_[hb.deviceId()].start();

      // also, if this is the 'active' controller, update its timestamp too
      if (hb.deviceId() == active_mc_.id) {
        active_mc_.last_update.start();
      }
      mtx_.unlock();
    }
  }

  // ---------------------------------------------------------------
  bool
  ActiveMC::check()
  {
    bool rval = false;
    bool have_active_mc = (!active_mc_.id.empty());
    bool NO_MC = false; /* special flag, set if no available MC after 10 sec */

    bool have_new_mc = false;
    bool lost_active_mc = false;
    bool no_available_mc = false;

    mtx_.lock();

    remove_expired_controllers();

    /* 1) check the status of things: */
    if (new_controller_available_) {
      have_new_mc = true;
      new_controller_available_ = false;
    }

    if (have_active_mc) {
      int64_t elapsed_msec = active_mc_.last_update.elapsed_msec();
      if (elapsed_msec >= 6000) /* lost active -- no updates for >= 6 seconds */
        lost_active_mc = true;

      auto num_available = available_mcs_.size();
      if (num_available == 0)
        no_available_mc = true;
    }

    /*
      fprintf( stderr, "%s(): have_new: %d    lost_active: %d   none_available: %d\n",
      __FUNCTION__, have_new_mc, lost_active_mc, no_available_mc );
    */

    /* 2) start relevant timer[s] based on the status
     */
    if (have_new_mc) {
      if (!new_mc_timer_.is_running()) {
        new_mc_timer_.start();
        std::clog << "ACTIVE_MC: 'New MC' Timer started" << std::endl;
      }
    }

    if (have_active_mc) {
      /* these timers matter only if we have an active_mc
       */
      if (lost_active_mc) {
        if (!lost_active_timer_.is_running()) {
          lost_active_timer_.start();
          std::clog << "ACTIVE_MC: 'Lost Active MC' Timer started" << std::endl;
        }
      }
      else {
        if (lost_active_timer_.is_running()) {
          lost_active_timer_.stop();
          std::clog << "ACTIVE_MC: 'Lost Active MC' Timer canceled" << std::endl;
        }
      }

      if (no_available_mc) {
        if (!no_mc_timer_.is_running()) {
          no_mc_timer_.start();
          std::clog << "ACTIVE_MC: 'No MC' Timer started" << std::endl;
        }
      }
      else {
        if (no_mc_timer_.is_running()) {
          no_mc_timer_.stop();
          std::clog << "ACTIVE_MC: 'No MC' Timer canceled" << std::endl;
        }
      }
    }

    /* 3) based on current state, check timer conditions 
     */
    if (!have_active_mc) {
      /* if there are any 'new' MC's (known for at least 3 sec)
       * - select one
       * - transition to STATE_ACTIVE_MC
       */
      if (new_mc_timer_.is_running()) {
        int64_t new_elapsed = new_mc_timer_.elapsed_msec();
        if (new_elapsed >= 3000) {
          std::clog << "ACTIVE_MC: 'New MC' Timer triggered" << std::endl;
          new_mc_timer_.stop(); /* cancel timer */

          /* select an active controller */
          rval = select_controller();
        }
      }
    }
    else {
      /* if we have 'new MC[s]' available (known at least 3 sec)
       * -- MAYBE: select a new MC [ This is unclear in the spec... ]
       */
      if (new_mc_timer_.is_running()) {
        int64_t elapsed = new_mc_timer_.elapsed_msec();
        if (elapsed >= 3000) {
          std::clog << "ACTIVE_MC: 'New MC' Timer triggered" << std::endl;
          new_mc_timer_.stop(); /* cancel timer */

          /* select an active controller */
          rval = select_controller();
        }
      }

      /* if we have NO AVAILABLE MC's (for at least 10 sec)
       * -- clear 'active MC'
       * -- transition to STATE_NO_MC
       * -- DEVICE processes 'CONFIG_ON_COMMS_LOSS'
       */
      if (no_mc_timer_.is_running()) {
        int64_t elapsed = no_mc_timer_.elapsed_msec();
        if (elapsed >= 10000) {
          std::clog << "ACTIVE_MC: 'No MC' Timer triggered" << std::endl;

          /* no controller, for a while, panic */
          clear_active_controller();
          rval = true;
          NO_MC = true;
        }
      }
      else /* we have some available mc's, so it's worth checking this */
      {
        /* if we have lost 'active MC' (since at least 6 sec )
         * -- clear 'active MC'
         * -- select new MC (from available list)
         */
        if (lost_active_timer_.is_running()) {
          int64_t elapsed = lost_active_timer_.elapsed_msec();
          if (elapsed >= 6000) {
            std::clog << "ACTIVE_MC: 'Lost Active MC' Timer triggered" << std::endl;

            /* select an active controller */
            rval = select_controller();
          }
        }
      }

    }

    /* finally, indicate a 'comms loss' (or 'no controller') condition:
     *   set iff we had a controller, but haven't had one for 10 sec or more
     */
    if (NO_MC) {
      /* todo: process/invoke  CONFIG_ON_COMMS_LOSS
       */
    }

    mtx_.unlock();

    return rval;
  }

  // ---------------------------------------------------------------
  //  when required, select the active controller from all available
  //   controllers based on 'priority'.
  //  Currently the spec says that 'priority' is based on the
  //  alpha-numeric sort of the deviceId's...
  // 
  //  return true if active_mc changes, false otherwise
  //
  bool ActiveMC::select_controller()
  {
    bool rval = false;

    robobrawl::Identity current_mc = active_mc_.id;

    /* consider all 'available' controllers, and select the 
     * 'active' one, based on alph/numeric comparison of ID's.
     *
     * OR, the v2 idl adds a 'MicrogridControllerInfo.priorityRanking',
     * and a comment suggests that this should be used, but there is
     * no detail about how to break a tie...
     *
     * I assume this will be detailed in an update of the MilStd...
     */

    bool first = true;
    for (const auto& [key, value]: available_mcs_) {
      if (first) {
        active_mc_.id = key;
        first = false;
      }
      else {
        if (key.compare(active_mc_.id) < 0) {
          active_mc_.id = key;
          active_mc_.last_update = value;
        }
      }
    }

    /* if the 'active_mc' changed, then send out a message update */
    if (current_mc != active_mc_.id) {
      /* changed! */
      rval = true;
    }

    std::clog << "ACTIVE_MC: picked: '"
              << active_mc_.id
              << "' ("
              << (rval ? "changed" : "no change")
              << ")"
              << std::endl;
    
    return rval;
  }

  // ---------------------------------------------------------------
  void
  ActiveMC::clear_active_controller()
  {
    active_mc_.id = "";
    active_mc_.last_update.stop();
  }

  // ---------------------------------------------------------------
  void
  ActiveMC::remove_expired_controllers()
  {

    for (auto it = available_mcs_.cbegin(); it != available_mcs_.cend(); /* no increment */) {
      int64_t elapsed = it->second.elapsed_msec();
      if (elapsed >= 3000) {
        std::clog <<  "ACTIVE_MC: remove expired: '"
                  << it->first
                  << "'"
                  << std::endl;
        it = available_mcs_.erase(it);
      }
      else {
        ++it;
      }
    }
  }

  void ActiveMC::clear()
  {
    dr_ = dds::core::null;
    active_mc_dw_ = dds::core::null;
  }

  robobrawl::Identity ActiveMC::get_active_controller()
  {
    mtx_.lock();
    robobrawl::Identity ret = active_mc_.id;
    mtx_.unlock();
    return ret;
  }

  bool ActiveMC::is_active_controller(const robobrawl::Identity& id)
  {
    mtx_.lock();
    bool ret = (id == active_mc_.id);
    mtx_.unlock();
    return ret;
  }

  void ActiveMC::on_data_available(dds::sub::DataReader<robobrawl::Heartbeat>& reader)
  {
    dds::sub::LoanedSamples<robobrawl::Heartbeat> samples = reader.take();
    for (const auto& sample: samples) {
      if (sample.info().valid()) {
        handle_heartbeat(sample.data());
      }
    }
  }


};
