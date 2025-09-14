#include "cobs.h"
namespace cobs {

    size_t cobs_encode(const uint8_t *data, size_t length, uint8_t *encoded_data) {
        uint8_t *encode = encoded_data; // Encoded byte pointer
        uint8_t *codep = encode++; // Output code pointer
        uint8_t code = 1; // Code value

        for (const uint8_t *byte = data; length--; ++byte) {
            if (*byte) // Byte not zero, write it
                *encode++ = *byte, ++code;

            if (!*byte || code == 0xff) { // Input is zero or block completed, restart
                *codep = code, code = 1, codep = encode;
                if (!*byte || length)
                    ++encode;
            }
        }
        *codep = code; // Write final code value
        return (size_t)(encode - encoded_data);
    }

    size_t cobs_decode(const uint8_t *encoded_data, size_t length, uint8_t *data) {
        const uint8_t *byte = encoded_data; // Encoded input byte pointer
        uint8_t *decode = data; // Decoded output byte pointer

        for (uint8_t code = 0xff, block = 0; byte < encoded_data + length; --block) {
            if (block) // Decode block byte
                *decode++ = *byte++;
            else {
                block = *byte++;             // Fetch the next block length
                if (block && (code != 0xff)) // Encoded zero, write it unless it's delimiter.
                    *decode++ = 0;
                code = block;
                if (!code) // Delimiter code found
                    break;
            }
        }
        return (size_t)(decode - (uint8_t *)data);
    }

}