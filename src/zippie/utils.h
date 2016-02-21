// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_UTILS_H_
#define ZIPPIE_ZIPPIE_UTILS_H_
#include <stdint.h>

#include <bitset>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


namespace zippie {
namespace utils {

typedef std::bitset<16> bit_flags;
typedef std::vector<unsigned char> byte_vec;
union signature_type {
    uint32_t value;
    unsigned char bytes[4];
};


class signature_does_not_match: public std::exception {
 private:
    std::string message;
 public:
    explicit signature_does_not_match(const char *msg): message(msg) {}
    ~signature_does_not_match() throw() {}
    virtual const char* what() const throw() {
        return message.c_str();
    }
};


class read_error: public std::exception {
 private:
    std::string message;
 public:
    explicit read_error(const char *msg): message(msg) {}
    ~read_error() throw() {}
    virtual const char* what() const throw() {
        return message.c_str();
    }
};


class bad_zip_file: public std::exception {
 private:
    std::string message;
 public:
    explicit bad_zip_file(const char *msg): message(msg) {}
    explicit bad_zip_file(std::string msg): message(msg) {}
    ~bad_zip_file() throw() {}
    virtual const char* what() const throw() {
        return message.c_str();
    }
};


std::string to_hex(const unsigned char* bytes, std::size_t length);
std::string to_hex(const byte_vec& bytes);
std::string to_hex(const signature_type& sign);
bool signature_matches(const signature_type& found,
                       const signature_type& expected);
bool starts_with(std::istream* source, const signature_type& expected);
std::streamsize read_into(std::istream* source, byte_vec* into,
                          std::streamsize length);
std::streamsize read_into(std::istream* source, std::string* into,
                          std::streamsize length);


template <class TRecord>
std::streamsize read_record(std::istream* source,
                            std::streampos index,
                            TRecord* rec,
                            std::streamsize rec_size,
                            const signature_type& expected) {
    source->seekg(index);
    source->read(reinterpret_cast<char *>(rec), rec_size);
    if (!signature_matches(rec->signature, expected)) {
        std::string msg = "signature " +
                          to_hex(rec->signature) +
                          "does not match " +
                          to_hex(expected);
        throw signature_does_not_match(msg.c_str());
    }
    std::streamsize bytes_read = source->gcount();
    if (bytes_read != rec_size) {
        std::stringstream msg;
        msg << "Read " << bytes_read << ", expected " << rec_size << " bytes";
        throw read_error(msg.str().c_str());
    }
    return bytes_read;
}

}  // namespace utils
}  // namespace zippie
#endif  // ZIPPIE_ZIPPIE_UTILS_H_
