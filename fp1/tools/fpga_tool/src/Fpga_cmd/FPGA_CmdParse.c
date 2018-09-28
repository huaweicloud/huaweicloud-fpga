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
#include "FPGA_CmdCommon.h"
#include "FPGA_CmdParse.h"
#include "FPGA_CmdMonitorMain.h"

#ifdef    __cplusplus
extern "C"{
#endif

extern UINT32 g_ulparseParaFlag;

INT8 *g_pacCommandEntryHelp[12] =
{
    "  Summary:",
    "    FpgaCmdEntry [Opcode] [-h/-?]",
    "  Description:",
    "     This procedure is the entry of all commands, through",
    "     the operation code to achieve the user's intention",
    "  Opcode:",
    "     LF --Load the fpga image",
    "     CF --Clear the fpga image",
    "     DF --Display the fpga resource maps",
    "     IF --Inquire the fpga image status",
    "     IL --Inquire the user led status",
    "     IV --Display the tool version"
};

INT8 *g_pacHfiLoadHelp[16] =
{
    "  Summary:",
    "      FpgaCmdEntry LF [Parameters]",
    "      Example: FpgaCmdEntry LF -S <slot num> -I <AEI id>",
    "               FpgaCmdEntry LF -h",
    "               FpgaCmdEntry LF -?",
    "  Description:",
    "      Load user's FPGA image to the specified slot number, and",
    "      return the status of the command. ",
    "  Parameters:",
    "      -S, --FpgaSlotInfo",
    "          The logical slot number for the FPGA image,which inquired by,range:0~7",
    "          execute 'FpgaCmdEntry DF' result.",
    "      -I, --Fpga-AEI-Id",
    "          The user's FPGA image id.",
    "      -h/-?, --Help",
    "          Display this help."
};

INT8 *g_pacHfiClearHelp[14] =
{
    "  Summary:",
    "      FpgaCmdEntry CF [Parameters]",
    "      Example: FpgaCmdEntry CF -S <slot num>",
    "               FpgaCmdEntry CF -h",
    "               FpgaCmdEntry CF -?",
    "  Description:",
    "      Clear user's FPGA image of the specified slot number, and",
    "      return the status of the command. ",
    "  Parameters:",
    "      -S, --FpgaSlotInfo",
    "          The logical slot number for the FPGA image,which inquired by,range:0~7",
    "          execute 'FpgaCmdEntry DF' result.",
    "      -h/-?, --Help",
    "          Display this help."
};

INT8 *g_pacInquireFpgaHelp[12] =
{
    "  Summary:",
    "      FpgaCmdEntry DF [Parameters]",
    "      Example: FpgaCmdEntry DF -D",
    "               FpgaCmdEntry DF -h",
    "               FpgaCmdEntry DF -?",
    "  Description",
    "      Display the FPGA map information of the user's vm.",
    "  Parameters:",
    "      -D, --FpgaInfo",
    "          Display the FPGA map information.",
    "      -h/-?, --help",
    "          Display this help."
};

INT8 *g_pacInquireImageHelp[14] =
{
    "  Summary:",
    "      FpgaCmdEntry IF [Parameters]",
    "      Example: FpgaCmdEntry IF -S <slot num>",
    "               FpgaCmdEntry IF -h",
    "               FpgaCmdEntry IF -?",
    "  Description:",
    "      Display the image status of the specified slot.Includes: AEI ID,",
    "      FPGA status,slot,DBDF...",
    "  Parameters:",
    "      -S, --fpga-image-slot",
    "          The logical slot number for the FPGA image.",
    "          Constraints: Positive integer from 0 to the total slots minus 1.",
    "      -h/-?, --help",
    "          Display this help.",
};

INT8 *g_pacInquireLedStatusHelp[14] =
{
    "  Summary:",
    "      FpgaCmdEntry IL [Parameters]",
    "      Example: FpgaCmdEntry IL -S <slot num>",
    "               FpgaCmdEntry IL -h",
    "               FpgaCmdEntry IL -?",
    "  Description:",
    "      Display the LED status of the specified slot.",
    "      General purpose architecture device doesn't support user LED.",
    "  Parameters:",
    "      -S, --fpga-image-slot",
    "          The logical slot number for the FPGA image.",
    "          Constraints: Positive integer from 0 to the total slots minus 1.",
    "      -h/-?, --help",
    "          Display this help.",
};

/*******************************************************************************
Function     : FPGA_ParseShowVersion
Description  : Display the version of tool
Input        : INT32 argc, INT8 *argv[]
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_ParseShowVersion( INT32 argc, INT8 *argv[] )
{
    if ( argc > INPUT_PARAS_NUM_MIN )
    {
        printf( "[***TIPS***] CMD-IV Input parameter number shouldn't be bigger than %d.\r\n", INPUT_PARAS_NUM_MIN );
        return SDKRTN_PARSE_INPUT_ERROR;
    }
    (void)argv;
    printf("FPGA Management Tools Version: %s\r\n", HFI_TOOL_VERSION);
    g_ulparseParaFlag = QUIT_FLAG;
    return SDKRTN_PARSE_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_ParseString2Uint
Description  : Convert a string to an unsigned integer
Input        : const INT8 *pcSstr
Output       : UINT32 *plNum
Return       : 0:sucess other:fail  
*******************************************************************************/
UINT32 FPGA_ParseString2Uint( UINT32 *plNum, const INT8 *pcSstr )
{
    INT8 *pcEnd = NULL;
    INT32 lVal = ( INT32 )SDKRTN_PARSE_ERROR_BASE;

    if ( NULL == plNum  )
    {
        LOG_ERROR( " FPGA_ParseString2Uint plNum is null  " );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( NULL == pcSstr  )
    {
        LOG_ERROR( " FPGA_ParseString2Uint pcSstr is null  " );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( 0 == *pcSstr )
    {
         LOG_ERROR( " FPGA_ParseString2Uint input null string " );
         return SDKRTN_PARSE_INPUT_ERROR;
    }

    errno = 0;

    lVal = ( INT32 )strtol( pcSstr, &pcEnd, 10 );

    /* The string has illegal characters */
    if ( *pcEnd )
    {
        LOG_ERROR( " FPGA_ParseString2Uint input is illegal character " );
        return SDKRTN_PARSE_INVALID_CHAR_ERROR;
    }

    /* overrange */
    if ( errno )
    {
        LOG_ERROR( " FPGA_ParseString2Uint input is out of range " );
        return SDKRTN_PARSE_INVALID_RANGE_ERROR;
    }

    if ( lVal < 0 )
    {
        LOG_ERROR( " FPGA_ParseString2Uint input is not match " );
        return SDKRTN_PARSE_INVALID_VALUE_ERROR;
    }

    *plNum = ( UINT32 )lVal;
    return SDKRTN_PARSE_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_ParsePrintHelpInfo
Description  : Print the help information
Input        : INT8 *pcCmdName,  INT8 *pcBuf[], UINT32 ulNum
Output       : None
Return       : 0:sucess other:fail  
*******************************************************************************/
void  FPGA_ParsePrintHelpInfo( INT8 *pcCmdName,  INT8 *pcBuf[], UINT32 ulNum )
{
    INT32 i = 0;

    if ( ( NULL == pcCmdName ) || ( NULL == pcBuf ) )
    {
        return;
    }

    printf( "%s\r\n", pcCmdName );

    for ( i = 0; i < ulNum; i++ )
    {
        printf( "%s\r\n", pcBuf[i] );
    }

    return;
}

/*******************************************************************************
Function     : FPGA_ParseClearHfi
Description  : Parse the load command
Input        : INT32 argc, INT8 *argv[] 
Output       : None
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_ParseClearHfi( INT32 argc, INT8 *argv[] )
{
    INT32 lOpt = 0;
    INT32 lHelpPrintedCount = 0;
    struct option strLongOptions[] =
    {
        {"FpgaSlotInfo", required_argument, 0, FPGA_SLOT_INFO},
        {"Helph", no_argument, 0, COMMAND_HELP_INFO},
        {"Help?", no_argument, 0, COMMAND_HELP_INFO1},
        {0, 0, 0, 0},
    };
    INT32 lLongIndex = 0;
    UINT32 ulRet = SDKRTN_PARSE_ERROR_BASE;
    UINT32 ulParaFlag = 0;

    if ( argc < INPUT_PARAS_FOR_PARSE_MIN )
    {
        printf( "[***TIPS***] CMD-CF Input parameter number shouldn't be less than %d.\r\n", INPUT_PARAS_FOR_PARSE_MIN );
        FPGA_ParsePrintHelpInfo(argv[0], g_pacHfiClearHelp, sizeof_array(g_pacHfiClearHelp));
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( argc > INPUT_PARAS_FOR_CF_MAX )
    {
        printf( "[***TIPS***] CMD-CF Input parameter number shouldn't be more than %d.\r\n", INPUT_PARAS_FOR_CF_MAX );
        FPGA_ParsePrintHelpInfo(argv[0], g_pacHfiClearHelp, sizeof_array(g_pacHfiClearHelp));
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    /* Check the format of the parameter */
    if ( '-' != *argv[2] )
    {
        printf("Parameter format is incorrect and should be prefixed '-'.\r\n");
        FPGA_ParsePrintHelpInfo(argv[0], g_pacHfiClearHelp, sizeof_array(g_pacHfiClearHelp));
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    /* Parse the input parameters */
    while ( ( lOpt = getopt_long( argc, argv, "S:?h",strLongOptions, &lLongIndex ) ) != ERROR )
    {
        ulParaFlag  = PARA_FLAG;
        switch ( lOpt )
        {
            /* -S */
            case FPGA_SLOT_INFO:
            {
                ulRet = FPGA_ParseString2Uint( &g_strFpgaModule.ulSlotIndex, optarg );
                if ( SDKRTN_PARSE_SUCCESS != ulRet )
                {
                    printf("[***TIPS***]Option -S should be followed by a decimal number\r\n" );
                    LOG_ERROR( "String2Uint failed %d", ulRet );
                    return ulRet;
                }

                if ( g_strFpgaModule.ulSlotIndex >= FPGA_SLOT_MAX )
                {
                    printf( "Fpga slot number(%u) must be less than %d\r\n", g_strFpgaModule.ulSlotIndex, FPGA_SLOT_MAX );
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiClearHelp, sizeof_array( g_pacHfiClearHelp ) );
                    return SDKRTN_PARSE_SLOT_ERROR;
                }
                break;
            }

            /* -h */
            case COMMAND_HELP_INFO:
            {
                FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiClearHelp, sizeof_array( g_pacHfiClearHelp ) );
                g_ulparseParaFlag = QUIT_FLAG;
                return SDKRTN_PARSE_SUCCESS;
            }

            /* -? */
            case COMMAND_HELP_INFO1:
            {
                if( PRINTED_COUNT == lHelpPrintedCount )
                {
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiClearHelp, sizeof_array( g_pacHfiClearHelp ) );
                    g_ulparseParaFlag = QUIT_FLAG;
                    lHelpPrintedCount = 1;
                    ( void )lHelpPrintedCount;
                    return SDKRTN_PARSE_SUCCESS;
                }
            }

            //lint -fallthrough
            default:
            {
                printf( "\r\nERROR: Invalid input parameter.\r\n" );
                FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiClearHelp, sizeof_array( g_pacHfiClearHelp ) );
                return SDKRTN_PARSE_INVALID_PARA_ERROR;
            }
        }
    }

    /* Exception handling */
    if ( ( ERROR == lOpt ) && ( 0 == ulParaFlag ) ) /*lint !e774*/
    {
        printf( "\r\nERROR: Invalid input parameter.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiClearHelp, sizeof_array( g_pacHfiClearHelp ) );
        return SDKRTN_PARSE_INVALID_PARA_ERROR;
    }

    return SDKRTN_PARSE_SUCCESS;
}


/*******************************************************************************
Function     : FPGA_ParseLoadHfi
Description  : Parse the load command
Input        : INT32 argc, INT8 *argv[] 
Output       : None
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_ParseLoadHfi( INT32 argc, INT8 *argv[] )
{
    INT32 lOpt = 0;
    INT32 lHelpPrintedCount = 0;
    struct option strLongOptions[] =
    {
        {"FpgaSlotInfo", required_argument, 0, FPGA_SLOT_INFO},
        {"FpgaFiId", required_argument, 0, HFI_ID_INFO},
        {"Helph", no_argument, 0, COMMAND_HELP_INFO},
        {"Help?", no_argument, 0, COMMAND_HELP_INFO1},
        {0, 0, 0, 0},
    };
    INT32 lLongIndex = 0;
    UINT32 ulRet = SDKRTN_PARSE_ERROR_BASE;
    UINT32 ulParaFlag = 0;

    if ( argc < INPUT_PARAS_FOR_PARSE_MIN )
    {
        printf( "[***TIPS***] CMD-LF Input parameter number shouldn't be less than %d.\r\n", INPUT_PARAS_FOR_PARSE_MIN );
        FPGA_ParsePrintHelpInfo(argv[0], g_pacHfiLoadHelp, sizeof_array(g_pacHfiLoadHelp));
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( argc > INPUT_PARAS_FOR_LF_MAX )
    {
        printf( "[***TIPS***] CMD-LF Input parameter number shouldn't be more than %d.\r\n", INPUT_PARAS_FOR_LF_MAX );
        FPGA_ParsePrintHelpInfo(argv[0], g_pacHfiLoadHelp, sizeof_array(g_pacHfiLoadHelp));
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    /* Check the format of the parameter */
    if ( '-' != *argv[2] )
    {
        printf("Parameter format is incorrect and should be prefixed '-'.\r\n");
        FPGA_ParsePrintHelpInfo(argv[0], g_pacHfiLoadHelp, sizeof_array(g_pacHfiLoadHelp));
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    /* Parse the input parameters */
    while ( ( lOpt = getopt_long( argc, argv, "S:I:?h",strLongOptions, &lLongIndex ) ) != ERROR )
    {
        ulParaFlag  = PARA_FLAG;
        switch ( lOpt )
        {
            /* -S */
            case FPGA_SLOT_INFO:
            {
                ulRet = FPGA_ParseString2Uint( &g_strFpgaModule.ulSlotIndex, optarg );
                if ( SDKRTN_PARSE_SUCCESS != ulRet )
                {
                    printf("[***TIPS***]Option -S should be followed by a decimal number\r\n" );
                    LOG_ERROR( "String2Uint failed %d", ulRet );
                    return ulRet;
                }

                if ( g_strFpgaModule.ulSlotIndex >= FPGA_SLOT_MAX )
                {
                    printf( "Fpga slot number(%u) must be less than %d\r\n", g_strFpgaModule.ulSlotIndex, FPGA_SLOT_MAX );
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiLoadHelp, sizeof_array( g_pacHfiLoadHelp ) );
                    return SDKRTN_PARSE_SLOT_ERROR;
                }
                break;
            }

            /* -I */
            case HFI_ID_INFO:
            {
                if ( HFI_ID_LEN != strnlen( optarg, HFI_ID_LEN + 1) )
                {
                    printf( "Input AEI id length must be %d bytes\r\n", HFI_ID_LEN );
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiLoadHelp, sizeof_array( g_pacHfiLoadHelp ) );
                    return SDKRTN_PARSE_HFI_ID_ERROR;
                }

                ulRet = strncpy_s( g_strFpgaModule.acHfiId, ( size_t )HFI_ID_LEN_MAX, optarg, ( sizeof( g_strFpgaModule.acHfiId ) - 1  ) );
                if ( OK != ulRet )
                {
                    LOG_ERROR( "ParaParseForFiLoad strncpy_s failed" );
                    return SDKRTN_PARSE_STRNCPY_ERROR;
                }

                g_strFpgaModule.acHfiId[sizeof(g_strFpgaModule.acHfiId) - 1] = '\0';
                break;
            }

            /* -h */
            case COMMAND_HELP_INFO:
            {
                FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiLoadHelp, sizeof_array( g_pacHfiLoadHelp ) );
                g_ulparseParaFlag = QUIT_FLAG;
                return SDKRTN_PARSE_SUCCESS;
            }

            /* -? */
            case COMMAND_HELP_INFO1:
            {
                if( PRINTED_COUNT == lHelpPrintedCount )
                {
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiLoadHelp, sizeof_array( g_pacHfiLoadHelp ) );
                    g_ulparseParaFlag = QUIT_FLAG;
                    lHelpPrintedCount = 1;
                    ( void )lHelpPrintedCount;
                    return SDKRTN_PARSE_SUCCESS;
                }
            }

            //lint -fallthrough
            default:
            {
                printf( "\r\nERROR: Invalid input parameter.\r\n" );
                FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiLoadHelp, sizeof_array( g_pacHfiLoadHelp ) );
                return SDKRTN_PARSE_INVALID_PARA_ERROR;
            }
        }
    }

    /* Exception handling */
    if ( ( ERROR == lOpt ) && ( 0 == ulParaFlag ) ) /*lint !e774*/
    {
        printf( "\r\nERROR: Invalid input parameter.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacHfiLoadHelp, sizeof_array( g_pacHfiLoadHelp ) );
        return SDKRTN_PARSE_INVALID_PARA_ERROR;
    }

    return SDKRTN_PARSE_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_ParseInquireFpgaResource
Description  : Parse the command of inquire fpga resource 
Input        : INT32 argc, INT8 *argv[] 
Output       : None
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_ParseInquireFpgaResource( INT32 argc, INT8 *argv[] )
{
    INT32 lOpt = 0;
    INT32 lHelpPrintedCount = 0;
    struct option strLongOptions[] =
    {
        {"FpgaInfo", no_argument, 0, DISPLAY_FPGA_PHY_INFO},
        {"Helph", no_argument, 0, COMMAND_HELP_INFO},
        {"Help?", no_argument, 0, COMMAND_HELP_INFO1},
        {0, 0, 0, 0},
    };
    INT32 lLongIndex = 0;

    if ( argc < INPUT_PARAS_FOR_PARSE_MIN )
    {
        printf( "[***TIPS***] CMD-DF Input parameter number shouldn't be less than %d.\r\n", INPUT_PARAS_FOR_PARSE_MIN );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireFpgaHelp, sizeof_array( g_pacInquireFpgaHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( argc > INPUT_PARAS_FOR_DF_MAX )
    {
        printf( "[***TIPS***] CMD-DF Input parameter number shouldn't be more than %d.\r\n", INPUT_PARAS_FOR_DF_MAX );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireFpgaHelp, sizeof_array( g_pacInquireFpgaHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    /* Check the format of the parameter */
    if ( '-' != *argv[2] )
    {
        printf( "Parameter format is incorrect and should be prefixed '-'. " );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireFpgaHelp, sizeof_array( g_pacInquireFpgaHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    /* Parse the input parameters */
    while ( ( lOpt = getopt_long( argc, argv, "D?h", strLongOptions, &lLongIndex ) ) != ERROR )
    {
        switch( lOpt )
        {

            /* -D */
            case DISPLAY_FPGA_PHY_INFO:
            {
                g_strFpgaModule.bShowInfo = true;
                return SDKRTN_PARSE_SUCCESS;
            }

            /* -h */
            case COMMAND_HELP_INFO:
            {
                FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireFpgaHelp, sizeof_array( g_pacInquireFpgaHelp ) );
                g_ulparseParaFlag = QUIT_FLAG;
                return SDKRTN_PARSE_SUCCESS;
            }
            
            /* - ? */
            case COMMAND_HELP_INFO1:
            {
                if( PRINTED_COUNT == lHelpPrintedCount )
                {
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireFpgaHelp, sizeof_array( g_pacInquireFpgaHelp ) );
                    g_ulparseParaFlag = QUIT_FLAG;
                    lHelpPrintedCount = 1;
                    ( void )lHelpPrintedCount;
                    return SDKRTN_PARSE_SUCCESS;
                }
            }

            //lint -fallthrough
            default:
            {
                printf( "\r\nERROR: Invalid input parameter.\r\n" );
                FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireFpgaHelp, sizeof_array(g_pacInquireFpgaHelp ) );
                return SDKRTN_PARSE_INVALID_PARA_ERROR;
            }
        }
    }
    
    /* Exception handling */
    if ( ERROR == lOpt ) /*lint !e774*/
    {
        printf( "\r\nERROR: Invalid input parameter.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireFpgaHelp, sizeof_array(g_pacInquireFpgaHelp ) );
        return SDKRTN_PARSE_INVALID_PARA_ERROR;
    }

    return SDKRTN_PARSE_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_ParseInquireFpgaImageInfo
Description  : Parse the command of inquire image information
Input        : INT32 argc, INT8 *argv[] 
Output       : None
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_ParseInquireFpgaImageInfo( INT32 argc, INT8 *argv[] )
{
    INT32 lOpt = 0;
    INT32 lHelpPrintedCount = 0;

    struct option stLongOptions[] =
    {
        {"FpgaSlotInfo", required_argument, 0, FPGA_SLOT_INFO},
        {"Helph", no_argument, 0, COMMAND_HELP_INFO},
        {"Help?", no_argument, 0, COMMAND_HELP_INFO1},
        {0, 0, 0, 0},
    };

    INT32 lLongIndex = 0;
    UINT32 ulRet = SDKRTN_PARSE_ERROR_BASE;

    if ( argc < INPUT_PARAS_FOR_PARSE_MIN )
    {
        printf( "[***TIPS***] CMD-IF Input parameter number shouldn't be less than %d.\r\n", INPUT_PARAS_FOR_PARSE_MIN );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array( g_pacInquireImageHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( argc > INPUT_PARAS_FOR_IF_MAX )
    {
        printf( "[***TIPS***] CMD-IF Input parameter number shouldn't be more than %d.\r\n", INPUT_PARAS_FOR_IF_MAX );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array( g_pacInquireImageHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( '-' != *argv[2] )
    {
        printf("[***TIPS***] Parameter format is incorrect and should be prefixed '-'.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array( g_pacInquireImageHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    while ( ( lOpt = getopt_long( argc, argv, "S:?h", stLongOptions, &lLongIndex ) ) != ERROR )
    {
        switch (lOpt)
        {

            /* -S */
            case FPGA_SLOT_INFO:
            {
                ulRet = FPGA_ParseString2Uint( &g_strFpgaModule.ulSlotIndex, optarg );
                if ( SDKRTN_PARSE_SUCCESS != ulRet )
                {
                    printf("[***TIPS***]Option -S should be followed by a decimal number\r\n" );
                    LOG_ERROR( "String2Uint failed %d", ulRet );
                    return ulRet;
                }

                if ( g_strFpgaModule.ulSlotIndex >= FPGA_SLOT_MAX )
                {
                    printf( "Fpga slot number(%u) must be less than %d.\r\n", g_strFpgaModule.ulSlotIndex, FPGA_SLOT_MAX );
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array( g_pacInquireImageHelp ) );
                    return SDKRTN_PARSE_SLOT_ERROR;
                }
                return SDKRTN_PARSE_SUCCESS;
            }

            /* -h */
            case COMMAND_HELP_INFO:
            {
                FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array( g_pacInquireImageHelp ) );
                g_ulparseParaFlag = QUIT_FLAG;
                return SDKRTN_PARSE_SUCCESS;
            }

            /* -? */
            case COMMAND_HELP_INFO1:
            {
                if( PRINTED_COUNT == lHelpPrintedCount )
                {
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array(g_pacInquireImageHelp ) );
                    g_ulparseParaFlag = QUIT_FLAG;
                    lHelpPrintedCount = 1;
                    ( void )lHelpPrintedCount;
                    return SDKRTN_PARSE_SUCCESS;
                }
            }

            //lint -fallthrough
            default:
            {
                printf( "\r\nERROR: Invalid input parameter.\r\n" );
                FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array( g_pacInquireImageHelp ) );
                return SDKRTN_PARSE_INVALID_PARA_ERROR;
            }
        }
    }

    /* Exception handling */
    if ( ERROR == lOpt ) /*lint !e774*/
    {
        printf( "\r\nERROR: Invalid input parameter.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireImageHelp, sizeof_array( g_pacInquireImageHelp ) );
        return SDKRTN_PARSE_INVALID_PARA_ERROR;
    }
    
    return SDKRTN_PARSE_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_ParseInquireLEDStatus
Description  : Parse the command of inquire led status
Input        : INT32 argc, INT8 *argv[] 
Output       : None
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_ParseInquireLEDStatus( INT32 argc, INT8 *argv[] )
{
    INT32 lOpt = 0;
    INT32 lHelpPrintedCount = 0;

    struct option stLongOptions[] =
    {
        {"FpgaSlotInfo", required_argument, 0, FPGA_SLOT_INFO},
        {"Helph", no_argument, 0, COMMAND_HELP_INFO},
        {"Help?", no_argument, 0, COMMAND_HELP_INFO1},
        {0, 0, 0, 0},
    };

    INT32 lLongIndex = 0;
    UINT32 ulRet = SDKRTN_PARSE_ERROR_BASE;

    if ( argc < INPUT_PARAS_FOR_PARSE_MIN )
    {
        printf( "[***TIPS***] CMD-IL Input parameter number shouldn't be less than %d.\r\n", INPUT_PARAS_FOR_PARSE_MIN );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireLedStatusHelp, sizeof_array( g_pacInquireLedStatusHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( argc > INPUT_PARAS_FOR_IL_MAX )
    {
        printf( "[***TIPS***] CMD-IL Input parameter number shouldn't be more than %d.\r\n", INPUT_PARAS_FOR_IL_MAX );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireLedStatusHelp, sizeof_array( g_pacInquireLedStatusHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    if ( '-' != *argv[2] )
    {
        printf( "Parameter format is incorrect and should be prefixed '-'.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireLedStatusHelp, sizeof_array( g_pacInquireLedStatusHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    while ( ( lOpt = getopt_long( argc, argv, "S:?h",stLongOptions, &lLongIndex ) ) != ERROR )
    {
        switch (lOpt)
        {

            /* -S */
            case FPGA_SLOT_INFO:
            {
                ulRet = FPGA_ParseString2Uint( &g_strFpgaModule.ulSlotIndex, optarg );
                if ( SDKRTN_PARSE_SUCCESS != ulRet )
                {
                    printf("[***TIPS***]Option -S should be followed by a decimal number\r\n" );
                    LOG_ERROR( "String2Uint failed %d", ulRet );
                    return ulRet;
                }

                if ( g_strFpgaModule.ulSlotIndex >= FPGA_SLOT_MAX )
                {
                    printf( "Fpga slot number(%u) must be less than %d\r\n", g_strFpgaModule.ulSlotIndex, FPGA_SLOT_MAX );
                    FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireLedStatusHelp, sizeof_array( g_pacInquireLedStatusHelp ) );
                    return SDKRTN_PARSE_SLOT_ERROR;
                }
                return SDKRTN_PARSE_SUCCESS;
            }

            /* -h */
            case COMMAND_HELP_INFO:
            {
                FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireLedStatusHelp, sizeof_array( g_pacInquireLedStatusHelp ) );
                g_ulparseParaFlag = QUIT_FLAG;
                return SDKRTN_PARSE_SUCCESS;
            }
            
            /* -? */
            case COMMAND_HELP_INFO1:
            {
                if( PRINTED_COUNT == lHelpPrintedCount)
                {
                    FPGA_ParsePrintHelpInfo(argv[0], g_pacInquireLedStatusHelp, sizeof_array(g_pacInquireFpgaHelp));
                    g_ulparseParaFlag = QUIT_FLAG;
                    lHelpPrintedCount = 1;
                    ( void )lHelpPrintedCount;
                    return SDKRTN_PARSE_SUCCESS;
                }
            }

            //lint -fallthrough
            default:
            {
                printf( "\r\nERROR: Invalid input parameter.\r\n" );
                FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireLedStatusHelp, sizeof_array( g_pacInquireLedStatusHelp ) );
                return SDKRTN_PARSE_INVALID_PARA_ERROR;
            }
        }
    }

    /* Exception handling */
    if ( ERROR == lOpt ) /*lint !e774*/
    {
        printf( "\r\nERROR: Invalid input parameter.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacInquireLedStatusHelp, sizeof_array( g_pacInquireLedStatusHelp ) );
        return SDKRTN_PARSE_INVALID_PARA_ERROR;
    }

    return SDKRTN_PARSE_SUCCESS;
}

/*******************************************************************************
Function     : FPGA_ParseCommand
Description  : Parse the command
Input        : INT32 argc, INT8 *argv[] 
Output       : None
Return       : 0:sucess other:fail 
*******************************************************************************/
UINT32 FPGA_ParseCommand( INT32 argc, INT8 *argv[] )
{
    INPUT_COMMAND_PARSE strInputParse[] =
    {
        {"LF", CMD_HFI_LOAD, FPGA_ParseLoadHfi},
        {"CF", CMD_HFI_CLEAR, FPGA_ParseClearHfi},
        {"IF", CMD_IMAGE_INQUIRE, FPGA_ParseInquireFpgaImageInfo},
        {"DF", CMD_RESOURSE_INQUIRE, FPGA_ParseInquireFpgaResource},
        {"IL", CMD_LED_STATUS_INQUIRE, FPGA_ParseInquireLEDStatus},
        {"IV", CMD_TOOL_VERSION, FPGA_ParseShowVersion},
    };
    INPUT_COMMAND_PARSE *pstrTempParse = NULL;
    INT8 *pcInputPara = NULL;
    INT32 i = 0;
    UINT32 ulRet = SDKRTN_PARSE_ERROR_BASE;

    if ( argc < INPUT_PARAS_NUM_MIN )
    {
        printf( "ERROR: Input parameter number should be 2 at least.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacCommandEntryHelp, sizeof_array( g_pacCommandEntryHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    pcInputPara = argv[1];

    /* Check the opt code lenth */
    if ( ( INPUT_OPTCODE_LENGTH_LIMIT - 1 ) != strnlen( pcInputPara, INPUT_OPTCODE_LENGTH_LIMIT ) )
    {
        printf( "[***TIPS***] All operate codes consist of 2 characters.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacCommandEntryHelp, sizeof_array( g_pacCommandEntryHelp ) );
        return SDKRTN_PARSE_INPUT_ERROR;
    }

    for ( i = 0; i < sizeof_array( strInputParse ); i++ )
    {
        pstrTempParse = &strInputParse[i];

        if ( !strncmp( ( const INT8 * )pstrTempParse->cpStr, ( const INT8 * )pcInputPara, OPTCODE_LENGTH_MAX ) )
        {
            g_strFpgaModule.ulOpcode = pstrTempParse->ulOpcode;

            ulRet = pstrTempParse->pfnFunc( argc, argv );
            if ( SDKRTN_PARSE_SUCCESS != ulRet )
            {
                LOG_ERROR( "Parse func[%d] executed failed, %d", i, ulRet );
            }

            break;
        }
    }

    if ( g_strFpgaModule.ulOpcode == ( UINT32 )INIT_VALUE )
    {
        printf( "[***TIPS***] The valid input operate code are listed below.\r\n" );
        FPGA_ParsePrintHelpInfo( argv[0], g_pacCommandEntryHelp, sizeof_array( g_pacCommandEntryHelp ) );
        return SDKRTN_PARSE_INVALID_CODE_ERROR;
    }

    return ulRet;
}

#ifdef    __cplusplus
}
#endif
