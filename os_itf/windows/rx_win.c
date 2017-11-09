

/****************************************************************************
*
*  os_itf\windows\rx_win.c
*
*  Copyright (c) 2017 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"


// rx_win
#include "os_itf/windows/rx_win.h"

//#include <winternl.h">
#include "version/rx_version.h"
#include "os_itf/rx_ositf.h"

#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <VersionHelpers.h>
#include <assert.h>
#include <mstcpip.h>




dword rx_last_eror()
{
	return GetLastError();
}

uuid_t g_null_uuid = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

void rx_generate_new_uuid(uuid_t* u)
{
	UuidCreate(u);
}
dword rx_uuid_to_string(const uuid_t* u, char* str)
{
	RPC_CSTR lbuff = NULL;
	if (RPC_S_OK == UuidToStringA(u, &lbuff))
	{
		//{9466850F-0DBF-4C81-89CD-AC633F5182B4}
		strcpy_s(str,0x40,lbuff);
		RpcStringFreeA(&lbuff);
		return RX_OK;
	}
	else
		return RX_ERROR;
}
dword rx_string_to_uuid(const char* str, uuid_t* u)
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
		return RX_OK;
	}
}

typedef enum  _PROCESS_INFORMATION_CLASS_EX {
	ProcessBasicInformation,
	ProcessQuotaLimits,
	ProcessIoCounters,
	ProcessVmCounters,
	ProcessTimes,
	ProcessBasePriority,
	ProcessRaisePriority,
	ProcessDebugPort,
	ProcessExceptionPort,
	ProcessAccessToken,
	ProcessLdtInformation,
	ProcessLdtSize,
	ProcessDefaultHardErrorMode,
	ProcessIoPortHandlers,
	ProcessPooledUsageAndLimits,
	ProcessWorkingSetWatch,
	ProcessUserModeIOPL,
	ProcessEnableAlignmentFaultFixup,
	ProcessPriorityClass,
	ProcessWx86Information,
	ProcessHandleCount,
	ProcessAffinityMask,
	ProcessPriorityBoost,
	MaxProcessInfoClass

} PROCESS_INFORMATION_CLASS_EX, *PPROCESS_INFORMATION_CLASS_EX;


typedef DWORD(__stdcall * 
	NtSetInformationProcess)(
	IN HANDLE               ProcessHandle,
	IN PROCESS_INFORMATION_CLASS_EX ProcessInformationClass,
	IN PVOID                ProcessInformation,
	IN ULONG                ProcessInformationLength);


typedef DWORD(__stdcall *
	NtSetTimerResolution) (
	IN ULONG RequestedResolution,
	IN BOOLEAN Set,
	OUT PULONG ActualResolution
	);

typedef DWORD(__stdcall *
	NtQueryTimerResolution) (
	OUT PULONG              MinimumResolution,
	OUT PULONG              MaximumResolution,
	OUT PULONG              CurrentResolution
	);


qword g_res;
qword g_start;

int rx_os_get_system_time(struct rx_time_struct_t* st)
{
	FILETIME ft;

	GetSystemTimeAsFileTime(&ft);
	st->t_value = (((qword)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	return RX_OK;
}


int rx_os_to_local_time(struct rx_time_struct_t* st)
{
	FILETIME mine, local;

	mine.dwLowDateTime = (DWORD)(st->t_value & 0xFFFFFFFF);
	mine.dwHighDateTime = (DWORD)(st->t_value >> 32);

	FileTimeToLocalFileTime(&mine, &local);

	st->t_value = (((qword)local.dwHighDateTime) << 32) + local.dwLowDateTime;
	return RX_OK;
}


int rx_os_to_utc_time(struct rx_time_struct_t* st)
{
	FILETIME mine, utc;

	mine.dwLowDateTime = (DWORD)(st->t_value & 0xFFFFFFFF);
	mine.dwHighDateTime = (DWORD)(st->t_value >> 32);

	LocalFileTimeToFileTime(&mine, &utc);

	st->t_value = (((qword)utc.dwHighDateTime) << 32) + utc.dwLowDateTime;
	return RX_OK;
}

int rx_os_split_time(const struct rx_time_struct_t* st, struct rx_full_time_t* full)
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
int rx_os_collect_time(const struct rx_full_time_t* full, struct rx_time_struct_t* st)
{
	FILETIME ft;
	SYSTEMTIME sys;

	sys.wYear = full->year;
	sys.wMonth = full->month;
	sys.wDay = full->day;
	sys.wDayOfWeek = 0;
	sys.wHour = full->w_day;
	sys.wMinute = full->minute;
	sys.wSecond = full->second;
	sys.wMilliseconds = full->milliseconds;

	if (!SystemTimeToFileTime(&sys, &ft))
		return RX_ERROR;

	st->t_value = (((qword)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;

	return RX_OK;
}

dword rx_border_rand(dword min, dword max)
{
	if (max > min)
	{
		UINT ret = 0;
		rand_s(&ret);
		ret = ret % (max - min) + min;
		return ret;
	}
	else
		return min;
}

size_t g_page_size = 0;

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



dword rx_atomic_add_fetch_32(volatile dword* val, int add)
{
	return (dword)InterlockedAdd((LONG*)val, add);
}

dword rx_atomic_inc_fetch_32(volatile dword* val)
{
	return (dword)InterlockedIncrement((LONG*)val);
}
dword rx_atomic_dec_fetch_32(volatile dword* val)
{
	return (dword)InterlockedDecrement((LONG*)val);
}
dword rx_atomic_fetch_32(volatile dword* val)
{
	return (dword)InterlockedAdd((LONG*)val, 0);
}

qword rx_atomic_inc_fetch_64(volatile qword* val)
{
	return (qword)InterlockedIncrement64((LONG64*)val);
}
qword rx_atomic_dec_fetch_64(volatile qword* val)
{
	return (qword)InterlockedDecrement64((LONG64*)val);
}
qword rx_atomic_fetch_64(volatile qword* val)
{
	return (qword)InterlockedAdd64((LONG64*)val, 0);
}


void init_fixed_drives();

int rx_big_endian = 0;
rx_thread_data_t rx_tls = 0;
const char* rx_server_name = NULL;
const char* rx_get_server_name()
{
	return rx_server_name;
}


extern HCRYPTPROV hcrypt;

const char* g_ositf_version = "ERROR!!!";
char ver_buffer[0x100];
rx_pid_t rx_pid;

void rx_initialize_os(rx_pid_t pid, int rt, rx_thread_data_t tls, const char* server_name)
{
	char temp[0x20];
	sprintf(temp, "%s Ver %d.%d.%d", RX_HAL_NAME, RX_HAL_MAJOR_VERSION, RX_HAL_MINOR_VERSION, RX_HAL_BUILD_NUMBER);
	create_module_version_string(temp, __DATE__, __TIME__, ver_buffer);
	g_ositf_version = ver_buffer;

	rx_server_name = server_name;
	rx_tls = tls;
	rx_pid = pid;
	// determine big endian or little endian
	union {
		dword i;
		char c[4];
	} bint = { 0x01020304 };
	rx_big_endian = (bint.c[0] == 1 ? 1 : 0);

	SYSTEM_INFO sys;
	ZeroMemory(&sys, sizeof(sys));
	GetSystemInfo(&sys);
	g_page_size = sys.dwPageSize;

	LARGE_INTEGER res;
	LARGE_INTEGER start;
	QueryPerformanceFrequency(&res);
	QueryPerformanceCounter(&start);
	g_res = res.QuadPart;
	g_start = start.QuadPart;

	DWORD err = 0;

	HMODULE hLib = LoadLibraryEx(L"ntdll.dll",NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	//HMODULE hLib = GetModuleHandle(L"ntdll.dl");

	if (hLib == NULL)
		err = GetLastError();


	if (hLib)
	{
		NtSetTimerResolution set = (NtSetTimerResolution)GetProcAddress(hLib, "NtSetTimerResolution");
		NtQueryTimerResolution query = (NtQueryTimerResolution)GetProcAddress(hLib, "NtQueryTimerResolution");

		if (query)
		{
			ULONG min = 0;
			ULONG max = 0;
			ULONG current = 0;

			DWORD ret = (query)(&min, &max, &current);

			printf("Timer data:\r\nmin resolution: %.3fms\r\nmax:%.3fms\r\ncur:%.3fms\r\n",
			(double)min / 10000.0,
			(double)max / 10000.0,
			(double)current / 10000.0);

		}

		if (set && rt)
		{
			ULONG act = 0;

			DWORD ret = (set)(500 * 10, TRUE, &act);
			if (ret == 0)
			{
				printf("Process resolution set to %0.3fms\r\n", (double)act/10000.0);
			}
		}

		FreeLibrary(hLib);
	}
	init_fixed_drives();

	struct WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);


	BOOL cret = CryptAcquireContext(&hcrypt, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);

#ifndef _DEBUG
	if (rt)
	{
		//SE_INC_BASE_PRIORITY_NAME

		BOOL pret = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
		printf("Process priority class set to RT %d!\r\n",(int)pret);
	}
#endif
}
void rx_deinitialize_os()
{
	if(hcrypt)
		CryptReleaseContext(hcrypt,0);
	WSACleanup();
}


////////////////////////////////////////////////////////////////////////////
// anyonimus pipe support

////////////////////////////////////////////////////////////////////////////
// anyonimus pipe support

void rx_initialize_server_side_pipe(struct pipe_server_t* pipes)
{
	ZeroMemory(pipes, sizeof(struct pipe_server_t));
}
void rx_initialize_client_side_pipe(struct pipe_client_t* pipes)
{
	ZeroMemory(pipes, sizeof(struct pipe_client_t));
}
int rx_create_server_side_pipe(struct pipe_server_t* pipes, size_t size)
{
	HANDLE temp_read = NULL;
	HANDLE temp_write = NULL;
	HANDLE temp_server_read = NULL;
	HANDLE temp_rerver_write = NULL;


	sys_handle_t me = GetCurrentProcess();

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	if (CreatePipe(&pipes->client_read, &temp_write, &sa, (DWORD)size))
	{
		if (DuplicateHandle(me, temp_write, me, &pipes->server_write, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			CloseHandle(temp_write);

			if (CreatePipe(&temp_read, &pipes->client_write, &sa, (DWORD)size))
			{
				if (DuplicateHandle(me, temp_read, me, &pipes->server_read, 0, FALSE, DUPLICATE_SAME_ACCESS))
				{
					CloseHandle(temp_read);

					return RX_OK;
				}

			}

		}

	}
	if (temp_read)
	{
		CloseHandle(temp_read);
	}
	if (temp_write)
	{
		CloseHandle(temp_write);
	}
	if (pipes->server_read)
	{
		CloseHandle(pipes->server_read);
		pipes->server_read = NULL;
	}
	if (pipes->server_write)
	{
		CloseHandle(pipes->server_write);
		pipes->server_write = NULL;
	}
	if (pipes->client_read)
	{
		CloseHandle(pipes->client_read);
		pipes->client_read = NULL;
	}
	if (pipes->client_write)
	{
		CloseHandle(pipes->client_write);
		pipes->client_write = NULL;
	}
	return RX_ERROR;
}
int rx_create_client_side_pipe(struct pipe_server_t* server_pipes, struct pipe_client_t* pipes)
{
	pipes->client_read = server_pipes->client_read;
	pipes->client_write = server_pipes->client_write;
	
	return RX_OK;
}
int rx_destry_server_side_pipe(struct pipe_server_t* pipes)
{
	if (pipes->client_read)
	{
		CloseHandle(pipes->client_read);
		pipes->client_read = NULL;
	}
	if (pipes->client_write)
	{
		CloseHandle(pipes->client_write);
		pipes->client_write = NULL;
	}
	if (pipes->server_read)
	{
		CloseHandle(pipes->server_read);
		pipes->server_read = NULL;
	}
	if (pipes->server_write)
	{
		CloseHandle(pipes->server_write);
		pipes->server_write = NULL;
	}
	return RX_OK;
}
int rx_destry_client_side_pipe(struct pipe_client_t* pipes)
{
	if (pipes->client_read)
	{
		CloseHandle(pipes->client_read);
		pipes->client_read = NULL;
	}
	if (pipes->client_write)
	{
		CloseHandle(pipes->client_write);
		pipes->client_write = NULL;
	}
	return RX_OK;
}



int rx_write_pipe_server(struct pipe_server_t* pipes, const void* data, size_t size)
{
	DWORD writen = 0;
	BOOL ret = WriteFile(pipes->server_write, &size, sizeof(size), &writen, NULL);
	if (!ret || writen != sizeof(size))
	{
		return RX_ERROR;
	}


	return RX_OK;
}
int rx_write_pipe_client(struct pipe_client_t* pipes, const void* data, size_t size)
{
	DWORD writen = 0;
	BOOL ret = WriteFile(pipes->client_write, data, (DWORD)size, &writen, NULL);
	if (!ret || writen != sizeof(size))
	{
		return RX_ERROR;
	}


	return RX_OK;
}
int rx_read_pipe_server(struct pipe_server_t* pipes, byte* data, size_t size)
{
	DWORD read = 0;

	BOOL ret = ReadFile(pipes->server_read, data, (DWORD)size, &read, NULL);
	if (!ret || read != size)
	{
		return RX_ERROR;
	}
	return RX_OK;
}
int rx_read_pipe_client(struct pipe_client_t* pipes, byte* data, size_t size)
{
	DWORD read = 0;

	BOOL ret = ReadFile(pipes->client_read, data, (DWORD)size, &read, NULL);
	if (!ret || read != size)
	{
		return RX_ERROR;
	}
	return RX_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////}
// IP 4 addresses
int rx_add_ip_address(dword addr, dword mask, int itf, ip_addr_ctx_t* ctx)
{
	ULONG NTEContext = 0;
	ULONG NTEInstance = 0;

	if (NO_ERROR == AddIPAddress(addr, mask, itf, &NTEContext, &NTEInstance))
	{
		*ctx = NTEContext;
		return 1;
	}
	return 0;
}
int rx_remove_ip_address(ip_addr_ctx_t ctx)
{
	if (ctx)
	{
		if (DeleteIPAddress(ctx) == NO_ERROR)
		{
			return 1;
		}
	}
	return 0;
}
int rx_is_valid_ip_address(dword addr, dword mask)
{
	BYTE tablebuff[4096];

	int ret = RX_ERROR;

	MIB_IPADDRTABLE* data = (MIB_IPADDRTABLE*)tablebuff;
	ULONG size = sizeof(tablebuff);

	dword err = GetIpAddrTable(data, &size, FALSE);

	if (SUCCEEDED(err))
	{
		for (dword i = 0; i<data->dwNumEntries; i++)
		{
			if (addr == data->table[i].dwAddr &&
				mask == data->table[i].dwMask &&
				(data->table[i].wType&(MIB_IPADDR_DISCONNECTED | MIB_IPADDR_DELETED)) == 0)
			{
				ret = RX_OK;
				break;
			}
		}
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////////
// system info

// RTL_OSVERSIONINFOEXW is defined in winnt.h
BOOL GetOsVersion(RTL_OSVERSIONINFOEXW* pk_OsVer)
{
	typedef LONG(WINAPI* tRtlGetVersion)(RTL_OSVERSIONINFOEXW*);

	memset(pk_OsVer, 0, sizeof(RTL_OSVERSIONINFOEXW));
	pk_OsVer->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

	HMODULE h_NtDll = GetModuleHandleW(L"ntdll.dl");
	tRtlGetVersion f_RtlGetVersion = (tRtlGetVersion)GetProcAddress(h_NtDll, "RtlGetVersion");

	if (!f_RtlGetVersion)
		return FALSE; // This will never happen (all processes load ntdll.dll)

	LONG Status = f_RtlGetVersion(pk_OsVer);
	return Status == 0; // STATUS_SUCCESS;
}

// determine version with file version of ntdll.dll?!?
BOOL GetOsVersionFromSystemFile(RTL_OSVERSIONINFOEXW* os)
{

	const char* fname = "ntdll.dll";

	ZeroMemory(os, sizeof(RTL_OSVERSIONINFOEXW));

	os->wProductType = IsWindowsServer() ? VER_NT_SERVER : VER_NT_WORKSTATION;

	DWORD len;
	DWORD dwDummyHandle; // will always be set to zero
	len = GetFileVersionInfoSizeA(fname, &dwDummyHandle);
	if (len > 0)
	{

		BYTE* pVersionInfo = (BYTE*)malloc(len); // allocate version info
		if (!GetFileVersionInfoA(fname, 0, len, pVersionInfo))
			return FALSE;

		LPVOID lpvi;
		UINT iLen;
		if (!VerQueryValueA(pVersionInfo, "\\", &lpvi, &iLen))
			return FALSE;

		// copy fixed info to myself, which am derived from VS_FIXEDFILEINFO
		VS_FIXEDFILEINFO ver = *((VS_FIXEDFILEINFO*)lpvi);

		os->dwMajorVersion = ver.dwFileVersionMS >> 16;
		os->dwMinorVersion = ver.dwFileVersionMS & 0xffff;
		os->dwBuildNumber = ver.dwFileVersionLS >> 16;


		free(pVersionInfo);

	}

	return TRUE;
}


void rx_collect_system_info(char* buffer, size_t buffer_size)
{
	RTL_OSVERSIONINFOEXW os;
	os.dwOSVersionInfoSize = sizeof(os);
	ZeroMemory(&os, sizeof(os));
	dword err = 0;
	if (!GetOsVersionFromSystemFile(&os))
		err = GetLastError();

	BOOL is_server = (os.wProductType != VER_NT_WORKSTATION);
	BOOL has_sp = os.szCSDVersion[0] != L'\0';

	SYSTEM_INFO sys;
	ZeroMemory(&sys, sizeof(sys));
	GetSystemInfo(&sys);

	char* model = "Unknown";

	switch (sys.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		model = "x64";
		break;
	case PROCESSOR_ARCHITECTURE_ARM:
		model = "ARM";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		model = "Itanium";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		model = "x86";
		break;
	}

	if (has_sp)
	{
		sprintf(buffer, "Windows %s %d.%d.%d + %S %d.%d [%s] ", is_server ? "Server" : "Workstation",
			(int)os.dwMajorVersion, (int)os.dwMinorVersion, (int)os.dwBuildNumber,
			os.szCSDVersion, (int)os.wServicePackMajor, (int)os.wServicePackMinor
			, model);
	}
	else
	{
		sprintf(buffer, "Windows %s %d.%d.%d [%s]", is_server ? "Server" : "Workstation",
			(int)os.dwMajorVersion, (int)os.dwMinorVersion, (int)os.dwBuildNumber
			, model);
	}

}
void rx_collect_processor_info(char* buffer, size_t buffer_size)
{
	SYSTEM_INFO sys;
	ZeroMemory(&sys, sizeof(sys));
	GetSystemInfo(&sys);	
	char temp_buff[0x100];
	char name_buff[0x100];
	name_buff[0] = L'\0';

	for (dword i = 0; i < sys.dwNumberOfProcessors; i++)
	{
		sprintf(temp_buff, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", i);

		HKEY hkey = NULL;
		DWORD reg_count = (dword)sizeof(name_buff);
		if (ERROR_SUCCESS == RegGetValueA(HKEY_LOCAL_MACHINE, temp_buff, "ProcessorNameString", RRF_RT_REG_SZ, NULL, name_buff, &reg_count))
		{

		}
		break;
	}
	sprintf(buffer, "%s ; Total Cores:%d", name_buff, sys.dwNumberOfProcessors);
}
void rx_collect_memory_info(qword* total, qword* free)
{
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	*total = statex.ullTotalPhys;
	*free = statex.ullAvailPhys;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// slim lock apstraction
void rx_slim_lock_create(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	InitializeCriticalSection(cs);
}
void rx_slim_lock_destroy(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	DeleteCriticalSection(cs);
}
void rx_slim_lock_aquire(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	EnterCriticalSection(cs);
}
void rx_slim_lock_release(pslim_lock_t plock)
{
	CRITICAL_SECTION* cs = (CRITICAL_SECTION*)plock;
	LeaveCriticalSection(cs);
}



void rx_rw_slim_lock_create(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	InitializeSRWLock(lock);
}
void rx_rw_slim_lock_destroy(prw_slim_lock_t plock)
{
	// nothing to do on windows, starange but jebi ga
}
void rx_rw_slim_lock_aquire_reader(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	AcquireSRWLockShared(lock);
}
void rx_rw_slim_lock_release_reader(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	ReleaseSRWLockShared(lock);
}
void rx_rw_slim_lock_aquire_writter(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	AcquireSRWLockExclusive(lock);
}
void rx_rw_slim_lock_release_writter(prw_slim_lock_t plock)
{
	PSRWLOCK lock = (PSRWLOCK)plock;
	ReleaseSRWLockExclusive(lock);
}
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// handles apstractions ( wait and the rest of the stuff
dword rx_handle_wait(sys_handle_t what, dword timeout)
{
	DWORD ret = WaitForSingleObject(what, timeout);
	if (ret == WAIT_OBJECT_0)
		return RX_WAIT_0;
	else if (ret == WAIT_TIMEOUT)
		return RX_WAIT_TIMEOUT;
	else
		return RX_WAIT_ERROR;

}
dword rx_handle_wait_for_multiple(sys_handle_t* what, size_t count, dword timeout)
{
	DWORD ret = WaitForMultipleObjects((DWORD)count, what, FALSE, timeout);
	if (ret >= WAIT_OBJECT_0 && ret<(WAIT_OBJECT_0 + count))
		return RX_WAIT_0 + (ret - WAIT_OBJECT_0);
	else if (ret == WAIT_TIMEOUT)
		return RX_WAIT_TIMEOUT;
	else
		return RX_WAIT_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// mutex apstractions ( wait and the rest of the stuff
sys_handle_t rx_mutex_create(int initialy_owned)
{
	return CreateMutex(NULL, initialy_owned, NULL);
}
int rx_mutex_destroy(sys_handle_t hndl)
{
	return CloseHandle(hndl);
}
int rx_mutex_aquire(sys_handle_t hndl, dword timeout)
{
	return WaitForSingleObject(hndl, timeout);
}
int rx_mutex_release(sys_handle_t hndl)
{
	return ReleaseMutex(hndl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
// event apstractions ( wait and the rest of the stuff
sys_handle_t rx_event_create(int initialy_set)
{
	return CreateEvent(NULL, FALSE, initialy_set, NULL);
}
int rx_event_destroy(sys_handle_t hndl)
{
	return CloseHandle(hndl);
}
int rx_event_set(sys_handle_t hndl)
{
	return SetEvent(hndl);
}
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// thread apstractions


struct _thread_start_arg_t
{
	void(*start_address)(void*);
	void* arg;

};

unsigned _stdcall _inner_handler(void* arg)
{
	struct _thread_start_arg_t* inner_arg;
	inner_arg = (struct _thread_start_arg_t*)(arg);



	DWORD ret = 0;

#ifdef RELEASE_TRACE

	init_thread_diagnostics();

	try
	{

#endif //RELEASE_TRACE

		CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);

		Sleep(0);// i don't know why, maybe microsoft does!!!


		(inner_arg->start_address)(inner_arg->arg);


		CoUninitialize();


#ifdef RELEASE_TRACE
	}
	catch (char)
	{
		system_exception* exp = static_cast<system_exception*>(TlsGetValue(_trace_eh_tlsIndex));
		if (exp != NULL)
		{
			exp->add_line(__FUNCTION__);
			exp->dump();
		}
		TerminateProcess(GetCurrentProcess(), 113);
	}
#endif //RELEASE_TRACE
	free(arg);
	return RX_ERROR;
}

sys_handle_t rx_thread_create(void(*start_address)(void*), void* arg, int priority, dword* thread_id)
{
	struct _thread_start_arg_t* inner_arg;
	int wpriority;

	switch (priority)
	{
	case RX_PRIORITY_IDLE:
		wpriority = THREAD_PRIORITY_IDLE;
		break;
	case RX_PRIORITY_LOW:
		wpriority = THREAD_PRIORITY_LOWEST;
		break;
	case RX_PRIORITY_BELOW_NORMAL:
		wpriority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case RX_PRIORITY_NORMAL:
		wpriority = THREAD_PRIORITY_NORMAL;
		break;
	case RX_PRIORITY_ABOVE_NORMAL:
		wpriority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case RX_PRIORITY_HIGH:
		wpriority = THREAD_PRIORITY_HIGHEST;
		break;
	case RX_PRIORITY_REALTIME:
		wpriority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	}

	inner_arg = (struct _thread_start_arg_t*)malloc(sizeof(struct _thread_start_arg_t));
	inner_arg->start_address = start_address;
	inner_arg->arg = arg;

	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, _inner_handler, inner_arg, CREATE_SUSPENDED, thread_id);

	if (handle == NULL)
		return NULL;

	
	SetThreadPriority(handle, priority);
	ResumeThread(handle);

	return handle;

}
int rx_thread_join(sys_handle_t what)
{
	return WaitForSingleObject(what, INFINITE) == WAIT_OBJECT_0 ? TRUE : FALSE;
}
int rx_thread_close(sys_handle_t what)
{
	return CloseHandle(what);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// basic apstractions
void rx_msleep(dword timeout)
{
	Sleep(timeout);
}
void rx_usleep(qword timeout)
{

}
dword rx_get_tick_count()
{
	LARGE_INTEGER ret;
	QueryPerformanceCounter(&ret);
	qword now = ret.QuadPart;
	DWORD tick = (DWORD)(now * 1000L / g_res);
	return tick;
}
qword rx_get_us_ticks()
{

	LARGE_INTEGER ret;
	QueryPerformanceCounter(&ret);
	qword now = ret.QuadPart;
	return (now * 1000000L / g_res);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

// directories stuff

size_t drives_count = 0;
char** drives = NULL;

void init_fixed_drives()
{
	int count = 0;
	DWORD drvs = GetLogicalDrives();
	char drv[] = "A:";
	char drvq[] = "A:\\";
	for (char cnt = 0; cnt < 32; cnt++)
	{
		if ((drvs&(0x1 << cnt)) != 0)
		{
			drv[0] = 'A' + cnt;
			drvq[0] = 'A' + cnt;
			UINT type = GetDriveTypeA(drvq);
			if (DRIVE_FIXED == type)
			{
				count++;
			}
		}
	}
	drives = (char**)malloc(sizeof(char*)*count);
	int i = 0;
	drv[0] = 'A';
	drvq[0] = 'A';
	for (char cnt = 0; cnt < 32; cnt++)
	{
		if ((drvs&(0x1 << cnt)) != 0)
		{
			drv[0] = 'A' + cnt;
			drvq[0] = 'A' + cnt;
			UINT type = GetDriveTypeA(drvq);
			if (DRIVE_FIXED == type)
			{
				drives[i] = (char*)malloc(sizeof(char) * 3);
				strcpy_s(drives[i], sizeof(char) * 3, drv);
			}
			i++;
		}
	}
	drives_count = count;
}



void rx_fill_directory_enrty(WIN32_FIND_DATAA* find_data, struct rx_file_directory_entry_t* entry)
{
	strcpy_s(entry->file_name, MAX_PATH, find_data->cFileName);
	entry->is_directory = ((find_data->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	entry->size = find_data->nFileSizeLow;
	entry->time.t_value = (((qword)find_data->ftLastWriteTime.dwHighDateTime) << 32) + find_data->ftLastWriteTime.dwLowDateTime;
}

find_file_handle_t rx_open_find_file_list(const char* path, struct rx_file_directory_entry_t* entry)
{
	char buff[MAX_PATH];
	struct find_file_struct_t* ret = 0;
	dword attrs = INVALID_FILE_ATTRIBUTES;
	BOOL directory = FALSE;

	if (path == NULL || path[0] == '\0')
	{// root directories
		ret = (struct find_file_struct_t*)malloc(sizeof(struct find_file_struct_t));
		ret->handle = 0;
		ret->idx = 0;
		strcpy_s(entry->file_name, MAX_PATH, drives[ret->idx]);
		entry->is_directory = 1;
		entry->size = 0;
		rx_os_get_system_time(&entry->time);
		return ret;
	}
	else
	{
		strcpy_s(buff, MAX_PATH, path);
		if (strlen(path) == 2 && path[1] == ':')
		{
			strcat_s(buff, MAX_PATH, "\\*");
		}
		else
		{
			attrs = GetFileAttributesA(path);
			if (attrs != INVALID_FILE_ATTRIBUTES && ((attrs&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
			{
				strcat_s(buff, MAX_PATH, "\\*");
				directory = TRUE;
			}
		}
		WIN32_FIND_DATAA find_data;
		ZeroMemory(&find_data, sizeof(find_data));
		find_file_handle_t hndl = FindFirstFileA(buff, &find_data);
		if (hndl == INVALID_HANDLE_VALUE)
			return RX_ERROR;

		ret = (struct find_file_struct_t*)malloc(sizeof(struct find_file_struct_t));
		ret->handle = hndl;
		ret->idx = -1;

		rx_fill_directory_enrty(&find_data, entry);

		return ret;
	}
}
int rx_get_next_file(find_file_handle_t hndl, struct rx_file_directory_entry_t* entry)
{
	if (hndl->handle)
	{
		WIN32_FIND_DATAA find_data;
		if (FindNextFileA(hndl->handle, &find_data))
		{
			rx_fill_directory_enrty(&find_data, entry);
			return RX_OK;
		}
		else
			return RX_ERROR;
	}
	else
	{// listing drives
		hndl->idx++;
		if (hndl->idx < drives_count)
		{
			strcpy_s(entry->file_name, MAX_PATH, drives[hndl->idx]);
			entry->is_directory = 1;
			entry->size = 0;
			rx_os_get_system_time(&entry->time);
			return RX_OK;
		}
		else
			return RX_ERROR;
	}
}

void rx_find_file_close(find_file_handle_t hndl)
{
	if (hndl->handle)
		FindClose(hndl->handle);
	free(hndl);
}


///////////////////////////////////////////////////////////////////////////////////////////////
// file handling functions

int rx_create_directory(const char* dir, int fail_on_exsists)
{
	if (!CreateDirectoryA(dir, NULL))
	{
		dword err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS && !fail_on_exsists)
		{
			return RX_OK;
		}
		else
		{
			return RX_ERROR;
		}
	}
	return RX_OK;
}

sys_handle_t rx_file(const char* path, int access, int creation)
{
	DWORD waccess = 0;
	DWORD wshare = 0;
	DWORD wcreation = 0;
	if (access&RX_FILE_OPEN_READ)
		waccess = GENERIC_READ;
	if (access&RX_FILE_OPEN_WRITE)
		waccess |= GENERIC_WRITE;

	wshare = FILE_SHARE_READ;

	switch (creation)
	{
	case RX_FILE_CREATE_ALWAYS:
		wcreation = CREATE_ALWAYS;
		break;
	case RX_FILE_CREATE_NEW:
		wcreation = CREATE_NEW;
		break;
	case RX_FILE_OPEN_ALWAYS:
		wcreation = OPEN_ALWAYS;
		break;
	case RX_FILE_OPEN_EXISTING:
		wcreation = OPEN_EXISTING;
		break;
	}
	sys_handle_t ret = CreateFileA(path, waccess, wshare, NULL, wcreation, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == ret)
	{
		DWORD err = GetLastError();
		return RX_ERROR;
	}
	else
		return ret;
}
int rx_file_read(sys_handle_t hndl, void* buffer, dword size, dword* readed)
{
	dword dummy = 0;
	if (readed == 0)
		readed = &dummy;
	return ReadFile(hndl, buffer, size, readed, NULL);
}
int rx_file_write(sys_handle_t hndl, const void* buffer, dword size, dword* written)
{
	dword dummy = 0;
	if (written == 0)
		written = &dummy;
	if (!WriteFile(hndl, buffer, size, written, NULL))
	{
		DWORD err = GetLastError();
		return RX_ERROR;
	}
	else
		return RX_OK;
}
int rx_file_close(sys_handle_t hndl)
{
	return CloseHandle(hndl);
}

int rx_file_delete(const char* path)
{
	return DeleteFileA(path);
}

int rx_file_rename(const char* old_path, const char* new_path)
{
	return MoveFileA(old_path, new_path);
}

int rx_file_exsist(const char* path)
{
	return _access(path, 0x7fffffff) == 0;
}


int rx_file_get_size(sys_handle_t hndl, qword* size)
{
	LARGE_INTEGER li;
	if (GetFileSizeEx(hndl, &li))
	{
		*size = li.QuadPart;
		return RX_OK;
	}
	else
		return RX_ERROR;
}
int rx_file_get_time(sys_handle_t hndl, struct rx_time_struct_t* tm)
{
	BY_HANDLE_FILE_INFORMATION fi;
	ZeroMemory(&fi, sizeof(fi));
	if (GetFileInformationByHandle(hndl, &fi))
	{
		tm->t_value = (((qword)fi.ftLastWriteTime.dwHighDateTime) << 32) + fi.ftLastWriteTime.dwLowDateTime;
		return RX_OK;
	}
	else
		return RX_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////
// completition ports


rx_kernel_dispather_t rx_create_kernel_dispathcer(int max)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, max);
}
dword rx_destroy_kernel_dispatcher(rx_kernel_dispather_t disp)
{
	return CloseHandle(disp);
}

dword rx_dispatch_events(rx_kernel_dispather_t disp)
{
	BYTE* temp;
	DWORD nBytes=0;
	ULONG_PTR WorkIndex=0;
	LPOVERLAPPED pOvl=NULL;
	struct rx_io_register_data_t* data;
	struct windows_overlapped_t* internal_data;

	BOOL ret = GetQueuedCompletionStatus(disp, &nBytes, &WorkIndex, &pOvl, INFINITE);
	if (ret)
	{
		if (nBytes == (DWORD)(-1))
		{// just post function
			if (WorkIndex == 0)
			{// stop signal return false
				ret = FALSE;
			}
			else
			{// dispatch function
				rx_callback callback = (rx_callback)WorkIndex;
				(callback)(pOvl);
			}
		}
		else
		{// regular io
			data = (struct rx_io_register_data_t*)WorkIndex;
			internal_data = (struct windows_overlapped_t*)data->internal;

			DWORD transfered = 0;
			ret = GetOverlappedResult(data->handle, pOvl, &transfered, FALSE);
			if (ret)
			{
				if (pOvl == &internal_data->m_read)
				{// read operation
					if (transfered != 0)
						(data->read_callback)(data->data, 0, transfered);
					else
						(data->shutdown_callback)(data->data, 255);
				}
				else if (pOvl == &internal_data->m_write)
				{
					(data->write_callback)(data->data, 0);
				}
				else if (pOvl == &internal_data->m_accept)
				{
					temp = (BYTE*)data->read_buffer;
					// skip local address
					temp += ACCEPT_ADDR_SIZE;
					(data->accept_callback)(data->data, 0, (sys_handle_t)internal_data->helper_socket, (struct sockaddr*)temp, (struct sockaddr*)data->read_buffer, transfered - ACCEPT_ADDR_SIZE);
				}
				else if (pOvl == &internal_data->m_connect)
				{
					(data->connect_callback)(data->data, 0);
				}
			}
			else
			{
				(data->shutdown_callback)(data->data, 255);
				ret = TRUE;
			}
		}
	}
	else
	{
		dword err = GetLastError();
		if (pOvl)
		{
			data = (struct rx_io_register_data_t*)WorkIndex;
			internal_data = (struct windows_overlapped_t*)data->internal;
			(data->shutdown_callback)(data->data, err);
			ret = TRUE;
		}

	}

	return ret;
}

dword rx_dispatcher_signal_end(rx_kernel_dispather_t disp)
{
	BOOL ret = PostQueuedCompletionStatus(disp, (DWORD)(-1), 0, NULL);
	if (!ret)
	{
		dword err = GetLastError();
	}
	return ret;
}
dword rx_dispatch_function(rx_kernel_dispather_t disp, rx_callback f, void* arg)
{
	
	BOOL ret = PostQueuedCompletionStatus(disp,(DWORD)(-1), (ULONG_PTR)f, (LPOVERLAPPED)arg);
	if (!ret)
	{
		dword err = GetLastError();
	}
	return ret;
}


dword rx_dispatcher_register(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data)
{
	struct windows_overlapped_t* internal_data = (struct windows_overlapped_t*)data->internal;
	internal_data->m_acceptex = NULL;
	HANDLE ret = CreateIoCompletionPort(data->handle, disp, (ULONG_PTR)data, 0);
	return RX_OK;
}



int rx_dispatcher_unregister(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data)
{
	return 0;// nothing here on windows closing handle is enougth
}

dword rx_socket_read(struct rx_io_register_data_t* what, size_t* readed)
{
	struct windows_overlapped_t* internal_data;
	internal_data = (struct windows_overlapped_t*)what->internal;
	LPOVERLAPPED povl = (LPOVERLAPPED)&internal_data->m_read;
	//Initialize Overlapped
	*readed = 0;
	ZeroMemory(povl, sizeof(OVERLAPPED));
	DWORD read = 0;
	BOOL ret = ReadFile(what->handle, what->read_buffer, (DWORD)what->read_buffer_size, &read, povl);
	if (!ret)
	{
		dword err = GetLastError();
		if (err != ERROR_IO_PENDING)
			return RX_ERROR;
	}
	return RX_ASYNC;
}
dword rx_socket_write(struct rx_io_register_data_t* what, const void* data, size_t count)
{
	struct windows_overlapped_t* internal_data;
	internal_data = (struct windows_overlapped_t*)what->internal;
	LPOVERLAPPED povl = (LPOVERLAPPED)&internal_data->m_write;
	//Initialize Overlapped
	ZeroMemory(povl, sizeof(OVERLAPPED));
	DWORD written = 0;
	BOOL ret = WriteFile(what->handle, data, (DWORD)count, &written, (LPOVERLAPPED)povl);
	if (!ret)
	{
		dword err = GetLastError();
		if (err != ERROR_IO_PENDING)
			return RX_ERROR;
	}
	return RX_ASYNC;
}

dword rx_socket_accept(struct rx_io_register_data_t* what)
{

	int buff = 1024 * 1024;
	int optlen = 4;
	BOOL nodly = TRUE;
	int err;
	dword error;
	DWORD recived = 0;
	struct windows_overlapped_t* internal_data = (struct windows_overlapped_t*)what->internal;
	LPOVERLAPPED povl = (LPOVERLAPPED)&internal_data->m_accept;
	// initialize AcceptEx
	if (internal_data->m_acceptex == NULL)
	{
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		DWORD returned = 0;
		int iret = WSAIoctl((SOCKET)what->handle, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidAcceptEx, sizeof(GuidAcceptEx),
			&internal_data->m_acceptex, sizeof(internal_data->m_acceptex),
			&returned, NULL, NULL);
		if (iret == SOCKET_ERROR)
		{
			return RX_ERROR;
		}
	}

	internal_data->helper_socket = (SOCKET)rx_create_and_bind_ip4_tcp_socket(NULL);
	if (internal_data->helper_socket == 0)
	{
		error = WSAGetLastError();
		return RX_ERROR;
	}


	//Initialize Overlapped
	ZeroMemory(povl, sizeof(OVERLAPPED));
	DWORD written = 0;
	BOOL ret = (internal_data->m_acceptex)((SOCKET)what->handle, internal_data->helper_socket, what->read_buffer, 0, ACCEPT_ADDR_SIZE, ACCEPT_ADDR_SIZE, &recived, (LPOVERLAPPED)povl);
	if (!ret)
	{
		err = GetLastError();
		if (err != ERROR_IO_PENDING)
			return RX_ERROR;
	}
	return RX_ASYNC;
}



dword rx_socket_connect(struct rx_io_register_data_t* what, struct sockaddr* addr, size_t addrsize)
{
	dword err;
	DWORD sent = 0;
	struct windows_overlapped_t* internal_data = (struct windows_overlapped_t*)what->internal;
	LPOVERLAPPED povl = (LPOVERLAPPED)&internal_data->m_connect;
	// initialize ConnectEx
	if (internal_data->m_connectex == NULL)
	{
		GUID guidConnectEx = WSAID_CONNECTEX;
		DWORD returned = 0;
		int iret = WSAIoctl((SOCKET)what->handle, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidConnectEx, sizeof(guidConnectEx),
			&internal_data->m_connectex, sizeof(internal_data->m_connectex),
			&returned, NULL, NULL);
		if (iret == SOCKET_ERROR)
		{
			return RX_ERROR;
		}
	}

	//Initialize Overlapped
	ZeroMemory(povl, sizeof(OVERLAPPED));

	BOOL ret = (internal_data->m_connectex)((SOCKET)what->handle, addr, (int)addrsize,NULL, 0, NULL, (LPOVERLAPPED)povl);
	if (!ret)
	{
		err = GetLastError();
		if (err == ERROR_IO_PENDING)
			ret = TRUE;
	}

	return ret != FALSE ? RX_ASYNC : RX_ERROR;
}

sys_handle_t rx_create_and_bind_ip4_tcp_socket(struct sockaddr_in* addr)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		int err = WSAGetLastError();     // convenience for the debugger
		return NULL;
	}

	int on = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));

	//////////////////////////////////////////////////////////////
	// keep alive settings
	struct tcp_keepalive kalive;
	kalive.onoff = 1;
	kalive.keepalivetime = 5000;
	kalive.keepaliveinterval = 50;

	DWORD returned = 0;

	DWORD bread = WSAIoctl(sock, SIO_KEEPALIVE_VALS, &kalive, sizeof(kalive), NULL, 0, &returned, NULL, NULL);

	
	if (addr)
	{
		addr->sin_family = AF_INET;// just in case

		if (bind(sock, (PSOCKADDR)addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			closesocket(sock);
			return NULL;
		}
	}
	return (sys_handle_t)sock;
}

dword rx_socket_listen(sys_handle_t handle)
{
	if (listen((SOCKET)handle, SOMAXCONN) == SOCKET_ERROR)
	{
		return RX_ERROR;
	}
	else
	{
		return RX_OK;
	}
}

void rx_close_socket(sys_handle_t handle)
{
	int err = 0;
	if (SOCKET_ERROR == shutdown((SOCKET)handle, 2))
		err = WSAGetLastError();
	closesocket((SOCKET)handle);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TLS code
rx_thread_data_t rx_alloc_thread_data()
{
	return TlsAlloc();
}
void rx_set_thread_data(rx_thread_data_t key, void* data)
{
	BOOL ret = TlsSetValue(key, data);
}
void* rx_get_thread_data(rx_thread_data_t key)
{
	return TlsGetValue(key);
}
void rx_free_thread_data(rx_thread_data_t key)
{
	TlsFree(key);
}
///////////////////////////////////////////////////////////////////////////////////////////////////


