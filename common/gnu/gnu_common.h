

/****************************************************************************
*
*  common\gnu\gnu_common.h
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


#ifndef gnu_common_h
#define gnu_common_h 1





#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <uuid/uuid.h>
#include <stdio.h>
#include <limits.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <sys/mman.h>
#include <pthread.h>
#include <openssl/ssl.h>

#define RX_COMPILER_NAME "GCC"
#define RX_COMPILER_VERSION (__GNUC__)
#define RX_COMPILER_MINOR (__GNUC_MINOR__)
#define RX_COMPILER_BUILD (__GNUC_PATCHLEVEL__)

#define _rx_func_ __PRETTY_FUNCTION__

typedef int rx_os_error_t;
typedef int sys_handle_t;

#define _snprintf_s(a,b,c,...) snprintf(a,c,__VA_ARGS__)

#define GET_IP4_ADDR(a) ((a).sin_addr.s_addr)
#define SET_IP4_ADDR(a, d) ((a).sin_addr.s_addr = (d))



#ifdef RXCOMMON_EXPORTS
#define RX_COMMON_API __attribute__((visibility("default")))
#else
#define RX_COMMON_API
#endif



#ifdef RXPLUGIN_EXPORTS
#define RX_PLUGIN_API __attribute__((visibility("default")))
#else
#define RX_PLUGIN_API
#endif



#ifdef RXPLATFORM_EXPORTS
#define RX_PLATFORM_API __attribute__((visibility("default")))
#else
#define RX_PLATFORM_API
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


#define SLIM_LOCK_SIZE (sizeof(pthread_mutex_t) + 8)
#define RW_SLIM_LOCK_SIZE (sizeof(pthread_mutex_t) + 8)



typedef void* rx_module_handle_t;
typedef void* rx_func_addr_t;


#include <openssl/hmac.h>

typedef struct _crypt_key_t
{
	uint8_t* data;
	uint32_t size;
} *crypt_key_t;

typedef HMAC_CTX* crypt_hash_t;



typedef void* rx_certificate_t;







#endif
