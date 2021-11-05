

/****************************************************************************
*
*  common\gnu\gnu_common.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef gnu_common_h
#define gnu_common_h 1




#ifdef RXCOMMON_EXPORTS
#define RX_COMMON_API __attribute__((visibility("default")))
#else
#define RX_COMMON_API
#endif

#ifdef _DEBUG
#define RX_ASSERT(v) assert(v)
#else
#define RX_ASSERT(v) ((void)0)
#endif

#ifndef __cplusplus
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif

#define memzero(S, N) memset((S), 0, (N))

#pragma pack(push)
#pragma pack(1)
typedef struct _GUID {
	unsigned int  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID;
#pragma pack(pop)


typedef GUID rx_uuid_;
typedef GUID rx_uuid_t;




#endif
