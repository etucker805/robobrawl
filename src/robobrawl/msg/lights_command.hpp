

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class LightsCommandWriter {
    public:
      // -------------------------------------------------------
      LightsCommandWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~LightsCommandWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::LightsCommand>(context.participant(), robobrawl::topic::TOPIC_LIGHTS_COMMAND);
        dw_ = dds::pub::DataWriter<robobrawl::LightsCommand>(context.publisher(), topic);

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
      void send(const robobrawl::Identity& reqDevId,
        const robobrawl::Identity& tgtId,
        const robobrawl::SystemId new_sys
        )
      {
        robobrawl::LightsCommand msg;
        msg.source(reqDevId);
        msg.target(tgtId);
        msg.newSystem(new_sys);
        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::LightsCommand> dw_;
  };

  // -------------------------------------------------------
  
  class LightsCommand_Client {
    public:
      virtual ~LightsCommand_Client() = default;

      virtual void handle_lights_command(const robobrawl::LightsCommand& comm) = 0;
  };

  // -------------------------------------------------------
  
  class LightsCommandListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::LightsCommand> {
    public:
      // -------------------------------------------------------
      LightsCommandListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~LightsCommandListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::LightsCommand>(context.participant(),
          robobrawl::topic::TOPIC_LIGHTS_COMMAND);
        dr_ = dds::sub::DataReader<robobrawl::LightsCommand>(context.subscriber(), topic);

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
      void add_client( LightsCommand_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( LightsCommand_Client* client )
      { 
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::LightsCommand>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::LightsCommand> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](LightsCommand_Client * client)
                           { client->handle_lights_command( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::LightsCommand> dr_;
      std::vector<LightsCommand_Client*> clients_;
  };
  
}
