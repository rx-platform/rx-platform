

/****************************************************************************
*
*  common\win32\rx_win_helpers.c
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


typedef DWORD(__stdcall*
	NtSetInformationProcess_t)(
		IN HANDLE               ProcessHandle,
		IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
		IN PVOID                ProcessInformation,
		IN ULONG                ProcessInformationLength);


typedef DWORD(__stdcall*
	NtSetTimerResolution_t) (
		IN ULONG RequestedResolution,
		IN BOOLEAN Set,
		OUT PULONG ActualResolution
		);

typedef DWORD(__stdcall*
	NtQueryTimerResolution_t) (
		OUT PULONG              MinimumResolution,
		OUT PULONG              MaximumResolution,
		OUT PULONG              CurrentResolution
		);


uint64_t g_res_m;
uint64_t g_res_u;
uint64_t g_start;

int g_init_count = 0;

RX_COMMON_API int rx_init_common_library(const rx_platform_init_data* init_data)
{
	if (g_init_count == 0)
	{
		rx_hd_timer = init_data->rx_hd_timer;

		LARGE_INTEGER res;
		LARGE_INTEGER start;
		QueryPerformanceFrequency(&res);
		QueryPerformanceCounter(&start);
		g_res_m = ((1ULL << 32) * 1'000ULL) / res.QuadPart;
		g_res_u = ((1ULL << 32) * 1'000'000ULL) / res.QuadPart;
		g_start = start.QuadPart;

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
	if (rx_hd_timer)
	{
		LARGE_INTEGER ret;
		QueryPerformanceCounter(&ret);
		uint64_t low_ticks = ret.LowPart;
		uint64_t high_ticks = ret.HighPart;
		low_ticks *= g_res_m;
		high_ticks *= g_res_m;
		return ((low_ticks >> 32) + high_ticks);
	}
	else
	{
		return GetTickCount64();
	}

}
RX_COMMON_API rx_timer_ticks_t rx_get_us_ticks()
{
	if (rx_hd_timer)
	{
		LARGE_INTEGER ret;
		QueryPerformanceCounter(&ret);
		uint64_t low_ticks = ret.LowPart;
		uint64_t high_ticks = ret.HighPart;
		low_ticks *= g_res_u;
		high_ticks *= g_res_u;

		return (low_ticks >> 32) + high_ticks;
	}
	else
	{
		return GetTickCount64() * 1000ULL;
	}
}

RX_COMMON_API int rx_os_get_system_time(struct rx_time_struct_t* st)
{
	FILETIME ft;

	GetSystemTimeAsFileTime(&ft);
	st->t_value = (((uint64_t)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	return RX_OK;
}


RX_COMMON_API int rx_os_to_local_time(struct rx_time_struct_t* st)
{
	FILETIME mine, local;

	mine.dwLowDateTime = (DWORD)(st->t_value & 0xFFFFFFFF);
	mine.dwHighDateTime = (DWORD)(st->t_value >> 32);

	FileTimeToLocalFileTime(&mine, &local);

	st->t_value = (((uint64_t)local.dwHighDateTime) << 32) + local.dwLowDateTime;
	return RX_OK;
}


RX_COMMON_API int rx_os_to_utc_time(struct rx_time_struct_t* st)
{
	FILETIME mine, utc;

	mine.dwLowDateTime = (DWORD)(st->t_value & 0xFFFFFFFF);
	mine.dwHighDateTime = (DWORD)(st->t_value >> 32);

	LocalFileTimeToFileTime(&mine, &utc);

	st->t_value = (((uint64_t)utc.dwHighDateTime) << 32) + utc.dwLowDateTime;
	return RX_OK;
}

RX_COMMON_API int rx_os_split_time(const struct rx_time_struct_t* st, struct rx_full_time_t* full)
{
	FILETIME ft;
	SYSTEMTIME sys;
	ft.dwLowDateTime = (DWORD)(st->t_value & 0xFFFFFFFF);
	ft.dwHighDateTime = (DWORD)(st->t_value >> 32);
	if (!FileTimeToSystemTime(&ft, &sys))
		return RX_ERROR;

	full->year = sys.wYear;
	full->month = sys.wMonth;
	full->day = sys.wDay;
	full->w_day = sys.wDayOfWeek;
	full->hour = sys.wHour;
	full->minute = sys.wMinute;
	full->second = sys.wSecond;
	full->milliseconds = sys.wMilliseconds;

	return RX_OK;
}
RX_COMMON_API int rx_os_collect_time(const struct rx_full_time_t* full, struct rx_time_struct_t* st)
{
	FILETIME ft;
	SYSTEMTIME sys;

	sys.wYear = full->year;
	sys.wMonth = full->month;
	sys.wDay = full->day;
	sys.wDayOfWeek = 0;
	sys.wHour = full->hour;
	sys.wMinute = full->minute;
	sys.wSecond = full->second;
	sys.wMilliseconds = full->milliseconds;

	if (!SystemTimeToFileTime(&sys, &ft))
		return RX_ERROR;

	st->t_value = (((uint64_t)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;

	return RX_OK;
}


uuid_t g_null_uuid = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

RX_COMMON_API void rx_generate_new_uuid(uuid_t* u)
{
	UuidCreate(u);
}
RX_COMMON_API int rx_uuid_to_string(const uuid_t* u, char* str)
{
	RPC_CSTR lbuff = NULL;
	if (RPC_S_OK == UuidToStringA(u, &lbuff))
	{
		//{9466850F-0DBF-4C81-89CD-AC633F5182B4}
		strcpy(str, lbuff);
		RpcStringFreeA(&lbuff);
		return RX_OK;
	}
	else
		return RX_ERROR;
}
RX_COMMON_API int rx_string_to_uuid(const char* str, uuid_t* u)
{
	char buff[0x40];
	sprintf_s(buff, sizeof(buff), "%s", str);
	uuid_t ret = g_null_uuid;
	if (RPC_S_OK == UuidFromStringA((RPC_CSTR)buff, &ret))
	{
		*u = ret;
		return RX_OK;
	}
	else
	{
		*u = g_null_uuid;
		return RX_ERROR;
	}
}



