

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/device/robobrawl_device.hpp"
#include <cstdint>

namespace robobrawl {

  // -------------------------------------------------------
  
  class ClockConfigWriter {
    public:
      // -------------------------------------------------------
      ClockConfigWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ClockConfigWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::ClockConfig>(context.participant(), robobrawl::topic::TOPIC_CLOCK_COMMAND);
        dw_ = dds::pub::DataWriter<robobrawl::ClockConfig>(context.publisher(), topic);

        dds::pub::qos::DataWriterQos dw_qos = dw_.qos();
        dw_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);
        dw_.qos(dw_qos);
      }

      // -------------------------------------------------------
      void clear()
      {
        dw_ = dds::core::null;
      }

      // ------------------------------------------------------- ( CHange ) 
      void send(const robobrawl::ClockConfig& comm )
      {
        this->dw_.write(comm);
      }

    protected:
      dds::pub::DataWriter<robobrawl::ClockConfig> dw_;
  };

  // -------------------------------------------------------
  
  class ClockConfig_Client {
    public:
      virtual ~ClockConfig_Client() = default;

      virtual void handle_clock_config(const robobrawl::ClockConfig& comm) = 0;
  };

  // -------------------------------------------------------
  
  class ClockConfigListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::ClockConfig> {
    public:
      // -------------------------------------------------------
      ClockConfigListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ClockConfigListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::ClockConfig>(context.participant(),
          robobrawl::topic::TOPIC_CLOCK_COMMAND);
        dr_ = dds::sub::DataReader<robobrawl::ClockConfig>(context.subscriber(), topic);

        dds::sub::qos::DataReaderQos dr_qos = dr_.qos();
        dr_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);
        dr_.qos(dr_qos);
    


        dr_.listener(this, dds::core::status::StatusMask::all());
      }

      // -------------------------------------------------------
      void clear()
      {
        dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      void add_client( ClockConfig_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( ClockConfig_Client* client )
      { 
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::ClockConfig>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ClockConfig> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](ClockConfig_Client * client)
                           { client->handle_clock_config( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ClockConfig> dr_;
      std::vector<ClockConfig_Client*> clients_;
  };
  
}
