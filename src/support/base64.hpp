#pragma once
#include <stdlib.h>
#include "defines.hpp"

namespace px {
    char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);
    unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length);
    
    string base64_encode(const string& data);
}

