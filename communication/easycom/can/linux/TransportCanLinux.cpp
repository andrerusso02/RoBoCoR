#include "TransportCanLinux.h"

#include <cstring>


TransportCanLinux::TransportCanLinux(const std::string& can_interface_name) {
    open_connection(can_interface_name);

    receiver_thread_ = std::thread([this]() {
        struct canfd_frame frame {};
        while (true) {
            int nbytes = read(socket_, &frame, sizeof(struct canfd_frame));
            if (nbytes < 0) {
                perror("Read");
                break;
            }
            if (nbytes < sizeof(struct canfd_frame)) {
                fprintf(stderr, "Read: Incomplete CAN frame\n");
                continue;
            }
            on_message_callback_(frame.can_id, frame.data, frame.len);
        }
    });
}

TransportCanLinux::~TransportCanLinux() {
    if (receiver_thread_.joinable()) {
        receiver_thread_.join();
    }
    close_connection();
}

int TransportCanLinux::open_connection(const std::string& can_interface_name) {
    // Create CAN RAW socket
    socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_ < 0) {
        perror("Socket");
        return 1;
    }

    // Locate the interface
    std::strcpy(ifr_.ifr_name, can_interface_name.c_str());
    if (ioctl(socket_, SIOCGIFINDEX, &ifr_) < 0) {
        perror("ioctl");
        return 1;
    }

    // Enable CAN FD support for this socket
    int enable_canfd = 1;
    if (setsockopt(socket_, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
                   &enable_canfd, sizeof(enable_canfd)) != 0) {
        perror("setsockopt CAN FD support");
        return 1;
                   }

    // Bind the socket to can0
    addr_.can_family  = AF_CAN;
    addr_.can_ifindex = ifr_.ifr_ifindex;

    if (bind(socket_, (struct sockaddr *)&addr_, sizeof(addr_)) < 0) {
        perror("Bind");
        return 1;
    }
    return 0;
}

int TransportCanLinux::close_connection() const {
    if (close(socket_) < 0) {
        perror("Close");
        return 1;
    }
    return 0;
}

int TransportCanLinux::send_message(int id, uint8_t *message, std::size_t length) {
    struct canfd_frame frame {};
    frame.can_id = id;
    memcpy(frame.data, message, length);
    frame.len = length;

    if (write(socket_, &frame, sizeof(struct canfd_frame)) != sizeof(struct canfd_frame)) {
        perror("Write");
        return 1;
    }
    return 0;
}
