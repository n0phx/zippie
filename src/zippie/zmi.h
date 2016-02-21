// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_ZMI_H_
#define ZIPPIE_ZIPPIE_ZMI_H_
#include <stdint.h>

#include <ios>
#include <string>

#include "zippie/cdfh.h"
#include "zippie/lfh.h"
#include "zippie/utils.h"
#include "zippie/streams/scopedstream.h"


namespace zippie {
// encryption header size
static const std::streamsize EH_SIZE = 12;


class ZipMemberInfo {
 private:
    std::streampos member_start_;
    std::streampos member_size_;
    streams::scopedistream source_;
    records::CentralDirFileHeader cdfh_;
    records::LocalFileHeader lfh_;
    utils::byte_vec eh_;
    bool loaded_;
    std::streampos file_data_offset_;
    std::streamsize file_data_size_;
    uint32_t crc32_;

 public:
    explicit ZipMemberInfo(std::istream* source,
                           std::streampos start,
                           std::streamsize size,
                           records::CentralDirFileHeader cdfh);
    void load();
    std::string filename();
    std::streampos file_data_offset();
    std::streamsize file_data_size();
    uint32_t crc32();
};

}  // namespace zippie
#endif  // ZIPPIE_ZIPPIE_ZMI_H_
