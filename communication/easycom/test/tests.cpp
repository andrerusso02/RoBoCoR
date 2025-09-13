#include <array>
#include <iostream>
#include <ostream>

// #include "../serial/cobs.h"
#include "../EasyCom.h"
#include "../can/TransportCanLinux.h"
#include "../Messages.h"

#define ID_CMD_VEL 3

struct CmdVel {
    double x;
    double y;
    double theta;
};

void on_cmd_vel(const CmdVel& cmd) {
    std::cout << cmd.x << ", " << cmd.y << ", " << cmd.theta << std::endl;
}


void on_message(int id, uint8_t* message, std::size_t length) {
    std::cout << "Received message with ID: " << id << " and length: " << length << std::endl;
    std::cout << "Data: ";
    for (std::size_t i = 0; i < length; ++i) {
        std::cout << std::hex << static_cast<int>(message[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

void test_transport_can_linux() {
    /*
    sudo modprobe vcan
    sudo ip link add dev can0 type vcan
    sudo ip link set can0 up
     */
    TransportCanLinux transport("can0");
    transport.set_on_message_callback(on_message);
    transport.send_message(4, (uint8_t *)"12345", 5);
}

void test_easycom_can_linux() {
    // cansend can0 003##0.00.00.00.00.00.00.F0.3F.00.00.00.00.00.00.00.40.00.00.00.00.00.00.08.40
    auto easycom = EasyCom::create<TransportCanLinux>("can0");
    easycom.subscribe<CmdVel>(static_cast<EasyComID>(ID_CMD_VEL), on_cmd_vel);
    easycom.send<CmdVel>(static_cast<EasyComID>(ID_CMD_VEL), CmdVel{1.0, 2.0, 3.0});
}

int main() {
    test_easycom_can_linux();
    return 0;
}
