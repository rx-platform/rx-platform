

/****************************************************************************
*
*  os_itf\linux\rx_linux.h
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


#ifndef rx_linux_h
#define rx_linux_h 1


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#if UINTPTR_MAX == 0xffffffff
#ifndef _TIME_BITS
#define _TIME_BITS 64
#define _FILE_OFFSET_BITS 64
#endif
#endif





#include "rx_gnu_version.h"



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
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <linux/if_packet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>


typedef unsigned int ip_addr_ctx_t;

#define EPOLL_READ_TYPE 1
#define EPOLL_WRITE_TYPE 2
#define EPOLL_ACCEPT_TYPE 3
#define EPOLL_CONNECT_TYPE 4
#define EPOLL_READ_FROM_TYPE 5
#define EPOLL_WRITE_TO_TYPE 6

#define SOCKET_ERROR (-1)

#define SOCKET_ADDR_SIZE (sizeof (struct sockaddr_storage))

typedef struct linux_epoll_subscriber_t
{
    int pending_operations;
    int epoll_handle;
    int write_handle;
	char read_type;
	char write_type;
	const void* write_buffer;
	size_t left_to_write;
	uint8_t addr_buffer[SOCKET_ADDR_SIZE];
	size_t addr_size;
	uint8_t read_addr_buffer[SOCKET_ADDR_SIZE];
	size_t read_addr_size;
} linux_epoll_subscriber;

#define INTERNAL_IO_EVENT_SIZE sizeof(struct linux_epoll_subscriber_t)

#define MAX_PATH 1024

#define TCP_BUFFER_SIZE 0x8000
#define UDP_BUFFER_SIZE 0x800
#define SERIAL_BUFFER_SIZE 0x800

#define ACCEPT_BUFFER_SIZE 0x100


#define closesocket(a) ::close(a)

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



typedef int rx_socket_t;
typedef void socket_data_t;
#define GS_INVALID_SOCKET (-1)


#define ntohll be64toh

#define htonll htobe64

typedef pthread_key_t rx_thread_data_t;

#ifdef PYTHON_SUPPORT
#include <python2.7/Python.h>
#endif


#include <openssl/ssl.h>

typedef struct rx_auth_context
{
	int fd;

	SSL* ssl;

	BIO* rbio; /* SSL reads from, we write to. */
	BIO* wbio; /* SSL writes to, we read from. */

	int init_finished;

} rx_auth_context_t;


typedef struct rx_cred
{
    SSL_CTX* ssl_ctx;
	uint32_t buffer_size;
	int32_t client;
} rx_cred_t;



struct rx_time_struct_t;



#ifdef __cplusplus
extern "C" {
#endif

#include "common/rx_common.h"

int rx_aquire_cert_credentials(rx_cred_t* cred, struct rx_time_struct_t* life_time, rx_certificate_t* cert, int client);



#ifdef __cplusplus
}
#endif




#include "os_itf/rx_ositf.h"


#endif
