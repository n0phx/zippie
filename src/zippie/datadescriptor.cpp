// copyright 2015 andrean franc
#include <stdint.h>

#include <ios>
#include <istream>

#include "zippie/datadescriptor.h"
#include "zippie/utils.h"


namespace zippie {
namespace records {

std::streamsize DataDescriptor::read(std::istream* source) {
    // seek backwards the expected size of the data descriptor from the end of
    // the stream
    source->seekg(DD_SIZE, std::ios_base::end);
    // skip optional signature
    source->seekg(sizeof(DD_SIGN), std::ios_base::cur);
    std::streamsize bytes_read = 0;
    source->read(reinterpret_cast<char *>(&record_), DD_SIZE - sizeof(DD_SIGN));
    bytes_read += source->gcount();
    return bytes_read;
}


uint32_t DataDescriptor::crc32() {
    return record_.crc32;
}


uint32_t DataDescriptor::compressed_size() {
    return record_.compressed_size;
}


uint32_t DataDescriptor::uncompressed_size() {
    return record_.uncompressed_size;
}

}  // namespace records
}  // namespace zippie
