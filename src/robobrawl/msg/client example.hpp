
#pragma once

#include "robobrawl.hh"
#include "robobrawl/components/robobrawl_context.hpp"
#include "robobrawl/components/qos_source.hpp"

#include <map>
#include <utility>

namespace robobrawl {

 

  // -------------------------------------------------------
  
  class PSR_Client {
    public:
      virtual ~PSR_Client() = default;

      virtual void handle_power_switch_request(const robobrawl::PowerSwitchRequest& req) = 0;
  };

  class PowerSwitchRequestListener : public dds::sub::NoOpDataReaderListener<robobrawl::PowerSwitchRequest> {
    struct RequestKey {
      robobrawl::Identity id;
      robobrawl::ConfigId config;
      robobrawl::RequestSequence seqId;
    };

    struct RequestKeyLess {
      bool operator()(const RequestKey& k1, const RequestKey& k2) const
      {
        if (k1.id < k2.id)
          return true;
        if ((k1.id == k2.id) && (k1.config < k2.config))
          return true;
        if ((k1.id == k2.id) && (k1.config == k2.config) && (k1.seqId < k2.seqId))
          return true;
        return false;
      }
    };

    public:
      PowerSwitchRequestListener()
        : psr_req_dr_(dds::core::null)
      {
      }

      // -------------------------------------------------------
      void init(PSR_Client* Psr_client, TmsContext& context, const robobrawl::Identity& deviceId)
      {
        this->psr_client_ = Psr_client;

        auto req_topic = dds::topic::Topic<robobrawl::PowerSwitchRequest>(context.participant(),
          robobrawl::topic::TOPIC_POWER_SWITCH_REQUEST);
        psr_req_dr_ = dds::sub::DataReader<robobrawl::PowerSwitchRequest>(context.subscriber(), req_topic,
          context.qos_source().command.reader_qos);
        psr_req_dr_.listener(this, dds::core::status::StatusMask::all());
      }

      // -------------------------------------------------------
      void clear()
      {
        psr_req_dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      void add_pending_request(const robobrawl::PowerSwitchRequest& request)
      {
        RequestKey key = {request.requestId().requestingDeviceId(), request.requestId().config(), request.sequenceId()};
        pending_requests_.insert(std::make_pair(key, request));
      }

      // -------------------------------------------------------
      bool get_pending_request(const robobrawl::Identity& id,
        const robobrawl::ConfigId config,
        const robobrawl::RequestSequence seqId,
        robobrawl::PowerSwitchRequest& request)
      {
        bool found = false;
        RequestKey key = {id, config, seqId};
        auto it = pending_requests_.find(key);
        if (it != pending_requests_.end()) {
          request = it->second;
          found = true;
        }
        return found;
      }

      // -------------------------------------------------------
      bool remove_pending_request(const robobrawl::Identity& id,
        const robobrawl::ConfigId config,
        const robobrawl::RequestSequence seqId)
      {
        bool found = false;
        RequestKey key = {id, config, seqId};
        auto it = pending_requests_.find(key);
        if (it != pending_requests_.end()) {
          pending_requests_.erase(it);
          found = true;
        }
        return found;
      }

      // -------------------------------------------------------
      void on_data_available(dds::sub::DataReader<robobrawl::PowerSwitchRequest>& reader) override
      {
        dds::sub::LoanedSamples<robobrawl::PowerSwitchRequest> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            if (psr_client_) {
              psr_client_->handle_power_switch_request(sample.data());
            }
          }
        }
      }

    protected:
      dds::sub::DataReader<robobrawl::PowerSwitchRequest> psr_req_dr_;
      PSR_Client* psr_client_ = nullptr;

      // keep a set of outstanding requests, so we can correlate with authorizations
      std::map<RequestKey, robobrawl::PowerSwitchRequest, RequestKeyLess> pending_requests_;
  };
}
