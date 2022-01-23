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

#if !defined(__MYTYPES_H__)
#define __MYTYPES_H__

// ----------------------------------------------------------------------------

#if defined(__cplusplus)

template< class TA, class TB > TA TMIN( TA a, TB b ) { return (a < b) ? a : b; }
template< class TA, class TB > TA TMAX( TA a, TB b ) { return (a > b) ? a : b; }

#endif /* defined(__cplusplus) */

// ----------------------------------------------------------------------------

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef u8 * pu8;
typedef u16 * pu16;
typedef u32 * pu32;
typedef s8 * ps8;
typedef s16 * ps16;
typedef s32 * ps32;

// ----------------------------------------------------------------------------

struct POINT {
	s16 x;
	s16 y;
};

struct RECT {
	s16 left;
	s16 top;
	s16 right;
	s16 bottom;
};

struct SIZE {
	s16 cx;
	s16 cy;
};

// ----------------------------------------------------------------------------

#endif /* !defined(__MYTYPES_H__) */


