

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class ButtonDataWriter {
    public:
      // -------------------------------------------------------
      ButtonDataWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ButtonDataWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::ButtonData>(context.participant(), robobrawl::topic::TOPIC_BUTTON_DATA);
        dw_ = dds::pub::DataWriter<robobrawl::ButtonData>(context.publisher(), topic);

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
        const robobrawl::SystemId& tgtId,

        const robobrawl::States state, 
        const robobrawl::ClockRealtime time)
      {
        robobrawl::ButtonData msg;
        msg.deviceId(reqDevId);

        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::ButtonData> dw_;
  };

  // -------------------------------------------------------
  
  class ButtonData_Client {
    public:
      virtual ~ButtonData_Client() = default;

      virtual void handle_button_data(const robobrawl::ButtonData& req) = 0;
  };

  // -------------------------------------------------------
  
  class ButtonDataListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::ButtonData> {
    public:
      // -------------------------------------------------------
      ButtonDataListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ButtonDataListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::ButtonData>(context.participant(),
          robobrawl::topic::TOPIC_STATE_CHANGE);
        dr_ = dds::sub::DataReader<robobrawl::ButtonData>(context.subscriber(), topic);

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
      void add_client( ButtonData_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( ButtonData_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::ButtonData>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ButtonData> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {

            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](ButtonData_Client * client)
                           { client->handle_button_data( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ButtonData> dr_;
      std::vector<ButtonData_Client*> clients_;
  };
  
}
