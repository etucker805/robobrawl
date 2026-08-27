#include "robobrawl_context.hpp"
#include <dds/coredx_udp_transport.h>

namespace robobrawl {
  void RobobrawlContext::init(uint32_t domain_id, const std::string& qos_xml_uri, const std::string& base_device_id,
    const std::string& serial_number)
  {
    // make sure the DomainParticipant is not enabled initially
    // so we can install a specially configured UDP Transport
    dds::domain::qos::DomainParticipantFactoryQos dpfqos;
    dpfqos << dds::core::policy::EntityFactory::ManuallyEnable();
    dds::domain::DomainParticipant::participant_factory_qos(dpfqos);

    // make sure the DomainParticipant doesn't enable entities that
    // it creates, so we can get everything created and configued first
    dds::domain::qos::DomainParticipantQos dpqos = dds::domain::DomainParticipant::default_participant_qos();
    dpqos << dds::core::policy::EntityFactory::ManuallyEnable();
    dds::domain::DomainParticipant::default_participant_qos(dpqos);

    base_id_ = base_device_id;
    serial_num_ = serial_number;


    participant_ = dds::domain::DomainParticipant(domain_id);
    // configure CoreDX DDS's udp transport:
    //   - enable IPv6 (not enabled by default)
//    {
//      CoreDX_UdpTransportConfig udpTransportConfig;
//      CoreDX_UdpTransport_get_default_config(&udpTransportConfig);
//      CoreDX_UdpTransport_get_env_config(&udpTransportConfig);
//      udpTransportConfig.use_ipv6 = 1;
//      CoreDX_Transport* transport = CoreDX_UdpTransport_create_transport(&udpTransportConfig);
//      participant_->add_transport(transport);
//      CoreDX_UdpTransport_clear_config(&udpTransportConfig);
//    }
    // enable the participant now, so we can go on to create other things
    participant_.enable();

    publisher_ = dds::pub::Publisher(this->participant_);
    subscriber_ = dds::sub::Subscriber(this->participant_);
  }

  void RobobrawlContext::clear()
  {
    subscriber_ = dds::core::null;
    publisher_ = dds::core::null;
    participant_ = dds::core::null;
  }

  dds::domain::DomainParticipant RobobrawlContext::participant()
  {
    return participant_;
  }

  dds::pub::Publisher RobobrawlContext::publisher() { return publisher_; }

  dds::sub::Subscriber RobobrawlContext::subscriber() { return subscriber_; }

  robobrawl::Identity RobobrawlContext::base_id() { return base_id_; }

  robobrawl::Identity RobobrawlContext::serial_num() { return serial_num_; }

  robobrawl::RequestSequence RobobrawlContext::next_request_id()
  {
    const std::lock_guard<std::mutex> lock(mtx_);
    return request_seq_id_++;
  }
}
