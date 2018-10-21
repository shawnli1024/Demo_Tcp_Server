/*
 *==============================================================================
 *
 *      Filename:  mo_sys.c
 *
 *   Description:  
 *
 *       Version:  0.0.1
 *       Created:  06/08/2018 15:58:15
 *      Revision:  none
 *      Compiler:  gcc
 *
 *        Author:  Jamin, junming.chen@metoak.loc
 *  Organization:  Copy Right Metoak Inc.
 *
 *==============================================================================
 */

#include "mo_sys.h"



/* --------------------------------------------------------------------------*/
/**
 * @synopsis  system_exec 
 *          call system and return return-code
 *
 * @param cmdstring
 *
 * @returns  0: success 
 *           other: failed
 */
/* ----------------------------------------------------------------------------*/
int system_exec(const char* cmdstring){
    if(NULL == cmdstring){
        return -1;
    }

    int status;

    status = system(cmdstring);
    if(status != 0)
    {
        fprintf(stderr, "cmd: %s\t error: %s\n", cmdstring, strerror(errno)); // Ouput or log errno info
        return -1;
    }

    if(WIFEXITED(status))
    {
        //get cmdstring return result

        if (0 == WEXITSTATUS(status))
        {
            //printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
            return 0;
        }
        else
        {
            //printf("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));
            return -1;
        }
    }
    else if(WIFSIGNALED(status) || WIFSTOPPED(status))
    {
        //if cmdstring is terminated by SIG, get the SIG value
        //printf("abnormal termination,signal number =%d\n", WTERMSIG(status));

        //if cmdstring is suspended by SIG, get the SIG value
        //printf("process stopped, signal number =%d\n", WSTOPSIG(status));

        return -1;
    }

    return -1;

}

