// copyright 2015 andrean franc
#include <stdint.h>

#include <fstream>
#include <ios>
#include <string>

#include "zippie/cdfh.h"
#include "zippie/extrafield.h"
#include "zippie/utils.h"


namespace zippie {
namespace records {

std::streamsize CentralDirFileHeader::read(std::ifstream* source,
                                           std::streampos offset) {
    CentralDirFileHeaderRecord record;
    std::streamsize bytes_read = 0;
    try {
        bytes_read += read_record(source,
                                  offset,
                                  &record,
                                  CDFH_SIZE,
                                  CDFH_SIGN);
    } catch (utils::signature_does_not_match& e) {
        throw utils::bad_zip_file("Central directory file header " +
                                  std::string(e.what()));
    } catch (utils::read_error& e) {
        throw utils::bad_zip_file("Central directory file header read error. " +
                                  std::string(e.what()));
    }
    // copy data that needs to be publicly accessible from the private record
    version_made_by_ = record.version_made_by;
    version_needed_to_extract_ = record.version_needed_to_extract;
    compression_method_ = record.compression_method;
    last_mod_file_time_ = record.last_mod_file_time;
    last_mod_file_date_ = record.last_mod_file_date;
    crc32_ = record.crc32;
    compressed_size_ = record.compressed_size;
    uncompressed_size_ = record.uncompressed_size;
    disk_number_start_ = record.disk_number_start;
    internal_file_attributes_ = record.internal_file_attributes;
    external_file_attributes_ = record.external_file_attributes;
    relative_offset_of_local_header_ = record.relative_offset_of_local_header;
    // initialize the ``flags_`` bitset with the freshly read raw data
    general_purpose_bit_flag_ = utils::bit_flags(
                                            record.general_purpose_bit_flag);
    // read fields with with variable sizes
    bytes_read += utils::read_into(source,
                                   &file_name_,
                                   record.file_name_length);
    bytes_read += read_extra_field(source, record.extra_field_length);
    bytes_read += utils::read_into(source,
                                   &file_comment_,
                                   record.file_comment_length);
    return bytes_read;
}


std::streamsize CentralDirFileHeader::read_extra_field(std::ifstream* source,
                                                       std::streamsize length) {
    std::streamsize bytes_read = 0;
    while (bytes_read < length) {
        ExtraField ef;
        bytes_read += ef.read(source);
        if (ef.header_matches(Z64_EIEFHI)) {
            if (uncompressed_size_ == 0xFFFFFFFF)
                ef.cast_into(&uncompressed_size_);

            if (compressed_size_ == 0xFFFFFFFF)
                ef.cast_into(&compressed_size_);

            if (relative_offset_of_local_header_ == 0xFFFFFFFF)
                ef.cast_into(&relative_offset_of_local_header_);

            if (disk_number_start_ == 0xFFFF)
                ef.cast_into(&disk_number_start_);
        }
        extra_fields_.insert(std::make_pair(ef.header_id(), ef));
    }
    return bytes_read;
}


std::streampos CentralDirFileHeader::get_local_file_header_offset() {
    return relative_offset_of_local_header_;
}


std::string CentralDirFileHeader::get_file_name() {
    return file_name_;
}

}  // namespace records
}  // namespace zippie
