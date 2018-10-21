/*
 *==============================================================================
 *
 *      Filename:  mo_sys.h
 *
 *   Description:  
 *
 *       Version:  1.0.0
 *       Created:  06/08/2018 15:57:36
 *      Revision:  none
 *      Compiler:  gcc
 *
 *        Author:  Jamin, junming.chen@metoak.loc
 *  Organization:  Copy Right Metoak Inc.
 *
 *==============================================================================
 */

#ifndef MO_SYS_cT4o3ew
#define MO_SYS_cT4o3ew

#include <stdio.h>
#include <stdlib.h>		//access()
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>
#include <libgen.h>  	//dirname()
#include <sys/statfs.h> //statfs
#include <sys/statvfs.h>


int system_exec(const char* cmdstring);


#ifdef __cplusplus
extern "C"{ 
#endif


#ifdef __cplusplus
}
#endif

#endif  /** end of MO_SYS_cT4o3ew define **/
