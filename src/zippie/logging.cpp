// copyright 2015 andrean franc
#include <iostream>
#include <string>

#include "zippie/logging.h"


namespace logging {
    Level current_level = ERROR;

    void set_level(Level chosen_level) {
        current_level = chosen_level;
    }

    void log(Level chosen_level, const std::string& message) {
        if (chosen_level <= current_level) {
            switch (chosen_level) {
                case CRITICAL:
                    std::cerr << "[CRITICAL] " << message << std::endl;
                    break;
                case ERROR:
                    std::cerr << "[ERROR] " << message << std::endl;
                    break;
                case WARNING:
                    std::cerr << "[WARNING] " << message << std::endl;
                    break;
                case INFO:
                    std::clog << "[INFO] " << message << std::endl;
                    break;
                case DEBUG:
                    std::clog << "[DEBUG] " << message << std::endl;
                    break;
            }
        }
    }

    void critical(const std::string& message) {
        log(CRITICAL, message);
    }

    void error(const std::string& message) {
        log(ERROR, message);
    }

    void warning(const std::string& message) {
        log(WARNING, message);
    }

    void info(const std::string& message) {
        log(INFO, message);
    }

    void debug(const std::string& message) {
        log(DEBUG, message);
    }
}  // namespace logging
