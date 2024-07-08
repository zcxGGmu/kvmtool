#include "kvm/util.h"

static void report_msg(const char *prefix, const char *err, va_list params)
{
    char msg[1024];
    vsnprintf(msg, sizeof(msg), err, params);
    fprintf(stderr, "%s%s\n", prefix, msg);    
}

/** Builtin err/warn/info/debug */
static void error_builtin(const char *err, va_list params)
{
    report_msg(" Error: ", err, params);   
}

static void warn_builtin(const char *err, va_list params)
{
    report_msg(" Warning: ", err, params);
}

static void info_builtin(const char *err, va_list params)
{
    report_msg(" Info: ", err, params);   
}

static void debug_builtin(const char *err, va_list params)
{
    report_msg(" Debug: ", err, params);   
}

static NORETURN void die_builtin(const char *err, va_list params)
{
    report_msg(" Fatal: ", err, params);
    exit(128);
}

/** Print err/warn/info/debug */
void pr_err(const char *err, ...)
{
    va_list params;
    
    if (loglevel < LOGLEVEL_ERROR)
        return;

    va_start(params, err);
    error_builtin(err, params);

    va_end(params);
}

void pr_warning(const char *warn, ...)
{
    va_list params;

    if (loglevel < LOGLEVEL_WARNING)
        return;

    va_start(params, warn);
    warn_builtin(warn, params);

    va_end(params);
}

void pr_info(const char *info, ...)
{
    va_list params;

    if (loglevel < LOGLEVEL_WARNING)
        return;

    va_start(params, info);
    warn_builtin(info, params);

    va_end(params);
}

/** Don't call directly, call pr_debug() instead */
void __pr_debug(const char *debug, ...)
{
    va_list params;

    va_start(params, debug);
    debug_builtin(debug, params);

    va_end(params);
}

void die(const char *err, ...)
{
    va_list params;

    va_start(params, err);
    die_builtin(err, params);
    va_end(params);
}

void die_perror(const char *s)
{
    perror(s);
    exit(1);
} 