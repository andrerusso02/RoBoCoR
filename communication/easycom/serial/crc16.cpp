#include "crc16.h"

#define INITIAL_VALUE 0xFFFF
#define POLYNOMIAL 0xA001

uint16_t crc16(const uint8_t* data, size_t length) {
     uint16_t crc = INITIAL_VALUE;
     uint16_t polynomial = 0xA001;

     for (size_t i = 0; i < length; i++) {
         crc ^= data[i];
         for (int j = 0; j < 8; j++) {
             if (crc & 0x0001) {
                 crc = (crc >> 1) ^ polynomial;
             } else {
                 crc >>= 1;
             }
         }
     }
     return crc;
 }
