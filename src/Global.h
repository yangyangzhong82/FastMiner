#pragma once

#define FM_DISABLE_COPY(ClassName)                                                                                     \
    ClassName(const ClassName&)            = delete;                                                                   \
    ClassName& operator=(const ClassName&) = delete

#define FM_DISABLE_MOVE(ClassName)                                                                                     \
    ClassName(ClassName&&)            = delete;                                                                        \
    ClassName& operator=(ClassName&&) = delete

#define FM_DISABLE_COPY_MOVE(ClassName)                                                                                \
    FM_DISABLE_COPY(ClassName);                                                                                        \
    FM_DISABLE_MOVE(ClassName)

#ifdef DEBUG
#define FM_TRACE(...) std::cout << __VA_ARGS__ << std::endl
#else
#define FM_TRACE(...) (void)0
#endif