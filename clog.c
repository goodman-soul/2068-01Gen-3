#include "clog.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>

#if CLOG_COLOR == CLOG_COLOR_ENABLE
#define CLOG_COLOR_RESET   "\033[0m"
#define CLOG_COLOR_DEBUG   "\033[36m"
#define CLOG_COLOR_INFO    "\033[32m"
#define CLOG_COLOR_WARN    "\033[33m"
#define CLOG_COLOR_ERROR   "\033[31m"
#define CLOG_COLOR_TIME    "\033[90m"
#define CLOG_COLOR_MODULE  "\033[35m"
#else
#define CLOG_COLOR_RESET   ""
#define CLOG_COLOR_DEBUG   ""
#define CLOG_COLOR_INFO    ""
#define CLOG_COLOR_WARN    ""
#define CLOG_COLOR_ERROR   ""
#define CLOG_COLOR_TIME    ""
#define CLOG_COLOR_MODULE  ""
#endif

static clog_config_t g_config;
static bool g_initialized = false;

static const char *g_level_str[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "NONE"
};

static const char *g_level_color[] = {
    CLOG_COLOR_DEBUG,
    CLOG_COLOR_INFO,
    CLOG_COLOR_WARN,
    CLOG_COLOR_ERROR,
    CLOG_COLOR_RESET
};

const char *clog_level_to_str(clog_level_t level)
{
    if (level < 0 || level >= CLOG_LEVEL_NONE) {
        return "UNKNOWN";
    }
    return g_level_str[level];
}

void clog_init(void)
{
    if (g_initialized) {
        return;
    }

    memset(&g_config, 0, sizeof(g_config));
    g_config.global_level = CLOG_LEVEL_DEBUG;
    g_config.color_enabled = (CLOG_COLOR == CLOG_COLOR_ENABLE);
    g_config.file_output = (CLOG_FILE_OUTPUT == CLOG_FILE_ENABLE);
    g_config.log_file = NULL;
    g_config.filter_count = 0;
    g_config.filter_allow_by_default = true;

    g_initialized = true;
}

void clog_cleanup(void)
{
    if (!g_initialized) {
        return;
    }

    if (g_config.log_file != NULL) {
        fclose(g_config.log_file);
        g_config.log_file = NULL;
    }

    g_initialized = false;
}

void clog_set_level(clog_level_t level)
{
    if (!g_initialized) {
        clog_init();
    }
    g_config.global_level = level;
}

void clog_set_color(bool enable)
{
    if (!g_initialized) {
        clog_init();
    }
    g_config.color_enabled = enable;
}

void clog_set_file_output(bool enable)
{
    if (!g_initialized) {
        clog_init();
    }
    g_config.file_output = enable;
}

void clog_set_log_file(const char *filepath)
{
    if (!g_initialized) {
        clog_init();
    }

    if (g_config.log_file != NULL) {
        fclose(g_config.log_file);
        g_config.log_file = NULL;
    }

    if (filepath != NULL) {
        strncpy(g_config.log_filepath, filepath, sizeof(g_config.log_filepath) - 1);
        g_config.log_file = fopen(filepath, "a");
    }
}

void clog_set_filter_allow_by_default(bool allow)
{
    if (!g_initialized) {
        clog_init();
    }
    g_config.filter_allow_by_default = allow;
}

int clog_add_module_filter(const char *module, clog_level_t level)
{
    if (!g_initialized) {
        clog_init();
    }

    if (module == NULL || g_config.filter_count >= CLOG_MAX_FILTERS) {
        return -1;
    }

    for (int i = 0; i < g_config.filter_count; i++) {
        if (strcmp(g_config.filters[i].module, module) == 0) {
            g_config.filters[i].level = level;
            return i;
        }
    }

    strncpy(g_config.filters[g_config.filter_count].module, module, CLOG_MAX_MODULE_LEN - 1);
    g_config.filters[g_config.filter_count].level = level;
    g_config.filter_count++;

    return g_config.filter_count - 1;
}

void clog_clear_module_filters(void)
{
    if (!g_initialized) {
        clog_init();
    }
    g_config.filter_count = 0;
}

bool clog_remove_module_filter(const char *module)
{
    if (!g_initialized || module == NULL) {
        return false;
    }

    for (int i = 0; i < g_config.filter_count; i++) {
        if (strcmp(g_config.filters[i].module, module) == 0) {
            for (int j = i; j < g_config.filter_count - 1; j++) {
                g_config.filters[j] = g_config.filters[j + 1];
            }
            g_config.filter_count--;
            return true;
        }
    }

    return false;
}

static clog_level_t get_module_level(const char *module)
{
    for (int i = 0; i < g_config.filter_count; i++) {
        if (strcmp(g_config.filters[i].module, module) == 0) {
            return g_config.filters[i].level;
        }
    }

    if (g_config.filter_allow_by_default) {
        return g_config.global_level;
    } else {
        return CLOG_LEVEL_NONE;
    }
}

static void get_timestamp(char *buf, int buf_len)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buf, buf_len, "%Y-%m-%d %H:%M:%S", tm_info);
}

void clog_log(clog_level_t level, const char *module, const char *fmt, ...)
{
    if (!g_initialized) {
        clog_init();
    }

    if (module == NULL || fmt == NULL) {
        return;
    }

    clog_level_t module_level = get_module_level(module);
    if (level < module_level || level >= CLOG_LEVEL_NONE) {
        return;
    }

    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));

    char msg_buf[CLOG_MAX_MSG_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
    va_end(args);

    FILE *out = (level >= CLOG_LEVEL_WARN) ? stderr : stdout;
    bool use_color = g_config.color_enabled;

    if (use_color) {
        fprintf(out, "%s%s%s %s[%s]%s %s%s%s: %s\n",
                CLOG_COLOR_TIME, timestamp, CLOG_COLOR_RESET,
                CLOG_COLOR_MODULE, module, CLOG_COLOR_RESET,
                g_level_color[level], g_level_str[level], CLOG_COLOR_RESET,
                msg_buf);
    } else {
        fprintf(out, "%s [%s] %s: %s\n",
                timestamp, module, g_level_str[level], msg_buf);
    }

    fflush(out);

#if CLOG_FILE_OUTPUT == CLOG_FILE_ENABLE
    if (g_config.file_output && g_config.log_file != NULL) {
        fprintf(g_config.log_file, "%s [%s] %s: %s\n",
                timestamp, module, g_level_str[level], msg_buf);
        fflush(g_config.log_file);
    }
#endif
}
