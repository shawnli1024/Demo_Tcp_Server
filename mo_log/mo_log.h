/// Copyright (c) 2017 Metoak.Co.Ltd. All rights reserved
#ifndef MO_LOG_H_DHSC71MI
#define MO_LOG_H_DHSC71MI

#include <stdio.h>
#include <stdarg.h>

#define LOG_LEVEL_DEBUG     (LOG_DEBUG)
#define LOG_LEVEL_RELEASE   (LOG_ERROR)

#ifdef EMBED
#define  LOG_DIR  ("/home/root/MOLog/saved_log/")
#define  LOG_PATH  ("/home/root/MOLog/Perception.log")
#else
#define  LOG_DIR  ("/tmp/MOLog/saved_log/")
#define  LOG_PATH  ("/tmp/MOLog/Perception.log")
#endif

#define LOG_SAVE_FILE_NAME_MAX_LENGTH   (1024)
#define LOG_UPDATE_POLLING_SECONDS      (10)

#ifdef EMBED
#define LOG_SIGNLE_FILE_CAPACITY_IN_BYTE   (10240)//1024*10, 10K
#define LOG_TOTAL_CAPACITY_IN_BYTE      (1048576) // 1024*1024, 1M
#else   //EMBED
#ifdef DEBUG
#define LOG_SIGNLE_FILE_CAPACITY_IN_BYTE   (1048576)//1024*1024, 1M
#define LOG_TOTAL_CAPACITY_IN_BYTE          (104857600) // 1024*1024*100, 100M
#else   //DEBUG
#define LOG_SIGNLE_FILE_CAPACITY_IN_BYTE   (102400)//1024*100, 100K
#define LOG_TOTAL_CAPACITY_IN_BYTE      (10485760) // 1024*1024*10, 10M
#endif  //DEBUG
#endif  //EMBED

#ifdef __cplusplus
extern "C"{
#endif

//FILE *mm_fp_target_proposal;
//FILE *mm_fp_target_track;

int mo_init_log(FILE **fp, const char *log_full_name);
void mo_destory_log(FILE *fp);
void mo_set_log(FILE *fp);

/// \brief Update log files.
///     If current log full, save it to LOG_DIR
///     If LOG_DIR full, delete oldest log files
///
/// \param log_name current log file name, e.g. Perception.log
///
/// \return 0:fail; 1:success
int mo_update_log(const char *log_name);

typedef void (*log_LockFn)(void *udata, int lock);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };


// NOTICE: do not modified this macro(MO_PUBLISH_MODE)
#if (MO_PUBLISH_MODE)
#define log_trace(...) do{ ; } while(0)
#define log_debug(...) do{ ; } while(0)
#define log_info(...)  do{ ; } while(0)
#define log_warn(...)  do{ ; } while(0)
#define log_error(...) do{ ; } while(0)
#define log_fatal(...) do{ ; } while(0)
#else 
#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#endif


void log_set_udata(void *udata);
void log_set_lock(log_LockFn fn);
void log_set_fp(FILE *fp);
void log_set_level(int level);
void log_set_quiet(int enable);

void log_log(int level, const char *file, int line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: MO_LOG_H_DHSC71MI */
