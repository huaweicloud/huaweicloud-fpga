/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2017 Huawei Technologies Co., Ltd. All rights reserved.
 *   All rights reserved.
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
 
#include "securec.h"

/*******************************************************************************
* <NAME>
*    memcpy_s
*
* <SYNOPSIS>
*    errno_t memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
*
* <FUNCTION DESCRIPTION>
*    memcpy_s copies count bytes from src to dest
*
* <INPUT PARAMETERS>
*    dest                       new buffer.
*    destMax                    Size of the destination buffer.
*    src                        Buffer to copy from.
*    count                      Number of characters to copy
*
* <OUTPUT PARAMETERS>
*    dest buffer                is updated.
*
* <RETURN VALUE>
*    EOK                        Success
*    EINVAL                     dest == NULL or strSrc == NULL
*    ERANGE                     count > destMax or destMax > 
*                               SECUREC_MEM_MAX_LEN or destMax == 0
*    EOVERLAP_AND_RESET         dest buffer and source buffer are overlapped
*
*    if an error occured, dest will be filled with 0.
*    If the source and destination overlap, the behavior of memcpy_s is undefined.
*    Use memmove_s to handle overlapping regions.
*******************************************************************************
*/

int memcpy_s(void* dest, size_t destMax, const void* src, size_t count)
{
    if (LIKELY( count <= destMax && dest && src
        && destMax <= SECUREC_MEM_MAX_LEN 
        && count > 0
        && ( (dest > src  &&  (void*)((UINT8T*)src  + count) <= dest) ||
        (src  > dest &&  (void*)((UINT8T*)dest + count) <= src) )
        ) ) 
    {
            (void)memcpy(dest, src, count);
            return EOK;    
    }
    else
    {
        /* meet some runtime violation, return error code */
        return -EINVAL;
    }
}


