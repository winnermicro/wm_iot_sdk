/* ***** BEGIN LICENSE BLOCK *****
 * Version: RCSL 1.0/RPSL 1.0
 *
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved.
 *
 * The contents of this file, and the files included with this file, are
 * subject to the current version of the RealNetworks Public Source License
 * Version 1.0 (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the RealNetworks Community Source License Version 1.0
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl,
 * in which case the RCSL will apply. You may also obtain the license terms
 * directly from RealNetworks.  You may not use this file except in
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks
 * applicable to this file, the RCSL.  Please see the applicable RPSL or
 * RCSL for the rights, obligations and limitations governing use of the
 * contents of the file.
 *
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the portions
 * it created.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * assembly.h - assembly language functions and prototypes for supported platforms
 *
 * - inline rountines with access to 64-bit multiply results
 * - x86 (_WIN32) and ARM (ARM_ADS, _WIN32_WCE) versions included
 * - some inline functions are mix of asm and C for speed
 * - some functions are in native asm files, so only the prototype is given here
 *
 * MULSHIFT32(x, y)    signed multiply of two 32-bit integers (x and y), returns top 32 bits of 64-bit result
 * FASTABS(x)          branchless absolute value of signed integer x
 * CLZ(x)              count leading zeros in x
 * MADD64(sum, x, y)   (Windows only) sum [64-bit] += x [32-bit] * y [32-bit]
 * SHL64(sum, x, y)    (Windows only) 64-bit left shift using __int64
 * SAR64(sum, x, y)    (Windows only) 64-bit right shift using __int64
 */

#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

#if (defined _WIN32 && !defined _WIN32_WCE) || (defined __WINS__ && defined _SYMBIAN) || defined(_OPENWAVE_SIMULATOR) || defined(WINCE_EMULATOR)    /* Symbian emulator for Ix86 */

#pragma warning( disable : 4035 )   /* complains about inline asm not returning a value */

static __inline int MULSHIFT32(int x, int y)
{
    __asm
    {
        mov     eax, x
        imul    y
        mov     eax, edx
    }
}

static __inline int FASTABS(int x)
{
    int sign;

    sign = x >> (sizeof(int) * 8 - 1);
    x ^= sign;
    x -= sign;

    return x;
}

static __inline int CLZ(int x)
{
    int numZeros;

    if (!x)
        return (sizeof(int) * 8);

    numZeros = 0;
    while (!(x & 0x80000000))
    {
        numZeros++;
        x <<= 1;
    }

    return numZeros;
}

/* MADD64, SHL64, SAR64:
 * write in assembly to avoid dependency on run-time lib for 64-bit shifts, muls
 *  (sometimes compiler thunks to function calls instead of code generating)
 * required for Symbian emulator
 */
static __inline __int64 MADD64(__int64 sum, int x, int y)
{
    unsigned int sumLo = ((unsigned int *)&sum)[0];
    int sumHi = ((int *)&sum)[1];

    __asm
    {
        mov     eax, x
        imul    y
        add     eax, sumLo
        adc     edx, sumHi
    }

    /* equivalent to return (sum + ((__int64)x * y)); */
}

static __inline __int64 SHL64(__int64 x, int n)
{
    unsigned int xLo = ((unsigned int *)&x)[0];
    int xHi = ((int *)&x)[1];
    unsigned char nb = (unsigned char)n;

    if (n < 32)
    {
        __asm
        {
            mov     edx, xHi
            mov     eax, xLo
            mov     cl, nb
            shld    edx, eax, cl
            shl     eax, cl
        }
    }
    else if (n < 64)
    {
        /* shl masks cl to 0x1f */
        __asm
        {
            mov     edx, xLo
            mov     cl, nb
            xor     eax, eax
            shl     edx, cl
        }
    }
    else
    {
        __asm
        {
            xor     edx, edx
            xor     eax, eax
        }
    }
}

static __inline __int64 SAR64(__int64 x, int n)
{
    unsigned int xLo = ((unsigned int *)&x)[0];
    int xHi = ((int *)&x)[1];
    unsigned char nb = (unsigned char)n;

    if (n < 32)
    {
        __asm
        {
            mov     edx, xHi
            mov     eax, xLo
            mov     cl, nb
            shrd    eax, edx, cl
            sar     edx, cl
        }
    }
    else if (n < 64)
    {
        /* sar masks cl to 0x1f */
        __asm
        {
            mov     edx, xHi
            mov     eax, xHi
            mov     cl, nb
            sar     edx, 31
            sar     eax, cl
        }
    }
    else
    {
        __asm
        {
            sar     xHi, 31
            mov     eax, xHi
            mov     edx, xHi
        }
    }
}

#elif (defined _WIN32) && (defined _WIN32_WCE)

/* use asm function for now (EVC++ 3.0 does horrible job compiling __int64 version) */
#define MULSHIFT32  xmp3_MULSHIFT32
int MULSHIFT32(int x, int y);

static __inline int FASTABS(int x)
{
    int sign;

    sign = x >> (sizeof(int) * 8 - 1);
    x ^= sign;
    x -= sign;

    return x;
}

static __inline int CLZ(int x)
{
    int numZeros;

    if (!x)
        return (sizeof(int) * 8);

    numZeros = 0;
    while (!(x & 0x80000000))
    {
        numZeros++;
        x <<= 1;
    }

    return numZeros;
}

#elif __NDS32__

#include "nds32_intrinsic.h"

static __inline int MULSHIFT32(int x, int y)
{
#ifdef __NDS32_EXT_DSP__
    return __nds32__smmul(x, y);
#else
    return (int)(((__int64)x * y) >> 32);
#endif
}

static __inline int FASTABS(int x)
{
    return __nds32__abs(x);
}
static __inline int CLZ(int x)
{
    return __nds32__clz(x);
}

static __inline __int64 MADD64(__int64 sum, int x, int y)
{
#ifdef __NDS32_EXT_DSP__
    return __nds32__smar64(sum, x, y);
#else
    return (sum + ((__int64)x * y));
#endif
}

static __inline __int64 SHL64(__int64 x, int n)
{
    return x << n;
}

static __inline __int64 SAR64(__int64 x, int n)
{
    return x >> n;
}




#elif defined ARM_ADS

static __inline int MULSHIFT32(int x, int y)
{
    /* important rules for smull RdLo, RdHi, Rm, Rs:
     *     RdHi and Rm can't be the same register
     *     RdLo and Rm can't be the same register
     *     RdHi and RdLo can't be the same register
     * Note: Rs determines early termination (leading sign bits) so if you want to specify
     *   which operand is Rs, put it in the SECOND argument (y)
     * For inline assembly, x and y are not assumed to be R0, R1 so it shouldn't matter
     *   which one is returned. (If this were a function call, returning y (R1) would
     *   require an extra "mov r0, r1")
     */
    int zlow;
    __asm
    {
        smull zlow, y, x, y
    }

    return y;
}

static __inline int FASTABS(int x)
{
    int t;

    __asm
    {
        eor t, x, x, asr #31
        sub t, t, x, asr #31
    }

    return t;
}

static __inline int CLZ(int x)
{
    int numZeros;

    if (!x)
        return (sizeof(int) * 8);

    numZeros = 0;
    while (!(x & 0x80000000))
    {
        numZeros++;
        x <<= 1;
    }

    return numZeros;
}

#elif defined(__GNUC__) && defined(ARM)

static __inline int MULSHIFT32(int x, int y)
{
    /* important rules for smull RdLo, RdHi, Rm, Rs:
     *     RdHi and Rm can't be the same register
     *     RdLo and Rm can't be the same register
     *     RdHi and RdLo can't be the same register
     * Note: Rs determines early termination (leading sign bits) so if you want to specify
     *   which operand is Rs, put it in the SECOND argument (y)
     * For inline assembly, x and y are not assumed to be R0, R1 so it shouldn't matter
     *   which one is returned. (If this were a function call, returning y (R1) would
     *   require an extra "mov r0, r1")
     */
    int zlow;
    __asm__ volatile ("smull %0,%1,%2,%3" : "=&r" (zlow), "=r" (y) : "r" (x), "1" (y)) ;

    return y;
}

static __inline int FASTABS(int x)
{
    int t;

    __asm__ volatile (
        "eor %0,%2,%2, asr #31;"
        "sub %0,%1,%2, asr #31;"
        : "=&r" (t)
        : "0" (t), "r" (x)
    );

    return t;
}

static __inline int CLZ(int x)
{
    int numZeros;

    if (!x)
        return (sizeof(int) * 8);

    numZeros = 0;
    while (!(x & 0x80000000))
    {
        numZeros++;
        x <<= 1;
    }

    return numZeros;
}

#else
//#define CSKY_OP

//#error Unsupported platform in assembly.h
static __inline int MULSHIFT32(int x, int y)
{
#ifdef CSKY_OP
    return (int)(((__int64)x * y) >> 32);
#else
	int ret = 0;
//	asm("mul.s32.h %2, %0, %1\n\t"
//		:"=r"(x), "=r"(y), "=r"(ret)
//		:"0"(x), "1"(y), "2"(ret));
	asm("mul.s32.h %0, %1, %2\n\t"
		:"=r"(ret)
		:"r"(x), "r"(y));
	return ret;
#endif
}


static __inline int FASTABS(int x)
{
#ifdef CSKY_OP
    int sign;

    sign = x >> (sizeof(int) * 8 - 1);
    x ^= sign;
    x -= sign;
    return x;
#else
	int ret;
	__asm__("abs %0, %1\n\t":"=r"(ret):"r"(x));
	return ret;
#endif
}

static __inline int CLZ(int x)
{
    int numZeros;
#ifdef CSKY_OP
    if (!x)
        return (sizeof(int) * 8);

    numZeros = 0;
    while (!(x & 0x80000000))
    {
        numZeros++;
        x <<= 1;
    }
#else
    numZeros = 0;
    __asm__("ff1 %1, %0\n\t":"=r"(x),"=r"(numZeros):"0"(x), "1"(numZeros));
#endif
    return numZeros;
}

static __inline __int64 MADD64(__int64 sum, int x, int y)
{
#ifdef CSKY_OP
    return (sum + ((__int64)x * y));
#else
	__asm__("mula.s32 %0, %1, %2\n\t"
		:"+r"(sum)
		:"r"(x), "r"(y));
	return sum;
#endif
}


static __inline __int64 SAR64(__int64 x, int n)
{
    return x >> n;
}

#endif  /* platforms */

#endif /* _ASSEMBLY_H */
