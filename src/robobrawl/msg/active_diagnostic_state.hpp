/*****************************************************************
 *   file:   active_diagnostic_state.hpp
 *   desc:   active diagnostic state declarations
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
#include "tms/components/qos_source.hpp"

namespace tms {
  class ActiveDiagnosticStateWriter {
    public:
      // -------------------------------------------------------
      ActiveDiagnosticStateWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ActiveDiagnosticStateWriter() = default;

      // -------------------------------------------------------
      void init(TmsContext& context, const tms::Identity& deviceId)
      {
        const auto topic = dds::topic::Topic<tms::ActiveDiagnosticState>(context.participant(),
          tms::topic::TOPIC_ACTIVE_DIAGNOSTIC_STATE);
        dw_ = dds::pub::DataWriter<tms::ActiveDiagnosticState>(context.publisher(),
          topic,
          context.qos_source().publish_last.writer_qos);
        state_.deviceId(deviceId);

        dds::pub::qos::DataWriterQos dw_qos = dw_.qos();
        dw_qos << dds::core::policy::UserData(dds::core::ByteSeq(deviceId.begin(), deviceId.end()));
        dw_.qos(dw_qos);

        changed_ = true; // trigger an initial write
      }

      // -------------------------------------------------------
      void clear()
      {
        dw_ = dds::core::null;
      }

      // -------------------------------------------------------
      void raise_trouble_code(tms::DiagnosticTroubleCode& dtc)
      {
        // add to list of active trouble codes
        // if list reaches max len, increment the 'overflow' counter
        changed_ = true;
      }

      void clear_trouble_code(tms::DiagnosticTroubleCode& dtc)
      {
        // remove trouble code (based on 'suspectParameter' as unique key?)
        // if list is at max len, maybe decrement the 'overflow' counter
      }

      // -------------------------------------------------------
      void publish()
      {
        if (changed_) {
          changed_ = false;
          this->dw_.write(state_);
        }
      }

    protected:
      dds::pub::DataWriter<tms::ActiveDiagnosticState> dw_;
      tms::ActiveDiagnosticState state_;
      bool changed_ = false;
  };

  // -------------------------------------------------------
  
  class ActiveDiagnosticStateReader : public dds::sub::NoOpDataReaderListener<tms::ActiveDiagnosticState> 
  {
    public:
      // -------------------------------------------------------
      ActiveDiagnosticStateReader() : dr_(dds::core::null) {}
    
      // -------------------------------------------------------
      virtual ~ActiveDiagnosticStateReader() = default;
    
      // -------------------------------------------------------
      void
      init(TmsContext& context)
      {
        auto topic =
            dds::topic::Topic<tms::ActiveDiagnosticState>(context.participant(),
              tms::topic::TOPIC_ACTIVE_DIAGNOSTIC_STATE);
        dr_ =
            dds::sub::DataReader<tms::ActiveDiagnosticState>(context.subscriber(),
              topic,
              context.qos_source().publish_last.reader_qos);
        
      }

      // -------------------------------------------------------
      void clear()
      {
        dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      bool get_state( tms::Identity devId, tms::ActiveDiagnosticState & state )
      {
        bool found = false;
        const std::lock_guard<std::mutex> lock(mtx_);
        auto it = states_.find( devId );
        if ( it != states_.end() ) {
          state = it->second;
          found = true;
        }
        return found;
      }
    
      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<tms::ActiveDiagnosticState>& reader) override
      {
        const dds::sub::LoanedSamples<tms::ActiveDiagnosticState> samples = reader.take();
        const std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            states_[ sample.data().deviceId() ] = sample.data();
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<tms::ActiveDiagnosticState> dr_;
      std::map< tms::Identity, tms::ActiveDiagnosticState>  states_;
  };
  
}
