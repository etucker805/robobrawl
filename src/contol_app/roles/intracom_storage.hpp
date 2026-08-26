/*****************************************************************
 *   file:   intracom_storage.hpp
 *   desc:   Intracom storage role declarations
 *
 *****************************************************************
 *
 *   Copyright(C) 2022-2026 Twin Oaks Computing, Inc
 *   All rights reserved.   Castle Rock, CO 80108
 *
 *****************************************************************
 *  This software has been provided pursuant to a License Agreement
 *  containing restrictions on its use.  This software contains
 *  valuable trade secrets and proprietary information of
 *  Twin Oaks Computing, Inc and is protected by law.  It may not be
 *  copied or distributed in any form or medium, disclosed to third
 *  parties, reverse engineered or used in any manner not provided
 *  for in said License Agreement except with the prior written
 *  authorization from Twin Oaks Computing, Inc.
 *****************************************************************/

#pragma once

#include "tms.hh"
#include "tms/components/tms_context.hpp"
#include "tms/device/tms_storage.hpp"

namespace tms {

  

  class Storage : public TMS_Storage {
  public:
    void init(TmsContext& context, const tms::Identity& dev_id, bool ac_active);

    void init_device_info() override;  // called by init() to configure device specific DeviceInfo

    void update_state() override;

    bool authorization_required(const tms::EnergyStartStopRequest& ess) override;

    void initiate_energy_start_stop(const tms::EnergyStartStopRequest& ess) override;

    void do_continuous() override;
  protected:
    tms::String32 serial_num_{};

    tms::EnergyStartStopLevel to_level_;
    tms::EnergyStartStopLevel currentLevel_;
    
    bool    ac_active_; // ac port active if true, dc if not.... 
    Timer   update_timer_;
    Timer   transition_timer_;
    float   runtime_;
    float   fuelLevel_;
  };

};


