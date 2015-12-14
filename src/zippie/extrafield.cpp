// copyright 2015 andrean franc
#include <stdint.h>

#include <istream>

#include "zippie/extrafield.h"
#include "zippie/utils.h"


namespace zippie {
namespace records {

std::streamsize ExtraField::read(std::istream* source) {
    std::streamsize bytes_read = 0;
    source->read(reinterpret_cast<char *>(&record_), EF_SIZE);
    bytes_read += source->gcount();
    bytes_read += utils::read_into(source, &data_, record_.data_size);
    return bytes_read;
}


bool ExtraField::header_matches(uint16_t expected) {
    return record_.header_id == expected;
}


uint16_t ExtraField::header_id() {
    return record_.header_id;
}

}  // namespace records
}  // namespace zippie
