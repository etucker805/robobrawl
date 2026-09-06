

#pragma once

#include "robobrawl.hh"

#include "robobrawl/components/robobrawl_context.hpp"
#include <map>

namespace robobrawl {

    class KnownDevices_Client {
    public:
      virtual ~KnownDevices_Client() = default;

      virtual void handle_new_device(const robobrawl::DeviceInfo& info) = 0;
  };

  class KnownDevices : public dds::sub::NoOpDataReaderListener<robobrawl::DeviceInfo> {
    public:

      // -------------------------------------------------------
      void init(RobobrawlContext& context);

      // -------------------------------------------------------
      void clear();

      // -------------------------------------------------------
      std::vector<std::string> known_device_ids();

      std::vector<robobrawl::SystemId> known_systems_;


      // -------------------------------------------------------
      const robobrawl::DeviceInfo& get_device_info(const std::string& dev_id);

      //--------------------------------------------------------
      void add_client( KnownDevices_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.push_back( client );
      }

      // -------------------------------------------------------
      void rem_client( KnownDevices_Client * client )
      {
        const std::lock_guard<std::mutex> lock(mtx_);
        clients_.erase( std::find(clients_.begin(), clients_.end(), client ) );
      }

      // -------------------------------------------------------
      [[nodiscard]] robobrawl::DeviceRole get_device_role(const std::string& dev_id) const;

      // -------------------------------------------------------
      void handle_device_info(const robobrawl::DeviceInfo& dev_info);

      //--------------------------------------------------------
      void handle_new_device(const robobrawl::DeviceInfo& dev_info);

      // -------------------------------------------------------
      void remove_device_info(const robobrawl::DeviceInfo& dev_info);

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::DeviceInfo>& reader) override;

    protected:
      dds::sub::DataReader<robobrawl::DeviceInfo> dr_ = dds::core::null;
      std::map<robobrawl::Identity, robobrawl::DeviceInfo> known_devices_{};
      std::map<robobrawl::Identity, robobrawl::SystemId>   device_to_system_{};


      std::mutex mtx_{};

      std::vector<KnownDevices_Client*> clients_;

  };

};

