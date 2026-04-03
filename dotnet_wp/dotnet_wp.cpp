

/****************************************************************************
*
*  dotnet_wp\dotnet_wp.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


// dotnet_wp
#include "dotnet_wp/dotnet_wp.h"
// dotnet_wp_host
#include "dotnet_wp/dotnet_wp_host.h"

#include "third-party/cxxopts/include/cxxopts.hpp"
#include "dotnet_wp/donet_wp_version.h"
#include "dotnet_wp/dotnet_wp_api.h"
#include "dotnet_wp/dotnet_wp_protocol.h"


#define STACK_LOG_SIZE 0x400



namespace rx_internal {

namespace dotnet {
char g_version[0x60] = { 0 };

template<typename T>
void dotnet_wp::send_message(T message, uint32_t trans_id)
{

	std::function<void(T)> send_message = [this, trans_id](T message)
		{
			auto buffer = pipe_client_->get_buffer();
			dotnet_wp_message_header* phead = buffer.alloc_from_buffer<dotnet_wp_message_header>();
			phead->known = RX_DOTNET_KNOWN;
			phead->type = T::msg_id;
			phead->trans = trans_id;

			auto write_result = message.serialize(buffer);
			if (!write_result)
			{
				printf("Error serializing message: %s\r\n", write_result.errors_line().c_str());
				pipe_client_->release_buffer(std::move(buffer));
				return;
			}
			// now write the full size of the message to the header
			phead->size = (uint32_t)buffer.size;
			auto result = pipe_client_->write_pipe(&buffer);
			if (!result)
			{
				printf("Error sending message from WP host: %s\r\n", result.errors_line().c_str());
			}
			pipe_client_->release_buffer(std::move(buffer));
		};
	sender_.append(rx_platform::jobs::rx_create_job<decltype(send_message), T>()
		(rx_reference_ptr(), std::move(send_message), std::move(message)));


}


// Class rx_internal::dotnet::dotnet_wp 

dotnet_wp::dotnet_wp()
	: sender_("dotnet-wp-sender", 0)
{
	static char version[0x60] = { 0 };
	if (!g_version[0])
	{
		ASSIGN_MODULE_VERSION(g_version, RX_DOTNET_WP_NAME, RX_DOTNET_WP_MAJOR_VERSION, RX_DOTNET_WP_MINOR_VERSION, RX_DOTNET_WP_BUILD_NUMBER);
	}
}


dotnet_wp::~dotnet_wp()
{
}



dotnet_wp& dotnet_wp::instance ()
{
	static dotnet_wp instance;
	return instance;
}

int dotnet_wp::run (int argc, char* argv[])
{

	printf("Starting %s...\r\n", RX_DOTNET_WP_NAME);


	printf("Parsing command line arguments...\r\n");
	rx_dotnet_config_data_t config;
	uint64_t read_handle = c_invalid_handle;
	uint64_t write_handle = c_invalid_handle;
	if (!parse_command_line(argc, argv, "rx-dotnet-wp", config, read_handle, write_handle))
	{
		printf("Exiting...\r\n");
		return 0;
	}
	printf("Initializing rx-common library...\r\n");
	// initializing library
	rx_platform_init_data common_data;
	common_data.is_debug = config.launch_debugger;
	common_data.rx_initial_heap_size = 1*1024*1024;//1 MB
	common_data.rx_alloc_heap_size = 256*1024;
	common_data.rx_heap_alloc_trigger = 800;
	common_data.rx_bucket_capacity = 0x1000;


	rx_thread_data_t tls = rx_alloc_thread_data();


	rx_initialize_os(0, 0, tls, config.launch_debugger, common_data.rx_initial_heap_size
		, common_data.rx_alloc_heap_size, common_data.rx_heap_alloc_trigger, common_data.rx_bucket_capacity);

	printf("Opening pipes...\r\n");

	pipe_client_t pipes = { (sys_handle_t)read_handle, (sys_handle_t)write_handle };
	

	pipe_client_ = std::make_unique<pipe_client>(pipes);


	int sync_ret = rx_sync_client_pipe(&pipes);
	

	
	sender_.run();

	printf("Initializing dotnet hosting...\r\n");

	packed_decoder_type packet_decoder;
	io::rx_io_buffer receive_buffer(0x10000, sizeof(dotnet_wp_message_header));
	dotnet_wp_message_header_t header_buffer;

	auto proto_result = rx_init_packet_decoder(&packet_decoder, &receive_buffer, pipe_header_function, sizeof(dotnet_wp_message_header_t), &header_buffer);

	if(proto_result !=RX_PROTOCOL_OK)
	{
		printf("Error initializing packet decoder: %s\r\n", rx_protocol_error_message(proto_result));
		return 1;
	}

	bool initialized = false;

	worker_initialized_message init_message;

	auto init_result = initialize(config);

	initialized = init_result;

	init_message.action_result = std::move(init_result);
	init_message.hosted_version = get_hosted_dotnet_version();
	init_message.dotnet_host_version = dotnet_host_version_;

	send_message(std::move(init_message), 0);

	if (!initialized)
	{
		sender_.end(2000);

		printf("Error initializing dotnet host\r\n");
		return 1;
	}
	while (true)
	{


		bool error = false;
		rx_const_packet_buffer buffer{};
		rx_result result = pipe_client_->read_pipe(&buffer);
		if (!result)
		{
			printf("Error reading from pipe in WP host: %s\r\n", result.errors_line().c_str());
			break;
		}
		rx_const_packet_buffer recv_buffer{};
		rx_init_const_packet_buffer(&recv_buffer, NULL, 0);
		while (!rx_buffer_eof(&buffer))
		{
			struct rx_const_packet_buffer_type recv_buffer {};


			proto_result = rx_decode_arrived(&packet_decoder, &buffer, &recv_buffer);
			if (proto_result != RX_PROTOCOL_OK)
			{
				printf("Error decoding arrived packet: %s\r\n", rx_protocol_error_message(result));
				error = true;
				break;
			}
			if (!rx_buffer_eof(&recv_buffer))
			{// we received something
				auto result = received_packet(recv_buffer);
				if(!result)
				{
					printf("Error processing received packet: %s\r\n", result.errors_line().c_str());
					error = true;
					break;
				}
				rx_init_const_packet_buffer(&recv_buffer, NULL, 0);
			}
		}
		if (error)
		{
			break;
		}

	}
	sender_.end();
	pipe_client_->close_pipe();

	return 0;
}

bool dotnet_wp::parse_command_line (int argc, char* argv[], const char* help_name, rx_dotnet_config_data_t& config, uint64_t& read_handle, uint64_t& write_handle)
{

	
	cxxopts::Options options(help_name, "");

	bool angular = false;

	options.add_options()
		("i,input", "Handle of the input pipe for this child process", cxxopts::value<uint64_t>(read_handle))
		("o,output", "Handle of the output pipe for this child process", cxxopts::value<uint64_t>(write_handle))
		("v,version", "Displays worker process version")
		("dotnet-version", ".NET Core hosting version", cxxopts::value<string_type>(config.requested_version))
		("dotnet-threads-min", "thread pool min threads", cxxopts::value<uint32_t>(config.min_threads))
		("dotnet-threads-max", "thread pool max threads", cxxopts::value<uint32_t>(config.max_threads))
		("dotnet-no-server-gc", "use client garbage collection", cxxopts::value<bool>(config.server_gc))
		("dotnet-non-concurrent-gc", "use non-concurrent garbage collection", cxxopts::value<bool>(config.concurrent_gc))
		("dotnet-debug-break", "launch debugger on startup of the dotnet runtime code", cxxopts::value<bool>(config.launch_debugger))
		("dotnet-app", "dotnet application path", cxxopts::value<string_type>(config.app_path))
		("dotnet-host", "dotnet hosting library path", cxxopts::value<string_type>(config.host_path))
		("dotnet-lib", "dotnet library path", cxxopts::value<string_type>(config.lib_path))
		("h,help", "Print help")
		;

	if (argv == nullptr || argc == 0)
	{
		printf("Invalid command line arguments\r\n\r\n");
		return false;
	}

	auto result = options.parse(argc, argv);
	if(result.count("version"))
	{
		printf("\r\n%s\r\n\r\n", g_version);
		return false;
	}
	if (result.count("help"))
	{
		printf("\r\nrx-dotnet-wp is a worker process used to host .NET runtime for {rx-platform}.\r\n");
		printf("It can be used only as a child process.\r\n\r\n");
		printf("%s\r\n", options.help().c_str());
		return false;
	}

	if (read_handle == c_invalid_handle || write_handle == c_invalid_handle)
	{
		printf("Invalid command line arguments, missing read/write handles\r\n\r\n");
		return false;
	}
	if(config.requested_version.empty())
	{
		printf("Invalid command line arguments, missing dotnet hosting version\r\n\r\n");
		return false;
	}
	if(config.app_path.empty())
	{
		printf("Invalid command line arguments, missing dotnet application path\r\n\r\n");
		return false;
	}
	if(config.host_path.empty())
	{
		printf("Invalid command line arguments, missing dotnet hosting library path\r\n\r\n");
		return false;
	}
	return true;
}

void dotnet_wp::append_trans (std::shared_ptr<dotnet_host_transaction> trans)
{
	auto id = trans->get_transaction_id();
	RX_ASSERT(id != 0);
	if (id)
	{
		std::scoped_lock lock(transactions_lock_);
		pending_wp_transactions_.emplace(id, trans);
		auto result = trans->start(*this);
		if(!result)
		{
			pending_wp_transactions_.erase(id);
			printf("Error starting transaction %u: %s\r\n", id, result.errors_line().c_str());
		}
	}
}

void dotnet_wp::append_send (wp_send_job_ptr job)
{
	sender_.append(std::move(job));
}

rx_result dotnet_wp::received_packet (rx_const_packet_buffer& received)
{

	io::rx_const_io_buffer io_buffer(&received);
	auto head = io_buffer.get_from_buffer<dotnet_wp_message_header_t>();
	if (!head)
	{
		return rx_result("Received packet is too small to contain header");
	}
	uint32_t type = head->type;
	switch (type)
	{
		case RX_DOTNET_INITIALIZE:
			// just a constant at the beginning our answer is preparing latter
			{
			}
			break;
		case build_types_message::msg_id:
			{
				build_types_message msg;
				auto result = msg.deserialize(io_buffer);
				if(!host_ || !result)
				{
					build_types_response result_msg;
					result_msg.action_result = std::move(result);
					send_message(std::move(result_msg), head->trans);
				}
				else
				{
					std::thread build_thread([this, trans_id = head->trans, lib = std::move(msg.library)]() mutable
						{
							build_types_response result_msg;
							result_msg.action_result = host_->do_build();
							send_message(std::move(result_msg), trans_id);
						});
					build_thread.detach();
				}
			}
			break;
		case reload_message::msg_id:
			{
				reload_message msg;
				auto result = msg.deserialize(io_buffer);
				if (!host_ || !result)
				{
					wp_result_response result_msg;
					result_msg.action_result = std::move(result);
					send_message(std::move(result_msg), head->trans);
				}
				else
				{
					std::thread build_thread([this, trans_id = head->trans, lib = std::move(msg.library)]() mutable
						{
							wp_result_response result_msg;
							result_msg.action_result = host_->do_reload(trans_id, lib);
							send_message(std::move(result_msg), trans_id);
						});
					build_thread.detach();
				}
			}
			break;
		case dotnet_stop_message::msg_id:
			{
				dotnet_stop_message msg;
				auto result = msg.deserialize(io_buffer);
				if (!host_ || !result)
				{
					wp_void_response result_msg;
					send_message(std::move(result_msg), head->trans);
				}
				else
				{
					rxwp_void_callback callback = [this, trans_id = head->trans]()
						{
							wp_void_response result_msg;
							send_message(std::move(result_msg), trans_id);

						};
					std::thread build_thread([this, trans_id = head->trans, callback = std::move(callback)]() mutable
						{
							host_->deinitialize(trans_id, std::move(callback));
						});
					build_thread.detach();

				}
			}
			break;
		case unload_message::msg_id:
			{
				unload_message msg;
				auto result = msg.deserialize(io_buffer);
				if (!host_ || !result)
				{
					wp_result_response result_msg;
					result_msg.action_result = std::move(result);
					send_message(std::move(result_msg), head->trans);
				}
				else
				{
					rxwp_result_callback callback = [this, trans_id = head->trans](rx_result&& result)
						{
							wp_result_response result_msg;
							result_msg.action_result = std::move(result);
							send_message(std::move(result_msg), trans_id);

						};
					std::thread build_thread([this, trans_id = head->trans, lib = std::move(msg.library), callback = std::move(callback)]() mutable
						{
							host_->do_unload(trans_id, lib, std::move(callback));
						});
					build_thread.detach();

				}
			}
			break;
		case dotnet_start_message::msg_id:
			{
				auto result = host_->start();
				RX_ASSERT(result);
				if (!result)
				{
					DOTNET_LOG_ERROR("dotnet_wp_hos::start", 500, "Error starting host:" + result.errors_line());
				}
			}
			break;
		case dotnet_bind_runtime_message::msg_id:
			{
				dotnet_bind_runtime_message msg;
				auto result = msg.deserialize(io_buffer);
				if (host_ && result)
				{
					switch(msg.type)
					{
						case rx_item_type::rx_object:
							host_->bind_runtime(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_struct_type:
							host_->bind_struct(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_source_type:
							host_->bind_source(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_mapper_type:
							host_->bind_mapper(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_event_type:
							host_->bind_event(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
					}
				}
			}
			break;
		case dotnet_unbind_runtime_message::msg_id:
			{
				dotnet_unbind_runtime_message msg;
				auto result = msg.deserialize(io_buffer);
				if (host_ && result)
				{
					switch (msg.type)
					{
						case rx_item_type::rx_object:
							host_->unbind_runtime(msg.id, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_struct_type:
							host_->unbind_struct(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_source_type:
							host_->unbind_source(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_mapper_type:
							host_->unbind_mapper(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
						case rx_item_type::rx_event_type:
							host_->unbind_event(msg.id, msg.path, msg.type_id, (void*)msg.ptr);
							break;
					}
				}
			}
			break;
		case dotnet_value_changed_message::msg_id:
			{
				dotnet_value_changed_message msg;
				auto result = msg.deserialize(io_buffer);
				if (host_ && result)
				{
					host_->runtime_value_changed(msg.idx, msg.val, msg.type, (void*)msg.whose);
				}
			}
			break;
		case dotnet_initial_values_message::msg_id:
			{
				dotnet_initial_values_message msg;
				auto result = msg.deserialize(io_buffer);
				if (host_ && result)
				{
					host_->send_initial_values(msg.names, msg.values, msg.type, (void*)msg.whose);
				}
			}
			break;
		case dotnet_execute_method_message::msg_id:
			{
				dotnet_execute_method_message msg;
				auto result = msg.deserialize(io_buffer);
				if (host_ && result)
				{
					result = host_->execute_method(head->trans, (void*)msg.inst, msg.method, msg.value);
				}
				if (!result)
				{
					dotnet_execute_method_response response_msg;
					response_msg.action_result = std::move(result);
					send_message(std::move(response_msg), head->trans);
				}
			}
			break;
		default:
			{
				if (head->trans && RX_IS_DOTNET_RESPONSE(head->type))
				{
					//RX_ASSERT(false);
					auto it = pending_wp_transactions_.find(head->trans);
					if (it != pending_wp_transactions_.end())
					{

						it->second->response(head->type, io_buffer);
						pending_wp_transactions_.erase(it);
						return true;
					}
					return rx_create_string("Unknown .NET process transaction id response: ", head->trans);
				}
				return rx_create_string("Unknown message type received from .NET process: ", head->type);
			}
	}
	return true;
}

rx_result dotnet_wp::initialize (rx_dotnet_config_data_t& config)
{

	auto result = load_hostfxr(config);
	if (!result)
	{
		DOTNET_LOG_ERROR("dotnet_hosting::initialize", 500, "Unable to load .NET runtime environment");
		return result;
	}
	auto runtimes = get_dotnet_runtimes();
	for (const auto& ver : runtimes)
	{
		DOTNET_LOG_TRACE("dotnet_hosting::initialize", 500, "Detected .NET runtime version: " + ver);
	}
	string_type hosted_ver = get_hosted_dotnet_version();
	if (hosted_ver.empty())
	{
		DOTNET_LOG_ERROR("dotnet_hosting::initialize", 500, "Unable to find requested .NET version " + config.requested_version);
		return rx_result("Unable to find requested .NET version.");
	}
	else
	{
		DOTNET_LOG_INFO("dotnet_hosting::initialize", 500, "Initializing Native host for version: " + hosted_ver);

		printf("Initializing Native host for path: %s\r\n", config.host_path.c_str());

		string_array host_files;
		string_array assembly_files;
		string_array lib_files;
		string_array files;
		string_array dirs;
		rx_list_files(config.host_path, "*.dll", files, dirs);
		for (const auto& one : files)
		{
			host_files.emplace_back(rx_combine_paths(config.host_path, one));
		}
		files.clear();
		dirs.clear();


		rx_list_files(config.app_path, "*.dll", files, dirs);
		for (const auto& one : files)
		{
			assembly_files.emplace_back(rx_combine_paths(config.app_path, one));
		}
		if (!config.lib_path.empty())
		{
			files.clear();
			dirs.clear();
			rx_list_files(config.lib_path, "*.dll", files, dirs);
			for (const auto& one : files)
			{
				lib_files.emplace_back(rx_combine_paths(config.lib_path, one));
			}
		}
		if (host_files.empty())
		{
			DOTNET_LOG_WARNING("dotnet_hosting::initialize", 500, "No .NET host assemblies found in " + config.host_path);
			return "No .NET host assemblies found in " + config.host_path;
		}
		if (assembly_files.empty())
		{
			DOTNET_LOG_WARNING("dotnet_hosting::initialize", 500, "No .NET user assemblies found in " + config.app_path);
			return "No .NET user assemblies found in " + config.app_path;
		}
		host_ = std::make_unique<dotnet_wp_host>(config);
		result = host_->initialize(host_files, assembly_files, lib_files, dotnet_host_version_);

		if (!result)
		{
			DOTNET_LOG_ERROR("dotnet_hosting::initialize", 500, "Error initializing .NET hosting. " + result.errors_line());
			host_.reset();
			return result;
		}


		DOTNET_LOG_INFO("dotnet_hosting::initialize", 500, ".NET hosting initialized.");
	}

	return true;
}

void dotnet_wp::log_event_fast (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, const string_type& message)
{
	write_log_message log_message;
	log_message.type = (int32_t)event_type;
	log_message.library = library;
	log_message.source = source;
	log_message.level = level;
	log_message.code = code;
	log_message.message = message;

	send_message(std::move(log_message), 0);

}

void dotnet_wp::log_event (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, const char* message, ... )
{
	char buff[STACK_LOG_SIZE];
	va_list args;
	va_start(args, message);

	int ret = vsnprintf(buff, STACK_LOG_SIZE, message, args);
	if (ret < STACK_LOG_SIZE)
		log_event_fast(event_type, library, source, level, code, buff);
	else
	{
		RX_ASSERT(false);
	}
	va_end(args);
}

void dotnet_wp::send_result_response (uint32_t trans_id, rx_result result)
{
	rx_internal::dotnet::wp_result_response msg;
	msg.action_result = std::move(result);
	rx_internal::dotnet::dotnet_wp::instance().send_message(std::move(msg), trans_id);
}

void dotnet_wp::send_fire_event (void* inst, string_type value, string_type queue, bool state, bool remove)
{

	rx_internal::dotnet::dotnet_fire_event_message msg;

	msg.inst = (intptr_t)inst;
	msg.value = std::move(value);
	msg.queue = std::move(queue);
	msg.state = state != 0;
	msg.remove = remove != 0;


	send_message(std::move(msg), 0);
}


// Class rx_internal::dotnet::pipe_client 

pipe_client::pipe_client (const pipe_client_t& pipes)
      : handles_(pipes),
        free_border_(0x1000)
	, buffer_(RX_DOTNET_PIPE_BUFFER_SIZE)
{
}


pipe_client::~pipe_client()
{
}



rx_result pipe_client::write_pipe (const rx_packet_buffer* buffer)
{
	if(buffer == NULL)
	{
		rx_write_pipe_client(&handles_, NULL, 0);
		// this is just a signal to the parent process that we are ready, no data to write
		return true;
	}
	/*std::ostringstream ss;
	char buff[0x4];
	ss << "\r\nWriting to pipe:";
	for (size_t i = 0; i < buffer->size && i < 0x100; i++)
	{
		if (i % 16 == 0)
			ss << "\r\n";
		sprintf(buff, "%02X ", ((uint8_t*)buffer->buffer_ptr)[i]);
		ss << buff;
	}
	ss << "\r\n";
	printf("%s", ss.str().c_str());*/
	auto ret = rx_write_pipe_client(&handles_, buffer->buffer_ptr, buffer->size);
	if (ret == RX_OK)
	{
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error writing pipe");
	}
}

rx_result pipe_client::read_pipe (rx_const_packet_buffer* buffer)
{
	if(buffer == NULL)
	{
		// this is just a signal from the parent process that we can start, no data to read
		rx_read_pipe_client(&handles_, NULL, NULL);
		return true;
	}
	size_t size = buffer_.get_size();
	auto ret = rx_read_pipe_client(&handles_, buffer_.buffer(), &size);
	if (ret == RX_OK)
	{
		/*std::ostringstream ss;
		char buff[0x4];
		ss << "\r\nRead from pipe:";
		for (size_t i = 0; i < size && i < 0x100; i++)
		{
			if (i % 16 == 0)
				ss << "\r\n";
			sprintf(buff, "%02X ", ((uint8_t*)buffer_.buffer())[i]);
			ss << buff;
		}
		ss << "\r\n";
		printf("%s", ss.str().c_str());*/
		rx_init_const_packet_buffer(buffer, buffer_.buffer(), size);
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error reading pipe");
	}
}

void pipe_client::close_pipe ()
{
	rx_destry_client_side_pipe(&handles_);
}

io::rx_io_buffer pipe_client::get_buffer ()
{
	if (!free_buffers_.empty())
	{
		io::rx_io_buffer ret = std::move(free_buffers_.top());
		free_buffers_.pop();
		return ret;
	}
	else
	{
		io::rx_io_buffer new_buffer(0x10000, 0);
		return new_buffer;
	}
}

void pipe_client::release_buffer (io::rx_io_buffer what)
{
	if (what.size <= free_border_)
	{
		what.reinit();
		free_buffers_.push(std::move(what));
	}
}


// Class rx_internal::dotnet::dotnet_wp_send_job 

dotnet_wp_send_job::dotnet_wp_send_job()
{
}


dotnet_wp_send_job::~dotnet_wp_send_job()
{
}



// Class rx_internal::dotnet::dotnet_host_transaction 

dotnet_host_transaction::dotnet_host_transaction()
{
}


dotnet_host_transaction::~dotnet_host_transaction()
{
}



// Class rx_internal::dotnet::build_type_transaction 

build_type_transaction::build_type_transaction (build_one_type_message arg, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        timeout_(timeout),
        trans_done_(false),
        arg_(arg)
{
}



rx_result build_type_transaction::start (dotnet_wp& wp) const
{
	wp.send_message(std::move(arg_), trans_id_);
	return true;
}

rx_result build_type_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
	if (msg_type != build_one_type_response::msg_id)
	{
		return rx_create_string("Invalid message type for build type response: ", msg_type);
	}
	build_one_type_response msg;
	auto result = msg.deserialize(buffer);
	if (!result)
	{
		result = rx_create_string("Error deserializing build type response: ", result.errors_line().c_str());
	}
	result_ = std::move(msg.action_result);
	trans_done_.set();
	return true;
}

void build_type_transaction::timeouted ()
{
	result_ = rx_result("Build type transaction timed out");
	trans_done_.set();
}

uint32_t build_type_transaction::get_transaction_id () const
{
	return trans_id_;
}

rx_result build_type_transaction::wait_result ()
{
	auto ret = trans_done_.wait_handle(timeout_ + 100);
	if (ret == RX_WAIT_TIMEOUT)
	{
		result_ = rx_result("Build type transaction timed out");
	}
	return std::move(result_);
}


// Class rx_internal::dotnet::build_runtime_transaction 

build_runtime_transaction::build_runtime_transaction (uint64_t dotnet_trans, build_runtime_message arg, dotnetRuntimeCallback_t callback, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        arg_(arg),
        callback_(callback),
        dotnet_trans_id_(dotnet_trans)
{
}



rx_result build_runtime_transaction::start (dotnet_wp& wp) const
{
	wp.send_message(std::move(arg_), trans_id_);

	return true;
}

rx_result build_runtime_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
	if (msg_type != build_runtime_response::msg_id)
	{
		return rx_create_string("Invalid message type for build runtime response: ", msg_type);
	}
	build_runtime_response msg;
	auto result = msg.deserialize(buffer);
	if (!result)
	{
		result = rx_create_string("Error deserializing build runtime response: ", result.errors_line().c_str());
	}
	callback_(dotnet_trans_id_, msg.action_result.move());
	return true;
}

void build_runtime_transaction::timeouted ()
{
	callback_(dotnet_trans_id_, rx_result("Build runtime transaction timed out").move());
}

uint32_t build_runtime_transaction::get_transaction_id () const
{
	return trans_id_;
}


// Class rx_internal::dotnet::wp_write_transaction 

wp_write_transaction::wp_write_transaction (uint64_t dotnet_trans, wp_write_value_message arg, dotnetRuntimeCallback_t callback, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        arg_(arg),
        callback_(callback),
        dotnet_trans_id_(dotnet_trans)
{
}



rx_result wp_write_transaction::start (dotnet_wp& wp) const
{
	wp.send_message(std::move(arg_), trans_id_);

	return true;
}

rx_result wp_write_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
	if (msg_type != wp_result_response::msg_id)
	{
		return rx_create_string("Invalid message type for build runtime response: ", msg_type);
	}
	wp_result_response msg;
	auto result = msg.deserialize(buffer);
	if (!result)
	{
		result = rx_create_string("Error deserializing build runtime response: ", result.errors_line().c_str());
	}
	callback_(dotnet_trans_id_, result.move());
	return true;
}

void wp_write_transaction::timeouted ()
{
	callback_(dotnet_trans_id_, rx_result("Build runtime transaction timed out").move());
}

uint32_t wp_write_transaction::get_transaction_id () const
{
	return trans_id_;
}


// Class rx_internal::dotnet::delete_runtime_transaction 

delete_runtime_transaction::delete_runtime_transaction (uint64_t dotnet_trans, delete_one_runtime_message arg, dotnetRuntimeCallback_t callback, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        timeout_(timeout),
        trans_done_(false),
        arg_(arg),
        callback_(callback),
        dotnet_trans_id_(dotnet_trans)
{
}



rx_result delete_runtime_transaction::start (dotnet_wp& wp) const
{
	wp.send_message(std::move(arg_), trans_id_);
	return true;
}

rx_result delete_runtime_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
	if (msg_type != wp_result_response::msg_id)
	{
		return rx_create_string("Invalid message type for delete runtime response: ", msg_type);
	}
	wp_result_response msg;
	auto result = msg.deserialize(buffer);
	if (!result)
	{
		result = rx_create_string("Error deserializing delete runtime response: ", result.errors_line().c_str());
	}
	callback_(dotnet_trans_id_, msg.action_result.move());
	return true;
}

void delete_runtime_transaction::timeouted ()
{
	callback_(dotnet_trans_id_, rx_result("Build runtime transaction timed out").move());
}

uint32_t delete_runtime_transaction::get_transaction_id () const
{
	return trans_id_;
}


// Class rx_internal::dotnet::delete_wp_type_transaction 

delete_wp_type_transaction::delete_wp_type_transaction (delete_one_type_message arg, uint32_t timeout)
      : trans_id_(rx_get_new_transaction_id()),
        timeout_(timeout),
        trans_done_(false),
        arg_(arg)
{
}



rx_result delete_wp_type_transaction::start (dotnet_wp& wp) const
{
	wp.send_message(std::move(arg_), trans_id_);
	return true;
}

rx_result delete_wp_type_transaction::response (uint32_t msg_type, io::rx_const_io_buffer& buffer)
{
	if (msg_type != wp_result_response::msg_id)
	{
		return rx_create_string("Invalid message type for delete type response: ", msg_type);
	}
	wp_result_response msg;
	auto result = msg.deserialize(buffer);
	if (!result)
	{
		result = rx_create_string("Error deserializing delete type response: ", result.errors_line().c_str());
	}
	result_ = std::move(msg.action_result);
	trans_done_.set();
	return true;
}

void delete_wp_type_transaction::timeouted ()
{
	result_ = rx_result("Build type transaction timed out");
	trans_done_.set();
}

uint32_t delete_wp_type_transaction::get_transaction_id () const
{
	return trans_id_;
}

rx_result delete_wp_type_transaction::wait_result ()
{
	auto ret = trans_done_.wait_handle(timeout_ + 100);
	if (ret == RX_WAIT_TIMEOUT)
	{
		result_ = rx_result("Delete type transaction timed out");
	}
	return std::move(result_);
}


} // namespace dotnet
} // namespace rx_internal

