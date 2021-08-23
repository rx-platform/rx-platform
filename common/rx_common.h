

/****************************************************************************
*
*  common\rx_common.h
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


#ifndef rx_common_h
#define rx_common_h 1



// standard return codes
#define RX_ERROR 0
#define RX_OK 1
#define RX_ASYNC 2

// Hide ugly details about RX_COMMON_API
#ifdef _MSC_VER // MSVC compiler
#include "win32/win32_common.h"
#endif
#ifdef __GNUC__ // GCC compiler
#include "gnu/gnu_common.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int rx_hd_timer; // use high definition timer

} rx_platform_init_data;




RX_COMMON_API int rx_init_common_library(const rx_platform_init_data* init_data);
RX_COMMON_API void rx_deinit_common_library();


// match pattern function
RX_COMMON_API int rx_match_pattern(const char* string, const char* Pattern, int bCaseSensitive);

// time related functions
typedef uint64_t rx_timer_ticks_t;

typedef struct rx_time_struct_t
{
	uint64_t t_value;
} rx_time_struct;

typedef struct rx_full_time_t
{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t w_day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t milliseconds;

} rx_full_time;



RX_COMMON_API int rx_os_get_system_time(struct rx_time_struct_t* st);
RX_COMMON_API int rx_os_to_local_time(struct rx_time_struct_t* st);
RX_COMMON_API int rx_os_to_utc_time(struct rx_time_struct_t* st);
RX_COMMON_API int rx_os_split_time(const struct rx_time_struct_t* st, struct rx_full_time_t* full);
RX_COMMON_API int rx_os_collect_time(const struct rx_full_time_t* full, struct rx_time_struct_t* st);

RX_COMMON_API rx_timer_ticks_t rx_get_tick_count();
RX_COMMON_API rx_timer_ticks_t rx_get_us_ticks();



#ifdef __cplusplus
}
#endif


#endif
