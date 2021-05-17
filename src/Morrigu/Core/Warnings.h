//
// Created by Mathis Lamidey on 2021-05-08.
//

#ifndef MORRIGU_WARNINGS_H
#define MORRIGU_WARNINGS_H

// clang-format off
#if defined(_MSC_VER)
    #define DISABLE_WARNING_PUSH                    __pragma(warning( push ))
    #define DISABLE_WARNING_POP                     __pragma(warning( pop ))
    #define DISABLE_WARNING(warningNumber)          __pragma(warning( disable : warningNumber ))

    #define DISABLE_WARNING_NONSTANDARD_EXTENSION   DISABLE_WARNING(4201)
    #define DISABLE_WARNING_CONSTEXPR_IF            DISABLE_WARNING(4127)
    #define DISABLE_WARNING_SHADOW                  // no warning equivalent
    #define DISABLE_WARNING_GREATER_SIZE_CAST       DISABLE_WARNING(4312)
    #define DISABLE_WARNING_UNSAFE_FUNCTIONS        DISABLE_WARNING(4996)
    #define DISABLE_WARNING_UNUSED_PARAMETER        DISABLE_WARNING(4100)
    #define DISABLE_WARNING_EXPRESSION_ALWAYS_TRUE  DISABLE_WARNING(4296)
    #define DISABLE_WARNING_PADDING_GENERATED       DISABLE_WARNING(4324)
    #define DISABLE_WARNING_ALIGNMENT_MODIFIED      DISABLE_WARNING(4103)

#elif defined(__GNUC__) || defined(__clang__)
    #define DO_PRAGMA(X) _Pragma(#X)
    #define DISABLE_WARNING_PUSH                    DO_PRAGMA(GCC diagnostic push)
    #define DISABLE_WARNING_POP                     DO_PRAGMA(GCC diagnostic pop)
    #define DISABLE_WARNING(warningName)            DO_PRAGMA(GCC diagnostic ignored #warningName)

    #define DISABLE_WARNING_NONSTANDARD_EXTENSION   // no warning equivalent ?
    #define DISABLE_WARNING_CONSTEXPR_IF            // no warning equivalent ?
    #define DISABLE_WARNING_SHADOW                  DISABLE_WARNING(-Wshadow)
    #define DISABLE_WARNING_GREATER_SIZE_CAST       DISABLE_WARNING(-Wint-to-void-pointer-cast)
    #define DISABLE_WARNING_UNSAFE_FUNCTIONS        DISABLE_WARNING(-Wdeprecated-declarations)
    #define DISABLE_WARNING_UNUSED_PARAMETER        DISABLE_WARNING(-Wunused-parameter)
    #define DISABLE_WARNING_EXPRESSION_ALWAYS_TRUE  DISABLE_WARNING(-Wtype-limits)
    #define DISABLE_WARNING_PADDING_GENERATED       DISABLE_WARNING(-Wpadded)
    #define DISABLE_WARNING_ALIGNMENT_MODIFIED      // no warning equivalent ?

#else
    // please define the warning supression macros specific to you compiler here
    #define DISABLE_WARNING_PUSH
    #define DISABLE_WARNING_POP
    #define DISABLE_WARNING(warningNumber)

    #define DISABLE_WARNING_NONSTANDARD_EXTENSION
    #define DISABLE_WARNING_CONSTEXPR_IF
    #define DISABLE_WARNING_SHADOW
    #define DISABLE_WARNING_GREATER_SIZE_CAST
    #define DISABLE_WARNING_UNSAFE_FUNCTIONS
    #define DISABLE_WARNING_UNUSED_PARAMETER
    #define DISABLE_WARNING_EXPRESSION_ALWAYS_TRUE
    #define DISABLE_WARNING_PADDING_GENERATED
    #define DISABLE_WARNING_ALIGNMENT_MODIFIED

#endif
// clang-format on

#endif  // MORRIGU_WARNINGS_H
