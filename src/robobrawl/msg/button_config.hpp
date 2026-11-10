

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class ButtonConfigWriter {
    public:
      // -------------------------------------------------------
      ButtonConfigWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ButtonConfigWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::ButtonConfig>(context.participant(), robobrawl::topic::TOPIC_BUTTON_COMMAND);
        dw_ = dds::pub::DataWriter<robobrawl::ButtonConfig>(context.publisher(), topic);

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
      void send(const robobrawl::ButtonConfig& msg)
      {
        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::ButtonConfig> dw_;
  };

  // -------------------------------------------------------
  
  class ButtonConfig_Client {
    public:
      virtual ~ButtonConfig_Client() = default;

      virtual void handle_button_config(const robobrawl::ButtonConfig& comm) = 0;
  };

  // -------------------------------------------------------
  
  class ButtonConfigListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::ButtonConfig> {
    public:
      // -------------------------------------------------------
      ButtonConfigListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ButtonConfigListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::ButtonConfig>(context.participant(),
          robobrawl::topic::TOPIC_BUTTON_COMMAND);
        dr_ = dds::sub::DataReader<robobrawl::ButtonConfig>(context.subscriber(), topic);

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
      void add_client( ButtonConfig_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( ButtonConfig_Client * client )
      { 
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::ButtonConfig>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ButtonConfig> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](ButtonConfig_Client * client)
                           { client->handle_button_config( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ButtonConfig> dr_;
      std::vector<ButtonConfig_Client*> clients_;
  };
  
}
