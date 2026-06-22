#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    CLOG_LEVEL_DEBUG = 0,
    CLOG_LEVEL_INFO  = 1,
    CLOG_LEVEL_WARN  = 2,
    CLOG_LEVEL_ERROR = 3,
    CLOG_LEVEL_NONE  = 4
} clog_level_t;

#define CLOG_COLOR_ENABLE   1
#define CLOG_COLOR_DISABLE  0

#define CLOG_FILE_ENABLE    1
#define CLOG_FILE_DISABLE   0

#ifndef CLOG_COLOR
#define CLOG_COLOR          CLOG_COLOR_ENABLE
#endif

#ifndef CLOG_FILE_OUTPUT
#define CLOG_FILE_OUTPUT    CLOG_FILE_ENABLE
#endif

#define CLOG_MAX_MODULE_LEN   32
#define CLOG_MAX_MSG_LEN      512
#define CLOG_MAX_FILTERS      16

typedef struct {
    char module[CLOG_MAX_MODULE_LEN];
    clog_level_t level;
} clog_filter_t;

typedef struct {
    clog_level_t global_level;
    bool color_enabled;
    bool file_output;
    FILE *log_file;
    char log_filepath[256];
    clog_filter_t filters[CLOG_MAX_FILTERS];
    int filter_count;
    bool filter_allow_by_default;
} clog_config_t;

void clog_init(void);
void clog_cleanup(void);

void clog_set_level(clog_level_t level);
void clog_set_color(bool enable);
void clog_set_file_output(bool enable);
void clog_set_log_file(const char *filepath);

void clog_set_filter_allow_by_default(bool allow);
int  clog_add_module_filter(const char *module, clog_level_t level);
void clog_clear_module_filters(void);
bool clog_remove_module_filter(const char *module);

void clog_log(clog_level_t level, const char *module, const char *fmt, ...);

#define clog_debug(module, fmt, ...) \
    clog_log(CLOG_LEVEL_DEBUG, module, fmt, ##__VA_ARGS__)

#define clog_info(module, fmt, ...) \
    clog_log(CLOG_LEVEL_INFO, module, fmt, ##__VA_ARGS__)

#define clog_warn(module, fmt, ...) \
    clog_log(CLOG_LEVEL_WARN, module, fmt, ##__VA_ARGS__)

#define clog_error(module, fmt, ...) \
    clog_log(CLOG_LEVEL_ERROR, module, fmt, ##__VA_ARGS__)

const char *clog_level_to_str(clog_level_t level);

#endif
