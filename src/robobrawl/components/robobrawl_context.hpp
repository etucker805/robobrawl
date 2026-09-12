#pragma once

#include <dds/dds.hpp>

#include "robobrawl.hh" //idl generated 

#include <mutex>

namespace robobrawl {
  class RobobrawlContext {
    public:
      virtual ~RobobrawlContext() = default;

      // ---------------------------------------------------------------
      void init(uint32_t domain_id,
        const std::string& qos_xml_uri,
        const std::string& base_device_id);

      // ---------------------------------------------------------------
      void clear();

      // ---------------------------------------------------------------
      dds::domain::DomainParticipant participant();

      dds::pub::Publisher publisher();

      dds::sub::Subscriber subscriber();

      robobrawl::Identity base_id();

      robobrawl::Identity serial_num();

      robobrawl::RequestSequence next_request_id();

    protected:
      dds::domain::DomainParticipant participant_ = dds::core::null;
      dds::pub::Publisher publisher_ = dds::core::null;
      dds::sub::Subscriber subscriber_ = dds::core::null;

      std::string base_id_;
      std::string serial_num_;

      std::mutex mtx_;
      robobrawl::RequestSequence request_seq_id_ = 0;
  };
}
