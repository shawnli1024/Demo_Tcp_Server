/// Copyright (c) 2017 Metoak.Co.Ltd. All rights reserved
/// \file mo_file_io.h
/// \brief
/// \author PangBo, bo.pang@metoak.com
/// \version 0.1
/// \date 2017-08-19

#ifndef MO_FILE_IO_H_HDCMZWRF
#define MO_FILE_IO_H_HDCMZWRF

#ifdef __cplusplus
extern "C"{ 
#endif

    /// \brief mo_is_folder_exist 检查目录是否存在
    ///
    /// \param path 文件夹路径
    ///
    /// \return 1:存在 0:不存在
    int mo_is_folder_exist(const char* path);


    /// \brief mo_is_file_exist 检查文件(支持所有类型)是否存在
    ///
    /// \param path 文件路径
    ///
    /// \return 1:存在 0:不存在
    int mo_is_file_exist(const char* path);


    /// \brief mo_mkdir 建立文件夹
    ///
    /// \param path 文件夹路径,支持递归
    ///
    /// \return -1:failure, 0:success
    int mo_mkdir(const char* path);


    /// \brief mo_split_path 将路径分成父路径和当前目录或文件
    ///     比如将 /a/b/c.cpp 分成/a/b 和 c.cpp
    ///     注意:使用完之后需要手动将dir_name和base_name 内存释放
    /// \param path 待分解路径名
    /// \param dir_name 路径名称
    /// \param base_name 最后目录或文件的名称
    ///
    /// \return 0:success; -1:path is NULL; -2: format wrong;
    int mo_split_path(const char *path, char **dir_name, char **base_name);


    /// \brief mo_split_ext 分离拓展名, path = root + ext;
    ///     a.cpp 分解为  a 和　.cpp
    ///     注意:使用完之后需要手动将dir_name和base_name 内存释放
    /// \param path
    /// \param root
    /// \param ext
    ///
    /// \return -1:failure, 0:success
    int mo_split_ext(const char *path, char **root, char **ext);


    /// \brief mo_join_two_path 连接两个路径名
    ///     例如path1="/tmp/" path="/Record/debug.bmp", joined_path="/tmp/Record/debug/bmp"
    ///     注意：需要手动释放joined_path的内存
    /// \param path1 目录1， 可为空字符串""，不可为NULL
    /// \param path2 目录2， 可为空字符串""，不可为NULL
    /// \param joined_path 合成的目录
    ///
    /// \return -1:failure, 0:success
    int mo_join_two_path(const char *path1, const char *path2, char **joined_path);


    /// \brief mo_join_path 连接多个路径名
    ///     比如mo_join_path(path, 3, "tmp", "Record/Proposal", "debug.bmp")
    ///         path返回: tmp/Record/Proposal/debug.bmp
    ///     注意：需要手动释放dst的内存
    /// \param dst 返回的目标路径
    /// \param path_num 路径数量
    /// \param ... 多个路径
    ///
    /// \return -1:failure, 0:success
    int mo_join_path(char **dst, const int path_num, ...);

    /// \brief mo_file_copy 复制文件
    ///
    /// \param src 源文件路径名
    /// \param dst 目标文件路径名
    ///
    /// \return -1:src not exist; -2: dst not exist; 0:success
    int mo_file_copy(const char *src, const char *dst);

    /// \brief mo_file_clear 清空文件
    ///
    /// \param file_name
    ///
    /// \return -1:failure, 0:success
    int mo_file_clear(const char* file_name);

    /// \brief mo_file_touch 新建文件
    ///
    /// \param file_name
    ///
    /// \return  0:success;
    ///         -1: file_name input error;
    ///         -2: can't mkdir parent dir
    ///         -3: can't open the file
    int mo_file_touch(const char *file_name);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief  mo_get_file_size :  Get file size in byte unit
     *
     * @param file_name : full name of a file
     *
     * @return : if success, return file size
     *            if failure, return -1
     */
    /* ----------------------------------------------------------------------------*/
    int mo_get_file_size(const char *file_name);

    /* --------------------------------------------------------------------------*/
    /**
     * @brief  mo_listdir : list current directory content with ext support
     *          eg: mo_listdir( "/tmp", ".log", files, nfiles);
     *          eg: mo_listdir( "/tmp", NULL, files, nfiles);
     *
     * @param dir INPUT DIRECTORY
     * @param ext EXTENSION OF FILE(CAN BE NULL, MEANS LIST ALL FILE AND DIRECTORY)
     * @param files OUTPUT FILES IN POINTER LIST FORMAT
     * @param nfiles TOTAL FILE/DIRECTORY NUMBERS
     * @param sorted DIR FILENAME IS sorted or not
     *
     * @return
     *      -1: failed, invalid input.
     *      0: success.
     */
    /* ----------------------------------------------------------------------------*/
    int mo_listdir(const char *dir, const char *ext, char ***files, int *nfiles, int sorted, char **allocated_memory);

    /* --------------------------------------------------------------------------*/
    /**
     * @brief  mo_free_listdir : free FILES in mo_get_file_size funcitons
     *
     * @param files     FILE POINTER LIST IN mo_get_file_size
     * @param nfiles    TOTAL FILE/DIRECTORY NUMBERS
     */
    /* 
       ----------------------------------------------------------------------------*/
    //void mo_free_listdir(char **files, char *allocated_memory);
    void mo_free_listdir(char ***files, char **allocated_memory);

    //DEPRECATED
    void mo_free_listdir_old(char **files, int nfiles);

    //DEPRECATED
    int mo_listdir_old(const char *dir, const char *ext, char ***files, int *nfiles);

    int mo_rmfile(const char* file_name);

    /* --------------------------------------------------------------------------*/
    /**
     * @brief  moGetAvailableSpaceSize : Get Filesystem Available Space Size By Path
     *
     * @param path      filesystem path
     *
     * @returns     available size in byte
     */
    /* ----------------------------------------------------------------------------*/
    double moGetAvailableSpaceSize(const char* path);

    /* --------------------------------------------------------------------------*/
    /**
     * @brief  moGetAvailableSpaceByByteUnit :
     *          Get Filesystem Available Space Size By Path
     *          eg: moGetAvailableSpaceByByteUnit("/", 'm'); // get root free space in M unit
     *
     * @param path      filesystem path
     *
     * @param byte_unit     support byte unit: b,k,m,g
     *
     * @returns     normal return : available size in (b,k,m,g)byte
     *              invalid return: < 0.0f
     */
    /* ----------------------------------------------------------------------------*/
    double moGetAvailableSpaceByByteUnit(const char* path, char byte_unit);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: MO_FILE_IO_H_HDCMZWRF */
