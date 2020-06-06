//
// Created by dado on 06/06/2020.
//

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <cstring>
#include <functional>

#define  LOG_TAG    "EventHorizon-cpp"

enum LogPriority {
    LOG_PRIOTITY_INFO,
    LOG_PRIORITY_ERROR,
    LOG_PRIORITY_WARN
};

enum class PerfOptions {
    Print,
    NoPrint
};

namespace LoggerLevel {
    inline const static std::string Error = "error";
    inline const static std::string Warning = "warning";
    inline const static std::string Info = "info";
}

// Release log functions
#define ASSERTR( X ) doAssert(X, #X, __LINE__, __FILE__)
//#define THREAD_INFO

#define ARRAY_LENGTH( X ) (sizeof(X) / sizeof((X)[0]))

using logCF = std::function<void( const char * )>;

void hookLogCallbackFunction( logCF _cf );
void logPrint( LogPriority logPriority, const char *tag, const char *fmt, ... );
void doAssert( bool condition, const char *text, int line, const char *file );
void doAssertV( bool condition, const char *text, int line, const char *file, const char *fmt, ... );
void platformBreakpoint();
void platformLogPrint( const std::string& logTag, float time, const std::string& message );

#ifndef _PRODUCTION_

#ifdef TRACE_FUNCTIONS
#define FUNCTION_BEGIN() logPrint(LOG_PRIOTITY_INFO, LOG_TAG, "%s() - begin\n", __FUNCTION__ )
#define FUNCTION_END() logPrint(LOG_PRIOTITY_INFO, LOG_TAG, "%s() - end\n", __FUNCTION__ )
#else
#define FUNCTION_BEGIN()
#define FUNCTION_END()
#endif

#ifdef THREAD_INFO
#define LOGI(X,...)  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGR(X,...)  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGE(X,...) logPrint(LOG_PRIORITY_ERROR,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGW(X,...)  logPrint(LOG_PRIORITY_WARN,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGN()  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "[Thread: %d] " __FILE__ ":%d" , threadId(), __LINE__)
#else
#define LOGI( ... ) logPrint(LOG_PRIOTITY_INFO,LOG_TAG,  __VA_ARGS__)
#define LOGR( ... )  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, __VA_ARGS__)
#define LOGE( ... ) logPrint(LOG_PRIORITY_ERROR,LOG_TAG, __VA_ARGS__)
#define LOGW( ... )  logPrint(LOG_PRIORITY_WARN,LOG_TAG, __VA_ARGS__)
#define LOGN()  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "" __FILE__ ":%d", __LINE__)
#define LOGRS( X ) { std::ostringstream _ss; _ss << X; LOGR(_ss.str().c_str()); }
#endif

#define CONDLOG( X ) LOGI("*** " #X " // %s", X ? "True" : "False");
#define LOGIT( X, ... ) logPrint(LOG_PRIOTITY_INFO,X,__VA_ARGS__)
#define ASSERT( X ) doAssert(X, #X, __LINE__, __FILE__)
#define ASSERTV( X, ... ) doAssertV(X, #X, __LINE__, __FILE__, __VA_ARGS__)
#define PERF_TIMER_START() perfTimerStart(0)
#define PERF_TIMER_END( NAME ) perfTimerEnd(0, NAME)
#define N_PERF_TIMER_START( INDEX ) perfTimerStart(INDEX)
#define N_PERF_TIMER_END( INDEX, NAME ) perfTimerEnd(INDEX, NAME)
#define ASSERTSTATIC( exp ) {typedef char temp[(exp) ? 1 : -1];}
#else
#define LOGI(...)
#define LOGIT(...)
#define LOGN(...)
#define ASSERT(X)
#define ASSERTV(X, ...)
#define PERF_TIMER_START()
#define PERF_TIMER_END(X)
#define N_PERF_TIMER_START(INDEX)
#define N_PERF_TIMER_END(INDEX, NAME)
#define ASSERTSTATIC(exp)

#define LOGR(...) logPrint(LOG_PRIOTITY_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) logPrint(LOG_PRIORITY_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGW(...)  logPrint(LOG_PRIORITY_WARN,LOG_TAG,__VA_ARGS__)
#define LOGRS( X ) { std::ostringstream _ss; _ss << X; LOGR(_ss.str().c_str()); }
#endif

