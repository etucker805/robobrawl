

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class ClaimSystemWriter {
    public:
      // -------------------------------------------------------
      ClaimSystemWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~ClaimSystemWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::ClaimSystem>(context.participant(), robobrawl::topic::TOPIC_CLAIM_SYSTEM);
        dw_ = dds::pub::DataWriter<robobrawl::ClaimSystem>(context.publisher(), topic);

        dds::pub::qos::DataWriterQos dw_qos = dw_.qos();
        dw_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::RELIABLE);
        dw_.qos(dw_qos);
      }

      // -------------------------------------------------------
      void clear()
      {
        dw_ = dds::core::null;
      }

      // ------------------------------------------------------- (update)
      void send(
        const robobrawl::Identity& reqDevId,
        const robobrawl::SystemId& tgtId,
        const int                  auth=0
      )
      {
        robobrawl::ClaimSystem msg;
        msg.source(reqDevId);
        msg.target(tgtId);
     
        msg.authority(auth);
        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::ClaimSystem> dw_;
  };

  // -------------------------------------------------------
  
  class ClaimSystem_Client {
    public:
      virtual ~ClaimSystem_Client() = default;

      virtual void handle_claim_system(const robobrawl::ClaimSystem& claim) = 0;
  };

  // -------------------------------------------------------
  
  class ClaimSystemListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::ClaimSystem> {
    public:
      // -------------------------------------------------------
      ClaimSystemListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~ClaimSystemListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::ClaimSystem>(context.participant(),
          robobrawl::topic::TOPIC_CLAIM_SYSTEM);
        dr_ = dds::sub::DataReader<robobrawl::ClaimSystem>(context.subscriber(), topic);

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
      void add_client( ClaimSystem_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( ClaimSystem_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::ClaimSystem>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::ClaimSystem> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](ClaimSystem_Client * client)
                           { client->handle_claim_system( sample.data() ); } );
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::ClaimSystem> dr_;
      std::vector<ClaimSystem_Client*> clients_;
  };
  
}
