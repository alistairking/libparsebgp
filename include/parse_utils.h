//
// Created by Induja on 4/6/2017.
//

#ifndef PARSE_LIB_PARSEUTILS_H
#define PARSE_LIB_PARSEUTILS_H
#include <iostream>
#include <cstring>

enum parse_msg_error {
    INCOMPLETE_MSG = -1;
    //TODO : More error types to come
};

uint32_t extract_from_buffer (unsigned char*& buffer, int &buf_len, void *output_buf, int output_len);

#endif //PARSE_LIB_PARSEUTILS_H
