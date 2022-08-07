#ifndef PSPLDEBUG_H
#define PSPLDEBUG_H

#define PSPL_DEBUG_FILE "pspl_debug.txt"

#ifdef __cplusplus
extern "C" {
#endif

void pspl_log_file(const char* format, ...);

#ifdef PSPL_DEBUG
#define pspl_log pspl_log_file
#else
inline void pspl_log(const char* format, ...) {}
#endif


#ifdef __cplusplus
}
#endif

#endif
