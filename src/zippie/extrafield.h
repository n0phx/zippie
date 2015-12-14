// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_EXTRA_FIELD_H_
#define ZIPPIE_ZIPPIE_EXTRA_FIELD_H_
#include <stdint.h>

#include <ios>
#include <istream>
#include <iostream>

#include "zippie/utils.h"


namespace zippie {
namespace records {

static const std::streamsize EF_SIZE = 4;
// zip64 extended information extra field header id
static const uint16_t Z64_EIEFHI = 0x0001;


#pragma pack(push, 1)  // set padding to 1 byte, saves previous value
struct ExtraFieldRecord {
    uint16_t header_id;
    uint16_t data_size;
};
#pragma pack(pop)  // restore previous pack value


class ExtraField {
 private:
    ExtraFieldRecord record_;
    utils::byte_vec data_;
    utils::byte_vec::size_type pos_;

 public:
    ExtraField(): pos_(0) {}
    std::streamsize read(std::istream* source);
    bool header_matches(uint16_t expected);
    uint16_t header_id();

    template<class TTarget>
    void cast_into(TTarget* into) {
        *into = *reinterpret_cast<const TTarget*>(&data_[pos_]);
        pos_ += sizeof(*into);
    }
};

}  // namespace records
}  // namespace zippie

#endif  // ZIPPIE_ZIPPIE_EXTRA_FIELD_H_
