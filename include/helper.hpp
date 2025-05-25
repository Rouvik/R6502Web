#ifndef INCLUDED_HELPER_HPP
#define INCLUDED_HELPER_HPP

#include <iostream>

// ####################### toggle casserts #######################
// #define NDEBUG

#include <cassert>

#define LOG_INFO << __FILE__ ":" << __LINE__ << " (" << __FUNCTION__ << ") -- " <<
// #define LOG_INFO << // Uncomment this line to disable detailed logging

// ####################### toggle project debug features on/off #######################
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG

#define LOG(TAG, msg)                                        \
    do                                                       \
    {                                                        \
        std::cout << "[LOG " #TAG "] " LOG_INFO msg << '\n'; \
    } while (0)

#define ERROR(TAG, msg)                                        \
    do                                                         \
    {                                                          \
        std::cerr << "[ERROR " #TAG "] " LOG_INFO msg << '\n'; \
    } while (0)

#define WARN(TAG, msg)                                        \
    do                                                        \
    {                                                         \
        std::cerr << "[WARN " #TAG "] " LOG_INFO msg << '\n'; \
    } while (0)

#else

#define LOG(TAG, msg) ((void)0)

#define ERROR(TAG, msg) ((void)0)

#define WARN(TAG, msg) ((void)0)

#endif

#endif // INCLUDED_HELPER_HPP