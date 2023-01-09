

/****************************************************************************
*
*  common\win32\win32_common.h
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


#ifndef win32_common_h
#define win32_common_h 1





#define _CRT_SECURE_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_RAND_S
#define NOMINMAX

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new ( _CLIENT_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DEBUG_CLIENTBLOCK new
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>

#include <windows.h>


#define RX_COMPILER_NAME "MSVC"
#define RX_COMPILER_VERSION (_MSC_VER/100-5)
#define RX_COMPILER_MINOR (_MSC_VER%100)
#define RX_COMPILER_BUILD (_MSC_FULL_VER%100000)

#define _CRT_RAND_S


#ifndef _rx_func_
#define _rx_func_ __FUNCTION__
#endif


typedef DWORD rx_os_error_t;
typedef HANDLE sys_handle_t;

#define GET_IP4_ADDR(a) ((a).sin_addr.S_un.S_addr)
#define SET_IP4_ADDR(a, d) ((a).sin_addr.S_un.S_addr = (d))



#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif


#ifdef RXCOMMON_EXPORTS
#define RX_COMMON_API __declspec(dllexport) 
#else
#define RX_COMMON_API __declspec(dllimport) 
#endif


#ifdef RXPLATFORM_EXPORTS
#define RX_PLATFORM_API __declspec(dllexport) 
#else
#define RX_PLATFORM_API __declspec(dllimport) 
#endif

#ifdef RXPLUGIN_EXPORTS
#define RX_PLUGIN_API __declspec(dllexport) 
#else
#define RX_PLUGIN_API __declspec(dllimport) 
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


// windows slim lock size
#define SLIM_LOCK_SIZE sizeof(CRITICAL_SECTION)
#define RW_SLIM_LOCK_SIZE sizeof(SRWLOCK)

typedef HINSTANCE rx_module_handle_t;
typedef FARPROC rx_func_addr_t;


typedef HCRYPTKEY crypt_key_t;
typedef HCRYPTHASH  crypt_hash_t;


#define SECURITY_WIN32

#include <Security.h>





#endif
