/*
 * Copyright (c) 2013-2016 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */
#ifndef MPREACHATTR_H_
#define MPREACHATTR_H_

#include "bgp_common.h"
#include <list>
#include <string>
#include "update_msg.h"

//namespace bgp_msg {

/**
 * \class   MPReachAttr
 *
 * \brief   BGP attribute MP_REACH parser
 * \details This class parses MP_REACH attributes.
 *          It can be extended to create attributes messages.
 */
/**
 * struct defines the MP_REACH_NLRI (RFC4760 Section 3)
 */
/*struct mp_reach_nlri {
    uint16_t       afi;                 ///< Address Family Identifier
    uint8_t        safi;                ///< Subsequent Address Family Identifier
    uint8_t        nh_len;              ///< Length of next hop
    unsigned char  *next_hop;           ///< Next hop - Pointer to data (normally does not require freeing)
    uint8_t        reserved;            ///< Reserved

    unsigned char  *nlri_data;          ///< NLRI data - Pointer to data (normally does not require freeing)
    uint16_t       nlri_len;            ///< Not in RFC header; length of the NLRI data
};*/

/**
 * Parse the MP_REACH NLRI attribute data
 *
 * \details
 *      Will parse the MP_REACH_NLRI data passed.  Parsed data will be stored
 *      in parsed_data.
 *
 * \param [in]   attr_len       Length of the attribute data
 * \param [in]   data           Pointer to the attribute data
 * \param [out]  parsed_data    Reference to parsed_update_data; will be updated with all parsed data
 *
 */
void libparsebgp_mp_reach_attr_parse_reach_nlri_attr(update_path_attrs *path_attrs, int attr_len, u_char *data);

/**
 * Parses mp_reach_nlri and mp_unreach_nlri (IPv4/IPv6)
 *
 * \details
 *      Will parse the NLRI encoding as defined in RFC4760 Section 5 (NLRI Encoding).
 *
 * \param [in]   isIPv4                     True false to indicate if IPv4 or IPv6
 * \param [in]   data                       Pointer to the start of the prefixes to be parsed
 * \param [in]   len                        Length of the data in bytes to be read
 * \param [in]   peer_info                  Persistent Peer info pointer
 * \param [out]  prefixes                   Reference to a list<prefix_tuple> to be updated with entries
 */
void libparsebgp_mp_reach_attr_parse_nlri_data_ipv4_ipv6(bool isIPv4, u_char *data, uint16_t len, std::list<update_prefix_tuple> &prefixes);

/**
 * Parses mp_reach_nlri and mp_unreach_nlri (IPv4/IPv6)
 *
 * \details
 *      Will parse the NLRI encoding as defined in RFC3107 Section 3 (Carrying Label Mapping information).
 *
 * \param [in]   isIPv4                 True false to indicate if IPv4 or IPv6
 * \param [in]   data                   Pointer to the start of the label + prefixes to be parsed
 * \param [in]   len                    Length of the data in bytes to be read
 * \param [in]   peer_info              Persistent Peer info pointer
 * \param [out]  prefixes               Reference to a list<label, prefix_tuple> to be updated with entries
 */
template <typename PREFIX_TUPLE>
void libparsebgp_mp_reach_attr_parse_nlri_data_label_ipv4_ipv6(bool isIPv4, u_char *data, uint16_t len, std::list<PREFIX_TUPLE> &prefixes);

/**
 * Decode label from NLRI data
 *
 * \details
 *      Decodes the labels from the NLRI data into labels string
 *
 * \param [in]   data                   Pointer to the start of the label + prefixes to be parsed
 * \param [in]   len                    Length of the data in bytes to be read
 * \param [out]  labels                 Reference to string that will be updated with labels delimited by comma
 *
 * \returns number of bytes read to decode the label(s) and updates string labels
 *
 */
static inline uint16_t decode_label(u_char *data, uint16_t len, std::string &labels);

//} /* namespace bgp_msg */

#endif /* MPREACHATTR_H_ */