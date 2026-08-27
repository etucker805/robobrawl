

#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/components/qos_source.hpp"
#include "robobrawl/components/timer.hpp"

#include <map>
#include <utility>

namespace robobrawl {
  class DeviceInfoWriter {
    public:
      // -------------------------------------------------------
      DeviceInfoWriter()
        : topic_(dds::core::null),
          dw_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      virtual ~DeviceInfoWriter() = default;

      // -------------------------------------------------------
      void
      init(TmsContext& context,
        const robobrawl::Identity& deviceId)
      {
        topic_ = dds::topic::Topic<robobrawl::DeviceInfo>(context.participant(), robobrawl::topic::TOPIC_DEVICE_INFO);
        dw_ = dds::pub::DataWriter<robobrawl::DeviceInfo>(context.publisher(), topic_,
          context.qos_source().publish_last.writer_qos);

        dds::pub::qos::DataWriterQos dw_qos = dw_.qos();
        dw_qos << dds::core::policy::UserData(dds::core::ByteSeq(deviceId.begin(), deviceId.end()));
        dw_.qos(dw_qos);
      }

      // -------------------------------------------------------
      void clear()
      {
        dw_ = dds::core::null;
        topic_ = dds::core::null;
      }

      // -------------------------------------------------------
      void publish(const robobrawl::DeviceInfo& device_info)
      {
        this->dw_.write(device_info);
      }

    protected:
      dds::topic::Topic<robobrawl::DeviceInfo> topic_;
      dds::pub::DataWriter<robobrawl::DeviceInfo> dw_;
  };
}
