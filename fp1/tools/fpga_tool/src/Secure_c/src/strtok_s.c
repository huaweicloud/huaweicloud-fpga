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
 *    strtok_s
 *
 * <SYNOPSIS>
 *    char* strtok_s(char* strToken, const char* strDelimit, char** context);
 *
 * <FUNCTION DESCRIPTION>
 *    The strtok_s function finds the next token in strToken.
 *
 * <INPUT PARAMETERS>
 *    strToken            String containing token or tokens.
 *    strDelimit          Set of delimiter characters.
 *    context             Used to store position information between calls
 *                        to strtok_s
 *
 * <OUTPUT PARAMETERS>
 *
 * <RETURN VALUE>
 *    Returns a pointer to the next token found in strToken.
 *    They return NULL when no more tokens are found.
 *    Each call modifies strToken by substituting a NULL character for the first
 *    delimiter that occurs after the returned token.
 *
 *    return value        condition
 *    NULL                context == NULL, strDelimit == NULL, strToken == NULL
 *                        && (*context) == NULL, or no token is found.
 *******************************************************************************
*/


char* strtok_s(char* strToken, const char* strDelimit, char** context)
{
    char* token = NULL;
    const char* ctl = NULL;

    /* validate delimiter and string context */
    if (context == NULL || strDelimit == NULL)
    {
        return NULL;
    }
    
    /*valid input string and string pointer from where to search*/
    if (strToken == NULL && (*context) == NULL)
    {
        return NULL;
    }

    /* If string is null, continue searching from previous string position stored in context*/
    if (NULL == strToken)
    {
        strToken = *context;
    }

    /* Find beginning of token (skip over leading delimiters). Note that
    * there is no token if this loop sets string to point to the terminal null. 
    */
    while (*strToken != 0 )
    {
        ctl = strDelimit;
        while ( *ctl != 0 && *ctl != *strToken)
        {
            ++ctl;
        }

        if (*ctl == 0) /*don't find any delimiter in string header, break the loop*/
        {
            break;
        }
        ++strToken;
    }

    token = strToken; /*point to updated position*/

    /* Find the rest of the token. If it is not the end of the string,
    * put a null there. 
    */
    for ( ; *strToken != 0 ; strToken++)
    {
        for (ctl = strDelimit; *ctl != 0 && *ctl != *strToken; ctl++)
            ;
        if (*ctl != 0) /*find a delimiter*/
        {
            *strToken++ = 0; /*set string termintor*/
            break;
        }
    }

    /* record string position for next search in the context */
    *context = strToken;

    /* Determine if a token has been found. */
    if (token == strToken)
    {
        return NULL;
    }
    else
    {
        return token;
    }
}


