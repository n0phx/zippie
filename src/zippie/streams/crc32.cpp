// copyright andrean franc
#include <stdint.h>

#include <string>
#include <vector>

#include "zippie/streams/crc32.h"


namespace zippie {

CRC32::CRC32(uint32_t initial): current_(initial) {}

uint32_t CRC32::calc(std::vector<uint8_t>::const_iterator start,
                     std::vector<uint8_t>::const_iterator end) {
    uint32_t result = INIT_XOR ^ current_;
    for (auto it = start; it != end; ++it) {
        result ^= (*it);
        uint8_t index = result & 0xFF;
        result = (result >> 8) ^ CRC32_TABLE[index];
    }
    current_ = result ^ FINAL_XOR;
    return current_;
}

uint32_t CRC32::calc(const std::vector<uint8_t>& data) {
    return calc(data.begin(), data.end());
}

uint32_t CRC32::calc(const std::string& data) {
    std::vector<uint8_t> vec_data(data.begin(), data.end());
    return calc(vec_data);
}

uint32_t CRC32::checksum() const {
    return current_;
}

}  // namespace zippie
