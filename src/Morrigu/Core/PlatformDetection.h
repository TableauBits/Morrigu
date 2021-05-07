//
// Created by Mathis Lamidey on 2021-04-03.
//

#ifndef MORRIGU_PLATFORMDETECTION_H
#define MORRIGU_PLATFORMDETECTION_H

// clang-format off
#ifdef _WIN32
#ifdef _WIN64
#define MRG_PLATFORM_WINDOWS
#else
#error "Windows 32 bits is not supported!"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
// Apple devices ALL fall in this category. Apple provides the following includes to know what exact platform we're exactly on:
    #include <TargetConditionals.h>

    // We can now use this include to define a bit more specifically what we are compiling on:
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define MRG_PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define MRG_PLATFORM_MACOS
    #else
        #error "Unkown Apple platform!"
    #endif
#elif defined(__ANDROID__)
    #define MRG_PLATFORM_ANDROID
    #error "Android platform not supported!"
#elif defined(__linux__)
    #define MRG_PLATFORM_LINUX
#else
    #error "Unknown platform detected!"
#endif
// clang-format on

#endif  // MORRIGU_PLATFORMDETECTION_H
