

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
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::StateChange> dr_;
      std::vector<StateChange_Client*> clients_;
  };
  
}
