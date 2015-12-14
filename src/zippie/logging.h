// copyright 2015 andrean franc
#ifndef ZIPPIE_ZIPPIE_LOGGING_H_
#define ZIPPIE_ZIPPIE_LOGGING_H_
#include <string>

namespace logging {
    enum Level {
        CRITICAL = 0,
        ERROR = 1,
        WARNING = 2,
        INFO = 3,
        DEBUG = 4
    };

    extern Level current_level;

    void set_level(Level chosen_level);
    void log(Level chosen_level, const std::string& message);
    void critical(const std::string& message);
    void error(const std::string& message);
    void warning(const std::string& message);
    void info(const std::string& message);
    void debug(const std::string& message);
}  // namespace logging
#endif  // ZIPPIE_ZIPPIE_LOGGING_H_
