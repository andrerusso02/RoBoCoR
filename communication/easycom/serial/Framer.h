#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

#include "../Config.h"
#include "../../../storage/LinearBuffer.h"

template<size_t Size>
class Framer {
public:

    using FrameHandler = std::function<void(uint8_t* data, size_t size)>;

    explicit Framer(uint8_t end_delimiter) : end_delimiter_(end_delimiter) {}

    void add_byte(uint8_t byte) {
        buffer_.push_back(byte);
        if (byte == end_delimiter_) {
            frame_handler_(buffer_.head_ptr() , buffer_.size());
            buffer_.clear();
        }
    }

    void register_frame_handler(const FrameHandler &frame_handler) {
        frame_handler_ = frame_handler;
    }

private:
    LinearBuffer<uint8_t, Size> buffer_;
    FrameHandler frame_handler_;
    uint8_t end_delimiter_;


};