// copyright 2015 andrean franc
#include <iomanip>
#include <ios>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "zippie/logging.h"
#include "zippie/utils.h"


namespace zippie {
namespace utils {

std::string to_hex(const unsigned char* bytes, std::size_t length) {
    std::stringstream msg;
    for (std::size_t i = 0; i < length; i++) {
        msg << std::hex
            << std::setfill('0')
            << std::setw(2)
            << static_cast<int>(bytes[i]) << " ";
    }
    return msg.str();
}


std::string to_hex(const byte_vec& bytes) {
    return to_hex(&bytes[0], bytes.size());
}


std::string to_hex(const signature_type& sign) {
    return to_hex(sign.bytes, sizeof(sign.value));
}


bool signature_matches(const signature_type& found,
                       const signature_type& expected) {
    logging::debug("Signature expected: " + to_hex(expected) +
                   "found: " + to_hex(found));
    return found.value == expected.value;
}


bool starts_with(std::istream* source, const signature_type& expected) {
    // returns whether the currect position starts with the passed in signature
    std::streampos current = source->tellg();  // backup current position
    signature_type found;
    source->read(reinterpret_cast<char *>(&found.value), sizeof(expected));
    bool matches = signature_matches(found, expected);
    source->seekg(current);  // restore backed up position
    return matches;
}


std::streamsize read_into(std::istream* source, byte_vec* into,
                          std::streamsize length) {
    if (length > 0) {
        into->resize(length);
        source->read(reinterpret_cast<char *>(&(*into)[0]), length);
        return source->gcount();
    }
    return 0;
}


std::streamsize read_into(std::istream* source, std::string* into,
                          std::streamsize length) {
    byte_vec into_vec;
    std::streamsize bytes_read = read_into(source, &into_vec, length);
    if (bytes_read > 0)
        into->assign(into_vec.begin(), into_vec.end());
    return bytes_read;
}

}  // namespace utils
}  // namespace zippie
