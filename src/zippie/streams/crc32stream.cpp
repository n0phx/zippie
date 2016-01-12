// copyright 2015 andrean franc
#include <stdint.h>

#include <algorithm>
#include <ios>
#include <istream>
#include <iostream>

#include "zippie/streams/crc32stream.h"


namespace zippie {
namespace streams {

crc32streambuf::crc32streambuf(const crc32streambuf& other):
                                                source_(other.source_),
                                                buffer_(other.buffer_.size()),
                                                pos_(other.pos_),
                                                crc32_(other.crc32()),
                                                expected_(other.expected_) {
    buffer_front_ = reinterpret_cast<char *>(&buffer_.front());
}


crc32streambuf::crc32streambuf(std::istream* source,
                               uint32_t expected,
                               std::size_t buffer_size): source_(source),
                                                         buffer_(buffer_size),
                                                         pos_(0),
                                                         crc32_(),
                                                         expected_(expected) {
    buffer_front_ = reinterpret_cast<char *>(&buffer_.front());
}


std::streambuf::int_type crc32streambuf::underflow() {
    if (!(gptr() < egptr())) {
        // intermediary buffer exhausted, attempt to read next chunk of data
        // into it
        source_->seekg(pos_);
        source_->read(buffer_front_, buffer_.size());
        std::streamsize bytes_read = source_->gcount();
        if (bytes_read == 0)
            return traits_type::eof();

        pos_ += bytes_read;
        // update checksum with freshly read data
        crc32_.calc(buffer_.begin(), buffer_.begin() + bytes_read);
        if (source_->eof() && crc32_.checksum() != expected_)
            throw checksum_error(expected_, crc32_.checksum());

        setg(buffer_front_, buffer_front_, buffer_front_ + bytes_read);
    }
    return traits_type::to_int_type(*gptr());
}

uint32_t crc32streambuf::crc32() const {
    return crc32_.checksum();
}

}  // namespace streams
}  // namespace zippie

