// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_SCOPEDSTREAM_H_
#define ZIPPIE_ZIPPIE_SCOPEDSTREAM_H_
#include <ios>
#include <istream>
#include <vector>


namespace zippie {
namespace streams {

static const std::size_t DEFAULT_BUFFER_SIZE = 256;


class scopedstreambuf: public std::streambuf {
 private:
    std::istream* source_;
    std::streampos start_;
    std::streamsize size_;
    std::streampos end_;
    bool honor_eof_;
    std::streampos pos_;
    std::vector<unsigned char> buffer_;
    char *buffer_front_;

    std::streampos seekoff(std::streamoff offset,
                           std::ios_base::seekdir direction,
                           std::ios_base::openmode mode = std::ios_base::in |
                                                          std::ios_base::out);
    std::streampos seekpos(std::streampos position,
                           std::ios_base::openmode mode = std::ios_base::in |
                                                          std::ios_base::out);
    int_type underflow();
    std::streamsize xsgetn(char* s, std::streamsize n);
    std::streampos abspos() const;

 public:
    scopedstreambuf(const scopedstreambuf& other);
    explicit scopedstreambuf(std::istream* source,
                             std::streampos scope_start,
                             std::streamsize scope_size,
                             std::size_t buffer_size = DEFAULT_BUFFER_SIZE,
                             bool honor_eof = true);
};


struct scopedistream_base {
    // use this as base class for scopedistream so the streambuf instance can
    // be initialized before calling the constructors of the superclasses.
    scopedstreambuf sbuf_;
    explicit scopedistream_base(const scopedstreambuf& sbuf): sbuf_(sbuf) {}
    explicit scopedistream_base(std::istream* source,
                                std::streampos scope_start,
                                std::streamsize scope_size,
                                std::size_t buffer_size,
                                bool honor_eof): sbuf_(source,
                                                       scope_start,
                                                       scope_size,
                                                       buffer_size,
                                                       honor_eof) {}
};
class scopedistream: virtual scopedistream_base, public std::istream {
 public:
    scopedistream(const scopedistream& other): scopedistream_base(other.sbuf_),
                                               std::ios(&this->sbuf_),
                                               std::istream(&this->sbuf_) {}
    explicit scopedistream(std::istream* source,
                           std::streampos scope_start,
                           std::streamsize scope_size,
                           std::size_t buffer_size = DEFAULT_BUFFER_SIZE,
                           bool honor_eof = true):
                                                scopedistream_base(source,
                                                                   scope_start,
                                                                   scope_size,
                                                                   buffer_size,
                                                                   honor_eof),
                                                std::ios(&this->sbuf_),
                                                std::istream(&this->sbuf_) {}
};

}  // namespace streams
}  // namespace zippie
#endif  // ZIPPIE_ZIPPIE_SCOPEDSTREAM_H_
