#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
size_t base32_decode(const char* input, uint8_t* output, size_t max_output);
