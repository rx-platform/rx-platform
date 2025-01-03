

/****************************************************************************
*
*  os_itf\windows\rx_win.h
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


#ifndef rx_win_h
#define rx_win_h 1


////## begin module%580FE720011D.includes preserve=yes
//
//#define _CRT_SECURE_NO_WARNINGS
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define _CRT_RAND_S
//#define NOMINMAX
//
//#define _CRTDBG_MAP_ALLOC
//
//#endif //RX_USE_OWN_HEAP
//#ifdef _DEBUG
//#define DEBUG_CLIENTBLOCK new ( _CLIENT_BLOCK , __FILE__ , __LINE__ )
////// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
////// allocations to be of _CLIENT_BLOCK type
//#else
//#define DEBUG_CLIENTBLOCK new
//#endif
//
//
//#include <WinSock2.h>
//#include <ws2tcpip.h>
//#include <ws2ipdef.h>
//#include <windows.h>
//
//#include <stdlib.h>
//#include <crtdbg.h>

#include "common/rx_common.h"





#include "rx_win32_version.h"


#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING__snprintfS
#define _WINSOCK_DEPRECATED_NO_WARNINGS



#include <winsock2.h>
#include <Mswsock.h>
#include <windows.h>
#include <crtdefs.h>
#include <wincrypt.h>
#include <IPHlpApi.h>
#include <io.h>



//#define snprintf _snprintf
#define scprintf _scprintf

typedef SOCKET gs_socket_t;
#define GS_INVALID_SOCKET ((HANDLE)INVALID_SOCKET)



typedef ULONG ip_addr_ctx_t;

typedef ULONG ip_addr_ctx_t;

typedef HANDLE rx_kernel_dispather_t;


typedef struct find_file_struct_t
{
	HANDLE handle;
	int idx;
} find_file_struct;

typedef struct find_file_struct_t* find_file_handle_t;


typedef DWORD rx_pid_t;


#ifdef _MSC_VER


#define SOCKET_ADDR_SIZE (sizeof (struct sockaddr_storage))

#define ACCEPT_BUFFER_SIZE (SOCKET_ADDR_SIZE*2)

#define TCP_BUFFER_SIZE 0x8000
#define UDP_BUFFER_SIZE 0x800
#define SERIAL_BUFFER_SIZE 0x800

typedef struct windows_overlapped_t
{
	OVERLAPPED m_read;
	OVERLAPPED m_read_from;
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



typedef int socklen_t;
typedef char socket_data_t;

typedef DWORD rx_thread_data_t;

//Python Support
#ifdef PYTHON_SUPPORT
#include "C:\Python\include/Python.h"
#endif

#include <inttypes.h>





#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif



#define SECURITY_WIN32

#include <Security.h>
#include <schannel.h>

typedef struct rx_cred
{
	CredHandle handle;
	uint32_t buffer_size;

	char name_buffer[0x200];
} rx_cred_t;

typedef struct rx_auth_context
{
	int has_handle;
	int has_context;
	CtxtHandle handle;

	char name_buffer[0x200];

} rx_auth_context_t;

struct rx_time_struct_t;



#ifdef __cplusplus
extern "C" {
#endif


#include "common/rx_common.h"

int rx_aquire_cert_credentials(rx_cred_t* cred, struct rx_time_struct_t* life_time, rx_certificate_t* cert, int client);
int rx_aquire_ntlm_credentials(rx_cred_t* cred, struct rx_time_struct_t* life_time
	, const char* user, const char* domain, const char* pass);


#ifdef __cplusplus
}
#endif


#include "os_itf/rx_ositf.h"



#endif
