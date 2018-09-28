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
 *    vsnprintf_s
 *
 * <SYNOPSIS>
 *    int vsnprintf_s(char* strDest, size_t destMax, size_t count, const char* format, valist ap);
 *
 * <FUNCTION DESCRIPTION>
 *    The vsnprintf_s function takes a pointer to an argument list, then formats
 *    and writes up to count characters of the given data to the memory pointed
 *    to by strDest and appends a terminating null.
 *
 * <INPUT PARAMETERS>
 *    strDest                Storage location for the output.
 *    destMax                The size of the strDest for output.
 *    count                  Maximum number of character to write(not including 
 *                           the terminating NULL)
 *    format                 Format-control string.
 *    ap                     pointer to list of arguments.
 *
 * <OUTPUT PARAMETERS>
 *    strDest                is updated
 *
 * <RETURN VALUE>
 *    vsnprintf_s returns the number of characters written, not including the 
 *    terminating null, or a negative value if an output error occurs. vsnprintf_s
 *    is included for compliance to the ANSI standard.
 *    If the storage required to store the data and a terminating null exceeds 
 *    destMax, the function set strDest to an empty strDest, and return -1.
 *    If strDest or format is a NULL pointer, or if count is less than or equal
 *    to zero, the function return -1.
 *
 *    ERROR CONDITIONS:
 *    Condition                       Return
 *    strDest is NULL                 -1
 *    format is NULL                  -1
 *    count <= 0                      -1
 *    destMax too small               -1(and strDest set to an empty string)
 *******************************************************************************
*/


int vsnprintf_s (char* strDest, size_t destMax, size_t count, const char* format, va_list arglist)
{
    int ret = 0;
    if (format == NULL || strDest == NULL || destMax == 0 || destMax > SECUREC_STRING_MAX_LEN || (count > (SECUREC_STRING_MAX_LEN - 1) &&  count != (size_t)-1))
    {
        if (strDest != NULL && destMax > 0)
        {
            strDest[0] = '\0';
        }
        return -1;
    }

    if ( destMax < count )
    {
        count = destMax;
    }

    ret = vsnprintf(strDest, count, format, arglist);

    return ret;
}
