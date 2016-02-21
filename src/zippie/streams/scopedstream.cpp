// copyright 2015 andrean franc
#include <algorithm>
#include <ios>
#include <istream>

#include "zippie/streams/scopedstream.h"


namespace zippie {
namespace streams {

scopedstreambuf::scopedstreambuf(const scopedstreambuf& other):
                                                source_(other.source_),
                                                start_(other.start_),
                                                size_(other.size_),
                                                end_(other.end_),
                                                honor_eof_(other.honor_eof_),
                                                pos_(other.pos_),
                                                buffer_(other.buffer_.size()) {
    buffer_front_ = reinterpret_cast<char *>(&buffer_.front());
}


scopedstreambuf::scopedstreambuf(std::istream* source,
                                 std::streampos scope_start,
                                 std::streamsize scope_size,
                                 std::size_t buffer_size,
                                 bool honor_eof):
                                                source_(source),
                                                start_(scope_start),
                                                size_(scope_size),
                                                end_(scope_start + scope_size),
                                                honor_eof_(honor_eof),
                                                pos_(0),
                                                buffer_(buffer_size) {
    buffer_front_ = reinterpret_cast<char *>(&buffer_.front());
}


std::streampos scopedstreambuf::seekoff(std::streamoff offset,
                                        std::ios_base::seekdir direction,
                                        std::ios_base::openmode mode) {
    if (direction == std::ios_base::beg) {
        // move to beginning of scope, emulating the beginning of stream
        source_->seekg(start_);
    } else if (direction == std::ios_base::end) {
        // move to end of scope, emulating the end of stream
        source_->seekg(end_);
    } else {
        // as multiple scopedstreams may use the same base file stream, make
        // sure the current position is the one that it expects it to be
        source_->seekg(abspos());
    }
    // direction is always current, and we emulate std::ios_base::beg and
    // std::ios_base::end behaviors
    source_->seekg(offset, std::ios_base::cur);
    std::streampos new_pos = source_->tellg();
    if (new_pos < start_ || new_pos > end_) {
        // out of boundaries, revert to previous position and report failure
        source_->seekg(abspos());
        return pos_type(off_type(-1));
    }
    // recalculate relative position in scopedstream
    pos_ = new_pos - start_;
    // moves intermediary buffer pointer to the end putting it into exhausted
    // state so next read operation will refill it with valid data
    setg(eback(), egptr(), egptr());
    return pos_type(pos_);
}


std::streampos scopedstreambuf::seekpos(std::streampos position,
                                        std::ios_base::openmode mode) {
    return seekoff(off_type(position), std::ios_base::beg, mode);
}


std::streambuf::int_type scopedstreambuf::underflow() {
    if (!(gptr() < egptr())) {
        // intermediary buffer exhausted, attempt to read next chunk of data
        // into it
        std::streamsize buffer_size = buffer_.size();
        std::streamsize left = static_cast<std::streamsize>(end_ - abspos());
        std::streamsize bytes_available = std::min(buffer_size, left);
        // because scopedbufs may be created by wrapping other subscopedbufs,
        // or multiple scopedbufs may wrap the same file stream, an eof should
        // have no meaning to it, clear the flag and seek to desired position,
        // unless it was specified to honor eofs of the source stream
        if (!honor_eof_ && source_->eof())
            source_->clear();
        source_->seekg(abspos());
        source_->read(buffer_front_, bytes_available);
        std::streamsize bytes_read = source_->gcount();
        if (bytes_read == 0)
            return traits_type::eof();

        pos_ += bytes_read;
        setg(buffer_front_, buffer_front_, buffer_front_ + bytes_read);
    }
    return traits_type::to_int_type(*gptr());
}


std::streamsize scopedstreambuf::xsgetn(char* s, std::streamsize n) {
    std::streamsize bytes_read = std::streambuf::xsgetn(s, n);
    // update relative position within the stream after advancing gptr
    pos_ = gptr() - buffer_front_;
    return bytes_read;
}


std::streampos scopedstreambuf::abspos() const {
    return start_ + pos_;
}

}  // namespace streams
}  // namespace zippie
