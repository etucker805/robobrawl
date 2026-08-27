

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/components/qos_source.hpp"

namespace robobrawl {
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
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        const auto topic = dds::topic::Topic<robobrawl::ActiveDiagnosticState>(context.participant(),
          robobrawl::topic::TOPIC_ACTIVE_DIAGNOSTIC_STATE);
        dw_ = dds::pub::DataWriter<robobrawl::ActiveDiagnosticState>(context.publisher(),
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
      void raise_trouble_code(robobrawl::DiagnosticTroubleCode& dtc)
      {
        // add to list of active trouble codes
        // if list reaches max len, increment the 'overflow' counter
        changed_ = true;
      }

      void clear_trouble_code(robobrawl::DiagnosticTroubleCode& dtc)
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
      dds::pub::DataWriter<robobrawl::ActiveDiagnosticState> dw_;
      robobrawl::ActiveDiagnosticState state_;
      bool changed_ = false;
  };

  // -------------------------------------------------------
  
  class ActiveDiagnosticStateReader : public dds::sub::NoOpDataReaderListener<robobrawl::ActiveDiagnosticState> 
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
            dds::topic::Topic<robobrawl::ActiveDiagnosticState>(context.participant(),
              robobrawl::topic::TOPIC_ACTIVE_DIAGNOSTIC_STATE);
        dr_ =
            dds::sub::DataReader<robobrawl::ActiveDiagnosticState>(context.subscriber(),
              topic,
              context.qos_source().publish_last.reader_qos);
        
      }

      // -------------------------------------------------------
      void clear()
      {
        dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      bool get_state( robobrawl::Identity devId, robobrawl::ActiveDiagnosticState & state )
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
      void on_data_available(dds::sub::DataReader<robobrawl::ActiveDiagnosticState>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ActiveDiagnosticState> samples = reader.take();
        const std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            states_[ sample.data().deviceId() ] = sample.data();
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ActiveDiagnosticState> dr_;
      std::map< robobrawl::Identity, robobrawl::ActiveDiagnosticState>  states_;
  };
  
}
