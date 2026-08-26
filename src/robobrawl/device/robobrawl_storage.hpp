

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_device.hpp"

#include "robobrawl/msg/power_port_state.hpp"
#include "robobrawl/msg/energy_start_stop.hpp"
#include "robobrawl/msg/storage_update.hpp"
#include "robobrawl/msg/auth_to_energize.hpp"
#include "robobrawl/msg/power_hardware_update.hpp"
#include "robobrawl/msg/dc_load_sharing.hpp"
#include "robobrawl/msg/ac_load_sharing.hpp"
#include "robobrawl/msg/dc_measurement.hpp"
#include "robobrawl/msg/ac_measurement.hpp"

namespace robobrawl {

  class TMS_Storage : public TMS_Device, public ATEReply_Client, public ESS_Client, public DCLS_Client, public ACLS_Client {
    public:
      void init(TmsContext& context, const robobrawl::Identity& dev_id) override;

      void init(TmsContext& context, const robobrawl::Identity& dev_id, bool ac_active);  // non-general... needed for swaping capability.


      void clear() override;

      void do_startup() override;

      void do_continuous() override;

      void do_medium() override;

      void do_slow() override;

      // -------------------------------------------------------
      virtual bool authorization_required(const robobrawl::EnergyStartStopRequest& ess) = 0;

      virtual void initiate_energy_start_stop(const robobrawl::EnergyStartStopRequest& ess) = 0;

      // -------------------------------------------------------
      void handle_ate_reply(const robobrawl::AuthorizationToEnergizeReply& reply) override;

      void handle_ess_request(const robobrawl::EnergyStartStopRequest& req) override;

      void handle_dcls_request(const robobrawl::dc::LoadSharingRequest& req) override;

      void handle_acls_request(const robobrawl::ac::LoadSharingRequest& req) override;


    protected:
      PowerPortStateWriter port_state_;
      StorageUpdateWriter storage_update_;
      PowerHardwareUpdateWriter power_hw_update_;
      DcMeasurementWriter dc_measurement_;
      AcMeasurementWriter ac_measurement_;
      EnergyStartStopStateWriter ess_state_;
      DcLoadSharingStateWriter dcls_state_;
      AcLoadSharingStateWriter acls_state_;
    
      EnergyStartStopRequestListener ess_;
      DcLoadSharingRequestListener dcls_;
      AcLoadSharingRequestListener acls_;
      AuthToEnergizeClient ate_;

      bool ac_active_;
  };

};
