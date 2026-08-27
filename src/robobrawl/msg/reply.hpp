

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class ReplyWriter {
    public:
      // -------------------------------------------------------
      ReplyWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ReplyWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::Reply>(context.participant(), robobrawl::topic::TOPIC_REPLY);
        dw_ = dds::pub::DataWriter<robobrawl::Reply>(context.publisher(), topic);

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
      void send(const robobrawl::Identity& reqDevId,
        const robobrawl::Identity& tgtId,

        const robobrawl::RequestSequence reqSeqId,
        const robobrawl::ReplyCode code,
        const robobrawl::String32& reason = "")
      {
        robobrawl::Reply reply;
        reply.requestingDeviceId(reqDevId);
        reply.targetDeviceId(tgtId);
     
        reply.requestSequenceId(reqSeqId);
        reply.status(robobrawl::ReplyStatus(code, reason));
        this->dw_.write(reply);
      }

    protected:
      dds::pub::DataWriter<robobrawl::Reply> dw_;
  };

  // -------------------------------------------------------
  
  class Reply_Client {
    public:
      virtual ~Reply_Client() = default;

      virtual void handle_reply(const robobrawl::Reply& req) = 0;
  };

  // -------------------------------------------------------
  
  class ReplyListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::Reply> {
    public:
      // -------------------------------------------------------
      ReplyListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ReplyListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::Reply>(context.participant(),
          robobrawl::topic::TOPIC_REPLY);
        dr_ = dds::sub::DataReader<robobrawl::Reply>(context.subscriber(), topic);

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
      void add_client( Reply_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( Reply_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::Reply>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::Reply> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](Reply_Client * client)
                           { client->handle_reply( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::Reply> dr_;
      std::vector<Reply_Client*> clients_;
  };
  
}
