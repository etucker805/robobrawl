

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
      init(RobobrawlContext& context,
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



  // -------------------------------------------------------
  
  class DeviceInfoReader : public dds::sub::NoOpDataReaderListener<robobrawl::DeviceInfo> 
  {
    public:
      // -------------------------------------------------------
      DeviceInfoReader() : dr_(dds::core::null) {}
    
      // -------------------------------------------------------
      virtual ~DeviceInfoReader() = default;
    
      // -------------------------------------------------------
      void
      init(RobobrawlContext& context)
      {
        auto topic =
            dds::topic::Topic<robobrawl::DeviceInfo>(context.participant(),
              robobrawl::topic::TOPIC_DEVICE_INFO);
        dr_ =
            dds::sub::DataReader<robobrawl::DeviceInfo>(context.subscriber(),
              topic,
              context.qos_source().publish_last.reader_qos);
        
      }

      // -------------------------------------------------------
      void clear()
      {
        dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      bool get_state( robobrawl::Identity devId, robobrawl::DeviceInfo & state )
      {
        bool found = false;
        const std::lock_guard<std::mutex> lock(mtx_);
        auto it = states_.find( devId );
        if ( it != states_.end() ) {
          state = it->second;
          found = true;
        }
        return found;
      }
    
      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::DeviceInfo>& reader) override
      {
        const dds::sub::LoanedSamples<robobrawl::DeviceInfo> samples = reader.take();
        const std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            states_[ sample.data().deviceId() ] = sample.data();
          }
        }
      }
    
    protected:
      std::mutex mtx_{};
      dds::sub::DataReader<robobrawl::DeviceInfo> dr_;
      std::map< robobrawl::Identity, robobrawl::DeviceInfo>  states_;
  };
}
