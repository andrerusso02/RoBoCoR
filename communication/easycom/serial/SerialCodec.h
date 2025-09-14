#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "cobs.h"
#include "crc16.h"


template<size_t MaxFrameLength>
class SerialCodec {
    /*
     *  Encoded frame: cobs( ID | payload | CRC16) | end_delimiter
     */
public:
    explicit SerialCodec(uint8_t end_delimiter) : end_delimiter_(end_delimiter) {
    }

    size_t encode(uint8_t id, const uint8_t *payload, const size_t payload_length, uint8_t *buff_encoded_data) {
        size_t message_length = concat_id_payload(id, payload, payload_length, buff_encode_);

        add_crc(buff_encode_, message_length, buff_encode_ + message_length);
        message_length+=2;

        size_t message_cobs_length = cobs::cobs_encode(buff_encode_, message_length, buff_encoded_data);
        buff_encoded_data[message_cobs_length] = end_delimiter_;
        return message_cobs_length + 1;
    }

    size_t decode(const uint8_t *received_frame, const size_t received_frame_length, uint8_t &id,
                         const uint8_t *buff_payload) {

        const size_t decoded_length = cobs::cobs_decode(received_frame, received_frame_length - 1, buff_decode_);

        if (check_crc(buff_decode_, decoded_length, buff_decode_ + decoded_length - 2)) {
            std::cerr << "CRC ERROR" << std::endl;
            return -1;
        }
        id = buff_decode_[0];
        size_t payload_length = decoded_length - 3; // - ID - CRC16
        memcpy((void *)buff_payload, buff_decode_ + 1, payload_length);

        return payload_length;
    }

private:
    static size_t concat_id_payload(const uint8_t id, const uint8_t *payload, const size_t payload_length,
                                    uint8_t *buff_message) {
        buff_message[0] = id;
        memcpy(buff_message + 1, payload, payload_length);
        return payload_length + 1;
    }

    static void add_crc(const uint8_t *source, const size_t source_length, uint8_t *dest_crc) {
        uint16_t crc = crc16(source, source_length);
        dest_crc[0] = crc & 0xFF;
        dest_crc[1] = (crc >> 8) & 0xFF;
    }

    static int check_crc(const uint8_t *source, const size_t source_length, const uint8_t *pos_crc) {
        const uint16_t crc = crc16(source, source_length);
        return
            pos_crc[0] != static_cast<uint8_t>(crc & 0xFF) ||
            pos_crc[1] != static_cast<uint8_t>((crc >> 8) & 0xFF);
    }

    uint8_t buff_encode_[MaxFrameLength] = {};
    uint8_t buff_decode_[MaxFrameLength] = {};
    uint16_t end_delimiter_;
};
