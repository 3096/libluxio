#include "lx/debug.hpp"

#include <switch.h>

// #define DEBUG_NXLINK_FORCED_IP 0x4500a8c0  // example: c0.a8.00.45 = 192.168.0.69

#ifdef DEBUG_NXLINK_FORCED_IP

#    include <netinet/in.h>

extern "C" {
extern in_addr __nxlink_host;
}

#endif

namespace lx {

#ifdef NDEBUG
// debugging is disabled, stub em
void __libnx_exception_handler(ThreadExceptionDump* ctx) {}
void debugInit() {}
void debugExit() {}

#else

#    include <cstdio>
#    include <string>

#    include "lvgl.h"

void lvglLog(lv_log_level_t level, const char* filePath, uint32_t lineNum, const char* funcName,
             const char* description) {
    LOG("level %d - %s:%d: %s %s\n", level, filePath, lineNum, funcName, description);
    if (level >= 3) {
        *(int*)nullptr = 0xB;  // force a crash
    }
}

#    ifdef DEBUG_LOG_FILE
FILE* g_debug_file;
extern "C" {
void __libnx_init_time(void);
}
#    endif

void debugInit() {
    TRY_FATAL(smInitialize());

#    ifdef DEBUG_LOG_FILE
    TRY_FATAL(timeInitialize());
    __libnx_init_time();
    time_t currentTime;
    TRY_FATAL(timeGetCurrentTime(TimeType_Default, (u64*)&currentTime));
    timeExit();

    std::string logName = "sdmc:/luxio" + std::to_string(currentTime) + ".log";
    g_debug_file = fopen(logName.c_str(), "w");
    if (g_debug_file == NULL) fatalThrow(0xf);
#    endif

#    ifdef DEBUG_NX_LINK

#        ifdef DEBUG_NXLINK_FORCED_IP
    __nxlink_host.s_addr = DEBUG_NXLINK_FORCED_IP;
#        endif

    nxlinkStdio();
#    endif

    smExit();

    lv_log_register_print_cb(lvglLog);

    LOGML("\n");
}

void debugExit() {
#    ifdef DEBUG_LOG_FILE
    fclose(g_debug_file);
#    endif
}

void* debugLogFormat(const char* prettyFunction, const char* fmt, const char* lineEnd, ...) {
    va_list args;
    va_start(args, lineEnd);

#    ifdef DEBUG_LOG_FILE
    if (prettyFunction) fprintf(g_debug_file, "%s: ", prettyFunction);
    vfprintf(g_debug_file, fmt, args);
    fprintf(g_debug_file, lineEnd);
    fflush(g_debug_file);
#    endif

#    ifdef DEBUG_NX_LINK
    if (prettyFunction) printf("%s: ", prettyFunction);
    vprintf(fmt, args);
    printf("%s", lineEnd);
#    endif

    va_end(args);

    return nullptr;
}

void __libnx_exception_handler(ThreadExceptionDump* ctx) {
    MemoryInfo mem_info;
    u32 page_info;
    svcQueryMemory(&mem_info, &page_info, ctx->pc.x);
    LOG("%#x exception with pc=%#lx", ctx->error_desc, ctx->pc.x - mem_info.addr);
    debugExit();
}

#endif

}  // namespace lx
