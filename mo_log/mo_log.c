/*
 * Copyright (c) 2017 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "mo_log.h"
#include "mo_file_io.h"
#include "mo_console_colors.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

static int update_flag = 0; //private

static struct {
    void *udata;
    log_LockFn lock;
    FILE *fp;
    int level;
    int quiet;
} L;


static const char *level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif


static void lock(void)   {
    if (L.lock) {
        L.lock(L.udata, 1);
    }
}


static void unlock(void) {
    if (L.lock) {
        L.lock(L.udata, 0);
    }
}


void log_set_udata(void *udata) {
    L.udata = udata;
}


void log_set_lock(log_LockFn fn) {
    L.lock = fn;
}


void log_set_fp(FILE *fp) {
    L.fp = fp;
}


void log_set_level(int level) {
    L.level = level;
    //MO_DEBUG_PRINT("Set log level %d\n", level);
}


void log_set_quiet(int enable) {
    L.quiet = enable ? 1 : 0;
}

/// \brief log_get_base_filename 得到文件名，去除文件夹全路径名
///
/// \param pFullPath 全路径字符串
///
/// \return 文件名指针位置
const char* log_get_base_filename(const char *pFullPath)
{
    char *slash="/";
    const char *substr_loc = pFullPath;
    const char *last_substr_loc;

    do {
        last_substr_loc= substr_loc;
        substr_loc = strstr(substr_loc, slash);
        if(substr_loc != NULL)
            substr_loc++;
    } while (substr_loc != NULL);
    return last_substr_loc;
}

void log_log(int level, const char *file, int line, const char *fmt, ...) {
    if (level < L.level) {
        return;
    }

    /* Acquire lock */
    lock();

    /* Get current time */
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    /*extract base file name*/
    const bool use_full_path = false;
    if(!use_full_path)
    {
        file = log_get_base_filename(file);
    }

    /* Log to stderr */
    if (!L.quiet) {
        va_list args;
        char buf[16];
        buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
#ifdef LOG_USE_COLOR
        fprintf(
                stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
                buf, level_colors[level], level_names[level], file, line);
#else
        fprintf(stderr, "%s %-5s %s:%d: ", buf, level_names[level], file, line);
#endif
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
    }

    /* Log to file */
    if (L.fp) {
        va_list args;
        char buf[32];
        buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
        fprintf(L.fp, "%s %-5s %s:%d: ", buf, level_names[level], file, line);
        va_start(args, fmt);
        vfprintf(L.fp, fmt, args);
        va_end(args);
        fprintf(L.fp, "\n");
        fflush(NULL);
    }

    /* Release lock */
    unlock();
}

int mo_init_log(FILE **fp, const char *log_full_name)
{
    //MO_DEBUG_PRINT("Init log %s\n", log_full_name);
    if(mo_is_file_exist(log_full_name) == 0)
    {
        char *dir_path, *base_name;
        if(mo_split_path(log_full_name, &dir_path, &base_name) != 0)
        {
            //MO_DEBUG_PRINT("log_full_name: %s have error!\n", log_full_name);
            return -1;
        }
        //MO_DEBUG_PRINT("Current log file directory %s not exist, mkdir it\n", dir_path);
        if(mo_mkdir(dir_path) != 0)
        {
            //MO_DEBUG_PRINT("Mkdir current log file dierectory %s fails\n", dir_path);
            free(dir_path);
            free(base_name);
            return -1;
        }

        free(dir_path);
        free(base_name);
    }

    *fp = fopen(log_full_name, "a+");
    log_set_fp(*fp);
#ifdef DEBUG
    log_set_level(LOG_LEVEL_DEBUG);
#else
    log_set_level(LOG_LEVEL_RELEASE);
#ifndef EMBED
    log_set_quiet(1);
#endif  //EMBED
#endif  //DEBUG
    
    update_flag = 1;
    
    return 0;
}

void mo_destory_log(FILE *fp)
{
    if(fp != NULL)
        fclose(fp);
    update_flag = 0;
}

void mo_set_log(FILE *fp)
{
    log_set_fp(fp);
#ifdef DEBUG
    log_set_level(LOG_LEVEL_DEBUG);
#else
    log_set_level(LOG_LEVEL_RELEASE);
#ifndef EMBED
    log_set_quiet(1);
#endif  //EMBED
#endif  //DEBUG


}

static void generate_record_log_name(const int id, char *log_name)
{
    memset(log_name, 0, LOG_SAVE_FILE_NAME_MAX_LENGTH);
    sprintf(log_name, "%s%08d.log", LOG_DIR, id);
}

static int get_record_log_idx(const char *log_name)
{
    char *name, *ext;
    int log_idx ;
    if(mo_split_ext(log_name, &name, &ext) != 0)
    {
        log_idx = -1;
    }
    else{
        assert(strcmp(ext, ".log") == 0);
        log_idx = atoi(name);
    }

    free(name);
    free(ext);
    return log_idx;
}

static int check_current_log_full(const char *log_name)
{
    int current_log_size = mo_get_file_size(log_name);
    if(current_log_size >= LOG_SIGNLE_FILE_CAPACITY_IN_BYTE)
        return 1;
    return 0;
}

static int check_total_log_full(const char **log_files, const int n_log_files)
{
    int total_log_size = 0;
    for (int i = 0; i < n_log_files; ++i) {
        char *full_name = NULL;
        mo_join_two_path(LOG_DIR, log_files[i], &full_name);
        int log_size = mo_get_file_size(full_name);
        free(full_name);
        if(log_size == -1)
        {
            //MO_DEBUG_PRINT("log file %s size computing error\n", log_files[i]);
        }
        else{
            total_log_size += log_size;
        }
    }

    if(total_log_size >= LOG_TOTAL_CAPACITY_IN_BYTE)
    {
        return 1;
    }
    return 0;
}

static const char* get_lastest_log_file(const char **log_files, const int n_log_files)
{
    if(n_log_files <= 0)
        return NULL;
    else
        return log_files[n_log_files-1];
}

static const char* get_oldest_log_files(const char **log_files, const int n_log_files)
{
    if(n_log_files <= 0)
        return NULL;
    else
    {
        return log_files[0];
    }
}

// rename existing log file name, start from 1.log, prevent log index increase to very large number
static void rename_existing_logs(const char *dir)
{
    char record_log_name[LOG_SAVE_FILE_NAME_MAX_LENGTH];

    const int sorted = 1;
    char **log_files = NULL;
    int n_log_files = 0;
    char *pMemory;
    int ret = mo_listdir(dir, ".log", &log_files, &n_log_files, sorted, 
                         &pMemory);
    if(ret != 0)
    {
        //MO_DEBUG_PRINT("get list dir fails\n");
        goto free_listdir;
    }
   // MO_DEBUG_PRINT("Existing log file number: %d\n", n_log_files);
    for (int i = 0; i < n_log_files; ++i) {
        char *full_name_dst = NULL;
        char *full_name_src = NULL;
        mo_join_two_path(LOG_DIR, log_files[i], &full_name_src);

        generate_record_log_name(i+1, record_log_name);
        full_name_dst = record_log_name;

        //MO_DEBUG_PRINT("Rename existing log %s to %s\n", full_name_src, full_name_dst);

        if(strcmp(full_name_dst, full_name_src) == 0)
        {
            free(full_name_src);
            continue;
        }

        mo_file_copy(full_name_src, full_name_dst);

        free(full_name_src);
    }
free_listdir:
    mo_free_listdir(&log_files, &pMemory);
}

static void delete_old_log(const char **log_files, const int n_log_files)
{
    const char* oldest_file = get_oldest_log_files(log_files, n_log_files);

    char *oldest_file_full_name;
    mo_join_two_path(LOG_DIR, oldest_file, &oldest_file_full_name);
    int ret = mo_rmfile(oldest_file_full_name);
    if(ret == 0)
    {
        //MO_DEBUG_PRINT("Total log directory is full, delete oldest log file %s\n", oldest_file_full_name);
    }
    else
    {
        //MO_DEBUG_PRINT("delete log %s failed\n", oldest_file);
    }
    free(oldest_file_full_name);
}

static void check_and_delete_old_logs()
{
    int res;
    do
    {
        char **log_files = NULL;
        int n_log_files = 0;
        char *pMemroy;
        int ret = mo_listdir(LOG_DIR, (const char*)".log", &log_files, 
                             &n_log_files, 1, &pMemroy);

        if(ret == -1){;} //error handle

        res = check_total_log_full((const char**)log_files, (const int)n_log_files);
        if(res)
        {
            delete_old_log((const char**)log_files, (const int)n_log_files);
        }
        mo_free_listdir(&log_files, &pMemroy);
    }
    while(res);
    return;
}

int mo_update_log(const char *log_name)
{
    char record_log_name[LOG_SAVE_FILE_NAME_MAX_LENGTH];
    if(log_name == NULL)
    {
        //MO_DEBUG_PRINT("update log_name is NULL\n");
        return -1;
    }

    if(mo_is_file_exist(log_name) == 0)
    {
        //MO_DEBUG_PRINT("log_name %s not exist!, touch it\n", log_name);

        if(mo_file_touch(log_name) != 0)
        {
            cc_fprintf(cc_color_t(CC_FG_YELLOW|CC_BG_RED), stderr, 
                "log_name %s touch fails, LOG CAN'T UPDATE!!!!!\n", log_name);
            return -1;
        }
    }

    if(mo_is_folder_exist(LOG_DIR) == 0)
    {
        //MO_DEBUG_PRINT("LOG_DIR not exist, mkdir %s\n", LOG_DIR);
        if(mo_mkdir(LOG_DIR) != 0)
        {
            cc_fprintf(cc_color_t(CC_FG_YELLOW|CC_BG_RED), stderr,
                    "mkdir LOG_DIR %s fails, LOG CAN'T UPDATE!!!\n", LOG_DIR);
            return -1;
        }
    }
    else{
        rename_existing_logs(LOG_DIR);
    }

    while(update_flag != 0)
    {
        char **log_files = NULL;
        int n_log_files = 0;
        char *pMemory;
        int ret = mo_listdir(LOG_DIR, ".log", &log_files, &n_log_files, 1, 
                             &pMemory);
        if(ret != 0)
        {
            //MO_DEBUG_PRINT("get list dir fails\n");
        }
        else{
            if(check_current_log_full(log_name) == 1)
            {
                int new_log_idx;
                if(n_log_files == 0)
                {
                    new_log_idx = 1; // start from 1.log
                }
                else{
                    const char* lastest_log_name  = get_lastest_log_file((const char**)log_files, (const int)n_log_files);

                    int lastest_log_idx = get_record_log_idx(lastest_log_name);
                    if(lastest_log_idx > 0)
                        new_log_idx = lastest_log_idx+1;
                    else
                    {
                        new_log_idx = n_log_files+1;
                        //MO_DEBUG_PRINT("Warninig! lastest_log_idx not correct, use index %d", new_log_idx);
                    }
                }

                if(new_log_idx > 0)
                {
                    generate_record_log_name(new_log_idx, record_log_name);

                    mo_file_copy(log_name, record_log_name);

                    mo_file_clear(log_name);

                    //MO_DEBUG_PRINT("Current log file full, save to %s\n", record_log_name);
                }
                else{
                    assert(0 && "impossible here");
                }
            }

            check_and_delete_old_logs();
            /*if(check_total_log_full(log_files, n_log_files))*/
            /*{*/
            /*delete_old_log(log_files, n_log_files);*/
            /*}*/
        }

        mo_free_listdir(&log_files, &pMemory);
        sleep(LOG_UPDATE_POLLING_SECONDS);
    }

    return 0;
}


