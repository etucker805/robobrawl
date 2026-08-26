/*****************************************************************
 *   file:   power_switch_req.hpp
 *   desc:   power switch request declarations
 *
 *****************************************************************
 *
 *   Copyright(C) 2022-2026 Twin Oaks Computing, Inc
 *   All rights reserved.   Castle Rock, CO 80108
 *
 *****************************************************************
 *  This software has been provided pursuant to a License Agreement
 *  containing restrictions on its use.  This software contains
 *  valuable trade secrets and proprietary information of
 *  Twin Oaks Computing, Inc and is protected by law.  It may not be
 *  copied or distributed in any form or medium, disclosed to third
 *  parties, reverse engineered or used in any manner not provided
 *  for in said License Agreement except with the prior written
 *  authorization from Twin Oaks Computing, Inc.
 *****************************************************************/

#pragma once

#include "tms.hh"
#include "tms/components/tms_context.hpp"
#include "tms/components/qos_source.hpp"

#include <map>
#include <utility>

namespace tms {

 

  // -------------------------------------------------------
  
  class PSR_Client {
    public:
      virtual ~PSR_Client() = default;

      virtual void handle_power_switch_request(const tms::PowerSwitchRequest& req) = 0;
  };

  class PowerSwitchRequestListener : public dds::sub::NoOpDataReaderListener<tms::PowerSwitchRequest> {
    struct RequestKey {
      tms::Identity id;
      tms::ConfigId config;
      tms::RequestSequence seqId;
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
      void init(PSR_Client* Psr_client, TmsContext& context, const tms::Identity& deviceId)
      {
        this->psr_client_ = Psr_client;

        auto req_topic = dds::topic::Topic<tms::PowerSwitchRequest>(context.participant(),
          tms::topic::TOPIC_POWER_SWITCH_REQUEST);
        psr_req_dr_ = dds::sub::DataReader<tms::PowerSwitchRequest>(context.subscriber(), req_topic,
          context.qos_source().command.reader_qos);
        psr_req_dr_.listener(this, dds::core::status::StatusMask::all());
      }

      // -------------------------------------------------------
      void clear()
      {
        psr_req_dr_ = dds::core::null;
      }

      // -------------------------------------------------------
      void add_pending_request(const tms::PowerSwitchRequest& request)
      {
        RequestKey key = {request.requestId().requestingDeviceId(), request.requestId().config(), request.sequenceId()};
        pending_requests_.insert(std::make_pair(key, request));
      }

      // -------------------------------------------------------
      bool get_pending_request(const tms::Identity& id,
        const tms::ConfigId config,
        const tms::RequestSequence seqId,
        tms::PowerSwitchRequest& request)
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
      bool remove_pending_request(const tms::Identity& id,
        const tms::ConfigId config,
        const tms::RequestSequence seqId)
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
      void on_data_available(dds::sub::DataReader<tms::PowerSwitchRequest>& reader) override
      {
        dds::sub::LoanedSamples<tms::PowerSwitchRequest> samples = reader.take();
        for (const auto& sample: samples) {
          if (sample.info().valid()) {
            if (psr_client_) {
              psr_client_->handle_power_switch_request(sample.data());
            }
          }
        }
      }

    protected:
      dds::sub::DataReader<tms::PowerSwitchRequest> psr_req_dr_;
      PSR_Client* psr_client_ = nullptr;

      // keep a set of outstanding requests, so we can correlate with authorizations
      std::map<RequestKey, tms::PowerSwitchRequest, RequestKeyLess> pending_requests_;
  };
}
