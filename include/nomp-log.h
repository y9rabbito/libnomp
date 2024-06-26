#if !defined(_NOMP_LOG_H_)
#define _NOMP_LOG_H_

#include <stddef.h>

#include "nomp-impl.h"

extern const char *ERR_STR_USER_MAP_PTR_IS_INVALID;
extern const char *ERR_STR_USER_DEVICE_IS_INVALID;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup nomp_internal_types
 *
 * @brief Defines nomp log levels. A log can be an error, warning or general
 * information.
 */
typedef enum {
  NOMP_ERROR   = 1, /**< Error type. */
  NOMP_WARNING = 2, /**< Warning type. */
  NOMP_INFO    = 3, /**< Information type. */
} nomp_log_type_t;

/**
 * @defgroup nomp_log_utils Logging utilities
 *
 * @brief Internal functions used for logging.
 */

int nomp_log_set_verbose(unsigned verbose);

int nomp_log_(const char *desc, int errorno, nomp_log_type_t type, ...);

#define NOMP_CASE_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define NOMP_CASE(...)                                         NOMP_CASE_IMPL(__VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 1, 0)

#define NOMP_FIRST_IMPL(first, ...) first
#define NOMP_FIRST(...)             NOMP_FIRST_IMPL(__VA_ARGS__, throwaway)

#define NOMP_REST_IMPL_WITH_2(first, ...) , __VA_ARGS__
#define NOMP_REST_IMPL_WITH_1(first)
#define NOMP_REST_IMPL_(num, ...) NOMP_REST_IMPL_WITH_##num(__VA_ARGS__)
#define NOMP_REST_IMPL(num, ...)  NOMP_REST_IMPL_(num, __VA_ARGS__)
#define NOMP_REST(...)            NOMP_REST_IMPL(NOMP_CASE(__VA_ARGS__), __VA_ARGS__)

/**
 * @ingroup nomp_log_utils
 *
 * @def nomp_log
 *
 * @brief Log an error, warning or an info message. Use this instead of
 * using the nomp_log_() function directly.
 *
 * @param errorno Error number (One of \ref nomp_error_codes. used only when
 * type is an error).
 * @param type Log type one of ::nomp_log_type_t.
 * @param ... Log message as a C-string followed by arguments.
 */
#define nomp_log(errorno, type, ...)                                           \
  nomp_log_(NOMP_FIRST(__VA_ARGS__), errorno, type, __FILE__,                  \
            __LINE__ NOMP_REST(__VA_ARGS__))

void nomp_log_finalize(void);

/**
 * @defgroup nomp_profiler_utils Profiling utilities
 *
 * @brief Internal functions for profiling.
 */

int nomp_profile_set_level(const int profile_level);

void nomp_profile(const char *name, int toggle, int sync);

void nomp_profile_result(void);

void nomp_profile_finalize(void);

#ifdef __cplusplus
}
#endif

#endif // _NOMP_LOG_H_
