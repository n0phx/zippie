// copyright 2015 andrean franc
#include <unistd.h>
#include <sys/stat.h>

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include "zippie/os.h"


namespace os {

std::string get_current_dir() {
    char buf[PATH_MAX];
    if (getcwd(buf, PATH_MAX) != 0)
        return std::string(buf);

    std::ostringstream msg;
    msg << "Cannot get working directory, error code: " << errno;
    throw std::runtime_error(msg.str());
}


std::string abspath(const std::string& path) {
    if (path.empty())
        return get_current_dir();

    char buf[PATH_MAX];
    if (realpath(path.c_str(), buf))
        return std::string(buf);

    std::ostringstream msg;
    msg << "Cannot resolve absolute path, error code: " << errno;
    throw std::runtime_error(msg.str());
}


std::string dirname(const std::string& path) {
    if (path.empty())
        return path;

    std::vector<std::string> separated = split(path);
    separated.pop_back();  // since parent is needed, ignore last segment
    std::string processed;
    if (path[0] == os::pathsep)
        processed.push_back(os::pathsep);  // include starting slash of path

    for (std::vector<std::string>::const_iterator it = separated.begin();
            it != separated.end();
            ++it) {
        processed = join(processed, *it);
    }
    return processed;
}


std::string join(const std::string& lhs, const std::string& rhs) {
    std::string dest(lhs);

    if (dest.empty())
        return rhs;

    if (dest[dest.length() - 1] != pathsep)
        dest += pathsep;

    return dest + rhs;
}


std::vector<std::string> split(const std::string &path, char delimiter) {
    std::vector<std::string> result;
    std::istringstream ss(path);
    std::string component;
    while (std::getline(ss, component, delimiter)) {
        result.push_back(component);
    }
    return result;
}


// function not exposed via header
void makedir(const char* path) {
    struct stat st;
    // stat returns 0 if path exists
    if (stat(path, &st) != 0) {
        int status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        // status is nonzero is mkdir fails, in which case other than for the
        // error if the path already existed, raise an exception
        if (status != 0 && errno != EEXIST) {
            std::ostringstream msg;
            msg << "Error creating directory, error code: " << errno;
            throw std::runtime_error(msg.str());
        }
    }
}


void makedirs(const std::string& path) {
    // path must be an absolute path
    std::vector<std::string> separated = split(path);
    std::string processed;
    processed.push_back(os::pathsep);  // include starting slash of path
    for (std::vector<std::string>::const_iterator it = separated.begin();
            it != separated.end();
            ++it) {
        processed = join(processed, *it);
        makedir(processed.c_str());
    }
}

}  // namespace os
