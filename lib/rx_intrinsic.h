

/****************************************************************************
*
*  lib\rx_intrinsic.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_intrinsic_h
#define rx_intrinsic_h 1




// This code here is for byte swapping, it is supposed to be fast
// and the best way to do it is by using intrinsics from the compiler
// we use it as macros in order to let compiler do the optimization right
// that is why preprocessor is used form different compilers
// this code is collected from:
// <https://github.com/google/cityhash/blob/8af9b8c2b889d80c22d6bc26ba0df1afb79a30db/src/city.cc#L50>

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)


#define rx_atomic_add_fetch_32(x,a) (int32_t)InterlockedAdd((LONG*)x, a)
#define rx_atomic_sub_fetch_32(x,a) (int32_t)InterlockedAdd((LONG*)x, -(a))
#define rx_atomic_inc_fetch_32(x) (int32_t)InterlockedIncrement((LONG*)x)
#define rx_atomic_dec_fetch_32(x) (int32_t)InterlockedDecrement((LONG*)x)
#define rx_atomic_fetch_32(x) (int32_t)InterlockedAdd((LONG*)x, 0)

#define rx_atomic_add_fetch_64(x,a) (int32_t)InterlockedAdd64((LONG64*)x, a)
#define rx_atomic_sub_fetch_64(x,a) (int32_t)InterlockedAdd64((LONG64*)x, -(a))
#define rx_atomic_inc_fetch_64(x) (int64_t)InterlockedIncrement64((LONG64*)x)
#define rx_atomic_dec_fetch_64(x) (int64_t)InterlockedDecrement64((LONG64*)x)
#define rx_atomic_fetch_64(x) (int64_t)InterlockedAdd64((LONG64*)x, 0);

#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_16(x) BSWAP_16(x)
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_16(x) swap16(x)
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else

#include <byteswap.h>

#define rx_atomic_add_fetch_32(x,a) (int32_t)__atomic_add_fetch((volatile int*)x, a, __ATOMIC_SEQ_CST)
#define rx_atomic_sub_fetch_32(x,a) (int32_t)__atomic_add_fetch((volatile int*)x, -(a), __ATOMIC_SEQ_CST)
#define rx_atomic_inc_fetch_32(x) (int32_t)__atomic_add_fetch((volatile int*)x, 1, __ATOMIC_SEQ_CST)
#define rx_atomic_dec_fetch_32(x) (int32_t)__atomic_add_fetch((volatile int*)x, -1, __ATOMIC_SEQ_CST)
#define rx_atomic_fetch_32(x) (int32_t)__atomic_add_fetch((volatile int*)val, 0, __ATOMIC_SEQ_CST)

#define rx_atomic_add_fetch_64(x,a) (int32_t)__atomic_add_fetch((volatile int64_t*)x, a, __ATOMIC_SEQ_CST)
#define rx_atomic_sub_fetch_64(x,a) (int32_t)__atomic_add_fetch((volatile int64_t*)x, -(a), __ATOMIC_SEQ_CST)
#define rx_atomic_inc_fetch_64(x) (int64_t)__atomic_add_fetch((volatile int64_t*)x, 1ll, __ATOMIC_SEQ_CST)
#define rx_atomic_dec_fetch_64(x) (int64_t)__atomic_add_fetch((volatile int64_t*)x, -1ll, __ATOMIC_SEQ_CST)
#define rx_atomic_fetch_64(x) (int64_t)__atomic_add_fetch((volatile int64_t*)val, 0ll, __ATOMIC_SEQ_CST)

#endif




#endif
