#ifndef MRG_CORE_HEADER
#define MRG_CORE_HEADER

#include "Logger.h"

#define MRG_BIT(x) (1 << (x))

// This is a workaround to ensure that both parameters a and b are getting evaluated in time.
#define MRG_PREPOC_PASTER(a, b) a##b
#define MRG_PREPOC_EVALUATOR(a, b) MRG_PREPOC_PASTER(a, b)

// disabling clang-format indentation temporarily makes for more readable code
// clang-format off
#ifndef NDEBUG
    #define MRG_DEBUG
#endif

#include "Core/PlatformDetection.h"

// defining common interface for function signature macro
#ifdef _MSC_VER
    #define MRG_FUNCSIG __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
    #define MRG_FUNCSIG __PRETTY_FUNCTION__ 
#endif

// defining debug Macros
#ifdef MRG_DEBUG
    // defining platform specific debug break
    #ifdef MRG_PLATFORM_WINDOWS
        /// Windows
        #define DEBUG_BREAK __debugbreak()
    #elif defined(MRG_PLATFORM_LINUX) || defined(MRG_PLATFORM_MACOS)
        /// POSIX
        #include <signal.h>
        #define DEBUG_BREAK raise(SIGTRAP)
    #else
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
    #define MRG_ASSERT(x, ...)
    #define MRG_CORE_ASSERT(x, ...)
#endif
// clang-format on

namespace MRG
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename... Args>
	[[nodiscard]] constexpr Scope<T> createScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename... Args>
	[[nodiscard]] constexpr Ref<T> createRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}  // namespace MRG

#endif
