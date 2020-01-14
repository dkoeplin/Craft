#ifndef CRAFT_SRC_CRAFT_UTIL_LOGGING_H_
#define CRAFT_SRC_CRAFT_UTIL_LOGGING_H_

#include <iostream>
#include <exception>

#define ERROR(message) \
    do { \
        std::cerr << "[error] " << __FILE__ << ":" << __LINE__ << ": " << message << "\n";                            \
    } while (false)

#define FATAL(message)  \
    do {                                                                                                               \
        std::cerr << "[error] " << __FILE__ << ":" << __LINE__ << ": " << message << "\n";                                \
        std::terminate();                                                                                              \
    } while (false)

#define REQUIRE(cond, message) \
    do { \
    if (!(cond)) { \
        std::cerr << "[error] " << __FILE__ << ":" << __LINE__ << ": " << message << "\n";                                \
        std::terminate();  \
    } \
    } while (false)

#endif //CRAFT_SRC_CRAFT_UTIL_LOGGING_H_
