#ifndef __RVAF_UTIL_H__
#define __RVAF_UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>     //va_list

#include <errno.h>
#include <signal.h>

#ifdef __GNUC__
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN
#endif

/** Log settings */
#define LOGLEVEL_ERROR      0
#define LOGLEVEL_WARNING    1
#define LOGLEVEL_INFO       2
#define LOGLEVEL_DEBUG      3

extern int loglevel;

extern void die(const char *err, ...) NORETURN __attribute__((format (printf, 1, 2)));
extern void die_perror(const char *s) NORETURN;

extern void pr_err(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern void pr_warning(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern void pr_info(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern void __pr_debug(const char *err, ...) __attribute__((format (printf, 1, 2)));
#define pr_debug(fmt, ...)   \
    do {                        \
        if (loglevel >= LOGLEVEL_DEBUG)     \
            __pr_debug("(%s) %s:%d" fmt, __FILE__,     \
                __func__, __LINE__, ##__VA_ARGS__);    \
    } while (0);

/** BUG/WARN macros */
#ifndef BUG_ON_HANDLER
#define BUG_ON_HANDLER(cond)    \
    do {                \
        if ((cond)) {   \
            pr_err("BUG at %s:%d", __FILE__, __LINE__);     \
            raise(SIGABRT);     \
        }   \
    } while (0)
#endif

#define BUG_ON(cond) BUG_ON_HANDLER((cond))

#endif
