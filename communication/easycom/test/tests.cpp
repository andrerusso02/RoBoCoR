#include <array>
#include <iostream>
#include <ostream>

// #include "../serial/cobs.h"
#include "../EasyCom.h"
#include "../can/TransportCanLinux.h"
#include "../serial/TransportSerialLinux.h"
#include "../Messages.h"

#define ID_CMD_VEL 3



struct CmdVel {
    double x;
    double y;
    double theta;
};

void on_cmd_vel(const CmdVel &cmd) {
    std::cout << cmd.x << ", " << cmd.y << ", " << cmd.theta << std::endl;
}


void on_message(int id, uint8_t *message, std::size_t length) {
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
    transport.send_message(4, (uint8_t *) "12345", 5);
}

void test_transport_serial_linux() {
    /*
    socat -d -d pty,raw,echo=0 pty,raw,echo=0
    xxd /dev/pts/3 (to visualize)
    */
    TransportSerialLinux transport_tx("/dev/pts/4", 9600);

    TransportSerialLinux transport_rx("/dev/pts/3", 9600);
    transport_rx.set_on_message_callback(on_message);

    uint8_t msg[] = {3, 2, 1, 0, 1, 2, 3};
    for (int i = 0; i < 10; ++i) {
        transport_tx.send_message(4, msg, 7);
        sleep(1);
    }
}

void test_easycom_can_linux() {
    // cansend can0 003##0.00.00.00.00.00.00.F0.3F.00.00.00.00.00.00.00.40.00.00.00.00.00.00.08.40
    auto easycom = EasyCom::create<TransportCanLinux>("can0");
    easycom.subscribe<CmdVel>(ID_CMD_VEL, on_cmd_vel);
    easycom.send<CmdVel>(ID_CMD_VEL, CmdVel{1.0, 2.0, 3.0});
}

void test_easycom_serial_linux() {
    auto easycom_1 = EasyCom::create<TransportSerialLinux>("/dev/pts/3", 9600);
    easycom_1.subscribe<CmdVel>(ID_CMD_VEL, on_cmd_vel);

    auto easycom_2 = EasyCom::create<TransportSerialLinux>("/dev/pts/4", 9600);

    easycom_2.send<CmdVel>(ID_CMD_VEL, CmdVel{1.0, 2.0, 3.0});
}

int main() {
    // test_transport_can_linux();
    // test_easycom_can_linux();
    // test_transport_serial_linux();
    test_easycom_serial_linux();
    return 0;
}
