

#pragma once

namespace robobrawl {
  class QosSource {
    struct QosPair {
      dds::pub::qos::DataWriterQos writer_qos;
      dds::sub::qos::DataReaderQos reader_qos;

      void init(dds::core::QosProvider provider)
      {
        writer_qos = provider.datawriter_qos();
        reader_qos = provider.datareader_qos();

        // --------------------------------------------------------------
        // todo: the TMS architecture (including the Army provided
        //       test tools) is still using XTypes v1.1 tech.  The
        //       intention is to transition to XTypes v1.2+
        //       at some point.
        //
        //  The code below will adjust the QoS defaults slightly to
        //  provide for XTypes v1.1 compatibility.
        //
        // -- we can remove this when all things support XTypes v1.2
        //

#define XTYPES_1_COMPAT (0)  // change to (0) when XTypes v1.2+ is required

#if (COREDX_DDS_VERSION_MAJOR > 5)
        if (XTYPES_1_COMPAT) {
          // set the Reader to send TypeObj v1 (instead of default TypeObj v2)
          auto rtps_reader_qos = reader_qos.policy<org::toc::coredx::core::policy::RTPSReader>();
          rtps_reader_qos.send_typeobj_v1(true);
          reader_qos << rtps_reader_qos;

          // set the Writer to send XCDR1 (instead of default XCDR2)
          writer_qos <<
              dds::core::policy::DataRepresentation(
                dds::core::policy::DataRepresentationIdSeq{DDS_XCDR1_DATA_REPRESENTATION});

          // set the Writer to send TypeObj v1 (in addition to the default TypeObj v2)
          auto rtps_writer_qos = writer_qos.policy<org::toc::coredx::core::policy::RTPSWriter>();
          rtps_writer_qos.send_typeobj_v1(true);
          writer_qos << rtps_writer_qos;
        }
#endif
        // --------------------------------------------------------------
      }
    };

    public:
      // ---------------------------------------------------------------
      QosSource() = default;

      // ---------------------------------------------------------------
      explicit QosSource(const char* qos_xml_uri)
      {
        init(qos_xml_uri);
      }

      // ---------------------------------------------------------------
      void init(const char* qos_xml_uri)
      {
        publish_last.init(dds::core::QosProvider(qos_xml_uri, "PublishLast"));
        rare.init(dds::core::QosProvider(qos_xml_uri, "Rare"));
        slow.init(dds::core::QosProvider(qos_xml_uri, "Slow"));
        medium.init(dds::core::QosProvider(qos_xml_uri, "Medium"));
        continuous.init(dds::core::QosProvider(qos_xml_uri, "Continuous"));
        command.init(dds::core::QosProvider(qos_xml_uri, "Command"));
        response.init(dds::core::QosProvider(qos_xml_uri, "Response"));
        reply.init(dds::core::QosProvider(qos_xml_uri, "Reply"));
      }

      // ---------------------------------------------------------------
      QosPair publish_last;
      QosPair rare;
      QosPair slow;
      QosPair medium;
      QosPair continuous;
      QosPair command;
      QosPair response;
      QosPair reply;
  };
}
