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
 *    strcpy_s
 *
 * <SYNOPSIS>
 *    errno_t strcpy_s(char* strDest, size_t destMax, const char* strSrc)
 *
 * <FUNCTION DESCRIPTION>
 *    The strcpy_s function copies the contents in the address of strSrc, 
 *    including the terminating null character, to the location specified by strDest. 
 *    The destination string must be large enough to hold the source string, 
 *    including the terminating null character. strcpy_s will return EOVERLAP_AND_RESET 
 *    if the source and destination strings overlap.
 *
 * <INPUT PARAMETERS>
 *    strDest                  Location of destination string buffer
 *    destMax                  Size of the destination string buffer.
 *    strSrc                   Null-terminated source string buffer.
 *
 * <OUTPUT PARAMETERS>
 *    strDest                  is updated.
 *
 * <RETURN VALUE>
 *    0                        success
 *    EINVAL                   strDest == NULL or strSrc == NULL
 *    ERANGE                   destination buffer is NOT enough,  or size of 
 *                             buffer is zero or greater than SECUREC_STRING_MAX_LEN
 *    EOVERLAP_AND_RESET       dest buffer and source buffer are overlapped
 *
 *    If there is a runtime-constraint violation, then if strDest is not a null 
 *    pointer and destMax is greater than zero and not greater than 
 *    SECUREC_STRING_MAX_LEN, then strcpy_s sets strDest[0] to the null character.
 *******************************************************************************
*/

int strcpy_error(char* strDest, size_t destMax, const char* strSrc)
{
    if (destMax == 0 || destMax > SECUREC_STRING_MAX_LEN)
    {
        return -ERANGE;
    }
    else if (strDest == NULL || strSrc == NULL)
    {
        if (strDest != NULL)
        {
            strDest[0] = '\0';
            return -EINVAL;
        }
        return -EINVAL;
    }
    else if (strlen(strSrc) + 1 > destMax)
    {
        strDest[0] = '\0';
        return -ERANGE;
    }
    else 
    {
        return EOK;
    }
}

int strcpy_s(char* strDest, size_t destMax, const char* strSrc)
{
    if ((destMax > 0 && destMax <= SECUREC_STRING_MAX_LEN && strDest != NULL && strSrc != NULL && strDest != strSrc)) {
        const char *endPos = strSrc;
        size_t srcStrLen = destMax;  /* use it to store the maxi length limit */

        //lint -e722
        while( *(endPos++) && srcStrLen-- > 0);  /* use srcStrLen as boundary checker */

        srcStrLen = endPos - strSrc;  /*with ending terminator*/
        if (srcStrLen <= destMax) 
        {
            if (strDest < strSrc) 
            {
                if (strDest + srcStrLen <= strSrc ) 
                {
                    (void)memcpy(strDest, strSrc, srcStrLen);    
                    return EOK;
                }
                else
                {
                    strDest[0] = '\0';
                    return -ERANGE;
                }
            }
            else
            {
                if (strSrc + srcStrLen <= strDest ) 
                {
                   (void)memcpy(strDest, strSrc, srcStrLen);
                    return EOK;
                }
                else
                {
                    strDest[0] = '\0';
                    return -ERANGE;
                }
            }
        }
    }
    
    return strcpy_error(strDest, destMax, strSrc);
}

