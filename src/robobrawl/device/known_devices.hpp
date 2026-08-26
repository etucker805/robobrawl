

#pragma once

#include "robobrawl.hh"

#include "robobrawl/components/robobrawl_context.hpp"
#include <map>

namespace robobrawl {

  class KnownDevices : public dds::sub::NoOpDataReaderListener<robobrawl::DeviceInfo> {
    public:

      // -------------------------------------------------------
      void init(TmsContext& context);

      // -------------------------------------------------------
      void clear();

      // -------------------------------------------------------
      std::vector<std::string> known_device_ids();

      // -------------------------------------------------------
      const robobrawl::DeviceInfo& get_device_info(const std::string& dev_id);

      // -------------------------------------------------------
      [[nodiscard]] robobrawl::DeviceRole get_device_role(const std::string& dev_id) const;

      // -------------------------------------------------------
      void handle_device_info(const robobrawl::DeviceInfo& dev_info);

      // -------------------------------------------------------
      void remove_device_info(const robobrawl::DeviceInfo& dev_info);

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::DeviceInfo>& reader) override;

    protected:
      dds::sub::DataReader<robobrawl::DeviceInfo> dr_ = dds::core::null;
      std::map<robobrawl::Identity, robobrawl::DeviceInfo> known_devices_{};

  };

};

