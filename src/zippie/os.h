// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_OS_H_
#define ZIPPIE_ZIPPIE_OS_H_
#include <string>
#include <vector>

namespace os {

const char pathsep =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif

std::string get_current_dir();
std::string abspath(const std::string& path);
std::string dirname(const std::string& path);
std::string join(const std::string& lhs, const std::string& rhs);
std::vector<std::string> split(const std::string &path,
                               char delimiter = pathsep);
void makedirs(const std::string& path);

}  // namespace os

#endif  // ZIPPIE_ZIPPIE_OS_H_
