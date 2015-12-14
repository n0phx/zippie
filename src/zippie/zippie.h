// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_ZIPPIE_H_
#define ZIPPIE_ZIPPIE_ZIPPIE_H_
#include <stdint.h>

#include <fstream>
#include <ios>
#include <map>
#include <string>
#include <vector>

#include "zippie/cdfh.h"
#include "zippie/scopedstream.h"
#include "zippie/utils.h"
#include "zippie/zmi.h"


namespace zippie {

typedef std::vector<CentralDirFileHeader> cdfh_vec;
typedef std::map<std::string, ZipMemberInfo> zmi_map;

static const std::streamsize MAX_COMMENT_SIZE = 65535;
// end of central directory
static const std::streamsize EOCD_SIZE = 22;
static const signature_type EOCD_SIGN = {0x06054b50};
// zip64 end of central directory locator
static const std::streamsize Z64_EOCD_LOC_SIZE = 20;
static const signature_type Z64_EOCD_LOC_SIGN = {0x07064b50};
// zip64 end of central directory
static const std::streamsize Z64_EOCD_SIZE = 56;
static const signature_type Z64_EOCD_SIGN = {0x06064b50};
// archive decryption header
static const std::streamsize ADH_SIZE = 12;
// archive extra data record
static const std::streamsize AEDR_SIZE = 8;
static const signature_type AEDR_SIGN = {0x08064b50};


// Byte alignment must be defined for all structs, otherwise input data cannot
// be read reliably
#pragma pack(push, 1)  // set padding to 1 byte, saves previous value
struct EOCD {
    /*
    end of central dir signature (0x06054b50)                       4 bytes
    number of this disk                                             2 bytes
    number of the disk with the start of the central directory      2 bytes
    total number of entries in the central directory on this disk   2 bytes
    total number of entries in the central directory                2 bytes
    size of the central directory                                   4 bytes
    offset of start of cent.dir.with respect to the start disk num. 4 bytes
    .ZIP file comment length                                        2 bytes
    .ZIP file comment       (variable size)
    */
    signature_type signature;
    uint16_t disk_number;
    uint16_t disk_start;
    uint16_t entries_this_disk;
    uint16_t entries_total;
    uint32_t central_dir_size;
    uint32_t central_dir_start_offset;
    uint16_t comment_length;
    std::string comment;
};


struct Z64EOCDLoc {
    /*
    zip64 end of central dir locator signature (0x07064b50)         4 bytes
    number of the disk with the start of the zip64 end of cent.dir. 4 bytes
    relative offset of the zip64 end of central directory record    8 bytes
    total number of disks                                           4 bytes
    */
    signature_type signature;
    uint32_t z64_eocd_disk_number;
    uint64_t z64_eocd_offset;
    uint32_t disk_count;
};


struct Z64EOCD {
    /*
    zip64 end of central dir signature (0x06064b50)                 4 bytes
    size of zip64 end of central directory record                   8 bytes
    version made by                                                 2 bytes
    version needed to extract                                       2 bytes
    number of this disk                                             4 bytes
    number of the disk with the start of the central directory      4 bytes
    total number of entries in the central directory on this disk   8 bytes
    total number of entries in the central directory                8 bytes
    size of the central directory                                   8 bytes
    offset of start of cent.dir.with respect to the start disk num. 8 bytes
    zip64 extensible data sector    (variable size)
    */
    signature_type signature;
    uint64_t record_size;
    uint16_t version_made_by;
    uint16_t version_needed_to_extract;
    uint32_t disk_number;
    uint32_t disk_start;
    uint64_t entries_this_disk;
    uint64_t entries_total;
    uint64_t central_dir_size;
    uint64_t central_dir_start_offset;
    byte_vec extensible_data_sector;
};


struct AEDRecord {
    /*
    archive extra data signature    4 bytes  (0x08064b50)
    extra field length              4 bytes
    extra field data                (variable size)
    */
    signature_type signature;
    uint32_t extra_field_length;
    byte_vec extra_field;
};
#pragma pack(pop)  // restore previous pack value


class ZipFile {
 private:
    std::ifstream fp_;
    std::streamsize filesize_;
    EOCD eocd_;
    Z64EOCDLoc z64_eocd_loc_;
    Z64EOCD z64_eocd_;
    AEDRecord aedr_;
    byte_vec adh_;
    zmi_map zip_file_infos_;
    bool is_zip64_;

    std::streampos search(const byte_vec& bytes);
    std::streampos search(const unsigned char* bytes, std::size_t length);
    std::streampos read_eocd();
    std::streampos read_z64_eocd_loc(std::streampos eocd_offset);
    void read_z64_eocd(std::streampos z64_eocd_offset,
                       std::streampos z64_eocd_loc_offset);
    std::streamsize read_extra_central_dir_data();
    void read_central_dir(std::streampos cd_start, std::streamsize cd_size);
    void read_archive();

 public:
    explicit ZipFile(const char* filename);
    ZipMemberInfo& getinfo(const std::string& filename);
    scopedistream open(const std::string& filename);
    void extract(const std::string& filename, const std::string& path);
    std::vector<std::string> namelist();
    void close();
};

}  // namespace zippie
#endif  // ZIPPIE_ZIPPIE_ZIPPIE_H_
