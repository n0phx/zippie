// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_STREAMS_CRC32STREAM_H_
#define ZIPPIE_ZIPPIE_STREAMS_CRC32STREAM_H_
#include <stdint.h>

#include <iomanip>
#include <ios>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "zippie/streams/crc32.h"


namespace zippie {
namespace streams {

class checksum_error: public std::runtime_error {
 private:
    uint32_t expected_;
    uint32_t got_;
    std::string message_;

 public:
    explicit checksum_error(uint32_t expected, uint32_t got):
                                                std::runtime_error("mismatch"),
                                                expected_(expected),
                                                got_(got) {
        std::stringstream msg;
        msg << std::hex
            << std::setfill('0')
            << std::setw(8)
            << "CRC32 checksum verification failed. Expected: "
            << expected_
            << " got: "
            << got_;
        message_ = msg.str();
    }
    virtual const char* what() const throw() {
        return message_.c_str();
    }
};

class crc32streambuf: public std::streambuf {
 private:
    std::shared_ptr<std::istream> source_;
    std::vector<unsigned char> buffer_;
    char *buffer_front_;
    std::streampos pos_;
    CRC32 crc32_;
    uint32_t expected_;

    int_type underflow();

 public:
    crc32streambuf(const crc32streambuf& other);
    explicit crc32streambuf(std::istream* source,
                            uint32_t expected,
                            std::size_t buffer_size = 256);
    uint32_t crc32() const;
};


struct crc32istream_base {
    crc32streambuf sbuf_;
    explicit crc32istream_base(const crc32streambuf& sbuf): sbuf_(sbuf) {}
    explicit crc32istream_base(std::istream* source,
                               uint32_t expected,
                               std::size_t buffer_size): sbuf_(source,
                                                               expected,
                                                               buffer_size) {}
};
class crc32istream: virtual crc32istream_base, public std::istream {
 public:
    crc32istream(const crc32istream& other): crc32istream_base(other.sbuf_),
                                             std::ios(&this->sbuf_),
                                             std::istream(&this->sbuf_) {}
    explicit crc32istream(std::istream* source,
                          uint32_t expected,
                          std::size_t buffer_size = 256):
                                                crc32istream_base(source,
                                                                  expected,
                                                                  buffer_size),
                                                std::ios(&this->sbuf_),
                                                std::istream(&this->sbuf_) {
        exceptions(std::ios::badbit);
    }
    uint32_t crc32() const {
        return this->crc32();
    }
};

}  // namespace streams
}  // namespace zippie
#endif  // ZIPPIE_ZIPPIE_STREAMS_CRC32STREAM_H_
