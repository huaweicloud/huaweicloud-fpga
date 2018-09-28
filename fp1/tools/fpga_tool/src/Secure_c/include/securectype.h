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
#ifndef __SECURECTYPE_H__
#define __SECURECTYPE_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#if defined(__GNUC__)  && !defined(WIN32) && !defined(SECUREC_PCLINT)
#define SECUREC_ATTRIBUTE(x,y)  __attribute__((format(printf, (x), (y) ))) 
#else
#define SECUREC_ATTRIBUTE(x,y)
#endif

#if defined(__GNUC__) && ((__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 3 /*above 3.4*/ ))  ) 
    long __builtin_expect(long exp, long c);
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x) 
#endif

#ifndef TWO_MIN
#define TWO_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define WCHAR_SIZE sizeof(wchar_t)

typedef signed char INT8T;
typedef unsigned char UINT8T;


typedef int INT32T;
typedef unsigned int UINT32T;
typedef long long INT64T;
typedef unsigned long long UINT64T;

/* define the max length of the string */
#define SECUREC_STRING_MAX_LEN (0x7fffffffUL)
#define SECUREC_WCHAR_STRING_MAX_LEN (SECUREC_STRING_MAX_LEN / WCHAR_SIZE)

/* add SECUREC_MEM_MAX_LEN for memcpy and memmove*/
#define SECUREC_MEM_MAX_LEN (0x7fffffffUL)
#define SECUREC_WCHAR_MEM_MAX_LEN (SECUREC_MEM_MAX_LEN / WCHAR_SIZE)

#if SECUREC_STRING_MAX_LEN > 0x7fffffff
#error "max string is 2G, or you may remove this macro"
#endif


#define IN_REGISTER register

#define SECC_MALLOC(x) malloc((size_t)(x))
#define SECC_FREE(x)   free((void *)(x))


#if (!defined(SECUREC_ON_64BITS) && defined(__GNUC__ ) && defined(__SIZEOF_POINTER__ ))
#if __SIZEOF_POINTER__ == 8
#define SECUREC_ON_64BITS
#endif
#endif

#if defined(__GNUC__) && ((__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 3 /*above 3.4*/ ))  ) 
    long __builtin_expect(long exp, long c);
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x) 
#endif

#endif    /*__SECURECTYPE_H__*/


