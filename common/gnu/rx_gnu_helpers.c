

/****************************************************************************
*
*  common\gnu\rx_gnu_helpers.c
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


#include "pch.h"

#include "../rx_common.h"



int rx_hd_timer = 1;
int g_init_count = 0;


RX_COMMON_API int rx_init_common_library(const rx_platform_init_data* init_data)
{
    if(g_init_count == 0)
    {
        rx_hd_timer = init_data->rx_hd_timer;

        g_init_count = 1;

        return RX_OK;
    }
    else
    {
        return RX_ERROR;
    }
}
RX_COMMON_API void rx_deinit_common_library()
{
}

RX_COMMON_API rx_timer_ticks_t rx_get_tick_count()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t ret = ts.tv_sec * 1000;
	ret = ret + ts.tv_nsec / 1000000ul;
	return (uint32_t)ret;
}
RX_COMMON_API rx_timer_ticks_t rx_get_us_ticks()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t ret = ts.tv_sec * 1000000ul;
	ret = ret + ts.tv_nsec / 1000ul;
	return ret;
}

#define TIME_CONVERSION_CONST  116444736000000000ull

uint64_t timeval_to_rx_time(const struct timespec* tv)
{
    if (tv->tv_sec == 0 && tv->tv_nsec == 0)
        return 0;
    else
    {
        uint64_t temp = ((uint64_t)tv->tv_nsec) / 100 + ((uint64_t)tv->tv_sec) * 10000000;
        temp += TIME_CONVERSION_CONST;
        return temp;
    }

}

void rx_time_to_timeval(uint64_t rx_time,struct timespec* tv)
{
    if (rx_time==0)
    {
        tv->tv_sec=0;
        tv->tv_nsec=0;
    }
    else
    {

        uint64_t temp = rx_time-TIME_CONVERSION_CONST;
        tv->tv_sec=temp/10000000;
        tv->tv_nsec=temp%10000000;
        tv->tv_nsec=tv->tv_nsec*100;
    }

}

RX_COMMON_API int rx_os_get_system_time(struct rx_time_struct_t* st)
{
    struct timespec tv;
    int ret=clock_gettime(CLOCK_REALTIME,&tv);
    if(ret==-1)
        return RX_ERROR;
    else
    {
        st->t_value=timeval_to_rx_time(&tv);
        return RX_OK;
    }
}
RX_COMMON_API int rx_os_to_local_time(struct rx_time_struct_t* st)
{
    assert(0);
    return RX_ERROR;
}
RX_COMMON_API int rx_os_to_utc_time(struct rx_time_struct_t* st)
{
    assert(0);
    return RX_ERROR;
}
RX_COMMON_API int rx_os_split_time(const struct rx_time_struct_t* st, struct rx_full_time_t* full)
{
    struct tm tm;
    struct timespec tv;
    rx_time_to_timeval(st->t_value,&tv);
    time_t tt=tv.tv_sec;
    gmtime_r(&tt,&tm);
    full->year=tm.tm_year+1900;
    full->month=tm.tm_mon+1;
    full->day=tm.tm_mday;
    full->w_day=tm.tm_wday;
    full->hour=tm.tm_hour;
    full->minute=tm.tm_min;
    full->second=tm.tm_sec;
    full->milliseconds=tv.tv_nsec/1000000;
    return RX_OK;
}
RX_COMMON_API int rx_os_collect_time(const struct rx_full_time_t* full, struct rx_time_struct_t* st)
{
    struct timespec tv;
    struct tm tm;
    tm.tm_year=full->year-1900;
    tm.tm_mon=full->month;
    tm.tm_mday=full->day;
    tm.tm_hour=full->hour;
    tm.tm_min=full->minute;
    tm.tm_sec=full->second;
    time_t tt=mktime(&tm);

    tv.tv_nsec=full->milliseconds*1000000ll;
    tv.tv_sec=tt;
    st->t_value=timeval_to_rx_time(&tv);

    return RX_OK;
}

/////////////////////////////////////
// uuid code


void linux_uuid_to_uuid(const uuid_t* uuid, rx_uuid_t* u)
{
    memcpy(u->Data4, &(*uuid)[8], 8);
    u->Data1 = ntohl(*((uint32_t*)&(*uuid)[0]));
    u->Data2 = ntohs(*((uint16_t*)&(*uuid)[4]));
    u->Data3 = ntohs(*((uint16_t*)&(*uuid)[6]));
}

void uuid_to_linux_uuid(const rx_uuid_t* u, uuid_t* uuid)
{
    memcpy(&(*uuid)[8], u->Data4, 8);
    *((uint32_t*)&(*uuid)[0]) = htonl(u->Data1);
    *((uint16_t*)&(*uuid)[4]) = ntohs(u->Data2);
    *((uint16_t*)&(*uuid)[6]) = ntohs(u->Data3);
}

RX_COMMON_API void rx_generate_new_uuid(rx_uuid_t* u)
{
    uuid_t uuid;
    uuid_generate(uuid);
    linux_uuid_to_uuid(&uuid, u);
}
RX_COMMON_API int rx_uuid_to_string(const rx_uuid_t* u, char* str)
{
    uuid_t uuid;
    uuid_to_linux_uuid(u, &uuid);
    uuid_unparse(uuid, str);
    return RX_OK;
}
RX_COMMON_API int rx_string_to_uuid(const char* str, rx_uuid_t* u)
{
    uuid_t uuid;
    if (uuid_parse(str, uuid) < 0)
        return RX_ERROR;
    linux_uuid_to_uuid(&uuid, u);
    return RX_OK;
}



