// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_DATA_DESCRIPTOR_H_
#define ZIPPIE_ZIPPIE_DATA_DESCRIPTOR_H_
#include <stdint.h>

#include <ios>
#include <istream>

#include "zippie/utils.h"


namespace zippie {
namespace records {

// data descriptor record size includes the size of the optional signature
static const std::streamsize DD_SIZE = 16;
static const utils::signature_type DD_SIGN = {0x08074b50};


#pragma pack(push, 1)  // set padding to 1 byte, saves previous value
struct DataDescriptorRecord {
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
};
#pragma pack(pop)  // restore previous pack value


class DataDescriptor {
 private:
    DataDescriptorRecord record_;

 public:
    std::streamsize read(std::istream* source);
    uint32_t crc32();
    uint32_t compressed_size();
    uint32_t uncompressed_size();
};

}  // namespace records
}  // namespace zippie
#endif  // ZIPPIE_ZIPPIE_DATA_DESCRIPTOR_H_
