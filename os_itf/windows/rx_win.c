

/****************************************************************************
*
*  os_itf\windows\rx_win.c
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




// rx_win
#include "os_itf/windows/rx_win.h"

#include "third-party/win_internals/ntdll.h"
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




typedef DWORD(__stdcall * 
	NtSetInformationProcess_t)(
	IN HANDLE               ProcessHandle,
	IN PROCESS_INFORMATION_CLASS ProcessInformationClass,
	IN PVOID                ProcessInformation,
	IN ULONG                ProcessInformationLength);


typedef DWORD(__stdcall *
	NtSetTimerResolution_t) (
	IN ULONG RequestedResolution,
	IN BOOLEAN Set,
	OUT PULONG ActualResolution
	);

typedef DWORD(__stdcall *
	NtQueryTimerResolution_t) (
	OUT PULONG              MinimumResolution,
	OUT PULONG              MaximumResolution,
	OUT PULONG              CurrentResolution
	);


void init_fixed_drives();

int rx_big_endian = 0;
rx_thread_data_t rx_tls = 0;
int rx_hd_timer = 1;


const char* rx_server_name = NULL;
char node_name_buff[0x100];
void collect_computer_name()
{
	DWORD szname = sizeof(node_name_buff);
	GetComputerNameA(node_name_buff, &szname);
	rx_server_name = node_name_buff;
}
const char* rx_get_node_name()
{
	if(rx_server_name==NULL)
		collect_computer_name();

	return rx_server_name;
}


const char* g_ositf_version = "ERROR!!!";
char ver_buffer[0x100];
rx_pid_t rx_pid;

LPWSTR LpcPortName = L"\\rx_platform_port";      // Name of the LPC port
#define LPC_DATA_SIZE 0x10
#define LPC_MSG_SIZE LPC_DATA_SIZE+sizeof(PORT_MESSAGE)
void test_lpc(void* arg)
{

	SECURITY_DESCRIPTOR sd;
	OBJECT_ATTRIBUTES ObjAttr;              // Object attributes for the name
	UNICODE_STRING PortName;
	NTSTATUS Status;
	HANDLE LpcPortHandle = NULL;
	HANDLE ServerHandle = NULL;
	BYTE RequestBuffer[LPC_MSG_SIZE];
	BYTE OutBuffer[LPC_MSG_SIZE];
	BOOL WeHaveToStop = FALSE;
	int nError;

	PPORT_MESSAGE message = (PPORT_MESSAGE)&RequestBuffer;
	PPORT_MESSAGE out_message = (PPORT_MESSAGE)&OutBuffer;

	__try     // try-finally
	{
		//
		// Initialize security descriptor that will be set to
		// "Everyone has the full access"
		//

		if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		{
			nError = GetLastError();
			__leave;
		}

		//
		// Set the empty DACL to the security descriptor
		//

		if (!SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE))
		{
			nError = GetLastError();
			__leave;
		}

		//
		// Initialize attributes for the port and create it
		//

		RtlInitUnicodeString(&PortName, LpcPortName);
		InitializeObjectAttributes(&ObjAttr, &PortName, 0, NULL, &sd);

		wprintf(L"Server: Creating LPC port \"%s\" (NtCreatePort) ...\n", LpcPortName);
		Status = NtCreatePort(&LpcPortHandle,
			&ObjAttr,
			0,
			sizeof(PORT_MESSAGE) + MAX_LPC_DATA,
			0);
		printf("Server: NtCreatePort result 0x%08lX\n", Status);

		if (!NT_SUCCESS(Status))
			__leave;

		while (WeHaveToStop == FALSE)
		{
			if (NT_SUCCESS(Status))
			{
				printf("Server: Listening to LPC port (NtListenPort) ...\n");
				Status = NtListenPort(LpcPortHandle,
					message);
				printf("Server: NtListenPort result 0x%08lX\n", Status);
			}

			//
			// Accept the port connection
			//

			if (NT_SUCCESS(Status))
			{
				printf("Server: Accepting LPC connection (NtAcceptConnectPort) ...\n");
				Status = NtAcceptConnectPort(&ServerHandle,
					NULL,
					message,
					TRUE,
					NULL,
					NULL);
				printf("Server: NtAcceptConnectPort result 0x%08lX\n", Status);
			}

			//
			// Complete the connection
			//

			if (NT_SUCCESS(Status))
			{
				printf("Server: Completing LPC connection (NtCompleteConnectPort) ...\n");
				Status = NtCompleteConnectPort(ServerHandle);
				printf("Server: NtCompleteConnectPort result 0x%08lX\n", Status);
			}

			//
			// Now accept the data request coming from the port.
			//

			if (NT_SUCCESS(Status))
			{
				printf("Server: Receiving LPC data (NtReplyWaitReceivePort) ...\n");
				Status = NtReplyWaitReceivePort(ServerHandle,
					NULL,
					NULL,
					message);
				printf("Server: NtReplyWaitReceivePort result 0x%08lX\n", Status);
			}
			if (NT_SUCCESS(Status))
			{
				memcpy(out_message, message,sizeof(PORT_MESSAGE));
				printf("Server: Sending reply (NtReplyPort) ...\n");
				Status = NtReplyPort(LpcPortHandle, out_message);
				printf("Server: NtReplyPort result 0x%08lX\n", Status);
			}
		}
	}
	__finally
	{
		if (LpcPortHandle != NULL)
			NtClose(LpcPortHandle);
	}

}

void test_client_lpc()
{
	SECURITY_QUALITY_OF_SERVICE SecurityQos;
	UNICODE_STRING PortName;
	NTSTATUS Status = STATUS_SUCCESS;
	HANDLE PortHandle = NULL;
	ULONG MaxMessageLength = 0;
	BYTE OutBuffer[LPC_MSG_SIZE];
	BYTE InBuffer[LPC_MSG_SIZE];
	PPORT_MESSAGE out_message = (PPORT_MESSAGE)&OutBuffer;
	PPORT_MESSAGE in_message = (PPORT_MESSAGE)&InBuffer;
	InitializeMessageHeader(out_message, LPC_MSG_SIZE, 0);
	InitializeMessageHeader(out_message, LPC_MSG_SIZE, 0);


	RtlInitUnicodeString(&PortName, LpcPortName);
	SecurityQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
	SecurityQos.ImpersonationLevel = SecurityImpersonation;
	SecurityQos.EffectiveOnly = FALSE;
	SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;

	printf("Client: Test sending LPC data of size less than 0x%lX bytes ...\n", MAX_LPC_DATA);
	wprintf(L"Client: Connecting to port \"%s\" (NtConnectPort) ...\n", LpcPortName);
	Status = NtConnectPort(&PortHandle,
		&PortName,
		&SecurityQos,
		NULL,
		NULL,
		&MaxMessageLength,
		NULL,
		NULL);
	printf("Client: NtConnectPort result 0x%08lX\n", Status);

	printf("Client: Sending request, waiting for reply (NtRequestWaitReplyPort)\n");
	Status = NtRequestWaitReplyPort(PortHandle, out_message, in_message);
	printf("Client: NtRequestWaitReplyPort result 0x%08lX\n", Status);
	Sleep(500);

	printf("Client: Closing the port (NtClose) \n");
	Status = NtClose(PortHandle);
	printf("Client: NtClose result 0x%08lX\n", Status);
}
void rx_init_hal_version()
{
	create_module_version_string(RX_HAL_NAME, RX_HAL_MAJOR_VERSION, RX_HAL_MINOR_VERSION, RX_HAL_BUILD_NUMBER, __DATE__, __TIME__, ver_buffer);
	g_ositf_version = ver_buffer;
}

struct WSAData wsaData;
int init_common_result = RX_ERROR;


uint32_t tlc_max_token = 0;

void rx_initialize_os(int rt, int hdt, rx_thread_data_t tls, int is_debug)
{
	rx_platform_init_data common_data;
	common_data.rx_hd_timer = hdt;
	common_data.is_debug = is_debug;

	init_common_result = rx_init_common_library(&common_data);
	
	collect_computer_name();



	PSecPkgInfoA pkgInfo;
	SECURITY_STATUS ss = QuerySecurityPackageInfoA(UNISP_NAME_A, &pkgInfo);

	if (ss == SEC_E_OK)
	{
		tlc_max_token = pkgInfo->cbMaxToken;
	}

	create_module_version_string(RX_HAL_NAME, RX_HAL_MAJOR_VERSION, RX_HAL_MINOR_VERSION, RX_HAL_BUILD_NUMBER, __DATE__, __TIME__, ver_buffer);
	g_ositf_version = ver_buffer;

	rx_tls = tls;
	rx_hd_timer = hdt;
	rx_pid = GetCurrentProcessId();
	// determine big endian or little endian
	union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };
	rx_big_endian = (bint.c[0] == 1 ? 1 : 0);



	DWORD err = 0;

	HMODULE hLib = LoadLibraryEx(L"ntdll.dll",NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	//HMODULE hLib = GetModuleHandle(L"ntdll.dl");

	if (hLib == NULL)
		err = GetLastError();


	if (hLib)
	{
		NtSetTimerResolution_t set = (NtSetTimerResolution_t)GetProcAddress(hLib, "NtSetTimerResolution");
		NtQueryTimerResolution_t query = (NtQueryTimerResolution_t)GetProcAddress(hLib, "NtQueryTimerResolution");

		if (query)
		{
			ULONG min = 0;
			ULONG max = 0;
			ULONG current = 0;

			DWORD ret = (query)(&min, &max, &current);
					   		}

		if (set)
		{
			ULONG act = 0;

			DWORD ret = (set)(500 * 10, TRUE, &act);
		}

		FreeLibrary(hLib);
	}
	init_fixed_drives();

	WSAStartup(MAKEWORD(2, 2), &wsaData);




#ifndef _DEBUG
	if (rt)
	{
		//SE_INC_BASE_PRIORITY_NAME

		BOOL pret = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	}
#endif
}

int drives_count = 0;
char** drives = NULL;

void rx_deinitialize_os()
{
	int i = 0;

	if (drives_count > 0 && drives)
	{
		for (; i < drives_count; i++)
			free(drives[i]);
	}
	if(drives)
		free(drives);

	WSACleanup();

	if (init_common_result)
		rx_deinit_common_library();
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
			temp_write = NULL;
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
	if (!ret || writen != size)
	{
		return RX_ERROR;
	}


	return RX_OK;
}
int rx_write_pipe_client(struct pipe_client_t* pipes, const void* data, size_t size)
{
	DWORD writen = 0;
	BOOL ret = WriteFile(pipes->client_write, data, (DWORD)size, &writen, NULL);
	if (!ret || writen != size)
	{
		return RX_ERROR;
	}


	return RX_OK;
}
int rx_read_pipe_server(struct pipe_server_t* pipes, uint8_t* data, size_t* size)
{
	DWORD read = 0;
	DWORD to_read = (DWORD)(*size);
	BOOL ret = ReadFile(pipes->server_read, data, to_read, &read, NULL);
	if (!ret)
	{
		return RX_ERROR;
	}
	*size = read;
	return RX_OK;
}
int rx_read_pipe_client(struct pipe_client_t* pipes, uint8_t* data, size_t* size)
{
	DWORD read = 0;
	DWORD to_read = (DWORD)(*size);
	BOOL ret = ReadFile(pipes->client_read, data, to_read, &read, NULL);
	if (!ret)
	{
		DWORD err = GetLastError();
		return RX_ERROR;
	}
	*size = read;
	return RX_OK;
}
//////////////////////////////////////////////////////////////////////////////
// system info

// RTL_OSVERSIONINFOEXW is defined in winnt.h
BOOL GetOsVersion(RTL_OSVERSIONINFOEXW* pk_OsVer)
{
	
	typedef LONG(WINAPI* tRtlGetVersion)(RTL_OSVERSIONINFOEXW*);

	memset(pk_OsVer, 0, sizeof(RTL_OSVERSIONINFOEXW));
	pk_OsVer->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	tRtlGetVersion f_RtlGetVersion = NULL;
	HMODULE hLib = LoadLibraryEx(L"ntdll.dll", NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (hLib)
	{
		f_RtlGetVersion = (tRtlGetVersion)GetProcAddress(hLib, "RtlGetVersion");
	}

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


	BOOL ret = GetOsVersion(os);
	if (ret)
	{
		os->wProductType = IsWindowsServer() ? VER_NT_SERVER : VER_NT_WORKSTATION;
	}
	return ret;

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
	uint32_t err = 0;
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
void rx_collect_processor_info(char* buffer, size_t buffer_size, size_t* count)
{
	SYSTEM_INFO sys;
	ZeroMemory(&sys, sizeof(sys));
	GetSystemInfo(&sys);	
	char temp_buff[0x100];
	char name_buff[0x100];
	name_buff[0] = L'\0';

	for (uint32_t i = 0; i < sys.dwNumberOfProcessors; i++)
	{
		sprintf(temp_buff, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", i);

		HKEY hkey = NULL;
		DWORD reg_count = (uint32_t)sizeof(name_buff);
		if (ERROR_SUCCESS == RegGetValueA(HKEY_LOCAL_MACHINE, temp_buff, "ProcessorNameString", RRF_RT_REG_SZ, NULL, name_buff, &reg_count))
		{

		}
		break;
	}
	*count = sys.dwNumberOfProcessors;
	sprintf(buffer, "%s ; Total Cores:%d", name_buff, sys.dwNumberOfProcessors);
}
void rx_collect_memory_info(size_t* total, size_t* free, size_t* process)
{
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	*total = (size_t)statex.ullTotalPhys;
	*free = (size_t)statex.ullAvailPhys;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// thread abstractions


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

#ifdef RX_PLATFORM_USE_COM

		CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);

#endif //RX_PLATFORM_USE_COM

		Sleep(0);// i don't know why, maybe microsoft does!!!

		srand((unsigned int)rx_get_tick_count());
		
		(inner_arg->start_address)(inner_arg->arg);
		
#ifdef RX_PLATFORM_USE_COM

		CoUninitialize();

#endif //RX_PLATFORM_USE_COM


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

sys_handle_t rx_thread_create(void(*start_address)(void*), void* arg, int priority, uint32_t* thread_id, const char* name)
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

	wchar_t buff[0x100];
	buff[0] = L'\0';
	MultiByteToWideChar(CP_ACP, 0, name, -1, buff, sizeof(buff) / sizeof(0));

	//SetThreadDescription(handle, buff);
	
	SetThreadPriority(handle, wpriority);
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
sys_handle_t rx_current_thread()
{
	return (sys_handle_t)(intptr_t)GetCurrentThreadId();
}
int rx_thread_set_afinity(sys_handle_t what, uint64_t mask)
{
	DWORD_PTR ret_val = SetThreadAffinityMask(what, mask);
	return ret_val ? RX_OK : RX_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// directories stuff


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
				if (i >= count)
					break;
				drives[i] = (char*)malloc(sizeof(char) * 3);
				strcpy_s(drives[i], sizeof(char) * 3, drv);
				i++;
			}
		}
	}
	drives_count = count;
}



void rx_fill_directory_enrty(WIN32_FIND_DATAA* find_data, struct rx_file_directory_entry_t* entry)
{
	strcpy_s(entry->file_name, MAX_PATH, find_data->cFileName);
	entry->is_directory = ((find_data->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	entry->size = find_data->nFileSizeLow;
	entry->time.t_value = (((uint64_t)find_data->ftLastWriteTime.dwHighDateTime) << 32) + find_data->ftLastWriteTime.dwLowDateTime;
}

find_file_handle_t rx_open_find_file_list(const char* path, struct rx_file_directory_entry_t* entry)
{
	char buff[MAX_PATH];
	struct find_file_struct_t* ret = 0;
	uint32_t attrs = INVALID_FILE_ATTRIBUTES;
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
		uint32_t err = GetLastError();
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
int rx_file_read(sys_handle_t hndl, void* buffer, uint32_t size, uint32_t* readed)
{
	uint32_t dummy = 0;
	if (readed == 0)
		readed = &dummy;
	return ReadFile(hndl, buffer, size, readed, NULL);
}
int rx_file_write(sys_handle_t hndl, const void* buffer, uint32_t size, uint32_t* written)
{
	uint32_t dummy = 0;
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
	return _access(path, 0) == 0;
}


int rx_file_get_size(sys_handle_t hndl, uint64_t* size)
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
int rx_file_get_time(sys_handle_t hndl, struct rx_time_struct_t* created, struct rx_time_struct_t* modified)
{
	BY_HANDLE_FILE_INFORMATION fi;
	ZeroMemory(&fi, sizeof(fi));
	if (GetFileInformationByHandle(hndl, &fi))
	{
		modified->t_value = (((uint64_t)fi.ftLastWriteTime.dwHighDateTime) << 32) + fi.ftLastWriteTime.dwLowDateTime;
		created->t_value = (((uint64_t)fi.ftCreationTime.dwHighDateTime) << 32) + fi.ftCreationTime.dwLowDateTime;
		return RX_OK;
	}
	else
		return RX_ERROR;
}

int rx_file_get_time_from_path(const char* path, struct rx_time_struct_t* created, struct rx_time_struct_t* modified)
{
	WIN32_FILE_ATTRIBUTE_DATA fi;
	ZeroMemory(&fi, sizeof(fi));
	if (GetFileAttributesExA(path, GetFileExInfoStandard, &fi))
	{
		modified->t_value = (((uint64_t)fi.ftLastWriteTime.dwHighDateTime) << 32) + fi.ftLastWriteTime.dwLowDateTime;
		created->t_value = (((uint64_t)fi.ftCreationTime.dwHighDateTime) << 32) + fi.ftCreationTime.dwLowDateTime;
		return RX_OK;
	}
	else
		return RX_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////
// completion ports


rx_kernel_dispather_t rx_create_kernel_dispathcer(int max)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, max);
}
uint32_t rx_destroy_kernel_dispatcher(rx_kernel_dispather_t disp)
{
	return CloseHandle(disp);
}

uint32_t rx_dispatch_events(rx_kernel_dispather_t disp)
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
					// READLOOP
					//if (transfered != 0)
						(data->read_callback)(data->data, 0, transfered);
					/*else
						(data->shutdown_callback)(data->data, 213);*/
				}
				else if (pOvl == &internal_data->m_write)
				{
					(data->write_callback)(data->data, 0);
				}
				else if (pOvl == &internal_data->m_read_from)
				{// read operation
					if (transfered != 0)
					{
						temp = (BYTE*)data->read_buffer;
						(data->read_from_callback)(data->data, 0, transfered, (struct sockaddr*)&temp[data->read_buffer_size - SOCKET_ADDR_SIZE - sizeof(INT)], SOCKET_ADDR_SIZE);
					}
					else
					{
						(data->shutdown_callback)(data->data, 255);
					}
				}
				else if (pOvl == &internal_data->m_accept)
				{
					temp = (BYTE*)data->read_buffer;
					// skip local address
					temp += SOCKET_ADDR_SIZE;
					(data->accept_callback)(data->data, 0, (sys_handle_t)internal_data->helper_socket, (struct sockaddr*)temp, (struct sockaddr*)data->read_buffer);
				}
				else if (pOvl == &internal_data->m_connect)
				{
					struct sockaddr* local_addr = NULL;
					struct sockaddr* remote_addr = NULL;
					struct sockaddr_storage local;
					int local_size = sizeof(local);
					struct sockaddr_storage remote;
					int remote_size = sizeof(remote);
					if (0 == getsockname((SOCKET)data->handle, (struct sockaddr*)&local, &local_size))
						local_addr = (struct sockaddr*)&local;
					setsockopt((SOCKET)data->handle, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
					if (0 == getpeername((SOCKET)data->handle, (struct sockaddr*)&remote, &remote_size))
						remote_addr = (struct sockaddr*)&remote;
					else
					{
						int err = WSAGetLastError();
					}

					(data->connect_callback)(data->data, 0, remote_addr, local_addr);
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
		uint32_t err = GetLastError();
		if (pOvl)
		{
			data = (struct rx_io_register_data_t*)WorkIndex;
			internal_data = (struct windows_overlapped_t*)data->internal;
			if (err == ERROR_PORT_UNREACHABLE && pOvl == &internal_data->m_read_from)
			{
				(data->read_from_callback)(data->data, 0, 0, NULL, 0);
				ret = TRUE;
			}
			else
			{
				(data->shutdown_callback)(data->data, err);
				ret = TRUE;
			}
		}

	}

	return ret;
}

uint32_t rx_dispatcher_signal_end(rx_kernel_dispather_t disp)
{
	BOOL ret = PostQueuedCompletionStatus(disp, (DWORD)(-1), 0, NULL);
	if (!ret)
	{
		uint32_t err = GetLastError();
	}
	return ret;
}
uint32_t rx_dispatch_function(rx_kernel_dispather_t disp, rx_callback f, void* arg)
{
	
	BOOL ret = PostQueuedCompletionStatus(disp,(DWORD)(-1), (ULONG_PTR)f, (LPOVERLAPPED)arg);
	if (!ret)
	{
		uint32_t err = GetLastError();
	}
	return ret;
}


uint32_t rx_dispatcher_register(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data)
{
	struct windows_overlapped_t* internal_data = (struct windows_overlapped_t*)data->internal;
	internal_data->m_acceptex = NULL;
	HANDLE ret = CreateIoCompletionPort(data->handle, disp, (ULONG_PTR)data, 0);
	return RX_OK;
}



int rx_dispatcher_unregister(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data)
{
	return 0;// nothing here on windows closing handle is enough
}

uint32_t rx_io_read(struct rx_io_register_data_t* what, size_t* readed)
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
		uint32_t err = GetLastError();
		if (err != ERROR_IO_PENDING)
			return RX_ERROR;
	}
	return RX_ASYNC;
}
uint32_t rx_io_write(struct rx_io_register_data_t* what, const void* data, size_t count)
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
		uint32_t err = GetLastError();
		if (err != ERROR_IO_PENDING)
			return RX_ERROR;
	}
	return RX_ASYNC;
}


uint32_t rx_socket_read_from(struct rx_io_register_data_t* what, size_t* readed, struct sockaddr_storage* addr)
{
	struct sockaddr* sock_addr;
	struct windows_overlapped_t* internal_data;
	internal_data = (struct windows_overlapped_t*)what->internal;
	LPOVERLAPPED povl = (LPOVERLAPPED)&internal_data->m_read_from;
	//Initialize Overlapped
	*readed = 0;
	ZeroMemory(povl, sizeof(OVERLAPPED));
	DWORD read = 0;
	WSABUF buff;
	buff.buf = (CHAR*)what->read_buffer;
	buff.len = (ULONG)what->read_buffer_size - SOCKET_ADDR_SIZE - sizeof(INT);
	sock_addr = (struct sockaddr*)&buff.buf[buff.len];
	INT* fromlen = (INT*)(&buff.buf[buff.len + SOCKET_ADDR_SIZE]);
	*fromlen = (INT)sizeof(struct sockaddr_storage);
	DWORD flags = 0;
	auto ret = WSARecvFrom((SOCKET)what->handle, &buff, 1, NULL, &flags, sock_addr, fromlen, povl, NULL);
	if (ret == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
			return RX_ERROR;
	}
	else
	{
		return RX_ASYNC;
	}
	return RX_ASYNC;
}
uint32_t rx_socket_write_to(struct rx_io_register_data_t* what, const void* data, size_t count, const struct sockaddr* addr, size_t addrsize)
{
	struct windows_overlapped_t* internal_data;
	internal_data = (struct windows_overlapped_t*)what->internal;
	LPOVERLAPPED povl = (LPOVERLAPPED)&internal_data->m_write;
	//Initialize Overlapped
	ZeroMemory(povl, sizeof(OVERLAPPED));
	DWORD written = 0;
	WSABUF buff;
	buff.buf = (CHAR*)data;
	buff.len = (ULONG)count;
	auto ret = WSASendTo((SOCKET)what->handle, &buff, 1, &written, 0, addr, (int)addrsize, (LPOVERLAPPED)povl, NULL);
	if (ret == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err == ERROR_PORT_UNREACHABLE)
			return RX_OK;// this is UDP so no error is expected
		if (err != WSA_IO_PENDING)
			return RX_ERROR;
	}
	return RX_ASYNC;
}


uint32_t rx_socket_accept(struct rx_io_register_data_t* what, uint32_t keep_alive)
{

	int buff = 1024 * 1024;
	int optlen = 4;
	BOOL nodly = TRUE;
	int err;
	uint32_t error;
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

	internal_data->helper_socket = (SOCKET)rx_create_and_bind_ip4_tcp_socket(NULL, keep_alive);
	if (internal_data->helper_socket == 0)
	{
		error = WSAGetLastError();
		SetLastError(error);
		return RX_ERROR;
	}


	//Initialize Overlapped
	ZeroMemory(povl, sizeof(OVERLAPPED));
	DWORD written = 0;
	BOOL ret = (internal_data->m_acceptex)((SOCKET)what->handle, internal_data->helper_socket, what->read_buffer, 0, SOCKET_ADDR_SIZE, SOCKET_ADDR_SIZE, &recived, (LPOVERLAPPED)povl);
	if (!ret)
	{
		err = GetLastError();
		if (err != ERROR_IO_PENDING)
		{
			SetLastError(err);
			return RX_ERROR;
		}
	}
	return RX_ASYNC;
}



uint32_t rx_socket_connect(struct rx_io_register_data_t* what, const struct sockaddr* addr, size_t addrsize)
{
	uint32_t err;
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
	struct sockaddr_in* in_addr = (struct sockaddr_in*)addr;

	BOOL ret = (internal_data->m_connectex)((SOCKET)what->handle, addr, (int)addrsize,NULL, 0, NULL, (LPOVERLAPPED)povl);
	if (!ret)
	{
		err = GetLastError();
		if (err == ERROR_IO_PENDING)
			ret = TRUE;
	}

	return ret != FALSE ? RX_ASYNC : RX_ERROR;
}

sys_handle_t rx_create_and_bind_ip4_tcp_socket(const struct sockaddr_in* addr, uint32_t keep_alive)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		int err = WSAGetLastError();     // convenience for the debugger
		SetLastError(err);
		return NULL;
	}

	int on = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));

	ULONG buff_len = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&buff_len, sizeof(buff_len));
	buff_len = 0;//just in case...
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&buff_len, sizeof(buff_len));
	

	//////////////////////////////////////////////////////////////
	// keep alive settings
	if (keep_alive > 0)
	{
		struct tcp_keepalive kalive;
		kalive.onoff = 1;
		kalive.keepalivetime = keep_alive;
		kalive.keepaliveinterval = 50;

		DWORD returned = 0;

		DWORD bread = WSAIoctl(sock, SIO_KEEPALIVE_VALS, &kalive, sizeof(kalive), NULL, 0, &returned, NULL, NULL);
	}

	
	if (addr)
	{
		if (addr->sin_family != AF_INET || bind(sock, (PSOCKADDR)addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			closesocket(sock);
			SetLastError(err);
			return NULL;
		}
	}
	return (sys_handle_t)sock;
}

sys_handle_t rx_create_and_bind_ip4_udp_socket(const struct sockaddr_in* addr)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		int err = WSAGetLastError();     // convenience for the debugger
		SetLastError(err);
		return NULL;
	}

	int on = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on));

	if (addr->sin_port != 0)
	{//this is listen or udp server socket mark resuse
		on = 1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	}

	ULONG buff_len = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&buff_len, sizeof(buff_len));
	buff_len = 0;//just in case...
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&buff_len, sizeof(buff_len));
	
	
	if (addr)
	{
		if (addr->sin_family != AF_INET || bind(sock, (PSOCKADDR)addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			closesocket(sock);
			SetLastError(err);
			return NULL;
		}
	}
	else
	{
		struct sockaddr_in any;
		any.sin_family = AF_INET;
		any.sin_port = 0;
		any.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(sock, (PSOCKADDR)&any, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			closesocket(sock);
			SetLastError(err);
			return NULL;
		}
	}
	return (sys_handle_t)sock;
}

uint32_t rx_socket_listen(sys_handle_t handle)
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
	{
		err = WSAGetLastError();
		SetLastError(err);
	}
	closesocket((SOCKET)handle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// COM port abstractions

void fill_dbc(DCB* dcb, uint32_t baud_rate, int stop_bits, int parity, uint8_t data_bits, int handshake)
{
	/*
	dcb->ByteSize=7;
	dcb->BaudRate=4800;
	dcb->StopBits=ONESTOPBIT;
	dcb->Parity=EVENPARITY;
	dcb->fRtsControl=RTS_CONTROL_ENABLE;
	dcb->fDtrControl=DTR_CONTROL_ENABLE;
	dcb->fOutxCtsFlow=FALSE;
	dcb->fOutxDsrFlow=FALSE;
	dcb->fOutX=FALSE;
	dcb->fInX=FALSE;
	dcb->fDsrSensitivity=FALSE;
	dcb->fAbortOnError=FALSE;
	*/
	dcb->ByteSize = data_bits;
	dcb->BaudRate = baud_rate;
	dcb->StopBits = stop_bits;
	dcb->Parity = parity;
	if (!handshake)
	{
		dcb->fRtsControl = RTS_CONTROL_ENABLE;
		dcb->fDtrControl = DTR_CONTROL_DISABLE;
		dcb->fOutxCtsFlow = FALSE;
		dcb->fOutxDsrFlow = FALSE;
		dcb->fDsrSensitivity = FALSE;
	}
	else
	{
		dcb->fRtsControl = RTS_CONTROL_HANDSHAKE;
		dcb->fDtrControl = DTR_CONTROL_HANDSHAKE;
		dcb->fOutxCtsFlow = TRUE;
		dcb->fOutxDsrFlow = TRUE;
		dcb->fDsrSensitivity = TRUE;

	}
	dcb->fOutX = FALSE;
	dcb->fInX = FALSE;
	dcb->fDsrSensitivity = FALSE;
	dcb->fAbortOnError = FALSE;
}

sys_handle_t rx_open_serial_port(const char* port, uint32_t baud_rate, int stop_bits, int parity, uint8_t data_bits, int handshake)
{
	char name[64];
	sprintf_s(name, 64, "\\\\.\\%s", port);
	sys_handle_t ret = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (ret != INVALID_HANDLE_VALUE)
	{
		BOOL success = FALSE;
		DCB dcb;
		ZeroMemory(&dcb, sizeof(dcb));
		dcb.DCBlength = sizeof(dcb);
		if (GetCommState(ret, &dcb))
		{
			fill_dbc(&dcb, baud_rate, stop_bits, parity, data_bits, handshake);
			dcb.fAbortOnError = 0;
			success = SetCommState(ret, &dcb);
			if (success)
			{
				COMMTIMEOUTS time;
				time.ReadIntervalTimeout = 1;
				time.ReadTotalTimeoutConstant = MAXDWORD;
				time.ReadTotalTimeoutMultiplier = 0;
				time.WriteTotalTimeoutConstant = 0;
				time.WriteTotalTimeoutMultiplier = 0;

				success = SetCommTimeouts(ret, &time);
			}
		}
		if (!success)
		{
			CloseHandle(ret);
			ret = NULL;
		}
	}
	else
	{
		ret = NULL;
	}
	return ret;
}
void rx_close_serial_port(sys_handle_t handle)
{
	CloseHandle(handle);
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


