

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class LightsConfigWriter {
    public:
      // -------------------------------------------------------
      LightsConfigWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~LightsConfigWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::LightsConfig>(context.participant(), robobrawl::topic::TOPIC_LIGHTS_COMMAND);
        dw_ = dds::pub::DataWriter<robobrawl::LightsConfig>(context.publisher(), topic);

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
      void send(const robobrawl::LightsConfig& msg )
      {
        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::LightsConfig> dw_;
  };

  // -------------------------------------------------------
  
  class LightsConfig_Client {
    public:
      virtual ~LightsConfig_Client() = default;

      virtual void handle_lights_config(const robobrawl::LightsConfig& comm) = 0;
  };

  // -------------------------------------------------------
  
  class LightsConfigListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::LightsConfig> {
    public:
      // -------------------------------------------------------
      LightsConfigListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~LightsConfigListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::LightsConfig>(context.participant(),
          robobrawl::topic::TOPIC_LIGHTS_COMMAND);
        dr_ = dds::sub::DataReader<robobrawl::LightsConfig>(context.subscriber(), topic);

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
      void add_client( LightsConfig_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( LightsConfig_Client* client )
      { 
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::LightsConfig>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::LightsConfig> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](LightsConfig_Client * client)
                           { client->handle_lights_config( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::LightsConfig> dr_;
      std::vector<LightsConfig_Client*> clients_;
  };
  
}
