
#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {
  class HeartbeatWriter {
    public:
      // -------------------------------------------------------
      HeartbeatWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        const auto topic = dds::topic::Topic<robobrawl::Heartbeat>(context.participant(), robobrawl::topic::TOPIC_HEARTBEAT);
        dw_ = dds::pub::DataWriter<robobrawl::Heartbeat>(context.publisher(), topic);
        hb_.deviceId(deviceId);

        dds::pub::qos::DataWriterQos dw_qos = dw_.qos();
        dw_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);
        dw_.qos(dw_qos);
      }

      // -------------------------------------------------------
      void clear()
      {
        dw_ = dds::core::null;
      }

      // -------------------------------------------------------
      void publish()
      {
        this->dw_.write(hb_);
        hb_.sequenceNumber(hb_.sequenceNumber() + 1);
      }

    protected:
      dds::pub::DataWriter<robobrawl::Heartbeat> dw_;
      robobrawl::Heartbeat hb_;
  };



    // -------------------------------------------------------
  
  class HeartbeatReader : public dds::sub::NoOpDataReaderListener<robobrawl::Heartbeat> 
  {
    public:
      // -------------------------------------------------------
      HeartbeatReader() : dr_(dds::core::null) {}
    
      // -------------------------------------------------------
      virtual ~HeartbeatReader() = default;
    
      // -------------------------------------------------------
      void
      init(RobobrawlContext& context)
      {
        auto topic = dds::topic::Topic<robobrawl::Heartbeat>(context.participant(),
                      robobrawl::topic::TOPIC_HEARTBEAT);
        dr_ = dds::sub::DataReader<robobrawl::Heartbeat>(context.subscriber(), topic);

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
      bool get_state( robobrawl::Identity devId, robobrawl::Heartbeat & state )
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
      void on_data_available(dds::sub::DataReader<robobrawl::Heartbeat>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::Heartbeat> samples = reader.take();
        const std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            states_[ sample.data().deviceId() ] = sample.data();
            // may need to odo some known devices proc... idk 
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::Heartbeat> dr_;
      std::map< robobrawl::Identity, robobrawl::Heartbeat>  states_;
  };
}
