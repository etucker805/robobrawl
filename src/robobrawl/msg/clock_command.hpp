

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class ClockCommandWriter {
    public:
      // -------------------------------------------------------
      ClockCommandWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ClockCommandWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::ClockCommand>(context.participant(), robobrawl::topic::TOPIC_CLOCK_COMMAND);
        dw_ = dds::pub::DataWriter<robobrawl::ClockCommand>(context.publisher(), topic);

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
        const robobrawl::Colors dis_color,
        const robobrawl::TimeData dis_time,
        const robobrawl::SystemId new_sys
        )
      {
        robobrawl::ClockCommand msg;
        msg.source(reqDevId);
        msg.target(tgtId);
        msg.displayColor(dis_color);
        msg.displayTIme(dis_time);
        msg.newSystem(new_sys);
        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::ClockCommand> dw_;
  };

  // -------------------------------------------------------
  
  class ClockCommand_Client {
    public:
      virtual ~ClockCommand_Client() = default;

      virtual void handle_clock_command(const robobrawl::ClockCommand& comm) = 0;
  };

  // -------------------------------------------------------
  
  class ClockCommandListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::ClockCommand> {
    public:
      // -------------------------------------------------------
      ClockCommandListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ClockCommandListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::ClockCommand>(context.participant(),
          robobrawl::topic::TOPIC_CLOCK_COMMAND);
        dr_ = dds::sub::DataReader<robobrawl::ClockCommand>(context.subscriber(), topic);

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
      void add_client( ClockCommand_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( ClockCommand_Client* client )
      { 
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::ClockCommand>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ClockCommand> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](ClockCommand_Client * client)
                           { client->handle_clock_command( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ClockCommand> dr_;
      std::vector<ClockCommand_Client*> clients_;
  };
  
}
