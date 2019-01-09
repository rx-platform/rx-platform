

/****************************************************************************
*
*  os_itf\linux\rx_linux.h
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


#ifndef rx_linux_h
#define rx_linux_h 1


#define GNU_SOURCE





#include "rx_gnu_version.h"


#define RX_COMPILER_NAME "GCC"
#define RX_COMPILER_VERSION (__GNUC__)
#define RX_COMPILER_MINOR (__GNUC_MINOR__)
#define RX_COMPILER_BUILD (__GNUC_PATCHLEVEL__)

#define _rx_func_ __PRETTY_FUNCTION__

#ifdef __cplusplus
#include <string>
typedef std::string string_type;
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <uuid/uuid.h>
#include <netdb.h>
#include <stdarg.h>
#include <dirent.h>
#include <signal.h>
#include <termios.h>

#ifdef _DEBUG
#define RX_ASSERT(v) assert(v)
#else
#define RX_ASSERT(v) ((void)(v))
#endif

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

typedef unsigned int ip_addr_ctx_t;

#define SLIM_LOCK_SIZE sizeof(pthread_mutex_t)
#define RW_SLIM_LOCK_SIZE sizeof(pthread_mutex_t)

#define EPOLL_READ_TYPE 1
#define EPOLL_WRITE_TYPE 2
#define EPOLL_ACCEPT_TYPE 3
#define EPOLL_CONNECT_TYPE 4

#define SOCKET_ERROR (-1)


typedef struct linux_epoll_subscriber_t
{
    int pending_operations;
    int epoll_handle;
    int write_handle;
	char read_type;
	char write_type;
	const void* write_buffer;
	size_t left_to_write;
} linux_epoll_subscriber;

#define INTERNAL_IO_EVENT_SIZE sizeof(struct linux_epoll_subscriber_t)

#define MAX_PATH 1024

#define TCP_BUFFER_SIZE 0x8000

#define ACCEPT_BUFFER_SIZE 0x100

#define closesocket(a) ::close(a)

typedef int sys_handle_t;
typedef struct linux_find_files_t
{
	DIR* pdir;
	char* filter;
} linux_find_files;
typedef struct linux_find_files_t* find_file_handle_t;

typedef struct linux_epoll_struct_t
{
	int epoll_fd;
	int pipe[2];

} linux_epool_struct;


typedef struct linux_epoll_struct_t* rx_kernel_dispather_t;


typedef pid_t rx_pid_t;


#define memzero(S, N) memset((S), 0, (N))


#define GET_IP4_ADDR(a) (a.sin_addr.s_addr)


typedef int rx_socket_t;
typedef void socket_data_t;
#define GS_INVALID_SOCKET (-1)


#define _snprintf_s(a,b,c,...) snprintf(a,c,__VA_ARGS__)

#define ntohll be64toh

#define htonll htobe64

typedef pthread_key_t rx_thread_data_t;

#include <openssl/hmac.h>

typedef struct _crypt_key_t
{
    uint8_t* data;
    uint32_t size;
} *crypt_key_t;

typedef HMAC_CTX* crypt_hash_t;

#ifdef PYTHON_SUPPORT
#include <python2.7/Python.h>
#endif




#include "os_itf/rx_ositf.h"


#endif
