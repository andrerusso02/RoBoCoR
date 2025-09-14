#pragma once

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <string>

#include <fstream>
#include <iostream>
#include <cstring>
#include <cassert>

#include <boost/asio.hpp>

#include <string>

#include "../TransportInterface.h"


class TransportCanLinux : public TransportInterface {
public:
    explicit TransportCanLinux(const std::string& can_interface_name);
    ~TransportCanLinux() override;

    int send_message(int id, uint8_t* message, std::size_t length) override;

private:
    std::thread receiver_thread_;
    int socket_;
    struct sockaddr_can addr_;
    struct ifreq ifr_;
    int open_connection(const std::string& can_interface_name);
    int close_connection() const;
};