#ifndef MRG_CORE_HEADER
#define MRG_CORE_HEADER

#include "Logger.h"

#define BIT(x) (1 << x)

// defining debug Macros
// disabling clang-format indentation temporarily makes for more readable code
// clang-format off
#ifndef NDEBUG
    // defining platform specific debug break
    #ifdef _MSC_VER
        /// Windows
        #define DEBUG_BREAK __debugbreak()
    #elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
        /// POSIX
        #include <signal.h>
        #define DEBUG_BREAK raise(SIGTRAP)
    #elif
        /// OTHERS, please write equivalent macro definition
        #define DEBUG_BREAK
    #endif
    // Defining ASSERT macros
    #define MRG_ASSERT(x, ...)                                                                                                                 \
        {                                                                                                                                      \
            if (!(x)) {                                                                                                                        \
                MRG_ERROR("Assertion failed: {0}", __VA_ARGS__);                                                                               \
                DEBUG_BREAK;                                                                                                                   \
            }                                                                                                                                  \
        }
    #define MRG_CORE_ASSERT(x, ...)                                                                                                            \
        {                                                                                                                                      \
            if (!(x)) {                                                                                                                        \
                MRG_ENGINE_ERROR("Assertion failed: {0}", __VA_ARGS__);                                                                        \
                DEBUG_BREAK;                                                                                                                   \
            }                                                                                                                                  \
        }
#else
    // should be optimized out by any compiler:
    #define MRG_ASSERT(x, ...) do {} while(0)
    #define MRG_CORE_ASSERT(x, ...) do {} while(0)
#endif
// clang-format on
#endif