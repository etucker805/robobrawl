

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"

namespace robobrawl {

  // -------------------------------------------------------
  
  class StateChangeWriter {
    public:
      // -------------------------------------------------------
      StateChangeWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~StateChangeWriter() = default;

      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& deviceId)
      {
        auto topic = dds::topic::Topic<robobrawl::StateChange>(context.participant(), robobrawl::topic::TOPIC_STATE_CHANGE);
        dw_ = dds::pub::DataWriter<robobrawl::StateChange>(context.publisher(), topic);

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
        const robobrawl::SystemId& tgtId,

        const robobrawl::States state, 
        const robobrawl::TimeData time)
      {
        robobrawl::StateChange msg;
        msg.source(reqDevId);
        msg.target(tgtId);
     
        msg.newState(state);
        msg.timerTime(time);
        this->dw_.write(msg);
      }

      void send(const robobrawl::Identity& reqDevId,
        const robobrawl::SystemId& tgtId,

        const robobrawl::States state)
      {
        robobrawl::StateChange msg;
        msg.source(reqDevId);
        msg.target(tgtId);
        msg.newState(state);
        msg.timerTime();
        this->dw_.write(msg);
      }

    protected:
      dds::pub::DataWriter<robobrawl::StateChange> dw_;
  };

  // -------------------------------------------------------
  
  class StateChange_Client {
    public:
      virtual ~StateChange_Client() = default;

      virtual void handle_state_change(const robobrawl::StateChange& state) = 0;
  };

  // -------------------------------------------------------
  
  class StateChangeListener :
      public dds::sub::NoOpDataReaderListener<robobrawl::StateChange> {
    public:
      // -------------------------------------------------------
      StateChangeListener() : dr_(dds::core::null) {}

      // -------------------------------------------------------
      virtual ~StateChangeListener() = default;
    
      // -------------------------------------------------------
      void init(RobobrawlContext& context, const robobrawl::Identity& device_id)
      {
        const auto topic = dds::topic::Topic<robobrawl::StateChange>(context.participant(),
          robobrawl::topic::TOPIC_STATE_CHANGE);
        dr_ = dds::sub::DataReader<robobrawl::StateChange>(context.subscriber(), topic);

        dds::sub::qos::DataReaderQos dr_qos = dr_.qos();
        dr_qos << dds::core::policy::Reliability(dds::core::policy::ReliabilityKind::BEST_EFFORT);
        dr_.qos(dr_qos);
        std::cout << "StateChange reader initialized" << std::endl;

        dr_.listener(this, dds::core::status::StatusMask::all());
      }

      // -------------------------------------------------------
      void clear()
      {
        dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      void add_client( StateChange_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( StateChange_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::StateChange>& reader) override
      {
        std::cout << "state Change but like the start yeah " << std::endl;

        const dds::sub::LoanedSamples<robobrawl::StateChange> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            std::cout << "state Change in here ya" << std::endl;
            const std::lock_guard<std::mutex> lock(mtx_);
            std::for_each( clients_.begin(), clients_.end(),
                           [sample](StateChange_Client * client)
                           { client->handle_state_change( sample.data() ); } );
          }
        }
      }

      // -------------------------------------------------------
      // debug: fires when a StateChange writer pairs / unpairs with this reader
      void on_subscription_matched(dds::sub::DataReader<robobrawl::StateChange>& reader,
        const dds::core::status::SubscriptionMatchedStatus& status) override
      {
        std::cout << "StateChange reader matched: current " << status.current_count()
                  << " (change " << status.current_count_change() << ")"
                  << ", total " << status.total_count() << std::endl;
      }

      // -------------------------------------------------------
      // debug: fires when a writer was found but its QoS is incompatible with ours
      void on_requested_incompatible_qos(dds::sub::DataReader<robobrawl::StateChange>& reader,
        const dds::core::status::RequestedIncompatibleQosStatus& status) override
      {
        std::cout << "StateChange reader incompatible QoS: total " << status.total_count()
                  << ", last policy id " << status.last_policy_id() << std::endl;
      }

      // -------------------------------------------------------
      // debug: fires when samples arrived but were dropped
      void on_sample_lost(dds::sub::DataReader<robobrawl::StateChange>& reader,
        const dds::core::status::SampleLostStatus& status) override
      {
        std::cout << "StateChange reader sample lost: total " << status.total_count() << std::endl;
      }

      // -------------------------------------------------------
      void on_sample_rejected(dds::sub::DataReader<robobrawl::StateChange>& reader,
        const dds::core::status::SampleRejectedStatus& status) override
      {
        std::cout << "StateChange reader sample rejected: total " << status.total_count() << std::endl;
      }

    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::StateChange> dr_;
      std::vector<StateChange_Client*> clients_;
  };
  
}
