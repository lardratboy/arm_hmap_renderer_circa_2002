// ----------------------------------------------------------------------------
/*
The MIT License (MIT)

Copyright (c) 2000- Brad P. Taylor

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
// ----------------------------------------------------------------------------

#if !defined(__BPT_SIN_COS_H__)
#define __BPT_SIN_COS_H__

#include "mytypes.h"

// ----------------------------------------------------------------------------

#define BPT_TRIG_ROM_STORAGE_PREFIX					const
#define BPT_TRIG_TABLE_STORAGE_PREFIX				/* nop */
#define BPT_TRIG_SHIFT								14
#define STORE_COS_TABLE_TWICE

// ----------------------------------------------------------------------------

#define TRIG_POW2_ENTRIES			9
#define TRIG_ANGLE_360				(1 << (TRIG_POW2_ENTRIES))
#define TRIG_ANGLE_MASK				((TRIG_ANGLE_360)-1)
#define TRIG_ANGLE_180				((TRIG_ANGLE_360)/2)
#define TRIG_ANGLE_90				((TRIG_ANGLE_360)/4)
#define TRIG_ANGLE_60				((TRIG_ANGLE_360)/6)
#define TRIG_ANGLE_45				((TRIG_ANGLE_360)/8)
#define TRIG_ANGLE_30				((TRIG_ANGLE_360)/12)
#define TRIG_ANGLE_15				((TRIG_ANGLE_360)/24)

// ----------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {
#endif

// ----------------------------------------------------------------------------

#if defined(STORE_COS_TABLE_TWICE)
extern BPT_TRIG_TABLE_STORAGE_PREFIX s16 g_SinAndCosTable[ (TRIG_ANGLE_360 * 2) + TRIG_ANGLE_90 ];
#else
extern BPT_TRIG_TABLE_STORAGE_PREFIX s16 g_SinAndCosTable[ (TRIG_ANGLE_360 * 1) + TRIG_ANGLE_90 ];
#endif

// ----------------------------------------------------------------------------

#if defined(__cplusplus)
};
#endif

#endif // __BPT_SIN_COS_H__

