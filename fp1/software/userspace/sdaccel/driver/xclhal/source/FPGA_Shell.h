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

#ifndef __FPGA_SHELL_H__
#define __FPGA_SHELL_H__



typedef unsigned char 	UINT8;
typedef unsigned short 	UINT16;
typedef unsigned int 	UINT32;
typedef unsigned long 	UINT64;
typedef long 	INT64;
typedef int 	INT32;
typedef short 	INT16;
typedef char 	INT8;


enum LOADSTATUS
{
	STATUS_NOT_PROGRAMMED = 0,
	STATUS_LOADED = 1,
	STATUS_FAILED = 2,
	STATUS_BUSY = 3,
	STATUS_INVALID_ID = 4
};


#define SEARCH_FROM_START 		0  
#define SEARCH_FROM_END 		1

#define FPGA_TEMP_NAME			"/tmp/fpga"
#define FPGA_TEMP_RESULT		"/tmp/fpga_result"
#define FPGA_CMD_ENTRY			"FpgaCmdEntry IF -S 0 > /tmp/fpga_result"

UINT8 * FPGA_GetStringMatch( UINT8 *pucStartAddr, UINT32 ulFileSize, const INT8 *pucMatchStr, UINT32 ulMatchSize, UINT32 ulFlag );

UINT32 FPGA_exec_shell_and_get_result( const INT8 *pshell, UINT8 **pRet, UINT32* lens);


int FPGA_get_load_info();


#endif