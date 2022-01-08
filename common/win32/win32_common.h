

/****************************************************************************
*
*  common\win32\win32_common.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef win32_common_h
#define win32_common_h 1




#ifdef RXCOMMON_EXPORTS
#define RX_COMMON_API __declspec(dllexport) 
#else
#define RX_COMMON_API __declspec(dllimport) 
#endif


#ifndef __cplusplus
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif

#define MAX max

#ifdef _DEBUG
#define RX_ASSERT(v) _ASSERT(v)
#else
#define RX_ASSERT(v) ((void)0)
#endif

#define memzero(S, N) ZeroMemory(S,N) 

typedef uuid_t rx_uuid_t;

#define ltoa _ltoa
#define ultoa _ultoa
#define lltoa _i64toa
#define ulltoa _ui64toa
#define gcvt _gcvt




#endif
