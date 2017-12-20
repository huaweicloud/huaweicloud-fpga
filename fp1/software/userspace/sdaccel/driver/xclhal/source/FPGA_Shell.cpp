/*-------------------------------------------------------------------------------
 * Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
 
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License. 
-------------------------------------------------------------------------------*/


#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "FPGA_Shell.h"

UINT8* trim_str( UINT8 *pstr, UINT32 lens)
{
	
	while (lens > 0)
	{
		if(*pstr == ' ' || *pstr == '\t')
		{
			pstr++;
			--lens;
		}
		else
			break;

	}
	
	return pstr;
}

LOADSTATUS Fpga_get_load_status(char *pStatus)
{
	if(!strncmp(pStatus, "NOT_PROGRAMMED", 14))
	{
		return STATUS_NOT_PROGRAMMED;
	}
	else if(!strncmp(pStatus, "LOADED", 6))
	{
		return STATUS_LOADED;
	}
	else if(!strncmp(pStatus, "FAILED", 6))
	{
		return STATUS_FAILED;
	}	
	else if(!strncmp(pStatus, "BUSY", 4))
	{
		return STATUS_BUSY;
	}
	else if(!strncmp(pStatus, "INVALID_ID", 10))
	{
		return STATUS_INVALID_ID;
	}
	
	return STATUS_NOT_PROGRAMMED;
}


/*****************************************************************************
 funtcion name  : FPGA_GetStringMatch
 function description : compare string
 input parameter  : UINT8 *pucStartAddr  --
             UINT32 ulFileSize    --
             UINT8 *pucMatchStr   --
             UINT32 ulMatchSize   --
             UINT32 ulFlag        --
 output parameter  : null
 return value  : address: sucess
             NULL:failure
 function call  : 
 called by function : 
 
 modified history  : 
      1. 2017/9/7    c00403281     

*****************************************************************************/
UINT8 * FPGA_GetStringMatch( UINT8 *pucStartAddr, UINT32 ulFileSize, const INT8 *pucMatchStr, UINT32 ulMatchSize, UINT32 ulFlag )
{
    UINT32 i = 0;
    UINT32 ulRet = 0;

    if ( NULL == pucMatchStr )
    {
        std::cout<< "[FPGA_GetStringMatch]Err: Match string length is NULL\r\n" ;
        return NULL;
    }

    if ( NULL == pucStartAddr )
    {
         std::cout<< "[FPGA_GetStringMatch]Err: StartAddr string length is NULL\r\n" ;
        return NULL;
    }

    if ( 0 == ulMatchSize )
    {
         std::cout<< "[FPGA_GetStringMatch]Err: Match string length is 0\r\n" ;
        return NULL;
    }
 
    if  ( ulMatchSize > ulFileSize )
    {
        std::cout<< "[FPGA_GetStringMatch]Err: Match string length is too long.\r\n";
        return NULL;
    }
    
    /* search key character from the end of file*/
    if ( SEARCH_FROM_END == ulFlag )
    {
        for ( i = ulMatchSize; i < ( ulFileSize - ulMatchSize ); i++ )
        {
            ulRet = strncmp( ( const INT8 * )( ( ( UINT64 )pucStartAddr + ulFileSize - i ) - 1 ), ( const INT8 * )pucMatchStr, ulMatchSize );
            if ( 0 == ulRet )
            {
                return ( UINT8 * )( ( ( UINT64 )pucStartAddr + ulFileSize - i ) - 1 );
            }
        }
    }
    else if ( SEARCH_FROM_START == ulFlag )     /* search key character from the beginning of file*/
    {
        for ( i = 0; i < ( ulFileSize - ulMatchSize ); i++ )
        {
            ulRet = strncmp( ( const INT8 * )( ( UINT64 )pucStartAddr + i ), ( const INT8 * )pucMatchStr, ulMatchSize );
            if ( 0 == ulRet )
            {
                return ( UINT8 * )( ( UINT64 )pucStartAddr + i );
            }
        }
    }
    else
    { 
         std::cout<< "[FPGA_GetStringMatch]Err: Flag error\r\n" ;
    }

    return NULL;
}

UINT32 FPGA_exec_shell_and_get_result( const INT8 *pshell, UINT8 **pRet, UINT32* lens)
{
    FILE *pFile = NULL;
    UINT8 *pucBuf = NULL;
    UINT8  aucTempDBDF[1024]={0};
    UINT8 *pKey = NULL;
    INT32 lLen = 0;
    UINT32 ulSize = 0;
    UINT32 ulFreadSize = 0;
    UINT32 i = 0;
    INT32 lStatus;
    UINT32 lRet; 
    pid_t pid;
    
	if(NULL == pshell)
	{
		std::cout<<"the shell cmd is null... \r\n";
		return -1;
	}
	
	
    pFile = fopen( FPGA_TEMP_NAME , "w+" );                                    /* create a new file */
    if ( NULL == pFile )
    {
        std::cout<< "[FPGA_GetSlotId] open " << FPGA_TEMP_NAME <<"fail, fp is NULL.\r\n" ;
        return -1;
    } 

    fputs( pshell, pFile );
    fflush( pFile );
    fclose( pFile );
    
    /* execute GetSlotId.sh */
    pid = fork();
    if ( pid == 0 )
    {  
        /* child process */ 
        if ( -1 == execlp( "bash", "bash", FPGA_TEMP_NAME, NULL ) )
        {  
            std::cout<<"[FPGA_GetSlotId] " <<std::endl; 
            exit( -1 );  
        }  
    }
    else if ( pid > 0)
    {  
        waitpid(pid, &lStatus, 0);

        /* child process quit WIFEXITED unormally   */
        if ( ( 0 == ( INT32 )WIFEXITED( lStatus ) ) || WEXITSTATUS( lStatus ) )
        {
            std::cout<< "[FPGA_GetSlotId]excute bash cmd error.\r\n";
            return -1;
        }
    } 
    else
    {
        std::cout<< "[FPGA_GetSlotId]fork fail.\r\n";
        return -1;
    }

    /* read the file which inlcude slot info */
    pFile = fopen( FPGA_TEMP_RESULT, "r" );
    if ( NULL == pFile )
    {
        std::cout<<"[FPGA_GetSlotId]ERR:Open file fail, File:"<<FPGA_TEMP_RESULT<<std::endl;
        return -1;
    }

    /* set the file pointer */
    lRet = fseek( pFile, 0L, SEEK_END );
    if ( 0 != lRet )
    {
        std::cout<<"[FPGA_GetSlotId]ERR:fseek file fail, File:" << FPGA_TEMP_RESULT <<std::endl;
        fclose( pFile );
        return -1;
    }

    /* Fetch the file length, ftell: return file length when sucess, return "-1" when failed */
    lLen = ( INT32 )ftell( pFile );
    if ( lLen <= 0 )
    {
        std::cout<< "[FPGA_GetSlotId]ERR:ftell file fail, File:"<<FPGA_TEMP_RESULT <<std::endl;
        fclose( pFile );
        return -1;
    }

    ulSize = ( UINT32 )lLen;

    /* Allocate memory for file  */
    pucBuf = ( UINT8 *)malloc( ulSize + 1);
    if ( NULL == pucBuf )
    {
        std::cout<<"[FPGA_GetSlotId]ERR:malloc fail"<<std::endl;
        fclose( pFile );
        return -1;
    }

    /* Copy file to memory*/
    rewind( pFile );                             /* Set the pointer the beginning of the file*/
    ulFreadSize = fread( pucBuf, 1, ulSize, pFile );
    if ( ulFreadSize != ulSize )
    {
        /* Free memory for the file */
        free( pucBuf );
        fclose( pFile );
        pucBuf = NULL;
        std::cout<< "[FPGA_GetSlotId]ERR:fread fail" <<std::endl;
		return -1;
    }
	
    fclose( pFile );
	
    /* Delete tempory file */
    if ( remove( FPGA_TEMP_NAME ) < 0 )
    {
        std::cout<< "[FPGA_GetSlotId]remove script err\r\n";
    }
    
    if( remove( FPGA_TEMP_RESULT ) < 0 )
	{
		std::cout<< "[FPGA_GetSlotId]remove script result err\r\n";
	}

	*pRet = pucBuf;
	*lens	 = ulSize;
	
    return 0;
}

int FPGA_get_load_info()
{
	UINT8* pBuf = NULL;
	UINT32 lens = 0;
	UINT32 nRet = -1;
	UINT8* pKey = NULL;
	UINT8* pStart = NULL;
	UINT8* pEnd = NULL;
	INT8 Match1[]="LoadStatusName";
	INT8  Value1[128] = {0};
	INT8 Match2[]="LoadErrName";
	INT8  Value2[128] = {0};
	UINT32 nTemp = 0;
	
	
	nRet = FPGA_exec_shell_and_get_result(FPGA_CMD_ENTRY, &pBuf, &lens);
	if(0 != nRet)
	{
		std::cout<<"fpga cmd exec failed:"<<FPGA_CMD_ENTRY<<std::endl;		
		return -1;
	}
	
	pKey = FPGA_GetStringMatch(pBuf, lens, Match1, strlen(Match1), SEARCH_FROM_START);
	if(NULL == pKey)
	{
		std::cout<<"ERROR:LoadStatusName key\n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	nTemp = pKey - pBuf;
	if( (lens - nTemp) < (strlen(Match1) + 1) )
	{
		std::cout<<"ERROR:something error in result file.\n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	pStart = pKey + strlen(Match1) + 1;
	pEnd = FPGA_GetStringMatch(pStart, lens - nTemp, "\n", 1, SEARCH_FROM_START);
	if(NULL == pEnd)
	{
		std::cout<<"ERROR:LoadStatusName pEnd\n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	memset(Value1, 0, 128);
	
	pStart=trim_str(pStart, pEnd - pStart);
	if(pEnd - pStart <= 0 || pEnd - pStart >= 128)
	{
		std::cout<<"ERROR:Get  LoadStatusName value failed.\n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	memcpy( Value1, pStart, pEnd - pStart );
	
	
	LOADSTATUS loadStatus = STATUS_NOT_PROGRAMMED;
	
	loadStatus = Fpga_get_load_status(Value1);

	switch(loadStatus)
	{
		case STATUS_NOT_PROGRAMMED:
		case STATUS_INVALID_ID:
		case STATUS_FAILED:	
		{
			if(NULL != pBuf)
			{
				free(pBuf);
				pBuf = NULL;
			}
			std::cout<<"ERROR:LoadStatus is "<< Value1<<std::endl;
			return loadStatus;
		}		
		case STATUS_BUSY:
		{
			if(NULL != pBuf)
			{
				free(pBuf);
				pBuf = NULL;
			}
			return loadStatus;
		}		
		case STATUS_LOADED:
		break;
		default:
		{
			if(NULL != pBuf)
			{
				free(pBuf);
				pBuf = NULL;
			}
			return -1;
		}	
	}
	
	pKey = FPGA_GetStringMatch(pBuf, lens, Match2, strlen(Match2), SEARCH_FROM_START);
	if(NULL == pKey)
	{
		std::cout<<"ERROR:LoadErrName key\n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	nTemp = pKey - pBuf;
	if( (lens - nTemp) < (strlen(Match2) + 1) )
	{
		std::cout<<"ERROR:something error in result file.\n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	pStart = pKey + strlen(Match2) + 1;
	pEnd = FPGA_GetStringMatch(pStart, lens - (pStart - pBuf), "\n", 1, SEARCH_FROM_START);
	if(NULL == pEnd)
	{
		std::cout<<"ERROR:LoadErrName pEnd \n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	memset(Value2, 0, 128);

	pStart=trim_str(pStart,pEnd - pStart);
	
	if(pEnd - pStart <= 0 || pEnd - pStart >= 128)
	{
		std::cout<<"ERROR:Get  LoadErrName value failed.\n";
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
	
	memcpy( Value2, pStart, pEnd - pStart );

	if(strncmp(Value2, "OK", 2))
	{
		std::cout<<"ERROR: the LoadErr is "<<Value2<<std::endl;
		if(NULL != pBuf)
		{
			free(pBuf);
			pBuf = NULL;
		}
		return -1;
	}
		
	if(NULL != pBuf)
	{
		free(pBuf);
		pBuf = NULL;
	}
	
	return STATUS_LOADED;
}
