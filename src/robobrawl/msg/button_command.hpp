

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class ButtonCommandWriter {
    public:
      // -------------------------------------------------------
      ButtonCommandWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ButtonCommandWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::ButtonCommand>(context.participant(), robobrawl::topic::TOPIC_BUTTON_COMMAND);
        dw_ = dds::pub::DataWriter<robobrawl::ButtonCommand>(context.publisher(), topic);

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
        const robobrawl::ButtonColor color,
        const robobrawl::SystemId new_sys
        )
      {
        robobrawl::ButtonCommand msg;
        msg.source(reqDevId);
        msg.target(tgtId);
        msg.newColor(color);
        msg.newSystem(new_sys);

        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::ButtonCommand> dw_;
  };

  // -------------------------------------------------------
  
  class ButtonCommand_Client {
    public:
      virtual ~ButtonCommand_Client() = default;

      virtual void handle_button_command(const robobrawl::ButtonCommand& comm) = 0;
  };

  // -------------------------------------------------------
  
  class ButtonCommandListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::ButtonCommand> {
    public:
      // -------------------------------------------------------
      ButtonCommandListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ButtonCommandListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::ButtonCommand>(context.participant(),
          robobrawl::topic::TOPIC_STATE_CHANGE);
        dr_ = dds::sub::DataReader<robobrawl::ButtonCommand>(context.subscriber(), topic);

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
      void add_client( ButtonCommand_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( ButtonCommand_Client * client )
      { 
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::ButtonCommand>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ButtonCommand> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](ButtonCommand_Client * client)
                           { client->handle_button_command( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ButtonCommand> dr_;
      std::vector<ButtonCommand_Client*> clients_;
  };
  
}
