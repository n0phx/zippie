// copyright 2015 andrean franc
#include <algorithm>
#include <ios>
#include <stdexcept>
#include <string>

#include "zippie/datadescriptor.h"
#include "zippie/lfh.h"
#include "zippie/utils.h"
#include "zippie/zmi.h"


namespace zippie {

ZipMemberInfo::ZipMemberInfo(std::istream* source,
                             std::streampos member_start,
                             std::streamsize member_size,
                             records::CentralDirFileHeader cdfh):
                                        member_start_(member_start),
                                        member_size_(member_size),
                                        source_(source,
                                                member_start_,
                                                member_size_,
                                                streams::DEFAULT_BUFFER_SIZE,
                                                false),
                                        cdfh_(cdfh),
                                        loaded_(false) {}


void ZipMemberInfo::load() {
    if (!loaded_) {
        // read local file header
        std::streamsize bytes_read = lfh_.read(&source_);
        // if file is encrypted, read the 12 byte encryption header
        if (lfh_.is_encrypted()) {
            bytes_read += utils::read_into(&source_, &eh_, EH_SIZE);
            // TODO: implement support for encrypted archives
            throw utils::bad_zip_file("Encrypted archives are not yet "
                                      "supported");
        }
        // after reading the local file header(and optionally the encryption
        // header), the current position should point to the start of the file
        // data itself, but because scopedstreams return their relative position
        // it must be added to the known absolute start position of the member
        file_data_offset_ = member_start_ + bytes_read;
        file_data_size_ = lfh_.compressed_size();
        crc32_ = lfh_.crc32();
        if (lfh_.has_data_descriptor()) {
            records::DataDescriptor dd;
            dd.read(&source_);
            file_data_size_ = dd.compressed_size();
            crc32_ = dd.crc32();
        }
        // subsequent opens will not have to re-read the local file header
        loaded_ = true;
    }
}


std::string ZipMemberInfo::filename() {
    return lfh_.filename();
}


std::streampos ZipMemberInfo::file_data_offset() {
    return file_data_offset_;
}


std::streamsize ZipMemberInfo::file_data_size() {
    return file_data_size_;
}


uint32_t ZipMemberInfo::crc32() {
    return crc32_;
}

}  // namespace zippie
