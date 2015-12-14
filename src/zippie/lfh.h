// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_LFH_H_
#define ZIPPIE_ZIPPIE_LFH_H_
#include <stdint.h>

#include <ios>
#include <map>
#include <string>

#include "zippie/extrafield.h"
#include "zippie/scopedstream.h"
#include "zippie/utils.h"


static const std::streamsize LFH_SIZE = 30;
static const signature_type LFH_SIGN = {0x04034b50};
static const int ENCRYPTION_BIT = 1;
static const int DATA_DESCRIPTOR_BIT = 3;


#pragma pack(push, 1)  // set padding to 1 byte, saves previous value
struct LocalFileHeaderRecord {
    /*
    local file header signature (0x04034b50)                        4 bytes
    version needed to extract                                       2 bytes
    general purpose bit flag                                        2 bytes
    compression method                                              2 bytes
    last mod file time                                              2 bytes
    last mod file date                                              2 bytes
    crc-32                                                          4 bytes
    compressed size                                                 4 bytes
    uncompressed size                                               4 bytes
    file name length                                                2 bytes
    extra field length                                              2 bytes
    file name (variable size)
    extra field (variable size)
    */
    signature_type signature;
    uint16_t version_needed_to_extract;
    uint16_t general_purpose_bit_flag;
    uint16_t compression_method;
    uint16_t last_mod_file_time;
    uint16_t last_mod_file_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t file_name_length;
    uint16_t extra_field_length;
};
#pragma pack(pop)  // restore previous pack value


class LocalFileHeader {
 private:
    uint16_t version_needed_to_extract_;
    uint16_t compression_method_;
    uint16_t last_mod_file_time_;
    uint16_t last_mod_file_date_;
    uint32_t crc32_;
    uint64_t compressed_size_;
    uint64_t uncompressed_size_;
    std::string file_name_;
    bit_flags general_purpose_bit_flag_;
    std::map<uint16_t, ExtraField> extra_fields_;

    std::streamsize read_extra_field(scopedistream* source,
                                     std::streamsize length);
 public:
    std::streamsize read(scopedistream* source);
    bool is_encrypted();
    bool has_data_descriptor();
    std::string filename();
    uint64_t compressed_size();
    uint32_t crc32();
};
#endif  // ZIPPIE_ZIPPIE_LFH_H_
