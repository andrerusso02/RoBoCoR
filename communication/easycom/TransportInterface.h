#pragma once
#include <array>
#include <cstdint>

using OnMessageCallback = std::function<void(uint8_t, uint8_t*, std::size_t)>;

class TransportInterface {

    public:
    TransportInterface();

    void send_message(int id, uint8_t* message, std::size_t length);

    void on_receive_message(int id, uint8_t* message, std::size_t length);

    void set_on_message_callback(OnMessageCallback callback);

private:
    OnMessageCallback on_message_callback_;






};
