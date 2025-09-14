//
// Created by arusso on 9/13/25.
//

#include "../serial/TransportSerialLinux.h"


#include <cstring>
#include <iostream>
#include <ostream>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <thread>
#include <unistd.h>


speed_t baud_to_constant(const int baud) {
    switch (baud) {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 500000: return B500000;
        case 1000000: return B1000000;
        default: return static_cast<speed_t>(-1);
    }
}


TransportSerialLinux::TransportSerialLinux(const std::string &device_path, int baudrate):
    framer_(Framer<FRAME_MAX_SIZE>(0x00)),
    codec_(SerialCodec<FRAME_MAX_SIZE>())
{
    open_connection(device_path, baudrate);

    auto frame_handler = [this](const uint8_t* data, size_t length) {
        uint8_t id;
        uint8_t buff_payload[FRAME_MAX_SIZE]; // TODO move to attribute
        ssize_t payload_length = codec_.decode(data, length, id, buff_payload);
        if (payload_length == -1) {
            std::cerr << "CRC ERROR" << std::endl;
            return;
        }
        on_message_callback_(id, buff_payload, payload_length);
    };
    framer_.register_frame_handler(frame_handler);

    receiver_thread_ = std::thread([this]() {
        uint8_t read_byte;
        while (true) {
            ssize_t nbytes = read(serial_port_, &read_byte, 1);
            if (nbytes < 0) {
                perror("Read");
                break;
            }
            if (nbytes == 0) {
                continue;
            }
            framer_.add_byte(read_byte);
        }
    });
}

TransportSerialLinux::~TransportSerialLinux() {
    close_connection();
}

int TransportSerialLinux::send_message(int id, uint8_t *message, std::size_t length) {
    uint8_t buff_encoded[FRAME_MAX_SIZE];
    size_t encoded_length = codec_.encode(id, message, length, buff_encoded);
    ssize_t nbytes = write(serial_port_, buff_encoded, encoded_length);
    if (nbytes < 0) {
        perror("Write");
        return -1;
    }
    return 0;
}

int TransportSerialLinux::open_connection(const std::string &device_path, int baudrate) {
    std::cout << "Opening serial port at " << device_path << " with baudrate " << baudrate << std::endl;
    serial_port_ = open(device_path.c_str(), O_RDWR | O_NOCTTY);
    /* Note: ODRW means open for r/w, O_NOCTTY means the tty device doesn't control our process.
     * Otherwise, our process could be killed if the devices sends a ctrl=c. */
    if (serial_port_ < 0) {
        std::cerr << "Failed to open serial port at " << device_path << std::endl;
        return -1;
    }

    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port_, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, baud_to_constant(baudrate));
    cfsetospeed(&tty, baud_to_constant(baudrate));

    // Save tty settings
    if (tcsetattr(serial_port_, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }
    return 0;
}

int TransportSerialLinux::close_connection() const {
    return close(serial_port_);
}
