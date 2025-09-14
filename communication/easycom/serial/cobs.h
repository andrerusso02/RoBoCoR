#pragma once

#include <stddef.h>
#include <stdint.h>

namespace cobs {
    size_t cobs_encode(const uint8_t *data, size_t length, uint8_t *encoded_data);
    size_t cobs_decode(const uint8_t *encoded_data, size_t length, uint8_t *data);
}