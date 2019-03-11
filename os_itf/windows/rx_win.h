

/****************************************************************************
*
*  os_itf\windows\rx_win.h
*
*  Copyright (c) 2018-2019 Dusan Ciric
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


#ifndef rx_win_h
#define rx_win_h 1



#include "targetver.h"


#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_RAND_S
#define NOMINMAX

#include <WinSock2.h>
#include <windows.h>



#include "rx_win32_version.h"


#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING__snprintfS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define RX_COMPILER_NAME "MSC"
#define RX_COMPILER_VERSION (_MSC_VER/100-5)
#define RX_COMPILER_MINOR (_MSC_VER%100)
#define RX_COMPILER_BUILD (_MSC_FULL_VER%100000)

#define _CRT_RAND_S


#ifndef _rx_func_
#define _rx_func_ __FUNCTION__
#endif



#include <winsock2.h>
#include <Mswsock.h>
#include <windows.h>
#include <crtdefs.h>
#include <wincrypt.h>
#include <IPHlpApi.h>

typedef HCRYPTKEY crypt_key_t;
typedef HCRYPTHASH  crypt_hash_t;


#ifdef _DEBUG
#define RX_ASSERT(v) assert(v)
#else
#define RX_ASSERT 
#endif

//#define snprintf _snprintf
#define scprintf _scprintf

typedef SOCKET gs_socket_t;
#define GS_INVALID_SOCKET ((HANDLE)INVALID_SOCKET)



#define SECURITY_WIN32

#include <Security.h>

typedef uuid_t rx_uuid_t;
typedef ULONG ip_addr_ctx_t;

typedef ULONG ip_addr_ctx_t;

typedef HANDLE sys_handle_t;

typedef HANDLE rx_kernel_dispather_t;


typedef struct find_file_struct_t
{
	HANDLE handle;
	int idx;
} find_file_struct;

typedef struct find_file_struct_t* find_file_handle_t;


typedef DWORD rx_pid_t;

#define memzero(S, N) ZeroMemory(S,N) 

#ifdef _MSC_VER
// windows slim lock size
#define SLIM_LOCK_SIZE sizeof(CRITICAL_SECTION)
#define RW_SLIM_LOCK_SIZE sizeof(SRWLOCK)


#define ACCEPT_ADDR_SIZE (sizeof (SOCKADDR_IN) + 0x10)

#define ACCEPT_BUFFER_SIZE (ACCEPT_ADDR_SIZE*2)

#define TCP_BUFFER_SIZE 0x8000

typedef struct windows_overlapped_t
{
	OVERLAPPED m_read;
	OVERLAPPED m_write;
	OVERLAPPED m_accept;
	OVERLAPPED m_connect;
	LPFN_ACCEPTEX m_acceptex;
	LPFN_CONNECTEX m_connectex;
	SOCKET helper_socket;
} windows_overlapped;


#define INTERNAL_IO_EVENT_SIZE sizeof(struct windows_overlapped_t)
#endif


#include <IPHlpApi.h>
typedef ULONG ip_addr_ctx_t;



#define GET_IP4_ADDR(a) (a.sin_addr.S_un.S_addr)


typedef int socklen_t;
typedef char socket_data_t;

typedef DWORD rx_thread_data_t;

//Python Support
#ifdef PYTHON_SUPPORT
#include "C:\Python\include/Python.h"
#endif

#include "C:\Python\include/Python.h"




#include "os_itf/rx_ositf.h"



#endif
