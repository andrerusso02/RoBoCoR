#pragma once
#include <cstdint>
#include <utility>
#include <functional>

using OnMessageCallback = std::function<void(uint8_t, uint8_t*, std::size_t)>;

class TransportInterface {

    public:
    virtual ~TransportInterface() = default;

    TransportInterface() = default;

    virtual int send_message(int id, uint8_t* message, std::size_t length) = 0;

    void set_on_message_callback(OnMessageCallback callback) {
        on_message_callback_ = std::move(callback);
    }

protected:
    OnMessageCallback on_message_callback_;
};

