

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class ClockDataWriter {
    public:
      // -------------------------------------------------------
      ClockDataWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ClockDataWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::ClockData>(context.participant(), robobrawl::topic::TOPIC_CLOCK_DATA);
        dw_ = dds::pub::DataWriter<robobrawl::ClockData>(context.publisher(), topic);

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
      void send(const robobrawl::Identity& devId,
                const robobrawl::SystemId& sysId,
                const long done 
                )

      {
        robobrawl::ClockData msg;
        msg.deviceId(devId);
        msg.sysName(sysId);
        msg.done(done);

        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::ClockData> dw_;
  };

  // -------------------------------------------------------
  
  class ClockData_Client {
    public:
      virtual ~ClockData_Client() = default;

      virtual void handle_clock_data(const robobrawl::ClockData& req) = 0;
  };

  // -------------------------------------------------------
  
  class ClockDataListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::ClockData> {
    public:
      // -------------------------------------------------------
      ClockDataListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ClockDataListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::ClockData>(context.participant(),
          robobrawl::topic::TOPIC_STATE_CHANGE);
        dr_ = dds::sub::DataReader<robobrawl::ClockData>(context.subscriber(), topic);

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
      void add_client( ClockData_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( ClockData_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::ClockData>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ClockData> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](ClockData_Client * client)
                           { client->handle_clock_data( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ClockData> dr_;
      std::vector<ClockData_Client*> clients_;
  };
  
}
