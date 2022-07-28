

/****************************************************************************
*
*  common\win32\rx_win_helpers.c
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


#include "pch.h"

#include "../rx_common.h"
#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"




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


rx_protocol_result_t rx_init_protocols(struct rx_hosting_functions* memory);
rx_protocol_result_t rx_deinit_protocols();


size_t g_page_size = 0;

extern HCRYPTPROV hcrypt;

int g_is_debug_instance = 0;

RX_COMMON_API int rx_init_common_library(const rx_platform_init_data* init_data)
{
	g_is_debug_instance = init_data->is_debug;
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

		rx_init_protocols(NULL);

		g_init_count = 1;

		SYSTEM_INFO sys;
		ZeroMemory(&sys, sizeof(sys));
		GetSystemInfo(&sys);
		g_page_size = sys.dwPageSize;

		BOOL cret = CryptAcquireContext(&hcrypt, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);


		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}
RX_COMMON_API void rx_deinit_common_library()
{
	rx_deinit_protocols();

	if (hcrypt)
		CryptReleaseContext(hcrypt, 0);
}
RX_COMMON_API int rx_is_debug_instance()
{
	return g_is_debug_instance;
}

uint32_t rx_border_rand(uint32_t min, uint32_t max)
{
	if (max > min)
	{
		uint32_t diff = (max - min);
		if (diff > RAND_MAX)
		{
			int shifts = 0;
			while (diff > RAND_MAX)
			{
				shifts++;
				diff >>= 1;
			}
			uint32_t gen = rand() << shifts;
			gen = gen % (max - min) + min;
			return gen;
		}
		else
		{
			uint32_t gen = rand();
			gen = gen % (max - min) + min;
			return gen;
		}
	}
	else
		return min;
}

size_t rx_os_page_size()
{
	RX_ASSERT(g_page_size);
	return g_page_size;
}
void* rx_allocate_os_memory(size_t size)
{
	return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}


void rx_deallocate_os_memory(void* p, size_t size)
{
	VirtualFree(p, 0, MEM_RELEASE);
}

void* rx_allocate_thread_memory(size_t size)
{
	return NULL;
}


RX_COMMON_API rx_module_handle_t rx_load_library(const char* path)
{
	return LoadLibraryA(path);
}
RX_COMMON_API rx_func_addr_t rx_get_func_address(rx_module_handle_t module_handle, const char* name)
{
	return GetProcAddress(module_handle, name);
}
RX_COMMON_API void rx_unload_library(rx_module_handle_t module_handle)
{
	if (module_handle)
		FreeLibrary(module_handle);
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
RX_COMMON_API void rx_ms_sleep(uint32_t timeout)
{
	Sleep(timeout);
}
RX_COMMON_API void rx_usleep(uint64_t timeout)
{
	Sleep((DWORD)(timeout / 1000));
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


///////////////////////////////////////////////////////////////////////////////////////////////////
// slim lock apstraction
RX_COMMON_API void rx_slim_lock_create(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	InitializeCriticalSection(cs);
}
RX_COMMON_API void rx_slim_lock_destroy(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	DeleteCriticalSection(cs);
}
RX_COMMON_API void rx_slim_lock_aquire(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	EnterCriticalSection(cs);
}
RX_COMMON_API void rx_slim_lock_release(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	LeaveCriticalSection(cs);
}



RX_COMMON_API void rx_rw_slim_lock_create(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	InitializeSRWLock(lock);
}
RX_COMMON_API void rx_rw_slim_lock_destroy(prw_slim_lock_t plock)
{
	// nothing to do on windows, strange but jebi ga
}
RX_COMMON_API void rx_rw_slim_lock_aquire_reader(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	AcquireSRWLockShared(lock);
}
RX_COMMON_API void rx_rw_slim_lock_release_reader(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	ReleaseSRWLockShared(lock);
}
RX_COMMON_API void rx_rw_slim_lock_aquire_writter(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	AcquireSRWLockExclusive(lock);
}
RX_COMMON_API void rx_rw_slim_lock_release_writter(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	ReleaseSRWLockExclusive(lock);
}
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// handles abstractions ( wait and the rest of the stuff
RX_COMMON_API uint32_t rx_handle_wait(sys_handle_t what, uint32_t timeout)
{
	DWORD ret = WaitForSingleObject(what, timeout);
	if (ret == WAIT_OBJECT_0)
		return RX_WAIT_0;
	else if (ret == WAIT_TIMEOUT)
		return RX_WAIT_TIMEOUT;
	else
		return RX_WAIT_ERROR;

}
RX_COMMON_API uint32_t rx_handle_wait_us(sys_handle_t what, uint64_t timeout)
{
	DWORD to_sleep = 0;
	if (timeout > 50)
		to_sleep = (DWORD)((timeout - 50) / 1000) + 1;
	DWORD ret = WaitForSingleObject(what, to_sleep);
	if (ret == WAIT_OBJECT_0)
		return RX_WAIT_0;
	else if (ret == WAIT_TIMEOUT)
		return RX_WAIT_TIMEOUT;
	else
		return RX_WAIT_ERROR;

}
RX_COMMON_API uint32_t rx_handle_wait_for_multiple(sys_handle_t* what, size_t count, uint32_t timeout)
{
	DWORD ret = WaitForMultipleObjects((DWORD)count, what, FALSE, timeout);
	if (ret >= WAIT_OBJECT_0 && ret < (WAIT_OBJECT_0 + count))
		return RX_WAIT_0 + (ret - WAIT_OBJECT_0);
	else if (ret == WAIT_TIMEOUT)
		return RX_WAIT_TIMEOUT;
	else
		return RX_WAIT_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// event abstractions ( wait and the rest of the stuff
RX_COMMON_API sys_handle_t rx_event_create(int initialy_set)
{
	return CreateEvent(NULL, FALSE, initialy_set, NULL);
}
RX_COMMON_API int rx_event_destroy(sys_handle_t hndl)
{
	return CloseHandle(hndl);
}
RX_COMMON_API int rx_event_set(sys_handle_t hndl)
{
	return SetEvent(hndl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////



RX_COMMON_API rx_os_error_t rx_last_os_error(const char* text, char* buffer, size_t buffer_size)
{
	size_t msg_len = 0;
	char* help_ptr;
	LPSTR msg = NULL;
	DWORD err = GetLastError();
	DWORD ret = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPSTR)&msg,
		1,
		NULL);
	if (ret)
	{
		size_t msg_len = strlen(msg);
		if (msg_len > 0)
		{
			help_ptr = msg + msg_len - 1;
			while (help_ptr > msg && *help_ptr == '\r' || *help_ptr == '\n')
				help_ptr--;
			*(help_ptr + 1) = '\0';
		}
		if (text)
			snprintf(buffer, buffer_size, "%s. %s (%u)", text, msg, err);
		else
			snprintf(buffer, buffer_size, "%s (%u)", msg, err);
		LocalFree((LPVOID)msg);
	}
	else
	{
		if (text)
			snprintf(buffer, buffer_size, "%s. Error retriveing error code (%u)", text, err);
		else
			snprintf(buffer, buffer_size, "Error retriveing error code (%u)", err);
	}
	return err;
}




