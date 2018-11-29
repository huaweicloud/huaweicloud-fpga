/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <getopt.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#include "securec.h"
#include "FPGA_Common.h"
#include "FPGA_CmdLog.h"

#ifdef    __cplusplus
extern "C"{
#endif

LOG_RECORD g_strLog =
{
    .acStr = "undefined",
    .pstrLoggers = NULL,
};

INT8 g_acErrNumBuf[ERR_NUM_BUFFER_SIZE] = { 0 };

LOGGER g_strLoggerKmsg =
{
    .pcName = "kmsg",
    .pfnLogInit = FPGA_LogInitKmsg,
    .pfnLog = FPGA_LogWriteKmsg,
};

INT32 g_lKmsgFd = 0;

INT8 *g_pacStr[] =
    {
        [LOG_LEVEL_ERROR] = "ERROR",
        [LOG_LEVEL_WARNING] = "WARNING",
        [LOG_LEVEL_INFO] = "INFO",
        [LOG_LEVEL_DEBUG] = "DEBUG",
        [LOG_LEVEL_END] = "UNKNOWN",
    };
INT8 *g_pacBuf[] =
{
    [LOG_LEVEL_ERROR] = "<3>",
    [LOG_LEVEL_WARNING] = "<4>",
    [LOG_LEVEL_INFO] = "<6>",
    [LOG_LEVEL_DEBUG] = "<7>",
    [LOG_LEVEL_END] = "",
};

/*******************************************************************************
Function     : FPGA_LogInit
Description  : Initialztion Log
Input        : None
Output       : None
Return       : 0:success other:fail
*******************************************************************************/
UINT32 FPGA_LogInit( void )
{
    UINT32 ulRet = SDKRTN_LOG_ERROR_BASE;

    /* Initialize the name of log module */
    ulRet = snprintf_s( g_strLog.acStr, LOG_MAX_STRING, ( sizeof( g_strLog.acStr ) - 1 ), "Fpga_Command" );
    if ( ulRet > ( sizeof( g_strLog.acStr ) - 1 ) )
    {
         printf( "Initialize log service failed.\r\n" );
         return SDKRTN_LOG_VSNPRINTF_ERROR;
    }

    /* Enable logging */
    ulRet = FPGA_LogEnable(  );
    if ( OK != ulRet )
    {
        printf( "Try to execute cmd with 'sudo' or use 'root' account.\r\n" );
        return ulRet;
    }

    return ulRet;
}
/*******************************************************************************
Function     : FPGA_LogEnable
Description  : Enable log module
Input        : None
Output       : None
Return       : 0:success other:fail
*******************************************************************************/
UINT32 FPGA_LogEnable( void )
{
    UINT32 ulRet = SDKRTN_LOG_ERROR_BASE;

    if ( ( !g_strLoggerKmsg.pfnLog ) || ( !g_strLoggerKmsg.pfnLogInit ) )
    {
        return SDKRTN_LOG_FUNC_ERROR;
    }
    
    /* Init */
    ulRet = g_strLoggerKmsg.pfnLogInit( );
    if ( ulRet )
    {
        return ulRet;
    }

    g_strLog.pstrLoggers = &g_strLoggerKmsg;

    return SDKRTN_LOG_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_LogGetLevel
Description  : Get the level of log
Input        : enLevel
Output       : None
Return       : Start address of the level
*******************************************************************************/
INT8 *FPGA_LogGetLevel( LOG_LEVEL enLevel )
{
    if ( enLevel >= LOG_LEVEL_END )
    {
        enLevel = LOG_LEVEL_END;
    }
    return g_pacStr[enLevel];
}

/*******************************************************************************
Function     : FPGA_LogStringErrNum
Description  : Tranform error number to string
Input        : iErrNum
Output       : None
Return       : Start address of the string
*******************************************************************************/
INT8 *FPGA_LogStringErrNum( INT32 iErrNum )
{
    INT32 lRet = ( INT32 )SDKRTN_LOG_ERROR_BASE;

    lRet = snprintf_s( g_acErrNumBuf, ERR_NUM_BUFFER_SIZE, ( sizeof( g_acErrNumBuf ) - 1 ), "err_num=%d", iErrNum );

    if ( ( UINT32 )lRet > ( sizeof( g_acErrNumBuf ) - 1 ) )
    {
        LOG_ERROR( "err_num  length too big %d.", lRet );
    }

    return g_acErrNumBuf;
}

/*******************************************************************************
Function     : FPGA_LogPrintString
Description  : Printf the input string
Input        : INT8 *format, ...
Output       : None
Return       : None
*******************************************************************************/
void FPGA_LogPrintString( INT8 *format, ...)
{
    va_list ap;
    INT32 lRet = ( INT32 )SDKRTN_LOG_ERROR_BASE;
    INT8 acMsgBuf[LOG_MESSAGE_LENGTH_MAX] = { 0 };

    //lint -e530
    va_start( ap, format );
    lRet = vsnprintf_s( acMsgBuf, LOG_MESSAGE_LENGTH_MAX, ( sizeof( acMsgBuf ) - 1 ), format, ap );
    va_end( ap );
    printf( "%s%s\n", ( size_t )lRet >= sizeof( acMsgBuf ) ? "( TRUNCATED ) " : "", acMsgBuf );
    fflush( stdout );

    return;
}

/*******************************************************************************
Function     : FPGA_LogStdoutMsg
Description  : Printf the message
Input        : INT8 *pcMsg
Output       : None
Return       : None
*******************************************************************************/
void FPGA_LogStdoutMsg( INT8 *pcMsg )
{
    if ( NULL == pcMsg )
    {
        return ;
    }
    printf( "%s\r\n", pcMsg );
    fflush( stdout );
    return ;
}

/*******************************************************************************
Function     : FPGA_LogProcMsg
Description  : Process log information
Input        : LOG_LEVEL enLevel, INT32 iLine, const INT8 *func, 
               const INT8 *format, va_list ap, INT32 lErrNum 
Output       : None
Return       : None
*******************************************************************************/
void FPGA_LogProcMsg( LOG_LEVEL enLevel, INT32 iLine,
                                const INT8 *func, const INT8 *format, va_list ap,
                                INT32 lErrNum )
{
    struct timespec strTs = { 0 };
    time_t strTs_s = 0;
    UINT64 ullTs_us = 0;
    struct tm strTm = { 0 };
    void *p = NULL;
    INT32 lRet  = ( INT32 )SDKRTN_LOG_ERROR_BASE;
    INT8 acMsgBuf[LOG_MESSAGE_LENGTH_MAX] = { 0 };
    UINT32 ulMsgLen = 0;
    UINT32 ulTimeLen = 0;

    if ( enLevel >= LOG_LEVEL_END )
    {
        enLevel = LOG_LEVEL_END;
    }

    if ( ( NULL == func ) || ( NULL == format ) )
    {
        return ;
    }

    /* Get real time of system */
    lRet = clock_gettime( CLOCK_REALTIME, &strTs );
    if ( OK != lRet )
    {
        return ;
    }
    strTs_s = strTs.tv_sec;

    /* Convert the system time to the global time */
    p = gmtime_r( &strTs_s, &strTm );
    if ( NULL == p )
    {
        return ;
    }

    /* Format the string of time */
    ulTimeLen = strftime( &acMsgBuf[ulMsgLen], sizeof( acMsgBuf ) - ulMsgLen, "%Y-%m-%dT%T", &strTm );

    if ( !ulTimeLen )
    {
        printf( "strftime( ) failed\r\n" );
        lRet = 0;
    }

    ulMsgLen += ulTimeLen;
    ullTs_us = strTs.tv_nsec / 1000;

    /* Save the microsecond information */
    //lint -e838
    lRet = snprintf_s( &acMsgBuf[ulMsgLen], ( size_t )( LOG_MESSAGE_LENGTH_MAX - ulMsgLen ),
                            ( size_t )( ( sizeof( acMsgBuf ) - ulMsgLen ) - 1 ), ".%06llu, ", ullTs_us );

    if ( ( size_t )lRet >= ( sizeof(acMsgBuf) - ulMsgLen ) )
    {
        FPGA_LogPrintString( "Snprintf_s() us failed %d", lRet );
        lRet = 0;
    }
    ulMsgLen += lRet;

    /* Save the name of log */
    lRet = snprintf_s( &acMsgBuf[ulMsgLen], ( size_t )( LOG_MESSAGE_LENGTH_MAX - ulMsgLen ),
                        ( size_t )( ( sizeof( acMsgBuf ) - ulMsgLen ) - 1 ), "%s, ", g_strLog.acStr );
    if ( ( size_t )lRet >= sizeof(acMsgBuf) - ulMsgLen )
    {
        FPGA_LogPrintString( "Snprintf_s( ) name failed %d", lRet );
        lRet = 0;
    }

    ulMsgLen += lRet;

    /* Save the level of log */
    lRet = snprintf_s( &acMsgBuf[ulMsgLen], ( size_t )(LOG_MESSAGE_LENGTH_MAX - ulMsgLen ),
                        ( size_t )( ( sizeof( acMsgBuf ) - ulMsgLen ) - 1 ), "%s, ", FPGA_LogGetLevel( enLevel ) );

    if ( ( size_t )lRet >= sizeof( acMsgBuf ) - ulMsgLen )
    {
        FPGA_LogPrintString( "Snprintf_s( ) log level failed %d", lRet );
        lRet = 0;
    }

    ulMsgLen += lRet;

    /* Save file name, line number and function name */
    lRet = snprintf_s( &acMsgBuf[ulMsgLen], ( size_t )(LOG_MESSAGE_LENGTH_MAX - ulMsgLen ),
                        ( size_t )( ( sizeof( acMsgBuf ) - ulMsgLen ) - 1 ), "%d: %s(): ", iLine, func );

    if ( ( size_t )lRet >= sizeof( acMsgBuf ) - ulMsgLen )
    {
        FPGA_LogPrintString( "Snprintf_s() file +line: func() failed %d", lRet );
        lRet = 0;
    }

    ulMsgLen += lRet;

    /* Save input information */
    lRet = vsnprintf_s( &acMsgBuf[ulMsgLen], ( size_t )( LOG_MESSAGE_LENGTH_MAX - ulMsgLen ),
            ( size_t )( ( sizeof( acMsgBuf ) - ulMsgLen ) - 1 ), format, ap );

    if ( ( size_t )lRet >= sizeof( acMsgBuf ) - ulMsgLen )
    {
        FPGA_LogPrintString( "Vsnprintf_s( ) msg failed %d", lRet );
        lRet = 0;
    }

    ulMsgLen += lRet;

    /* Save the error number */
    if ( lErrNum )
    {
        lRet = snprintf_s( &acMsgBuf[ulMsgLen], ( size_t )( LOG_MESSAGE_LENGTH_MAX - ulMsgLen ),
              ( size_t )( ( sizeof( acMsgBuf ) - ulMsgLen ) - 1 ), ": %s",
              FPGA_LogStringErrNum(lErrNum));

        if ( ( size_t )lRet >= sizeof( acMsgBuf ) - ulMsgLen )
        {
            FPGA_LogPrintString( "Snprintf_s( ) error message failed %d", lRet );
            lRet = 0;
        }

        ulMsgLen += lRet;
    }
    
    /* Add '\n' */
    lRet = snprintf_s( &acMsgBuf[ulMsgLen], ( size_t )( LOG_MESSAGE_LENGTH_MAX - ulMsgLen ),
                           ( size_t )( ( sizeof(acMsgBuf) - ulMsgLen ) - 1 ), "\n" );
    if ( ( size_t )lRet >= ( sizeof( acMsgBuf ) - ulMsgLen ) )
    {
        FPGA_LogPrintString( "Snprintf_s() new line failed %d", lRet );
        lRet = 0;
    }
    ulMsgLen += lRet;

    lRet = ( INT32 )g_strLog.pstrLoggers->pfnLog( enLevel, acMsgBuf );

    if ( SDKRTN_LOG_SUCCESS != lRet )
    {
        FPGA_LogStdoutMsg( acMsgBuf );
        FPGA_LogPrintString( "Could not log a message via the %s logger: %s",
        g_strLog.pstrLoggers->pcName, FPGA_LogStringErrNum( -lRet ) );
    }

    return ;
}

/*******************************************************************************
Function     : FPGA_LogErr
Description  : Record error log
Input        : INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ...
Output       : None
Return       : None
*******************************************************************************/
void FPGA_LogErr( INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ...)
{
    va_list ap;
    INT32 lErrNum = errno;
    
    if ( ( NULL == pcFunc ) || ( NULL == pcFormat ) )
    {
        return ;
    }

    //lint -e530
    va_start( ap, pcFormat );
    FPGA_LogProcMsg( LOG_LEVEL_ERROR,  lLine, pcFunc, pcFormat, ap, lErrNum );
    va_end( ap );
    errno = lErrNum;

    return;
}

/*******************************************************************************
Function     : FPGA_LogWarning
Description  : Record warning log
Input        : INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ...
Output       : None
Return       : None
*******************************************************************************/
void FPGA_LogWarning( INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ... )
{
    va_list ap;
    INT32 lErrNum = errno;

    if ( ( NULL == pcFunc ) || ( NULL == pcFormat ) )
    {
        return ;
    }

    //lint -e530
    va_start( ap, pcFormat );
    FPGA_LogProcMsg( LOG_LEVEL_WARNING, lLine, pcFunc, pcFormat, ap, lErrNum );
    va_end( ap );
    errno = lErrNum;
    return;
}

/*******************************************************************************
Function     : FPGA_LogInfo
Description  : Record information log
Input        : INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ...
Output       : None
Return       : None
*******************************************************************************/
void FPGA_LogInfo( INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ... )
{
    va_list ap;
    INT32 lErrNum = errno;

    if ( ( NULL == pcFunc ) || ( NULL == pcFormat ) )
    {
        return ;
    }

    //lint -e530
    va_start( ap, pcFormat );
    FPGA_LogProcMsg( LOG_LEVEL_INFO, lLine, pcFunc, pcFormat, ap, lErrNum );
    va_end( ap );
    errno = lErrNum;

    return;
}

/*******************************************************************************
Function     : FPGA_LogDebug
Description  : Record debug log
Input        : INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ...
Output       : None
Return       : None
*******************************************************************************/
void FPGA_LogDebug( INT32 lLine, const INT8 *pcFunc, const INT8 *pcFormat, ... )
{
    va_list ap;
    INT32 lErrNum = errno;

    if ( ( NULL == pcFunc ) || ( NULL == pcFormat ) )
    {
        return ;
    }

    //lint -e530
    va_start( ap, pcFormat );
    FPGA_LogProcMsg( LOG_LEVEL_DEBUG, lLine, pcFunc, pcFormat, ap, lErrNum );
    va_end( ap );
    errno = lErrNum;

    return;
}

/*******************************************************************************
Function     : FPGA_LogInitKmsg
Description  : Init kmsg
Input        : None
Output       : None
Return       : 0:success other:fail
*******************************************************************************/
UINT32 FPGA_LogInitKmsg( void )
{
    INT32 lFd = ERROR;

    lFd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
    
    if ( ERROR == lFd )
    {
        return SDKRTN_LOG_OPEN_ERROR;
    }

    g_lKmsgFd = lFd;

    return SDKRTN_LOG_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_LogKmsgStrLevel
Description  : Get kmsg level
Input        : LOG_LEVEL enLevel
Output       : None
Return       : Start address of the level
*******************************************************************************/
INT8 * FPGA_LogKmsgStrLevel( LOG_LEVEL enLevel )
{
    if ( enLevel >= LOG_LEVEL_END )
    {
        enLevel = LOG_LEVEL_END;
    }
    return g_pacBuf[enLevel];
}

/*******************************************************************************
Function     : FPGA_LogWriteLoop
Description  : Write log in a loop
Input        : INT32 lFd, void *pBuffer, UINT32 ulSize
Output       : None
Return       : 0:success other:fail
*******************************************************************************/
UINT32 FPGA_LogWriteLoop( INT32 lFd, void *pBuffer, UINT32 ulSize )
{
    INT32 lCur = 0;
    const UINT8 *ucpBuf = NULL;
    UINT32 ulSent = 0;

    if ( 0 == lFd )
    {
        return SDKRTN_LOG_INPUT_ERROR;
    }
    
    if ( NULL == pBuffer )
    {
        return SDKRTN_LOG_INPUT_ERROR;
    }

    ucpBuf = pBuffer;

    /* Write file */
    for ( ulSent = 0; ulSent < ulSize; ulSent += lCur )
    {
        do
        {
            lCur = write( lFd, ucpBuf + ulSent, ulSize - ulSent );
        } 
        while ( lCur == -1 && errno == EINTR );   /* reexecute write operation when it was interrupted by semaphore  */

        if ( ( lCur < 0 ) ||( 0 == lCur  ) )
        {
            return SDKRTN_LOG_WRITE_LOOP_ERROR;
        }
    }

    return SDKRTN_LOG_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_LogWriteKmsg
Description  : Write kmsg log
Input        : LOG_LEVEL enLevel, INT8 *pcMessage
Output       : None
Return       : 0:success other:fail
*******************************************************************************/
UINT32 FPGA_LogWriteKmsg( LOG_LEVEL enLevel, INT8 *pcMessage )
{
    UINT32 ulRet = SDKRTN_LOG_ERROR_BASE;
    INT8 acMsgBuf[LOG_MESSAGE_LENGTH_MAX + 8] = { 0 };
    size_t MsgLen = 0;

    if ( NULL == pcMessage )
    {
        return SDKRTN_LOG_INPUT_ERROR;
    }

    ulRet = snprintf_s( acMsgBuf, ( LOG_MESSAGE_LENGTH_MAX + 8 ), ( sizeof( acMsgBuf ) - 1 ), "%s%s",
          FPGA_LogKmsgStrLevel(enLevel), pcMessage );
    if ( ( size_t )ulRet >= sizeof( acMsgBuf ) )
    {
        return SDKRTN_LOG_SNPRINTF_ERROR;
    }
    MsgLen = ulRet;
    
    /*write the log to the kmsg file */
    ulRet = FPGA_LogWriteLoop( g_lKmsgFd, acMsgBuf, MsgLen );
    if ( SDKRTN_LOG_SUCCESS != ulRet )
    {
        return ulRet;
    }
    return SDKRTN_LOG_SUCCESS;
}

#ifdef    __cplusplus
}
#endif
