/*****************************************************************
 *   file:   intracom_storage.cpp
 *   desc:   Intracom storage role implementation
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

#include "intracom_storage.hpp"

namespace tms {

  void Storage::init(TmsContext& context, const tms::Identity& dev_id, bool ac_active)
  {
    ac_active_ = ac_active;
    tms::Identity active_dev_id = dev_id + (ac_active_ ? "-AC" : "-DC" );
    
    serial_num_ = context.serial_num();
    TMS_Storage::init(context, dev_id, ac_active);
    currentLevel_ = tms::EnergyStartStopLevel::ESSL_OFF;

    ess_state_.start_transition( currentLevel_,
                                 0.0f,
                                 tms::PowerSwitchTransitionCode::PSTC_NONE );
    ess_state_.complete_transition( currentLevel_,
                                    tms::PowerSwitchReason::PSR_STARTUP,
                                    NULL );


    // initialize port status
    tms::PowerPortStatus port_status;
    port_status.portNumber(1); // must correspond with port in DeviceInfo
    port_status.cableStatus(tms::CableSenseStatus::CS_UNKNOWN);
    port_status.switchStatus(tms::PowerSwitchStatus(tms::CircuitContinuity::CC_OPEN, // present
                                                    tms::CircuitContinuity::CC_OPEN, // future (for example, if transition)
                                                    false,  // requestLock: switch known to be locked, requests will get PRECONDITION_FAILED
                                                    tms::PowerSwitchReason::PSR_STARTUP,
                                                    dds::core::nullopt,
                                                    tms::PowerSwitchTransitionCode::PSTC_NONE));
    port_state_.set_power_port_status(port_status);


    tms::ac::LoadSharingParameters acls_param;
      // todo : fill in appropriate values for each port mode... 
    acls_param.maxFrequency(55);
    acls_param.minFrequency(45);
    acls_param.maxReactivePower();
    acls_param.minReactivePower();
    acls_param.maxRealPower();
    acls_param.minRealPower();
    acls_param.maxVoltage();
    acls_param.minVoltage();

    acls_state_.set_power_port_status(acls_param);


    tms::dc::LoadSharingParameters dcls_param;

    dcls_param.maxAmperage();
    dcls_param.minAmperage();
    dcls_param.maxVoltage();
    dcls_param.minVoltage();

    dcls_state_.set_power_port_status(dcls_param);



    
    update_timer_.start();
  }

  // -------------------------------------------------------
  void Storage::init_device_info()
  {

    // -----------------------------------------------------------
    // Initialize the DeviceInfo with appropriate info

    device_info_.deviceId(this->device_id_);
    device_info_.role(tms::DeviceRole::ROLE_STORAGE);

    // device_info.product();
    device_info_.product().nsn();
    device_info_.product().gtin();
    device_info_.product().manufacturerName("Intracom");
    device_info_.product().modelName("TBD");
    device_info_.product().modelNumber("TBD");
    device_info_.product().serialNumber(serial_num_);
    device_info_.product().softwareVersion("v0.1");
    device_info_.topics().dataModelVersion(tms::TMS_VERSION);

    device_info_.topics().publishedConditionalTopics({
      ac_active_ ? tms::topic::TOPIC_AC_LOAD_SHARING_STATE : tms::topic::TOPIC_DC_LOAD_SHARING_STATE,
      ac_active_ ? tms::topic::TOPIC_AC_MEASUREMENT_UPDATE : tms::topic::TOPIC_DC_MEASUREMENT_UPDATE,
    });

    device_info_.topics().publishedOptionalTopics({
      tms::topic::TOPIC_AUTHORIZATION_TO_ENERGIZE_RESULT
    });

    device_info_.topics().supportedRequestTopics({
      ac_active_ ? tms::topic::TOPIC_AC_LOAD_SHARING_REQUEST  : tms::topic::TOPIC_DC_LOAD_SHARING_REQUEST, 
      tms::topic::TOPIC_AUTHORIZATION_TO_ENERGIZE_REQUEST,  
      tms::topic::TOPIC_ENERGY_START_STOP_REQUEST
    });

    // device_info.topics().extensionTopics( ); // empty

    tms::ControlHardwareInfo ctrlHardware(true, -30.0f, 50.0f, 1);
    device_info_.controlHardware() = ctrlHardware;

    tms::PowerHardwareInfo powerHardware;
    
    tms::EnergyStorageInfo energyStorage(1.0f, 0.0f, -30.0f, 25.0f, 45.0f);
    powerHardware.energyStorage(energyStorage);

    tms::PowerElectronicsInfo powerElectronics(-30.0f, 50.0f);
    powerHardware.powerElectronics(powerElectronics);
    

    device_info_.powerHardware() = powerHardware;     
    // device_info.controlParameters(); // N/A at this time
    // device_info.metricParameters();  // N/A at this time
    
    tms::PowerPortInfo powerPort[1];
    powerPort[0].portNumber(1);
    powerPort[0].wiring( ac_active_ ? tms::CircuitWiring::WIRING_AC_3WYE : tms::CircuitWiring::WIRING_DC ); // dc may be dc3wire, not reported.
    powerPort[0].directionality(tms::PowerPortDirectionality::PPD_IN_OUT); // todo: assuming true for both ac, dc
    powerPort[0].hasSwitch(true);
    powerPort[0].hasExternalMeter(false);
    powerPort[0].hasInternalMeter(true);
    powerPort[0].hasSummaryMeasurementUpdate(false);


    //  todo: fill in these values, for both ac and dc modes
    powerPort[0].minAmperage( 1 );  // tbd
    powerPort[0].maxAmperage( 50 ); // tbd
    powerPort[0].shortCircuitAmperage( 100 ); //tbd
    powerPort[0].minVoltage();     // tbd
    powerPort[0].maxVoltage();     // tbd
    
    //  todo: fill in values for ac mode
    if( ac_active_ ) { // AC data
      powerPort[0].maxFrequency(55);  // tbd
      powerPort[0].minFrequency(45);  // tbd
    }

    powerPort[0].connectorInfo().features({
                                            tms::PowerConnectorFeature::PCF_CABLE_MEASUREMENT, 
                                            tms::PowerConnectorFeature::PCF_COMMUNICATION
                                          }); 
    powerPort[0].connectorInfo().physicalType(tms::PowerConnectorType::CONNECTOR_BUS); 
    // powerPort[0].connectorInfo().polarity(tms::PowerConnectorPolarity::POLARITY_SOCKET); // Not provided at this time 


    tms::PowerSwitchInfo switchInfo1;
    switchInfo1.features({
        tms::PowerSwitchFeature::PSF_MANUAL_OPEN,        //   features []
        tms::PowerSwitchFeature::PSF_MANUAL_CLOSE });
    // switchInfo1.interruptAmperage(600.0f);   // tbd 
    powerPort[0].switchInfo() = switchInfo1;

    tms::GroundingInfo grounds[1];
    grounds[0].groundNumber(1);
    grounds[0].groundType(tms::GroundingDesignType::GROUNDING_SOLID);
    


    tms::EnergyStartStopLevel essLevels[] = {
        tms::EnergyStartStopLevel::ESSL_OFF,           // todo: just a guess, add or remove here as appropriate
        tms::EnergyStartStopLevel::ESSL_OPERATIONAL
    };
    size_t levelCount = sizeof(essLevels) / sizeof(essLevels[0]);
    
    tms::StorageInfo storage;
    
    // storage.features({tms::StorageFeature::STORF_VEHICLE, tms::StorageFeature::STORF_CHARGING});
    // storage.maxChargeEnergy(32400000.0f); // Joules
    storage.supportedEnergyStartStopLevels().assign(essLevels, essLevels + levelCount);
    storage.loadSharing().portNumber(1); // must correspond to a [the] port defined above.
    
    tms::PowerDeviceInfo powerDevice;
    powerDevice.powerPorts().assign(powerPort, powerPort + 1);
    powerDevice.grounds().assign(grounds, grounds + 1);  
    powerDevice.storage() = storage;

    
    device_info_.powerDevice() = powerDevice;
  
  }

  // -------------------------------------------------------
  // invoked ~100Hz
  void
  Storage::update_state()
  {
    // todo: the values provided below are just
    //       for example purposes
    //  they are collected here just for illustration purposes.


    // ---------------------------------------------
    // Storage Update
    //   todo: get and update status
    tms::StorageUpdate storage_state;
    storage_state.internalVoltage(25.0f);     // V
    // if available
    storage_state.stateOfCharge(0.95f);       // fraction of rated capacity: 0 .. 1
    storage_state.availableEnergy(1000000.0); // J
    storage_state.holdTime(30 * 60 * 60);     // s

    tms::Curve2D chargeCurve;
    tms::Point2D points[] = {{0.0f, 0.0f},
                             {1.0f, 1.0f},
                             {4.0f, 4.0f},
                             {4.5f, 4.5f}};
    chargeCurve.points().assign(points, points + 4);

    storage_state.chargeTime(chargeCurve);       // (W,s)
    storage_state.dischargeTime(chargeCurve);    // (W,s)
    storage_state.maxChargeTime(chargeCurve);    // (W,s)
    storage_state.maxDischargeTime(chargeCurve); // (W,s)

    // if available
    storage_state.maxChargeRate(68.0f);
    // if available
    storage_state.maxDischargeRate(-67.0f);

    storage_update_.set(storage_state);

    // ---------------------------------------------
    // PowerPort State
    //   todo: get and update status
    tms::PowerPortStatus port_status;
    port_status.portNumber(1); // must correspond with port in DeviceInfo
    port_status.cableStatus(tms::CableSenseStatus::CS_UNKNOWN);
    // port_status.cableId     // only if 'smart cable' attached
    // port_status.connection  // only if connected to a known 'remote' port
    port_status.switchStatus(tms::PowerSwitchStatus(tms::CircuitContinuity::CC_UNKNOWN, // present
        tms::CircuitContinuity::CC_UNKNOWN, // future (for example, if transition)
        false,  // requestLock: switch known to be locked, requests will get PRECONDITION_FAILED
        tms::PowerSwitchReason::PSR_STARTUP,
        dds::core::nullopt,
        tms::PowerSwitchTransitionCode::PSTC_NONE));
    port_state_.set_power_port_status(port_status);


    // ---------------------------------------------
    // only the relevent of these two messages is sent, depending on if ac or dc port is active. 
    // ---------------------------------------------
    // DC Measurement Detail [ 1Hz ] 
    tms::dc::MeasurementUpdate dc_detail;
    {
      dc_detail.timeMeasured(tms::now_monotonic());
      tms::dc::PowerPortMeasurement port_measurement;
      port_measurement.portNumber(1); // must correspond to port in DeviceInfo
      // dc_detail.externalMeasurement();  // empty, no external meter
      // a measurement for a single DC power line: [todo: add as many as appropriate]
      tms::dc::PowerLineMeasurement line_measurement;
      line_measurement.voltage(25.0f);      // (V)
      line_measurement.amperage(400.0f);    // (A)
      line_measurement.power(25.0 * 400.0); // (W)
      port_measurement.line().push_back(line_measurement);
      port_measurement.powerRateOfChange(1.0f); // (W/s)
      dc_detail.internalMeasurement().push_back(port_measurement);
    }
    dc_measurement_.set_detail(dc_detail);


    // ---------------------------------------------
    // AC Measurement Detail [ 1Hz ] 
    tms::ac::MeasurementUpdate ac_detail;
    {
      ac_detail.timeMeasured(tms::now_monotonic());
      tms::ac::PowerPortMeasurement port_measurement;
      port_measurement.portNumber(1); // must correspond to port in DeviceInfo
      // ac_detail.externalMeasurement();  // empty, no external meter
      
      // a measurement for a single AC power line: [todo: add as many as appropriate]
      tms::ac::PowerLineMeasurement line_measurement;
      line_measurement.voltage(25.0f);        // (V)
      line_measurement.amperage(400.0f);      // (A)
      line_measurement.frequency(45.0f);      // (Hz)
      line_measurement.phaseOffset(1.0f);     // (Rad)
      line_measurement.realPower(100.0f);     // 
      line_measurement.reactivePower(100.0f); //  
      port_measurement.line().push_back(line_measurement);

      tms::ac::PhaseAngleSequence phase_angle_seq; // list of phase angles, if applicable
      // port_measurement.phaseAngle(phase_angle_seq);

      ac_detail.internalMeasurement().push_back(port_measurement);
    }
    ac_measurement_.set_detail(ac_detail);


  }

  // -------------------------------------------------------
  bool
  Storage::authorization_required( const tms::EnergyStartStopRequest& ess_request )
  {
    // request has already been validated for sanity
    
    // if this will energize a circuit, we may need authorization
    bool auth_required = false;

    switch (ess_request.toLevel())
      {
      case tms::EnergyStartStopLevel::ESSL_ANY:
      case tms::EnergyStartStopLevel::ESSL_UNKNOWN:
        // shouldn't happen
        break;
                        
      case tms::EnergyStartStopLevel::ESSL_OFF:
        // easy.
        break;
                        
      case tms::EnergyStartStopLevel::ESSL_WARM:
      case tms::EnergyStartStopLevel::ESSL_IDLE:
      case tms::EnergyStartStopLevel::ESSL_READY:
      case tms::EnergyStartStopLevel::ESSL_READY_SYNCED:
        // will not energize circuit (right?), no problem.
        break;
                        
      case tms::EnergyStartStopLevel::ESSL_OPERATIONAL:
        {
          // we could examine how long the circuit has been 'dead',
          // if not 'dead for a long time', then we don't need auth.
          // but, for simplicity, we will always ask for auth (unless configured otherwise).
          auth_required = true; // todo: make this configurable.
        }
        break;
      }
    return auth_required;
  }

  // -------------------------------------------------------
  void
  Storage::initiate_energy_start_stop(const tms::EnergyStartStopRequest& ess_request )
  {
    // -------------------------------------------------------
    // todo: interact with hardware to execute/start the transition
    printf("ESS Initiated \n");

    float expectedDuration = 30.0f; // todo: determine this based on the hardware and the requested change
    tms::PowerSwitchTransitionCode transitionCode = tms::PowerSwitchTransitionCode::PSTC_IN_PROGRESS; // todo: determine this

    // record the 'pending change' in ess_state
    ess_state_.start_transition( ess_request.toLevel(), expectedDuration, transitionCode);

    // todo: periodically check (maybe in do_continuous()) the current state; and, 
    // when it changes invoke ess.complete_transition()
    transition_timer_.start();
    to_level_ = ess_request.toLevel();

  }


  // -------------------------------------------------------
  void Storage::do_continuous()
  {
    TMS_Storage::do_continuous();
    
    if ( transition_timer_.is_running() ) {
      if ( transition_timer_.elapsed_msec() > 10.0f * 1000 ) {
        std::cout << this->device_id_ << ": Change Energy Level: " <<
          currentLevel_ << " -> " << to_level_ << std::endl;
        currentLevel_ = to_level_;
        ess_state_.complete_transition( to_level_,
                                        tms::PowerSwitchReason::PSR_REQUEST,
                                        NULL );
        transition_timer_.stop();

      }
    }
  }
  
};
