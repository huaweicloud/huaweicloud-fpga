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
*    memset_s
*
* <SYNOPSIS>
*    errno_t memset_s(void* dest, size_t destMax, int c, size_t count)
*
* <FUNCTION DESCRIPTION>
*    Sets buffers to a specified character.
*
* <INPUT PARAMETERS>
*    dest                       Pointer to destination.
*    destMax                    The size of the buffer.
*    c                          Character to set.
*    count                      Number of characters.
*
* <OUTPUT PARAMETERS>
*    dest buffer                is uptdated.
*
* <RETURN VALUE>
*    EOK                        Success
*    EINVAL                     dest == NULL
*    ERANGE                     count > destMax or destMax > SECUREC_MEM_MAX_LEN 
*                               or destMax == 0
*******************************************************************************
*/


errno_t memset_s(void* dest, size_t destMax, int c, size_t count)
{
    if (LIKELY(count <= destMax  && dest  && destMax <= SECUREC_MEM_MAX_LEN )) 
    {    

            (void)memset(dest, c, count);
            return EOK;     
    }
    else
    {
        /* meet some runtime violation, return error code */
        return -EINVAL;
    }
}
