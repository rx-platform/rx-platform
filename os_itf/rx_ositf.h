

/****************************************************************************
*
*  os_itf\rx_ositf.h
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


#ifndef rx_ositf_h
#define rx_ositf_h 1


// including C style standard integer types
#include <stdint.h>
// errors above all
#include "rx_errors.h"


// include common shared library
#include "common/rx_common.h"


#ifdef __cplusplus
extern "C" {
#endif



	extern const char* g_ositf_version;

	///////////////////////////////////////////////////////////////////
	// anonymous pipes

	typedef struct pipe_server_t
	{
		sys_handle_t server_read;
		sys_handle_t server_write;
		sys_handle_t client_read;
		sys_handle_t client_write;
	} pipe_server;

	typedef struct pipe_client_t
	{
		sys_handle_t client_read;
		sys_handle_t client_write;
	} client_server;

	void rx_initialize_server_side_pipe(struct pipe_server_t* pipes);
	void rx_initialize_client_side_pipe(struct pipe_client_t* pipes);
	int rx_create_server_side_pipe(struct pipe_server_t* pipes, size_t size);
	int rx_create_client_side_pipe(struct pipe_server_t* server_pipes, struct pipe_client_t* pipes);
	int rx_destry_server_side_pipe(struct pipe_server_t* pipes);
	int rx_destry_client_side_pipe(struct pipe_client_t* pipes);

	int rx_write_pipe_server(struct pipe_server_t* pipes, const void* data, size_t size);
	int rx_write_pipe_client(struct pipe_client_t* pipes, const void* data, size_t size);
	int rx_read_pipe_server(struct pipe_server_t* pipes, void* data, size_t* size);
	int rx_read_pipe_client(struct pipe_client_t* pipes, void* data, size_t* size);

	///////////////////////////////////////////////////////////////
	// PHYSICAL INTERFACES
	///////////////////////////////////////////////////////////////
	enum interface_status_type
	{
		interface_status_disconnected = 0,
		interface_status_active
	};
#define MAC_ADDR_SIZE 6
#define ETH_CARD_NAME_SIZE 0x100


	struct IP_interface
	{
		char ip_address[0x40];
		char broadcast_address[0x40];
		char network[0x40];
		char name[ETH_CARD_NAME_SIZE];
		size_t index;
		enum interface_status_type status;
	};


	struct ETH_interface
	{
		uint8_t mac_address[MAC_ADDR_SIZE];
		char name[ETH_CARD_NAME_SIZE];
		char description[ETH_CARD_NAME_SIZE];
		size_t index;
		enum interface_status_type status;
		size_t ip_addrs_size;
		struct IP_interface* ip_addrs;
	};


	//////////////////////////////////////////////////////////////////////////////////////////////
	// list ethernet cards
	int rx_list_eth_cards(struct ETH_interface** interfaces, size_t* count);
	///////////////////////////////////////////////////////////////////////////////////////////////
	// IP addresses
	int rx_add_ip_address(uint32_t addr, uint32_t mask, int itf, ip_addr_ctx_t* ctx);
	int rx_remove_ip_address(ip_addr_ctx_t ctx);
	int rx_is_valid_ip_address(uint32_t addr, uint32_t mask);

	extern int rx_big_endian;
	extern rx_thread_data_t rx_tls;
	extern rx_pid_t rx_pid;
	const char* rx_get_node_name();
	void rx_init_hal_version();
	void rx_initialize_os(int rt, int hdt, rx_thread_data_t tls, int is_debug, size_t initial_heap, size_t heap_alloc, size_t heap_trigger, size_t bucket_capacity);
	void rx_deinitialize_os();


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//system information related functions

	void rx_collect_system_info(char* buffer, size_t buffer_size);
	void rx_collect_processor_info(char* buffer, size_t buffer_size, size_t* count);
	void rx_collect_memory_info(size_t* total, size_t* free, size_t* process);
	/////////////////////////////////////////////////////////////////////////////////////////////////


	//directories stuff
	typedef struct rx_file_directory_entry_t
	{
		int is_directory;
		char file_name[MAX_PATH];
		uint32_t size;
		rx_time_struct time;
	} rx_file_directory_entry;


	// returns NULL if not succeeded
	find_file_handle_t rx_open_find_file_list(const char* path, struct rx_file_directory_entry_t* entry);
	// while not zero files are fetched
	int rx_get_next_file(find_file_handle_t hndl, struct rx_file_directory_entry_t* entry);
	void rx_find_file_close(find_file_handle_t hndl);



	///////////////////////////////////////////////////////////////////////////////////////////////
	// file handling functions
#define RX_FILE_OPEN_READ 1
#define RX_FILE_OPEN_WRITE 2
#define RX_FILE_OPEN_BOTH 3

#define RX_FILE_CREATE_ALWAYS 1
#define RX_FILE_CREATE_NEW 2
#define RX_FILE_OPEN_ALWAYS 3
#define RX_FILE_OPEN_EXISTING 4

	/////////////////////////////////////////////////////////////////////////////////////////////
	// file related stuff
	/////////////////////////////////////////////////////////////////////////////////////////////
	int rx_create_directory(const char* path, int fail_on_exsist);


	sys_handle_t rx_file(const char* path, int access, int creation);
	int rx_file_read(sys_handle_t hndl, void* buffer, uint32_t size, uint32_t* readed);
	int rx_file_write(sys_handle_t hndl, const void* buffer, uint32_t size, uint32_t* written);
	int rx_file_get_size(sys_handle_t hndl, uint64_t* size);
	int rx_file_get_time(sys_handle_t hndl, struct rx_time_struct_t* created,  struct rx_time_struct_t* modified);
	int rx_file_get_time_from_path(const char* path, struct rx_time_struct_t* created,  struct rx_time_struct_t* modified);
	int rx_file_close(sys_handle_t hndl);
	int rx_file_delete(const char* path);
	int rx_file_rename(const char* old_path, const char* new_path);
	int rx_file_exsist(const char* path);
	/////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////
	// completion ports

	typedef void(*rx_callback)(void*);

	typedef int(*rx_io_read_callback)(void*, uint32_t status, size_t);
	typedef int(*rx_io_read_from_callback)(void*, uint32_t status, size_t, struct sockaddr*, size_t);
	typedef int(*rx_io_write_callback)(void*, uint32_t status);
	typedef int(*rx_io_connect_callback)(void*, uint32_t status, struct sockaddr*, struct sockaddr*);
	typedef int(*rx_io_accept_callback)(void*, uint32_t status, sys_handle_t, struct sockaddr*, struct sockaddr*);
	typedef int(*rx_io_shutdown_callback)(void*, uint32_t status);


	typedef struct rx_io_register_data_t
	{
		sys_handle_t handle;
		rx_io_read_callback read_callback;
		rx_io_read_from_callback read_from_callback;
		rx_io_write_callback write_callback;
		rx_io_connect_callback connect_callback;
		rx_io_accept_callback accept_callback;
		rx_io_shutdown_callback shutdown_callback;
		void* data;
		void* read_buffer;
		size_t read_buffer_size;
		uint8_t internal[INTERNAL_IO_EVENT_SIZE];
	} rx_io_register_data;



	rx_kernel_dispather_t rx_create_kernel_dispathcer(int max);
	uint32_t rx_destroy_kernel_dispatcher(rx_kernel_dispather_t disp);
	uint32_t rx_dispatcher_signal_end(rx_kernel_dispather_t disp);

	uint32_t rx_dispatch_events(rx_kernel_dispather_t disp);

	uint32_t rx_dispatch_function(rx_kernel_dispather_t disp, rx_callback f, void* arg);


	uint32_t rx_dispatcher_register(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data);
	int rx_dispatcher_unregister(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data);

	uint32_t rx_io_read(struct rx_io_register_data_t* what, size_t* readed);
	uint32_t rx_io_write(struct rx_io_register_data_t* what, const void* data, size_t count);

	uint32_t rx_socket_read_from(struct rx_io_register_data_t* what, size_t* readed, struct sockaddr_storage* addr);
	uint32_t rx_socket_write_to(struct rx_io_register_data_t* what, const void* data, size_t count, const struct sockaddr* addr, size_t addrsize);

	uint32_t rx_socket_accept(struct rx_io_register_data_t* what, uint32_t keep_alive);
	uint32_t rx_socket_connect(struct rx_io_register_data_t* what, const struct sockaddr* addr, size_t addrsize);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// socket abstractions
	sys_handle_t rx_create_and_bind_ip4_tcp_socket(const struct sockaddr_in* addr, uint32_t keep_alive);
	sys_handle_t rx_create_and_bind_ip4_udp_socket(const struct sockaddr_in* addr, const struct sockaddr_in* multicast);
	uint32_t rx_socket_listen(sys_handle_t handle);
	void rx_close_socket(sys_handle_t handle);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// COM port abstractions
#define RX_ONESTOPBIT 0
#define RX_ONE5STOPBITS 1
#define RX_TWOSTOPBITS 2

#define RX_NOPARITY 0
#define RX_ODDPARITY 1
#define RX_EVENPARITY 2
#define RX_MARKPARITY 3
#define RX_SPACEPARITY 4

	sys_handle_t rx_open_serial_port(const char* port, uint32_t baud_rate, int stop_bits, int parity, uint8_t data_bits, int handshake);
	void rx_close_serial_port(sys_handle_t handle);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	// thread abstractions

#define RX_PRIORITY_IDLE -3
#define RX_PRIORITY_LOW -2
#define RX_PRIORITY_BELOW_NORMAL -1
#define RX_PRIORITY_NORMAL 0
#define RX_PRIORITY_ABOVE_NORMAL 1
#define RX_PRIORITY_HIGH 2
#define RX_PRIORITY_REALTIME 3
	sys_handle_t rx_thread_create(void(*start_address)(void*), void* arg, int priority, uint32_t* thread_id, const char* name);
	int rx_thread_join(sys_handle_t what);
	int rx_thread_close(sys_handle_t what);
	sys_handle_t rx_current_thread();
	int rx_thread_set_afinity(sys_handle_t what, uint64_t mask);
	///////////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////////
	// TLS code
	rx_thread_data_t rx_alloc_thread_data();
	void rx_set_thread_data(rx_thread_data_t key, void* data);
	void* rx_get_thread_data(rx_thread_data_t key);
	void rx_free_thread_data(rx_thread_data_t key);
	///////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Ethernet sockets


	typedef void* peth_socket;

	uint32_t rx_create_ethernet_socket(const char* adapter_name, peth_socket* psock);
	uint32_t rx_send_ethernet_packet(peth_socket psock, const void* buffer, size_t size);
	uint32_t rx_recive_ethernet_packet(peth_socket psock, uint8_t** buffer, size_t* recv_size, int* has_more, struct timeval* tv);
	uint32_t rx_close_ethernet_socket(peth_socket psock);


	int rx_init_auth_context(rx_auth_context_t* ctx);
	int rx_is_auth_context_complete(rx_auth_context_t* ctx);
	int rx_deinit_auth_context(rx_auth_context_t* ctx);
	int rx_accept_credentials(rx_cred_t* cred, rx_auth_context_t* ctx, const void* data, size_t size, void* out_data, size_t* out_size);
	int rx_connect_credentials(rx_cred_t* cred, rx_auth_context_t* ctx, const void* data, size_t size, void* out_data, size_t* out_size);
	int rx_decrypt_message(rx_auth_context_t* ctx, const void* data, size_t size, void* out_data, size_t* out_size);
	int rx_encrypt_message(rx_auth_context_t* ctx, const void* data, size_t size, size_t* processed_count, void* out_data, size_t* out_size);

	int rx_get_peer_certificate(rx_auth_context_t* ctx, rx_certificate_t* cert);

	int rx_release_credentials(rx_cred_t* cred);

#ifdef __cplusplus
}
#endif





#endif
