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
 *    snprintf_s
 *
 * <SYNOPSIS>
 *    int snprintf_s(char* strDest, size_t destMax, size_t count, const char* format, ...);
 *
 * <FUNCTION DESCRIPTION>
 *    The snprintf_s function formats and stores count or fewer characters in 
 *    strDest and appends a terminating null. Each argument (if any) is converted
 *    and output according to the corresponding format specification in format.
 *    The formatting is consistent with the printf family of functions; If copying
 *    occurs between strings that overlap, the behavior is undefined.
 *
 * <INPUT PARAMETERS>
 *    strDest                 Storage location for the output.
 *    destMax                 The size of the storage location for output. Size 
 *                            in bytes for snprintf_s or size in words for snwprintf_s.
 *    count                   Maximum number of character to store.
 *    format                  Format-control string.
 *
 * <OUTPUT PARAMETERS>
 *    strDest                 is updated
 *
 * <RETURN VALUE>
 *    snprintf_s returns the number of characters stored in strDest, not counting
 *    the terminating null character.
 *    If the storage required to store the data and a terminating null exceeds 
 *    destMax, the function set strDest to an empty string, and return -1.
 *    If strDest or format is a NULL pointer, or if count is less than or equal
 *    to zero, the function return -1.
 *******************************************************************************
*/

int snprintf_s (char* strDest, size_t destMax, size_t count, const char* format, ...)
{
    if (LIKELY( count <= destMax && strDest && format 
            && destMax <= SECUREC_MEM_MAX_LEN
            && count > 0 ))
    {
        int ret = 0;
        va_list arglist;

        //lint -e530
        va_start(arglist, format);
        ret = vsnprintf(strDest, count, format, arglist);
        va_end(arglist);
        return ret;
    }
    else 
    {
        return EINVAL;
    }
}


