#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {
  class ActiveControllerStateWriter final {
    public:
      // -------------------------------------------------------
      ActiveControllerStateWriter()
        : topic_(dds::core::null),
          dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ActiveControllerStateWriter() = default;

      // -------------------------------------------------------
      void
      init(RobobrawlContext& context,
        const robobrawl::Identity& deviceId)
      {
        topic_ =
            dds::topic::Topic<robobrawl::ActiveMicrogridControllerState>(context.participant(),
              robobrawl::topic::TOPIC_ACTIVE_MICROGRID_CONTROLLER_STATE);
        dw_ =
            dds::pub::DataWriter<robobrawl::ActiveMicrogridControllerState>(context.publisher(), topic_);
        state_.deviceId(deviceId);

        dds::pub::qos::DataWriterQos dw_qos = dw_.qos();
        dw_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);
        dw_.qos(dw_qos);
      }

      // -------------------------------------------------------
      void clear()
      {
        dw_ = dds::core::null;
        topic_ = dds::core::null;
      }

      // -------------------------------------------------------
      void publish(robobrawl::Identity& ac_id)
      {
        if (ac_id.empty())
          state_.masterId().reset();
        else
          state_.masterId(ac_id);
        this->dw_.write(state_);
      }

    protected:
      dds::topic::Topic<robobrawl::ActiveMicrogridControllerState> topic_;
      dds::pub::DataWriter<robobrawl::ActiveMicrogridControllerState> dw_;
      robobrawl::ActiveMicrogridControllerState state_;
  };

  // -------------------------------------------------------
  
  class ActiveControllerStateReader : public dds::sub::NoOpDataReaderListener<robobrawl::ActiveMicrogridControllerState> 
  {
    public:
      // -------------------------------------------------------
      ActiveControllerStateReader() : dr_(dds::core::null) {}
    
      // -------------------------------------------------------
      virtual ~ActiveControllerStateReader() = default;
    
      // -------------------------------------------------------
      void
      init(RobobrawlContext& context)
      {
        auto topic =
            dds::topic::Topic<robobrawl::ActiveMicrogridControllerState>(context.participant(),
              robobrawl::topic::TOPIC_ACTIVE_MICROGRID_CONTROLLER_STATE);
        dr_ =
            dds::sub::DataReader<robobrawl::ActiveMicrogridControllerState>(context.subscriber(), topic);
        
        dds::sub::qos::DataReaderQos dr_qos = dr_.qos();
        dr_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);
        dr_.qos(dr_qos);
    
          
      }

      // -------------------------------------------------------
      void clear()
      {
        dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      bool get_state( robobrawl::Identity devId, robobrawl::ActiveMicrogridControllerState & state )
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
      void on_data_available(dds::sub::DataReader<robobrawl::ActiveMicrogridControllerState>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ActiveMicrogridControllerState> samples = reader.take();
        const std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            states_[ sample.data().deviceId() ] = sample.data();
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ActiveMicrogridControllerState> dr_;
      std::map< robobrawl::Identity, robobrawl::ActiveMicrogridControllerState>  states_;
  };
}
