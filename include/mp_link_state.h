/*
 * Copyright (c) 2015 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#ifndef _OPENBMP_MPLINKSTATE_H_
#define _OPENBMP_MPLINKSTATE_H_

#include <cstdint>
#include <cinttypes>
#include <sys/types.h>
#include "mp_reach_attr.h"
#include "mp_un_reach_attr.h"
//#include "Logger.h"
//#include "MsgBusInterface.hpp"

namespace bgp_msg {

 //   class MPLinkState {

 //   public:
        /**
         * Defines the BGP link state NLRI types
         *      https://tools.ietf.org/html/draft-ietf-idr-ls-distribution-10#section-3.2
         */
        enum nlri_types {
            NLRI_TYPE_NODE = 1,                        ///< Node
            NLRI_TYPE_LINK,                             ///< Link
            NLRI_TYPE_IPV4_PREFIX,                      ///< IPv4 Prefix
            NLRI_TYPE_IPV6_PREFIX                       ///< IPv6 Prefix
        };

        /**
         * Defines the NLRI protocol-id values
         */
        enum nlri_protocol_ids {
            NLRI_PROTO_ISIS_L1 = 1,                    ///< IS-IS Level 1
            NLRI_PROTO_ISIS_L2,                        ///< IS-IS Level 2
            NLRI_PROTO_OSPFV2,                         ///< OSPFv2
            NLRI_PROTO_DIRECT,                         ///< Direct
            NLRI_PROTO_STATIC,                         ///< Static configuration
            NLRI_PROTO_OSPFV3,                         ///< OSPFv3
            NLRI_PROTO_EPE=7                           ///< EPE per draft-ietf-idr-bgpls-segment-routing-epe
        };

        /**
         * Node (local and remote) common fields
         */
        struct node_descriptor {
            uint32_t    asn;                           ///< BGP ASN
            uint32_t    bgp_ls_id;                     ///< BGP-LS Identifier
            uint8_t     igp_router_id[8];              ///< IGP router ID
            uint8_t     ospf_area_Id[4];               ///< OSPF area ID
            uint32_t    bgp_router_id;                 ///< BGP router ID (draft-ietf-idr-bgpls-segment-routing-epe)
            uint8_t     hash_bin[16];                  ///< binary hash for node descriptor
        };

        /**
         * Node descriptor Sub-TLV's
         *      Used by both remote and local node descriptors
         */
        enum node_descr_sub_types {
            NODE_DESCR_LOCAL_DESCR              = 256,      ///< Local node descriptor
            NODE_DESCR_REMOTE_DESCR,                        ///< Remote node descriptor

            NODE_DESCR_AS                       = 512,      ///< Autonomous System (len=4)
            NODE_DESCR_BGP_LS_ID,                           ///< BGP-LS Identifier (len=4)
            NODE_DESCR_OSPF_AREA_ID,                        ///< OSPF Area-ID (len=4)
            NODE_DESCR_IGP_ROUTER_ID,                       ///< IGP Router-ID (len=variable)
            NODE_DESCR_BGP_ROUTER_ID                        ///< BGP Router ID (draft-ietf-idr-bgpls-segment-routing-epe)
        };


        /**
         * Node (local and remote) common fields
         */
        struct link_descriptor {
            uint32_t    local_id;                           ///< Link Local ID
            uint32_t    remote_id;                          ///< Link Remote ID
            uint8_t     intf_addr[16];                      ///< Interface binary address
            uint8_t     nei_addr[16];                       ///< Neighbor binary address
            uint32_t    mt_id;                              ///< Multi-Topology ID
            bool        is_ipv4;                             ///< True if IPv4, false if IPv6
        };

        /**
         * Link Descriptor Sub-TLV's
         */
        enum link_descr_sub_types {
            LINK_DESCR_ID                       = 258,      ///< Link Local/Remote Identifiers 22/4 (rfc5307/1.1)
            LINK_DESCR_IPV4_INTF_ADDR,                      ///< IPv4 interface address 22/6 (rfc5305/3.2)
            LINK_DESCR_IPV4_NEI_ADDR,                       ///< IPv4 neighbor address 22/8 (rfc5305/3.3)
            LINK_DESCR_IPV6_INTF_ADDR,                      ///< IPv6 interface address 22/12 (rfc6119/4.2)
            LINK_DESCR_IPV6_NEI_ADDR,                       ///< IPv6 neighbor address 22/13 (rfc6119/4.3)
            LINK_DESCR_MT_ID                                ///< Multi-Topology Identifier
        };


        /**
         * Node (local and remote) common fields
         */
        struct prefix_descriptor {
            char        ospf_route_type[32];                ///< OSPF Route type in string form for DB enum
            uint32_t    mt_id;                              ///< Multi-Topology ID
            uint8_t     prefix[16];                         ///< Prefix binary address
            uint8_t     prefix_bcast[16];                   ///< Prefix broadcast/ending binary address
            uint8_t     prefix_len;                         ///< Length of prefix in bits
        };

        /**
         * Prefix Descriptor Sub-TLV's
         */
        enum prefix_descr_sub_types {
            PREFIX_DESCR_MT_ID                  = 263,      ///< Multi-Topology Identifier (len=variable)
            PREFIX_DESCR_OSPF_ROUTE_TYPE,                   ///< OSPF Route Type (len=1)
            PREFIX_DESCR_IP_REACH_INFO                      ///< IP Reachability Information (len=variable)
        };

        /**
         * OSPF Route Types
         */
        enum ospf_route_types {
            OSPF_RT_INTRA_AREA                  = 1,        ///< Intra-Area
            OSPF_RT_INTER_AREA,                             ///< Inter-Area
            OSPF_RT_EXTERNAL_1,                             ///< External type 1
            OSPF_RT_EXTERNAL_2,                             ///< External type 2
            OSPF_RT_NSSA_1,                                 ///< NSSA type 1
            OSPF_RT_NSSA_2                                  ///< NSSA type 2
        };

    struct libparseBGP_mp_link_state_parsed_data {
        std::string peer_addr;                       ///< Printed form of the peer address for logging

        parsed_update_data *parsed_data;       ///< Parsed data structure
        parsed_data_ls *ls_data;           ///< Parsed LS Data
    };
        /**
         * Constructor for class
         *
         * \details Handles bgp Extended Communities
         *
         * \param [in]     logPtr       Pointer to existing Logger for app logging
         * \param [in]     peerAddr     Printed form of peer address used for logging
         * \param [out]    parsed_data  Reference to parsed_update_data; will be updated with all parsed data
         * \param [in]     enable_debug Debug true to enable, false to disable
         */

        void libParseBGP_mp_link_state_init(libparseBGP_mp_link_state_parsed_data *data,std::string peer_address,parsed_update_data *parse_data);

            //       virtual ~MPLinkState();

        /**
         * MP Reach Link State NLRI parse
         *
         * \details Will handle parsing the link state NLRI
         *
         * \param [in]   nlri           Reference to parsed NLRI struct
         */
        void libParseBGP_mp_link_state_parse_reach_link_state(libparseBGP_mp_link_state_parsed_data *data, mp_reach_nlri &nlri);

        /**
         * MP UnReach Link State NLRI parse
         *
         * \details Will handle parsing the unreach link state NLRI
         *
         * \param [in]   nlri           Reference to parsed NLRI struct
         */
        void libParseBGP_mp_link_state_parse_unreach_link_state(libparseBGP_mp_link_state_parsed_data *data, mp_unreach_nlri &nlri);


//    private:
        //bool             debug;                           ///< debug flag to indicate debugging
        //Logger           *logger;                         ///< Logging class pointer

        /**********************************************************************************//*
         * Parse Prefix Descriptor sub-tlvs
         *
         * \details will parse a prefix descriptor (series of sub-tlv's)
         *
         * \param [in]   data           Pointer to the start of the node NLRI data
         * \param [in]   data_len       Length of the data
         * \param [out]  info           prefix descriptor information returned/updated
         * \param [in]   isIPv4         Bool value to indicate IPv4(true) or IPv6(false)
         *
         * \returns number of bytes read
         */
        int libParseBGP_mp_link_state_parse_descr_prefix(u_char *data, int data_len, prefix_descriptor &info, bool is_ipv4);

        /**********************************************************************************//*
         * Hash node descriptor info
         *
         * \details will produce a hash for the node descriptor.  Info hash_bin will be updated.
         *
         * \param [in/out]  info           Node descriptor information returned/updated
         * \param [out]  hash_bin       Node descriptor information returned/updated
         */
        void genNodeHashId(node_descriptor &info);

 //   };

} /* namespace bgp_msg */

#endif //_OPENBMP_MPLINKSTATE_H_
