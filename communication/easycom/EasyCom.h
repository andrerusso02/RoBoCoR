#pragma once
#include <memory>

#include "Framer.h"


class EasyCom {

    using TransmitFunc = void(*)(const uint8_t* data, size_t length);

    template <typename T>
    using ReceiveFunc = void(*)(const T& data);

public:

    EasyCom() : framer_(Framer<FRAME_MAX_SIZE>(END_DELIMITER)) {
        framer_.register_frame_handler(std::bind(&EasyCom::on_frame, this, std::placeholders::_1, std::placeholders::_2));
    }

    template <typename T>
    void send(T data) {
        // get ID from type
        // serialize data : ID + bytes + crc16
        // cobs encode
        // add delimiter
        // call send method
    }

    void register_transmit_func(TransmitFunc func) {
        transmit_func_ = func;
    }

    template <typename T>
    void subscribe(ReceiveFunc<T> func) {
        // Find ID of type T
        dispatcher.register_callback(id, func);
    }

    void receive(uint8_t byte) {
        framer_.add_byte(byte);
    }

private:
    static std::shared_ptr<EasyCom> instance_;
    TransmitFunc transmit_func_{nullptr};
    Framer<FRAME_MAX_SIZE> framer_;

    void on_frame(uint8_t* data, size_t size) {
        // decode cobs
        // deserialize : get ID + bytes + crc16
        // check crc16
    }
};

