#include "base32_decode.h"

// Base32 decoding function
// Parameters:
//   input         - base32-encoded null-terminated string
//   output        - buffer to store decoded bytes
//   max_output    - maximum size of output buffer
// Returns: number of bytes written to output
size_t base32_decode(const char* input, uint8_t* output, size_t max_output) {
    uint32_t buffer = 0;
    int bits_left = 0;
    size_t out_len = 0;

    while (*input && out_len < max_output) {
        char c = toupper((unsigned char)*input++);
        uint8_t val;

        if (c >= 'A' && c <= 'Z') {
            val = c - 'A';
        } else if (c >= '2' && c <= '7') {
            val = c - '2' + 26;
        } else if (c == '=') {
            break;  // padding character
        } else {
            continue;  // skip invalid chars like spaces or dashes
        }

        buffer = (buffer << 5) | val;
        bits_left += 5;

        if (bits_left >= 8) {
            bits_left -= 8;
            output[out_len++] = (uint8_t)((buffer >> bits_left) & 0xFF);
        }
    }

    return out_len;
}
