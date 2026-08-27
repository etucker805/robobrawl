
#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/components/qos_source.hpp"

namespace robobrawl {
  class HeartbeatWriter {
    public:
      // -------------------------------------------------------
      HeartbeatWriter()
        : dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      void init(TmsContext& context, const robobrawl::Identity& deviceId)
      {
        const auto topic = dds::topic::Topic<robobrawl::Heartbeat>(context.participant(), robobrawl::topic::TOPIC_HEARTBEAT);
        dw_ = dds::pub::DataWriter<robobrawl::Heartbeat>(context.publisher(), topic, context.qos_source().medium.writer_qos);
        hb_.deviceId(deviceId);

        dds::pub::qos::DataWriterQos dw_qos = dw_.qos();
        dw_qos << dds::core::policy::UserData(dds::core::ByteSeq(deviceId.begin(), deviceId.end()));
        dw_.qos(dw_qos);
      }

      // -------------------------------------------------------
      void clear()
      {
        dw_ = dds::core::null;
      }

      // -------------------------------------------------------
      void publish()
      {
        this->dw_.write(hb_);
        hb_.sequenceNumber(hb_.sequenceNumber() + 1);
      }

    protected:
      dds::pub::DataWriter<robobrawl::Heartbeat> dw_;
      robobrawl::Heartbeat hb_;
  };
}
