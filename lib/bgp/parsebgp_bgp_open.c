#include "parsebgp_bgp_open.h"
#include "parsebgp_error.h"
#include "parsebgp_utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static parsebgp_error_t parse_capabilities(parsebgp_opts_t *opts,
                                           parsebgp_bgp_open_t *msg,
                                           uint8_t *buf, size_t *lenp,
                                           size_t remain)
{
  size_t len = *lenp, nread = 0;
  parsebgp_bgp_open_capability_t *cap;

  while ((remain - nread) > 0) {
    if ((msg->capabilities =
           realloc(msg->capabilities, sizeof(parsebgp_bgp_open_capability_t) *
                                        (msg->capabilities_cnt + 1))) == NULL) {
      return PARSEBGP_MALLOC_FAILURE;
    }
    cap = &msg->capabilities[msg->capabilities_cnt++];

    // Code
    PARSEBGP_DESERIALIZE_VAL(buf, len, nread, cap->code);

    // Length
    PARSEBGP_DESERIALIZE_VAL(buf, len, nread, cap->len);

    // process data based on the code
    switch (cap->code) {

    case PARSEBGP_BGP_OPEN_CAPABILITY_MPBGP:
      if (cap->len != 4) {
        return PARSEBGP_INVALID_MSG;
      }
      // AFI
      PARSEBGP_DESERIALIZE_VAL(buf, len, nread, cap->values.mpbgp.afi);
      cap->values.mpbgp.afi = ntohs(cap->values.mpbgp.afi);

      // Reserved
      PARSEBGP_DESERIALIZE_VAL(buf, len, nread, cap->values.mpbgp.reserved);

      // SAFI
      PARSEBGP_DESERIALIZE_VAL(buf, len, nread, cap->values.mpbgp.safi);
      break;

    case PARSEBGP_BGP_OPEN_CAPABILITY_AS4:
      if (cap->len != 4) {
        return PARSEBGP_INVALID_MSG;
      }
      PARSEBGP_DESERIALIZE_VAL(buf, len, nread, cap->values.asn);
      cap->values.asn = ntohl(cap->values.asn);
      break;

    // capabilities with data that we are ignoring (since OpenBMP is ignoring
    // it)
    case PARSEBGP_BGP_OPEN_CAPABILITY_OUTBOUND_FILTER:
    case PARSEBGP_BGP_OPEN_CAPABILITY_GRACEFUL_RESTART:
    case PARSEBGP_BGP_OPEN_CAPABILITY_MULTI_SESSION:
      nread += cap->len;
      break;

    // capabilities with no extra data:
    case PARSEBGP_BGP_OPEN_CAPABILITY_ROUTE_REFRESH:
    case PARSEBGP_BGP_OPEN_CAPABILITY_ROUTE_REFRESH_ENHANCED:
    case PARSEBGP_BGP_OPEN_CAPABILITY_ROUTE_REFRESH_OLD:
      if (cap->len != 0) {
        fprintf(stderr,
                "ERROR: Expecting no extra data for BGP OPEN capability %d, "
                "but found %d bytes\n",
                cap->code, cap->len);
        return PARSEBGP_INVALID_MSG;
      }
      break;

    default:
      PARSEBGP_SKIP_NOT_IMPLEMENTED(
        opts, buf, nread, remain - nread,
        "OPEN Capability %d is either unknown or currently unsupported",
        cap->code);
      break;
    }
  }

  *lenp = nread;
  return PARSEBGP_OK;
}

static parsebgp_error_t parse_params(parsebgp_opts_t *opts,
                                     parsebgp_bgp_open_t *msg, uint8_t *buf,
                                     size_t *lenp, size_t remain)
{
  size_t len = *lenp, nread = 0, slen;
  parsebgp_error_t err;
  uint8_t u8;

  msg->capabilities = NULL;
  msg->capabilities_cnt = 0;

  while ((remain - nread) > 0) {
    // Ensure this is a capabilities parameter
    PARSEBGP_DESERIALIZE_VAL(buf, len, nread, u8);
    if (u8 != 2) {
      PARSEBGP_SKIP_NOT_IMPLEMENTED(opts, buf, nread, remain - nread,
                                    "Unsupported BGP OPEN parameter type (%d). "
                                    "Only the Capabilities parameter (Type 2) "
                                    "is supported",
                                    u8);
    }

    // Capabilities Length
    PARSEBGP_DESERIALIZE_VAL(buf, len, nread, u8);

    // parse this capabilities parameter
    slen = len - nread;
    if ((err = parse_capabilities(opts, msg, buf, &slen, u8)) != PARSEBGP_OK) {
      return err;
    }
    nread += slen;
    buf += slen;
  }

  *lenp = nread;
  return PARSEBGP_OK;
}

parsebgp_error_t parsebgp_bgp_open_decode(parsebgp_opts_t *opts,
                                          parsebgp_bgp_open_t *msg,
                                          uint8_t *buf, size_t *lenp,
                                          size_t remain)
{
  size_t len = *lenp, nread = 0, slen;
  parsebgp_error_t err;

  // Version
  PARSEBGP_DESERIALIZE_VAL(buf, len, nread, msg->version);

  // ASN
  PARSEBGP_DESERIALIZE_VAL(buf, len, nread, msg->asn);
  msg->asn = ntohs(msg->asn);

  // Hold Time
  PARSEBGP_DESERIALIZE_VAL(buf, len, nread, msg->hold_time);
  msg->hold_time = ntohs(msg->hold_time);

  // BGP ID
  PARSEBGP_DESERIALIZE_VAL(buf, len, nread, msg->bgp_id);

  // Parameters Length
  PARSEBGP_DESERIALIZE_VAL(buf, len, nread, msg->param_len);

  // no params
  if (msg->param_len == 0) {
    *lenp = nread;
    return PARSEBGP_OK;
  }

  // Parse the capabilities
  slen = len - nread;
  if ((err = parse_params(opts, msg, buf, &slen, (remain - nread))) !=
      PARSEBGP_OK) {
    return err;
  }
  nread += slen;
  buf += slen;

  if (nread != remain) {
    fprintf(stderr, "ERROR: Trailing data after OPEN Capabilities.\n");
    return PARSEBGP_INVALID_MSG;
  }

  *lenp = nread;
  return PARSEBGP_OK;
}

void parsebgp_bgp_open_destroy(parsebgp_bgp_open_t *msg)
{
  if (msg == NULL) {
    return;
  }

  // we don't (currently) have any capabilities that use dynamic memory, so for
  // now just free the capabilities array
  free(msg->capabilities);
}

void parsebgp_bgp_open_dump(parsebgp_bgp_open_t *msg, int depth)
{
  PARSEBGP_DUMP_STRUCT_HDR(parsebgp_bgp_open_t, depth);

  PARSEBGP_DUMP_INT(depth, "Version", msg->version);
  PARSEBGP_DUMP_INT(depth, "ASN", msg->asn);
  PARSEBGP_DUMP_INT(depth, "Hold Time", msg->hold_time);
  PARSEBGP_DUMP_IP(depth, "BGP ID", PARSEBGP_BGP_AFI_IPV4, msg->bgp_id);
  PARSEBGP_DUMP_INT(depth, "Parameters Length", msg->param_len);
  PARSEBGP_DUMP_INT(depth, "Capabilities Count", msg->capabilities_cnt);
  depth++;
  int i;
  parsebgp_bgp_open_capability_t *cap;
  for (i = 0; i < msg->capabilities_cnt; i++) {
    cap = &msg->capabilities[i];

    PARSEBGP_DUMP_STRUCT_HDR(parsebgp_bgp_open_capability_t, depth);

    PARSEBGP_DUMP_INT(depth, "Code", cap->code);
    PARSEBGP_DUMP_INT(depth, "Length", cap->len);

    depth++;
    switch (cap->code) {
    case PARSEBGP_BGP_OPEN_CAPABILITY_MPBGP:
      PARSEBGP_DUMP_STRUCT_HDR(parsebgp_bgp_open_capability_mpbgp_t, depth);

      PARSEBGP_DUMP_INT(depth, "AFI", cap->values.mpbgp.afi);
      PARSEBGP_DUMP_INT(depth, "Reserved", cap->values.mpbgp.reserved);
      PARSEBGP_DUMP_INT(depth, "SAFI", cap->values.mpbgp.safi);
      break;

    case PARSEBGP_BGP_OPEN_CAPABILITY_AS4:
      PARSEBGP_DUMP_INT(depth, "AS4 ASN", cap->values.asn);
      break;

    // capabilities with data that we are ignoring (since OpenBMP is ignoring
    // it)
    case PARSEBGP_BGP_OPEN_CAPABILITY_OUTBOUND_FILTER:
    case PARSEBGP_BGP_OPEN_CAPABILITY_GRACEFUL_RESTART:
    case PARSEBGP_BGP_OPEN_CAPABILITY_MULTI_SESSION:
      PARSEBGP_DUMP_INFO(depth, "Note: Ignored Capability Data\n");
      break;

    // capabilities with no extra data:
    case PARSEBGP_BGP_OPEN_CAPABILITY_ROUTE_REFRESH:
    case PARSEBGP_BGP_OPEN_CAPABILITY_ROUTE_REFRESH_ENHANCED:
    case PARSEBGP_BGP_OPEN_CAPABILITY_ROUTE_REFRESH_OLD:
      break;

    default:
      break;
    }
    depth--;
  }
}