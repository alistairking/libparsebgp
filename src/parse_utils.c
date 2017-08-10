//
// Created by Induja on 4/6/2017.
//

#include "parse_utils.h"
#include <string.h>
/**
 * Function to extract data from buffer
 *
 * @param buffer    Containes the data
 * @param buf_len    Length of buffer provided
 * @param output_buf Data from buffer is stored into this
 * @param output_len Length to be stored in output buffer
 *
 * @return  size of data stored in output_buf
 */
ssize_t extract_from_buffer(unsigned char **buffer, int *buf_len,
                            void *output_buf, ssize_t output_len)
{
  if (output_len > *buf_len)
    return INCOMPLETE_MSG; // return (output_len - buf_len);
  memcpy(output_buf, *buffer, output_len);
  *buffer = (*buffer + output_len);
  *buf_len -= output_len;
  return output_len;
}

/**
 *  Simple function to swap bytes around from network to host or
 *  host to networking.  This method will convert any size byte variable,
 *  unlike ntohs and ntohl.
 *
 * @param [in/out] var   Variable containing data to update
 * @param [in]     size  Size of var - Default is size of var
 */
void SWAP_BYTES(void *var, int size)
{
  if (size <= 1)
    return;

  u_char *v = (u_char *)var;

  // Allocate a working buffer
  u_char buf[size];

  // Make a copy
  memcpy(buf, var, size);

  int i2 = 0;
  for (int i = size - 1; i >= 0; i--)
    v[i2++] = buf[i];
}
