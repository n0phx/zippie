// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_CDFH_H_
#define ZIPPIE_ZIPPIE_CDFH_H_
#include <stdint.h>

#include <ios>
#include <fstream>
#include <map>
#include <string>

#include "zippie/extrafield.h"
#include "zippie/utils.h"


namespace zippie {
namespace records {

static const std::streamsize CDFH_SIZE = 46;
static const utils::signature_type CDFH_SIGN = {0x02014b50};


#pragma pack(push, 1)  // set padding to 1 byte, saves previous value
struct CentralDirFileHeaderRecord {
    /*
    central file header signature (0x02014b50)                      4 bytes
    version made by                                                 2 bytes
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
    file comment length                                             2 bytes
    disk number start                                               2 bytes
    internal file attributes                                        2 bytes
    external file attributes                                        4 bytes
    relative offset of local header                                 4 bytes
    file name (variable size)
    extra field (variable size)
    file comment (variable size)
    */
    utils::signature_type signature;
    uint16_t version_made_by;
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
    uint16_t file_comment_length;
    uint16_t disk_number_start;
    uint16_t internal_file_attributes;
    uint32_t external_file_attributes;
    uint32_t relative_offset_of_local_header;
};
#pragma pack(pop)  // restore previous pack value


class CentralDirFileHeader {
 private:
    uint16_t version_made_by_;
    uint16_t version_needed_to_extract_;
    uint16_t compression_method_;
    uint16_t last_mod_file_time_;
    uint16_t last_mod_file_date_;
    uint32_t crc32_;
    uint64_t compressed_size_;
    uint64_t uncompressed_size_;
    uint32_t disk_number_start_;
    uint16_t internal_file_attributes_;
    uint32_t external_file_attributes_;
    uint64_t relative_offset_of_local_header_;
    std::string file_name_;
    std::string file_comment_;
    utils::bit_flags general_purpose_bit_flag_;
    std::map<uint16_t, ExtraField> extra_fields_;

    std::streamsize read_extra_field(std::ifstream* source,
                                     std::streamsize length);

 public:
    std::streamsize read(std::ifstream* source, std::streampos offset);
    std::streampos get_local_file_header_offset();
    std::string get_file_name();
};

}  // namespace records
}  // namespace zippie
#endif  // ZIPPIE_ZIPPIE_CDFH_H_
