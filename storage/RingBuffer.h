#pragma once

#include <cstddef>

template <typename T, size_t Capacity>
class RingBuffer {
public:
    RingBuffer() : idx_write(0), idx_read(0), full(false) {}

    // Single element write
    bool push(const T& item) {
        if (isFull()) return false;
        buffer[idx_write] = item;
        advance_write();
        return true;
    }

    // Single element read
    bool pop(T& item) {
        if (empty()) return false;
        item = buffer[idx_read];
        advance_read();
        return true;
    }

    // Batch write (returns number of items written)
    size_t write(const T* data, size_t count) {
        size_t written = 0;
        while (written < count && !isFull()) {
            buffer[idx_write] = data[written++];
            advance_write();
        }
        return written;
    }

    // Batch read (returns number of items read)
    size_t read(T* data, size_t count) {
        size_t readCount = 0;
        while (readCount < count && !empty()) {
            data[readCount++] = buffer[idx_read];
            advance_read();
        }
        return readCount;
    }

    bool empty() const {
        return (!full && (idx_write == idx_read));
    }

    bool isFull() const {
        return full;
    }

    size_t capacity() const {
        return Capacity;
    }

    size_t size() const {
        if (full) return Capacity;
        if (idx_write >= idx_read) return idx_write - idx_read;
        return Capacity + idx_write - idx_read;
    }

private:
    T buffer[Capacity];
    size_t idx_write;
    size_t idx_read;
    bool full;

    void advance_write() {
        idx_write = (idx_write + 1) % Capacity;
        full = (idx_write == idx_read);
    }

    void advance_read() {
        full = false;
        idx_read = (idx_read + 1) % Capacity;
    }
};