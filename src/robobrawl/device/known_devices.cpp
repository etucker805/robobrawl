
#include "known_devices.hpp"

namespace robobrawl {

  void KnownDevices::init(TmsContext& context)
  {
    auto topic = dds::topic::Topic<robobrawl::DeviceInfo>(context.participant(), robobrawl::topic::TOPIC_DEVICE_INFO);
    dr_ = dds::sub::DataReader<robobrawl::DeviceInfo>(context.subscriber(), topic,
        context.qos_source().publish_last.reader_qos);
    dr_.listener(this, dds::core::status::StatusMask::all());
  }

  void KnownDevices::clear()
  {
    dr_.listener(nullptr, dds::core::status::StatusMask::all());
    dr_ = dds::core::null;
  }

  std::vector<std::string> KnownDevices::known_device_ids()
  {
    std::vector<std::string> ids;
    for (auto const& imap: known_devices_) {
      ids.push_back(imap.first);
    }
    return ids;
  }

  const robobrawl::DeviceInfo& KnownDevices::get_device_info(const std::string& dev_id)
  {
    if (auto search = known_devices_.find(dev_id); search != known_devices_.end())
      return search->second;
    else
      throw std::runtime_error("unknown");
  }

  robobrawl::DeviceRole KnownDevices::get_device_role(const std::string& dev_id) const
  {
    if (auto search = known_devices_.find(dev_id); search != known_devices_.end())
      return search->second.role();
    else
      return robobrawl::DeviceRole::ROLE_UNKNOWN;
  }

  void KnownDevices::handle_device_info(const robobrawl::DeviceInfo& dev_info)
  {
    const auto [it, added] = known_devices_.insert(std::make_pair(dev_info.deviceId(), dev_info));
    if (!added) {
      it->second = dev_info;
    }
    else {
      std::clog << "NEW  DEVICE: '" << dev_info.deviceId() << "'" << std::endl;
    }
  }

  void KnownDevices::remove_device_info(const robobrawl::DeviceInfo& dev_info)
  {
    std::clog << "DEAD DEVICE: '" << dev_info.deviceId() << "'" << std::endl;
    known_devices_.erase(dev_info.deviceId());
  }

  void KnownDevices::on_data_available(dds::sub::DataReader<robobrawl::DeviceInfo>& reader)
  {
    dds::sub::LoanedSamples<robobrawl::DeviceInfo> samples = reader.take();
    for (const auto& sample: samples) {
      if (sample.info().valid()) {
        handle_device_info(sample.data());
      }
      else {
        remove_device_info(sample.data()); // key is valid, but no other fields
      }
    }
  }


}
