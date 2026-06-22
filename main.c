#include "clog.h"
#include <stdio.h>
#include <unistd.h>

#define MODULE_MAIN     "main"
#define MODULE_NETWORK  "network"
#define MODULE_DATABASE "database"
#define MODULE_UI       "ui"
#define MODULE_UTILS    "utils"

void simulate_network(void)
{
    clog_debug(MODULE_NETWORK, "初始化网络套接字，端口: %d", 8080);
    clog_info(MODULE_NETWORK, "服务器已启动，监听 0.0.0.0:8080");

    usleep(100000);
    clog_debug(MODULE_NETWORK, "收到客户端连接请求，IP: 192.168.1.100");
    clog_info(MODULE_NETWORK, "客户端连接成功，会话ID: 0x%08X", 0xA1B2C3D4);

    usleep(100000);
    clog_warn(MODULE_NETWORK, "连接超时重试，次数: %d/3", 2);
    clog_debug(MODULE_NETWORK, "重试连接中...");

    usleep(100000);
    clog_error(MODULE_NETWORK, "连接失败，错误码: %d，原因: 连接被拒绝", 111);
    clog_info(MODULE_NETWORK, "正在尝试备用服务器...");
}

void simulate_database(void)
{
    clog_debug(MODULE_DATABASE, "正在加载数据库配置文件: %s", "/etc/db/config.conf");
    clog_info(MODULE_DATABASE, "数据库连接池初始化完成，连接数: %d", 10);

    usleep(100000);
    clog_debug(MODULE_DATABASE, "执行SQL查询: SELECT * FROM users WHERE id = %d", 1001);
    clog_info(MODULE_DATABASE, "查询成功，返回 %d 条记录", 1);

    usleep(100000);
    clog_warn(MODULE_DATABASE, "查询耗时超过阈值: %.2fms，建议添加索引", 156.78);
    clog_debug(MODULE_DATABASE, "慢查询日志已记录");

    usleep(100000);
    clog_error(MODULE_DATABASE, "事务提交失败，错误: %s", "死锁检测");
    clog_info(MODULE_DATABASE, "正在执行回滚操作...");
    clog_info(MODULE_DATABASE, "回滚完成，数据一致性已恢复");
}

void simulate_ui(void)
{
    clog_debug(MODULE_UI, "正在渲染主界面组件");
    clog_info(MODULE_UI, "用户登录成功，用户名: %s", "admin");

    usleep(100000);
    clog_debug(MODULE_UI, "加载资源文件: %s", "assets/icons.png");
    clog_debug(MODULE_UI, "解析样式表，共 %d 条规则", 256);

    usleep(100000);
    clog_warn(MODULE_UI, "字体文件加载失败，使用默认字体: %s", "missing_font.ttf");
    clog_info(MODULE_UI, "界面渲染完成，FPS: %.1f", 59.8);

    usleep(100000);
    clog_error(MODULE_UI, "空指针异常，位置: render_button() at line %d", 452);
}

void simulate_utils(void)
{
    clog_debug(MODULE_UTILS, "工具类初始化，内存池大小: %dKB", 1024);
    clog_info(MODULE_UTILS, "配置项加载完成，共 %d 项", 64);

    usleep(100000);
    clog_debug(MODULE_UTILS, "正在计算哈希值，输入长度: %d", 2048);
    clog_debug(MODULE_UTILS, "哈希计算完成: 0x%016llX", 0x123456789ABCDEF0ULL);

    usleep(100000);
    clog_warn(MODULE_UTILS, "内存使用超过警告阈值: %.1f%%", 85.5);
    clog_info(MODULE_UTILS, "已触发垃圾回收");

    usleep(100000);
    clog_error(MODULE_UTILS, "加密算法初始化失败，密钥长度不足");
}

void print_separator(const char *title)
{
    printf("\n");
    printf("========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

int main(void)
{
    clog_init();

    print_separator("测试1: 默认配置 - 所有模块全部输出");
    clog_info(MODULE_MAIN, "日志系统已启动，默认级别: %s", clog_level_to_str(CLOG_LEVEL_DEBUG));

    simulate_network();
    simulate_database();
    simulate_ui();
    simulate_utils();

    print_separator("测试2: 全局日志级别设置为 INFO（过滤 DEBUG）");
    clog_set_level(CLOG_LEVEL_INFO);
    clog_info(MODULE_MAIN, "全局级别已调整为: %s", clog_level_to_str(CLOG_LEVEL_INFO));

    simulate_network();
    simulate_database();

    print_separator("测试3: 按模块过滤 - 只允许 database 显示 DEBUG");
    clog_set_level(CLOG_LEVEL_INFO);
    clog_add_module_filter(MODULE_DATABASE, CLOG_LEVEL_DEBUG);
    clog_info(MODULE_MAIN, "已设置模块过滤: database=DEBUG, 其他=INFO");

    simulate_network();
    simulate_database();
    simulate_ui();

    print_separator("测试4: 黑名单模式 - 默认禁止，仅允许 network 输出");
    clog_set_filter_allow_by_default(false);
    clog_clear_module_filters();
    clog_add_module_filter(MODULE_NETWORK, CLOG_LEVEL_DEBUG);
    clog_info(MODULE_MAIN, "已设置黑名单模式: 仅 network 模块可输出");

    simulate_network();
    simulate_database();
    simulate_ui();
    simulate_utils();

    print_separator("测试5: 多模块不同级别配置");
    clog_set_filter_allow_by_default(true);
    clog_clear_module_filters();
    clog_set_level(CLOG_LEVEL_DEBUG);
    clog_add_module_filter(MODULE_NETWORK, CLOG_LEVEL_INFO);
    clog_add_module_filter(MODULE_DATABASE, CLOG_LEVEL_WARN);
    clog_add_module_filter(MODULE_UI, CLOG_LEVEL_ERROR);
    clog_add_module_filter(MODULE_UTILS, CLOG_LEVEL_NONE);

    clog_info(MODULE_MAIN, "模块级别配置:");
    clog_info(MODULE_MAIN, "  network  -> INFO");
    clog_info(MODULE_MAIN, "  database -> WARN");
    clog_info(MODULE_MAIN, "  ui       -> ERROR");
    clog_info(MODULE_MAIN, "  utils    -> NONE (完全关闭)");
    clog_info(MODULE_MAIN, "  main     -> DEBUG (默认)");

    simulate_network();
    simulate_database();
    simulate_ui();
    simulate_utils();
    clog_debug(MODULE_MAIN, "main 模块可以输出 DEBUG");
    clog_info(MODULE_MAIN, "main 模块可以输出 INFO");
    clog_warn(MODULE_MAIN, "main 模块可以输出 WARN");
    clog_error(MODULE_MAIN, "main 模块可以输出 ERROR");

    print_separator("测试6: 关闭颜色输出（嵌入式环境适用）");
    clog_set_color(false);
    clog_clear_module_filters();
    clog_set_level(CLOG_LEVEL_DEBUG);
    clog_info(MODULE_MAIN, "颜色输出已关闭");

    simulate_network();
    simulate_database();

    print_separator("测试7: 启用文件输出");
    clog_set_color(true);
    clog_set_log_file("app.log");
    clog_info(MODULE_MAIN, "文件输出已启用，日志将同时写入 app.log");

    simulate_network();
    simulate_database();

    print_separator("测试8: 关闭文件输出（嵌入式环境适用）");
    clog_set_file_output(false);
    clog_info(MODULE_MAIN, "文件输出已关闭，仅控制台输出");

    simulate_ui();

    print_separator("测试完成");
    clog_info(MODULE_MAIN, "所有测试用例执行完毕");

    clog_cleanup();
    return 0;
}
