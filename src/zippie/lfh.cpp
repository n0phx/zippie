// copyright 2015 andrean franc
#include <stdint.h>

#include <ios>
#include <string>

#include "zippie/extrafield.h"
#include "zippie/lfh.h"
#include "zippie/utils.h"
#include "zippie/streams/scopedstream.h"


namespace zippie {
namespace records {

std::streamsize LocalFileHeader::read(streams::scopedistream* source,
                                      std::streampos offset) {
    LocalFileHeaderRecord record;
    std::streamsize bytes_read = 0;
    // since scoped file pointer is used, boundaries are already set so we just
    // seek to 0, which will always be the start of the local file header
    try {
        bytes_read += utils::read_record(source,
                                         offset,
                                         &record,
                                         LFH_SIZE,
                                         LFH_SIGN);
    } catch (utils::signature_does_not_match& e) {
        throw utils::bad_zip_file("Local file header " + std::string(e.what()));
    } catch (utils::read_error& e) {
        throw utils::bad_zip_file("Local file header read error. " +
                                  std::string(e.what()));
    }
    // copy data that needs to be publicly accessible from the private record
    version_needed_to_extract_ = record.version_needed_to_extract;
    compression_method_ = record.compression_method;
    last_mod_file_time_ = record.last_mod_file_time;
    last_mod_file_date_ = record.last_mod_file_date;
    crc32_ = record.crc32;
    compressed_size_ = record.compressed_size;
    uncompressed_size_ = record.uncompressed_size;
    // initialize the ``flags_`` bitset with the freshly read raw data
    general_purpose_bit_flag_ = utils::bit_flags(
                                            record.general_purpose_bit_flag);
    // read fields with with variable sizes
    bytes_read += utils::read_into(source,
                                   &file_name_,
                                   record.file_name_length);
    bytes_read += read_extra_field(source, record.extra_field_length);
    return bytes_read;
}


std::streamsize LocalFileHeader::read_extra_field(streams::scopedistream* src,
                                                  std::streamsize length) {
    std::streamsize bytes_read = 0;
    while (bytes_read < length) {
        ExtraField ef;
        bytes_read += ef.read(src);
        if (ef.header_matches(Z64_EIEFHI)) {
            if (uncompressed_size_ == 0xFFFFFFFF)
                ef.cast_into(&uncompressed_size_);

            if (compressed_size_ == 0xFFFFFFFF)
                ef.cast_into(&compressed_size_);
        }
        extra_fields_.insert(std::make_pair(ef.header_id(), ef));
    }
    return bytes_read;
}


bool LocalFileHeader::is_encrypted() {
    return general_purpose_bit_flag_.test(ENCRYPTION_BIT);
}


bool LocalFileHeader::has_data_descriptor() {
    return general_purpose_bit_flag_.test(DATA_DESCRIPTOR_BIT);
}


std::string LocalFileHeader::filename() {
    return file_name_;
}


uint64_t LocalFileHeader::compressed_size() {
    return compressed_size_;
}


uint32_t LocalFileHeader::crc32() {
    return crc32_;
}

}  // namespace records
}  // namespace zippie
