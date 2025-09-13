#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>

#include "Config.h"


using CallBackFunction = std::function<void(const uint8_t*, size_t)>;

struct PairIDCallback {
    uint16_t id;
   CallBackFunction callback;
};

class Dispatcher {
public:

    Dispatcher() = default;

    void register_callback(uint8_t id, const CallBackFunction &func) {
        if (pos_in_callback_array < DISPATCHER_MAX_CALLBACKS) {
            pairs_id_callback[pos_in_callback_array++] = {id, func};
        }
    }

    void dispatch(uint8_t id, uint8_t* data, uint16_t length) const {
        for (const PairIDCallback& pair : pairs_id_callback) {
            if (pair.id == id) {
                pair.callback(data, length);
            }
        }
    }


private:
    std::array<PairIDCallback, DISPATCHER_MAX_CALLBACKS> pairs_id_callback;
    int pos_in_callback_array = 0;


};