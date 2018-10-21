/// Copyright (c) 2017 Metoak.Co.Ltd. All rights reserved

#include "mo_file_io.h"
#include "mo_sys.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <assert.h>

int mo_is_folder_exist(const char* path)
{
    DIR *dp;
    if ((dp = opendir(path)) == NULL)
    {
        return 0;
    }
    closedir(dp);
    return 1;
}

int mo_is_file_exist(const char* path)
{
    int ret = access(path, F_OK);//access成功为0，失败-1
    return ret == 0 ? 1 : 0;
}

int mo_system_exec(const char* cmdstring){

    unsigned char _use_new_system_call = 1;

    if(_use_new_system_call)
    {
        return system_exec(cmdstring);
    }else
    {
        if(NULL == cmdstring){
            return -1;
        }

        int status;
        status = system(cmdstring);
        if(status < 0)
        {
            printf("cmd: %s\t error: %s", cmdstring, strerror(errno)); // Ouput or log errno info
            return -1;
        }

        return 0;
    }
}
    
int mo_file_copy(const char* src,const char* dst)
{
    FILE *input,*output;
    char ch;
    if((input = fopen(src,"r")) == NULL)
    {
        printf("'%s' does not exist!\n", src);
        return -1;
    }
    if((output = fopen(dst,"w")) == NULL)
    {
        fclose(input);
        printf("Wrong output file name or Permission denied, try system cp!\n");
        
        //使用系统命令进行拷贝
        char cmdString[1024];
        bzero(cmdString, strlen(cmdString));
        sprintf(cmdString, "cp -f %s %s && sync", src, dst);
    
        return mo_system_exec(cmdString);
    }
    ch = fgetc(input);
    while(!feof(input)) //判断后一位的状态(C语言问题)
    {
        fputc(ch,output);
        ch = fgetc(input);
    }
    
    fflush(output);
    fclose(input);
    fclose(output);
    return 0;
}

int mo_file_touch(const char *file_name)
{
    if(file_name == NULL)
        return -1;
    if(mo_is_file_exist(file_name) == 1)
        return 0;

    char *dir_name, *base_name;
    mo_split_path(file_name, &dir_name, &base_name);

    if(mo_mkdir(dir_name) != 0)
    {
        fprintf(stderr, "When touch file, can't mkdir parent dir %s\n", dir_name);
        if(dir_name) free(dir_name);
        if(base_name) free(base_name);
        return -2;
    }
    FILE* fp;
    if((fp = fopen(file_name, "w+")) == NULL)
    {
        printf("file touch fail\n");
        return -3;
    }
    fclose(fp);

    if(dir_name) free(dir_name);
    if(base_name) free(base_name);
    return 0;
}

int mo_file_clear(const char *file_name)
{
    if(file_name == NULL || mo_is_file_exist(file_name) ==0)
        return -1;
    FILE* fp;
    if((fp = fopen(file_name, "w")) == NULL)
    {
        printf("file clear fail");
        return -1;
    }
    fclose(fp);
    return 0;
}

int mo_mkdir(const char* path)
{
    if(mo_is_folder_exist(path) == 1)
    {
        return 0;
    }
    const char sep = '/';
    char *parent= (char*)malloc(strlen(path)+1);
    strcpy(parent, path);

    char *sep_loc = strrchr(parent, sep);
    *sep_loc = '\0';
    /*printf("parent is %s\n", parent);*/

    if(mo_mkdir(parent) != 0)
    {
        free(parent);
        return -1;
    }
    free(parent);

    if(path[strlen(path)-1] == '/')
        return 0;

    int status;
    status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return status;
}

int mo_split_path(const char *path, char **dir_name, char **base_name)
{
    if(path == NULL)
    {
        perror("path is empty!");
        return -1;
    }

    int full_path_len = strlen(path);
    char *path_copy = (char*)malloc(full_path_len+1);
    strcpy(path_copy, path);

    if(path_copy[full_path_len] == '/')
    {
        path_copy[full_path_len]= '\0';
        full_path_len--;
    }

    const char sep = '/';
    char *sep_loc = strrchr(path_copy, sep);
    if(sep_loc == NULL)
    {
        free(path_copy);
        return -2;
    }
    int base_len = strlen(sep_loc);
    int dir_len = full_path_len - base_len ;

    *base_name = (char*)malloc(base_len);
    *dir_name = (char*)malloc(dir_len+1);

    strcpy(*base_name, sep_loc+1); //跳过'/'
    char *p_path_copy = path_copy;
    char *p_dir = *dir_name;
    for (int i = 0; i < dir_len; ++i) {
        *p_dir++ = *p_path_copy++;
    }
    *p_dir= '\0';
    free(path_copy);
    return 0;
}

int mo_split_ext(const char *path, char **root, char **ext)
{
    if(path == NULL)
    {
        perror("path is empty!");
        return -1;
    }
    int full_path_len = strlen(path);
    char *path_copy = (char*)malloc(full_path_len+1);
    strcpy(path_copy, path);

    const char sep = '.';
    char *sep_loc = strrchr(path_copy, sep);
    if(sep_loc == NULL)
    {
        free(path_copy);
        return -1;
    }
    int ext_len = strlen(sep_loc);
    int root_len = full_path_len - ext_len ;

    *root = (char*)malloc(root_len+1);
    *ext = (char*)malloc(ext_len+1);

    strcpy(*ext, sep_loc);
    char *p_path_copy = path_copy;
    char *p_root = *root;
    for (int i = 0; i < root_len; ++i) {
        *p_root++ = *p_path_copy++;
    }
    *p_root= '\0';
    free(path_copy);
    return 0;
}

int mo_join_two_path(const char *path1, const char *path2, char **joined_path)
{
    if(path1 == NULL )
    {
        perror("path1 is empty!");
        return -1;
    }
    if(path2 == NULL)
    {
        perror("path2 is empty!");
        return -1;
    }
    int len1 = strlen(path1);
    int len2 = strlen(path2);
    const char *p1 = path1;
    const char *p2 = path2;
    int add_slash = 0;
    if(len1 > 0 && path1[len1-1] == '/')
    {
        if(path2[0] == '/')
        {
            p2++;
            len2--;
        }
    }
    else
    {
        if(path2[0] != '/' && len1 > 0) // if path1="", path2 ="a.c", joined_path should be "a.c"
        {
            add_slash = 1;
        }
    }

    *joined_path = (char*)malloc(len1+len2+add_slash+1);
    if(*joined_path == NULL)
    {
        perror("mo_malloc joined_path failed\n");
        return -1;
    }
    char *pj = *joined_path;
    strcpy(pj, p1);
    pj += len1;
    if(add_slash)
    {
        *pj++ = '/';
    }
    strcpy(pj, p2);
    pj += len2;
    *pj = '\0';

    return 0;
}

int mo_join_path(char **dst, const int path_num, ...)
{
    if(path_num < 2)
    {
        perror("joined path number should larger than two");
        return -1;
    }
    int i = 0;
    va_list ap;
    va_start(ap, path_num);
    const char *path1 = va_arg(ap, char*);
    char *joined_path;
    for (i = 1; i < path_num; ++i) {
        char *p_garbage = joined_path;
        const char *path2 = va_arg(ap, char*);
        if(mo_join_two_path(path1, path2, &joined_path) != 0)
        {
            return -1;
        }
        path1 = joined_path;
        if(p_garbage)
        {
            free(p_garbage);
        }
    }
    *dst = joined_path;
    va_end(ap);
    return 0;
}

int mo_get_file_size(const char *file_name)
{
    if(! mo_is_file_exist(file_name))
        return -1;

    struct stat st;
    stat(file_name, &st);
    int nFSize = st.st_size;
    return nFSize;

}

static int cmpstringp(const void *p1, const void *p2)
{
   return strcmp(* (char * const *) p1, * (char * const *) p2);
}

// 0: unmatch 1: match; <0 : error
static int check_ext_match(const char *file_name, const char *ext) 
{
    if(file_name == NULL) {
        return -1;
    }
    if(ext == NULL) {
        return -2;
    }
    //filter extension files
    char* pExtPosFile = (char*)(strrchr(file_name, '.')+1);
    if(pExtPosFile == NULL) {
       return 0; 
    }
    
    //extension dot check
    const char* pExt = ext;
    if(*pExt == '.')
        pExt++;
        
   if(strncmp(pExtPosFile, pExt, strlen(pExt)) == 0 )
   {
       return 1;
   } else
   {
       return 0;
   }
}

static void calculate_file_cnt_and_file_size_in_dir(const char *dir, int 
*nFiles, int *nTotalSize, const char *ext)
{
    const char* pCur = ".";
    const char* pPre = "..";
    
    *nFiles = 0;
    *nTotalSize = 0;
    
    if(dir == NULL || !mo_is_folder_exist(dir) )
    {
        return;
    }
    struct dirent *pDir;
    DIR *pD;
    pD = opendir(dir);
    
    int file_name_len_total = 0;
    int file_num_total = 0;
    int cur_file_len = 0;
    if(pD)
    {
        //calculate total file name len
        while( (pDir = readdir(pD)) != NULL )
        {
            const char *pFileName = pDir->d_name;
            //filter current folder and previous folder
            if( strcmp(pFileName, pCur) == 0 )
                continue;
            if( strcmp(pFileName, pPre) == 0 )
                continue;
            
            if(ext != NULL && check_ext_match(pFileName, ext) <= 0) {
                continue;
            }
            
            file_num_total++;
            cur_file_len = strlen(pFileName);
            file_name_len_total += cur_file_len;
        }
    }
    closedir(pD);
    
    *nFiles = file_num_total;
    *nTotalSize = file_name_len_total;
}

static void copy_dir_to_memory(const char *dir, const char *ext, char *memory, 
                              char **file_names, int sorted, 
                               const int total_file_num, 
                               const int total_file_size) 
{
    struct dirent *pDir;
    DIR *pD;
    pD = opendir(dir);
    
    const char* pCur = ".";
    const char* pPre = "..";
    
    int file_name_idx = 0;
    if(pD)
    {
        char *pMemPos = memory;
        while( (pDir = readdir(pD)) != NULL ) {
            const char *pFileName = pDir->d_name;
            //filter current folder and previous folder
            if( strcmp(pFileName, pCur) == 0 )
                continue;
            if( strcmp(pFileName, pPre) == 0 )
                continue;
            
            if(ext != NULL && check_ext_match(pFileName, ext) <= 0) {
                continue;
            }
            
            strcpy(pMemPos,pFileName);
            file_names[file_name_idx] = pMemPos;
            file_name_idx++;
            pMemPos += (strlen(pFileName)+1);
        }
        
        assert(total_file_num == file_name_idx);
        assert(pMemPos == memory+total_file_size+total_file_num);
    }
    closedir(pD);
    
    if(sorted) {
        qsort(file_names, file_name_idx, sizeof(char*), cmpstringp);
    }
}

int mo_listdir(const char *dir, const char *ext, char ***files, int *nfiles, 
               int sorted, char **allocated_memory)
{
    
    *nfiles = 0;
    *files  = NULL;
    
    if(dir == NULL || !mo_is_folder_exist(dir) )
        return -1;

    int total_file_num;
    int total_file_size;
    
    calculate_file_cnt_and_file_size_in_dir(dir, &total_file_num, 
                                            &total_file_size, ext);
    if(total_file_num == 0 || total_file_size == 0) {
        return 0;
    }
    // NOTE: each file has a '\0' at end, so add total_file_num 
    char* file_name_memory = (char*)malloc(total_file_size+total_file_num);
    char** file_names = (char**)malloc(sizeof(char*)*total_file_num);
    
    *allocated_memory = file_name_memory;
    copy_dir_to_memory(dir, ext, file_name_memory, file_names, 
                       sorted, total_file_num, total_file_size);
    
    *files = file_names;
    *nfiles = total_file_num;
    
//     printf("total_file_num: %d, total_file_size: %d\n",total_file_num, 
//            total_file_size );

    return 0;
}

void mo_free_listdir(char ***files, char **allocated_memory)
{
    free(*files);
    *files = NULL;
    free(*allocated_memory);
    *allocated_memory = NULL;
}

int mo_listdir_old(const char *dir, const char *ext, char ***files, int *nfiles)
{
    #define DEFAULT_FILE_NAME_SPACE (4096)

    const char* pCur = ".";
    const char* pPre = "..";
    
    if(dir == NULL)
        return -1;

    *nfiles = 0;
    **files  = NULL;
    if( ! mo_is_folder_exist(dir) )
    {
        return -1;
    }

    char** ppFiles = *files;
    struct dirent *pDir;
    DIR *pD;
    pD = opendir(dir);

    int sizeLevel = 1;
    char* file_name_stack = (char*)malloc(DEFAULT_FILE_NAME_SPACE*sizeLevel);
    int indexBias[DEFAULT_FILE_NAME_SPACE];
    memset(indexBias, 0, DEFAULT_FILE_NAME_SPACE);
    char* pFileName = file_name_stack;
    //int nExtNum = 0;

    if(pD)
    {
        //calculate total file name len
        int file_name_len = 0;
        int cur_file_len = 0;
        while( (pDir = readdir(pD)) != NULL )
        {
            //filter current folder and previous folder
            if( strcmp(pDir->d_name, pCur) == 0 )
                continue;
            if( strcmp(pDir->d_name, pPre) == 0 )
                continue;

            //filter extension files
            char* pFile = strrchr(pDir->d_name, '.');
            //no extension
            if(NULL != ext)
            {
                //extension dot check
                const char* pExt = strrchr(ext, '.');
                if(pExt == NULL)
                    pExt = ext;
                else
                    pExt = pExt+1;

                if( pFile != NULL)
                {
                    if( strncmp(pFile+1, pExt, strlen(pExt)) != 0 )
                    {
                        continue;
                    }
                }else
                {
                    continue;
                }
            }
            cur_file_len = strlen(pDir->d_name);
            //if remain space not enough, double stack
            if( (file_name_len + cur_file_len + 1) > (DEFAULT_FILE_NAME_SPACE*sizeLevel) )
            {
                sizeLevel = sizeLevel<<1;
                file_name_stack = (char*)malloc(DEFAULT_FILE_NAME_SPACE*sizeLevel);
                memcpy(file_name_stack, pFileName, (DEFAULT_FILE_NAME_SPACE*(sizeLevel>>1)));
                free(pFileName);
                pFileName = file_name_stack;
            }

            //data save
            indexBias[*nfiles] = file_name_len;
            memcpy(file_name_stack+indexBias[*nfiles], pDir->d_name, cur_file_len);
            file_name_len += (cur_file_len+1);
            *(file_name_stack + file_name_len - 1) = '\0';
            *nfiles += 1;
            if(*nfiles >= DEFAULT_FILE_NAME_SPACE)
                break;
        }
        closedir(pD);
        //copy result to output
        char* pOutBuffer = (char*)malloc(file_name_len);
        ppFiles = (char**)malloc((*nfiles+1)*sizeof(char*));
        *files = ppFiles;
        memcpy(pOutBuffer, file_name_stack, file_name_len);
        for(int i = 0; i < *nfiles; i++)
        {
            *(ppFiles+i) = pOutBuffer + indexBias[i];
        }
        *(ppFiles+*nfiles) = pOutBuffer;
        qsort(*files, *nfiles, sizeof(char **), cmpstringp);

        //mo_free pre-buffer
        free(file_name_stack);
    }else
    {
        return -1;
    }
    /*printf("Extention file number: (%d/%d)\n", nExtNum, *nfiles);*/
    return 0;
}

void mo_free_listdir_old(char **files, int nfiles)
{
    free(*(files+nfiles));
    *files = NULL;
    free(files);
    return;
}

int mo_rmfile(const char* file_name)
{
    return remove(file_name);
}

// // Simple test
// int main(int argc, char *argv[])
// {
// #include <assert.h>
// 
//     assert(mo_is_folder_exist("/tmp") == 1);
//     assert(mo_is_file_exist("/home/pangbo/spf13-vim.sh") == 1);
//     int ret;
//     const char* dir_path = "/tmp/abc/";
//     assert(mo_is_folder_exist(dir_path) == 0);
//     ret  = mo_mkdir(dir_path);
//     assert(ret == 0);                     
//     assert(mo_is_folder_exist(dir_path) == 1);
//                                             
//     char *root, *ext;                     
//     ret = mo_split_ext("a.cpp", &root, &ext);
//     assert(ret == 0);                     
//     assert(strcmp(root, "a") == 0);       
//     assert(strcmp(ext, ".cpp") == 0);     
//     mo_free(root);                        
//     mo_free(ext);
//                                             
//     char *join;                           
//     ret = mo_join_two_path("/tmp/", "/Record/debug.bmp", &join);
//     assert(ret == 0);                     
//     assert(strcmp(join, "/tmp/Record/debug.bmp") == 0);
//     mo_free(join);
//     ret = mo_join_path(&join, 3, "tmp", "Record/Proposal", "debug.bmp");
//     assert(ret == 0);                     
//     assert(strcmp("tmp/Record/Proposal/debug.bmp", join) == 0);
//     mo_free(join);
//     const char *pDirPath = 
//         "/home/pangbo/data/dataset/minitest_png/20170426_1850_1940";
//     char **files;
//     int file_num;
//     char *pMem;
//     for(int i=0; i< 10000; i++) {
//         mo_listdir(pDirPath, "png", &files, &file_num, 1, &pMem);
//         
//         printf("fileNum %d\n", file_num);
//         for(int i=0; i<10; i++) {
//             printf("%s\n", files[i]);
//         }
//         mo_free_listdir(&files, &pMem);
//     }
//     
//     return 0;
// }


double moGetAvailableSpaceByByteUnit(const char* path, char byte_unit)
{
    double _available_size;
    struct statvfs stat;

    if (statvfs(path, &stat) != 0) 
    {
        return -1.0f;
    }else
    {
        _available_size = 1.0 * stat.f_bsize * stat.f_bavail;
        switch ( byte_unit ) {
            case 'b':	
            case 'B':	
                break;

            case 'k':	
            case 'K':	
                _available_size /= 1024.0f;
                break;

            case 'm':	
            case 'M':	
                _available_size /= (1024.0f*1024.0f);
                break;

            case 'g':	
            case 'G':	
                _available_size /= (1024.0f*1024.0f*1024.0f);
                break;

            default:	
                _available_size = -2.0f;
                break;
        }				/* -----  end switch  ----- */
        return _available_size;
    }
}

double moGetAvailableSpaceSize(const char* path)
{
    return moGetAvailableSpaceByByteUnit(path, 'b');
}
