#pragma once

#include <string>

#include "../TransportInterface.h"
#include "Framer.h"
#include "SerialCodec.h"
#include "../Config.h"

#include <thread>


class TransportSerialLinux: public TransportInterface {
public:
    TransportSerialLinux(const std::string& device_path,  int baudrate);
    ~TransportSerialLinux() override;

    int send_message(int id, uint8_t* message, std::size_t length) override;

private:
    std::thread receiver_thread_;
    int serial_port_ = -1;
    Framer<FRAME_MAX_SIZE> framer_;
    SerialCodec<FRAME_MAX_SIZE> codec_;
    int open_connection(const std::string& device_path, int baudrate);
    int close_connection() const;
};

