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
 *    strncpy_s
 *
 * <SYNOPSIS>
 *    errno_t strncpy_s( char* strDest, size_t destMax, const char* strSrc, size_t count);
 *
 * <FUNCTION DESCRIPTION>
 *    Copy the contents from strSrc, including the terminating null character, 
 *    to the location specified by strDest.
 *
 * <INPUT PARAMETERS>
 *    strDest                     Destination string.
 *    destMax                     The size of the destination string, in characters.
 *    strSrc                      Source string.
 *    count                       Number of characters to be copied.
 *
 * <OUTPUT PARAMETERS>
 *    strDest                     is updated
 *
 * <RETURN VALUE>
 *    EOK(0)                      success
 *    EINVAL                      strDest == NULL or strSrc == NULL
 *    ERANGE                      destMax is zero or greater than SECUREC_STRING_MAX_LEN,
 *                                or count > SECUREC_STRING_MAX_LEN, or destMax is too small
 *    EOVERLAP_AND_RESET          buffer and source buffer are overlapped
 *
 *    If there is a runtime-constraint violation, then if strDest is not a null
 *    pointer and destMax is greater than zero and not greater than SECUREC_STRING_MAX_LEN,
 *    then strncpy_s sets strDest[0] to the null character.
 *******************************************************************************
*/

errno_t strncpy_s(char* strDest, size_t destMax, const char* strSrc, size_t count)
{

    if ( LIKELY( destMax > 0 && destMax <= SECUREC_STRING_MAX_LEN && strDest != NULL && strSrc != NULL && count <= SECUREC_STRING_MAX_LEN && count > 0 && count <= destMax ) ) 
    {
        size_t strLen = strlen( strSrc );

        if ( count > strLen )
        {
            count = strLen; /* without ending terminator */
        }

        if ( ( strDest < strSrc && strDest + ( count + 1 ) <= strSrc )
            || (strSrc < strDest && strSrc + ( count + 1 ) <= strDest )
            || strDest == strSrc )  
        {
            /*Not overlap*/
            ( void )memcpy( strDest, strSrc, count ); /* copy string by count bytes */
            strDest[count + 1] = '\0';
            return EOK;
        }
        else 
        {
            strDest[0] = '\0';
            return EOVERLAP_AND_RESET;
        }
    }

    else if ( 0 == count && NULL != strDest )
    {
        strDest[0] = '\0';
        return EOK;
    }

    else 
    {
        return EINVAL;
    }
}
