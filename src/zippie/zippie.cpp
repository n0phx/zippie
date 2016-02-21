// copyright 2015 andrean franc
#include <stdint.h>

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "zippie/cdfh.h"
#include "zippie/logging.h"
#include "zippie/os.h"
#include "zippie/utils.h"
#include "zippie/zippie.h"
#include "zippie/zmi.h"
#include "zippie/streams/crc32stream.h"

namespace zippie {

ZipFile::ZipFile(const char* filename): z64_eocd_loc_(),
                                        z64_eocd_(),
                                        aedr_(),
                                        is_zip64_(false) {
    fp_.open(filename, std::ifstream::ate | std::ifstream::binary);
    filesize_ = fp_.tellg();
    read_archive();
}


ZipFile::~ZipFile() {
    fp_.close();
}


void ZipFile::close() {
    fp_.close();
}


ZipMemberInfo& ZipFile::getinfo(const std::string& filename) {
    if (!fp_.is_open())
        throw std::runtime_error("Zip file already closed.");

    ZipMemberInfo& zmi = zip_file_infos_.at(filename);
    zmi.load();
    return zmi;
}


std::unique_ptr<std::istream> ZipFile::open(const std::string& filename) {
    if (!fp_.is_open())
        throw std::runtime_error("Zip file already closed.");

    ZipMemberInfo& zmi = getinfo(filename);
    std::istream* sis = new streams::scopedistream(&fp_,
                                                   zmi.file_data_offset(),
                                                   zmi.file_data_size(),
                                                   streams::DEFAULT_BUFFER_SIZE,
                                                   false);
    return std::unique_ptr<std::istream>(new streams::crc32istream(sis,
                                                                   zmi.crc32()));
}


void ZipFile::extract(const std::string& filename, const std::string& path) {
    if (!fp_.is_open())
        throw std::runtime_error("Zip file already closed.");

    std::string dest = os::join(os::abspath(path), filename);
    if (!dest.empty() && dest[dest.length() - 1] == os::pathsep) {
        os::makedirs(dest);
        return;
    }
    // make sure destination folder exists
    std::string parentdir = os::dirname(dest);
    if (!parentdir.empty() && parentdir != dest)
        os::makedirs(parentdir);

    std::unique_ptr<std::istream> sis(open(filename));
    std::ofstream out(dest.c_str(),
                      std::ios::out | std::ios::binary | std::ios::trunc);
    try {
        std::copy(std::istreambuf_iterator<char>(*sis),
                  std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(out));
    } catch (streams::checksum_error& e) {
        throw utils::bad_zip_file(e.what());
    }
}


std::vector<std::string> ZipFile::namelist() {
    if (!fp_.is_open())
        throw std::runtime_error("Zip file already closed.");

    std::vector<std::string> result;
    for (zmi_map::const_iterator it = zip_file_infos_.begin();
            it != zip_file_infos_.end();
            ++it) {
        result.push_back(it->first);
    }
    return result;
}


std::streampos ZipFile::search(const utils::byte_vec& bytes) {
    // search the file from the current position for the passed in bytes
    utils::byte_vec::size_type bytes_size = bytes.size();
    if (bytes_size == 0) {
        throw std::invalid_argument("No bytes to search for.");
    }
    utils::byte_vec buffer;
    unsigned char first_byte = bytes[0];

    std::copy(std::istreambuf_iterator<char>(fp_),
              std::istreambuf_iterator<char>(),
              std::back_inserter(buffer));

    utils::byte_vec::iterator current = buffer.begin();

    while (current != buffer.end()) {
        current = std::find(current, buffer.end(), first_byte);
        if (current + bytes_size < buffer.end()) {
            if (std::equal(bytes.begin(), bytes.end(), current)) {
                return current - buffer.begin();
            }
            ++current;
        }
    }
    return -1;
}


std::streampos ZipFile::search(const unsigned char* bytes, std::size_t length) {
    const utils::byte_vec target(bytes, bytes + length);
    return search(target);
}


void ZipFile::read_archive() {
    std::streampos eocd_offset = read_eocd();
    std::streampos z64_eocd_loc_offset = read_z64_eocd_loc(eocd_offset);
    if (z64_eocd_loc_offset > -1) {
        if (z64_eocd_loc_.disk_count != 1) {
            throw utils::bad_zip_file("Multi-disk zip files are not "
                                      "supported.");
        }
        read_z64_eocd(z64_eocd_loc_.z64_eocd_offset, z64_eocd_loc_offset);
        read_central_dir(z64_eocd_.central_dir_start_offset,
                         z64_eocd_.central_dir_size);
    } else {
        read_central_dir(eocd_.central_dir_start_offset,
                         eocd_.central_dir_size);
    }
}


std::streampos ZipFile::read_eocd() {
    std::streampos search_start = 0;
    std::streamoff search_offset = records::MAX_COMMENT_SIZE +
                                   records::EOCD_SIZE;
    // if filesize_ is smaller than the maximum possible size of a comment
    // section, we cannot guess how big it is actually, so start the search
    // from the beginning of the file, otherwise from the end of file(minus
    // the size of eocd and comment section)
    if (filesize_ > search_offset) {
        search_start = filesize_ - search_offset;
    }
    // locate eocd index, starting search the from ``search_start``
    fp_.seekg(search_start);
    std::streampos eocd_offset = search_start +
                                 search(records::EOCD_SIGN.bytes,
                                        sizeof(records::EOCD_SIGN));
    try {
        utils::read_record(&fp_,
                           eocd_offset,
                           &eocd_,
                           records::EOCD_SIZE,
                           records::EOCD_SIGN);
    } catch (utils::signature_does_not_match& e) {
        throw utils::bad_zip_file("End of central directory header " +
                                  std::string(e.what()));
    } catch (utils::read_error& e) {
        throw utils::bad_zip_file("End of central directory header read "
                                  "error " + std::string(e.what()));
    }
    utils::read_into(&fp_, &eocd_.comment, eocd_.comment_length);
    return eocd_offset;
}


std::streampos ZipFile::read_z64_eocd_loc(std::streampos eocd_offset) {
    std::streampos z64_eocd_loc_offset = eocd_offset -
                                         records::Z64_EOCD_LOC_SIZE;
    try {
        utils::read_record(&fp_,
                           z64_eocd_loc_offset,
                           &z64_eocd_loc_,
                           records::Z64_EOCD_LOC_SIZE,
                           records::Z64_EOCD_LOC_SIGN);
    } catch (utils::signature_does_not_match& e) {
        // archive has no zip64 eocd header
        return -1;
    } catch (utils::read_error& e) {
        throw utils::bad_zip_file("Zip64 end of central directory locator "
                                  "header read error. " +
                                  std::string(e.what()));
    }
    return z64_eocd_loc_offset;
}


void ZipFile::read_z64_eocd(std::streampos z64_eocd_offset,
                            std::streampos z64_eocd_loc_offset) {
    try {
        read_record(&fp_,
                    z64_eocd_offset,
                    &z64_eocd_,
                    records::Z64_EOCD_SIZE,
                    records::Z64_EOCD_SIGN);
    } catch (utils::signature_does_not_match& e) {
        throw utils::bad_zip_file("Zip64 end of central directory header " +
                                  std::string(e.what()));
    } catch (utils::read_error& e) {
        throw utils::bad_zip_file("Zip64 end of central directory header read "
                                  "error " + std::string(e.what()));
    }
    // the amount of bytes remaining between the current position and the start
    // of the zip64 end of central directory locator header is the size of the
    // zip64 extensible data sector
    std::streamsize eds_size = z64_eocd_loc_offset - fp_.tellg();
    utils::read_into(&fp_, &z64_eocd_.extensible_data_sector, eds_size);
    // set internal flag indicating this is a zip64 archive
    is_zip64_ = true;
}


std::streamsize ZipFile::read_extra_central_dir_data() {
    std::streamsize bytes_read = 0;
    // check if the current position already points at the start of a central
    // directory file header. in case it does, there is no extra data to be
    // read, so just return
    if (starts_with(&fp_, records::CDFH_SIGN))
        return bytes_read;
    // compare the signature found at the current position with the signature
    // of the archive extra data record
    if (!starts_with(&fp_, records::AEDR_SIGN))
        // it's not the start of the archive extra data record, so this must
        // be the 12 byte long archive decryption header. its bytes are not
        // counted into the size of the central directory header, so no need
        // to account for the data read here.
        utils::read_into(&fp_, &adh_, records::ADH_SIZE);
    // at this point it either read the archive decryption header and moved 12
    // bytes further or there was no archive decryption header and it's already
    // at the position to read the archive extra data record.
    try {
        bytes_read += utils::read_record(&fp_,
                                         fp_.tellg(),
                                         &aedr_,
                                         records::AEDR_SIZE,
                                         records::AEDR_SIGN);
    } catch (utils::signature_does_not_match& e) {
        // the signature found at the current position does not match the
        // archive extra data record signature, so the archive must be corrupt,
        // because it could not find the central directory file header signature
        // previously either
        throw utils::bad_zip_file("Archive extra data record " +
                                  std::string(e.what()));
    } catch (utils::read_error& e) {
        throw utils::bad_zip_file("Archive extra data record read error. " +
                                  std::string(e.what()));
    }
    bytes_read += utils::read_into(&fp_,
                                   &aedr_.extra_field,
                                   aedr_.extra_field_length);
    // the file position should be exactly at the start of the actual central
    // directory. the size of the archive extra data record counts as part of
    // the size of the central directory header
    return bytes_read;
}


void ZipFile::read_central_dir(std::streampos cd_start,
                               std::streamsize cd_size) {
    // move to alleged start position of central directory
    fp_.seekg(cd_start);
    // attempt to read extra data from the beginning of the central directory
    std::streamsize bytes_read = read_extra_central_dir_data();
    if (bytes_read > 0)
        // TODO: implement support for encrypted archives
        throw utils::bad_zip_file("Encrypted archives are not yet supported");

    // collect all central directory file headers
    cdfh_vec ordered_cdfhs;
    while (bytes_read < cd_size) {
        records::CentralDirFileHeader cdfh;
        bytes_read += cdfh.read(&fp_, cd_start + bytes_read);
        ordered_cdfhs.push_back(cdfh);
    }
    // now that all the local file header offsets are available, the boundaries
    // of local file headers and their data are known
    cdfh_vec::size_type cdfh_count = ordered_cdfhs.size();
    for (cdfh_vec::size_type i = 0; i != cdfh_count; i++) {
        records::CentralDirFileHeader& cdfh = ordered_cdfhs[i];
        std::streampos start = cdfh.get_local_file_header_offset(),
                       end;
        if (i + 1 < cdfh_count)
            end = ordered_cdfhs[i + 1].get_local_file_header_offset();
        else
            end = cd_start;

        zip_file_infos_.insert(std::make_pair(cdfh.get_file_name(),
                                              ZipMemberInfo(&fp_,
                                                            start,
                                                            end - start,
                                                            cdfh)));
    }
}

}  // namespace zippie
