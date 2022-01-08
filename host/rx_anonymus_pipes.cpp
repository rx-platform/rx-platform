

/****************************************************************************
*
*  host\rx_anonymus_pipes.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_anonymus_pipes
#include "host/rx_anonymus_pipes.h"
// rx_pipe
#include "host/rx_pipe.h"

#include "system/runtime/rx_operational.h"
#include "system/server/rx_server.h"
#include "lib/rx_func_to_go.h"
#include "lib/rx_job.h"


namespace host {

namespace pipe {

// Class host::pipe::local_pipe_port 

local_pipe_port::local_pipe_port (const pipe_client_t& pipes, rx_pipe_host* host)
      : pipe_handles_(pipes),
        active_(false)
	, pipes_(host, this)
{
}



rx_result local_pipe_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.tags->set_item_static("Pipe.InPipe"s, (uint64_t)(pipe_handles_.client_read), ctx);
	ctx.tags->set_item_static("Pipe.OutPipe"s, (uint64_t)(pipe_handles_.client_write) , ctx);
	return true;
}

rx_result local_pipe_port::receive_loop (rx_pipe_host* host)
{
	auto ticks = rx_get_tick_count();
	rx_timer_ticks_t wait_period = 2000;
#ifdef _DEBUG
	wait_period = 2000000;
#endif
	while (!active_)
	{
		rx_ms_sleep(50);
		if ((rx_get_tick_count() - ticks) > wait_period)
		{
			break;
		}
	}
	if (!active_)// if we break the loop?
	{
		host->pipe_run_result("Waiting for pipe connection timeouted.");
		return true;
	}

	host->pipe_run_result(true);


	add_stack_endpoint(&pipes_.stack_entry_, nullptr, nullptr);
	pipes_.receive_loop([this] (int64_t count)
		{
		});

	return true;
}

rx_result local_pipe_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
	if (active_)
	{
		RX_ASSERT(false);
		return "Already started.";
	}
	auto result = pipes_.open(pipe_handles_, [this](int64_t count)
		{
		});
	if (!result)
		return result;

	active_ = true;

	return true;
}

void local_pipe_port::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
}

rx_result local_pipe_port::stop_passive ()
{
	return true;
}


// Class host::pipe::anonymus_pipe_client 

anonymus_pipe_client::anonymus_pipe_client (const pipe_client_t& pipes)
      : handles_(pipes)
		,buffer_(RX_PIPE_BUFFER_SIZE)
{
}


anonymus_pipe_client::~anonymus_pipe_client()
{
}



rx_result anonymus_pipe_client::write_pipe (const rx_packet_buffer* buffer)
{
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

rx_result anonymus_pipe_client::read_pipe (rx_const_packet_buffer* buffer)
{
	size_t size = buffer_.get_size();
	auto ret = rx_read_pipe_client(&handles_, buffer_.buffer(), &size);
	if (ret == RX_OK)
	{
		rx_init_const_packet_buffer(buffer, buffer_.buffer(), size);
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error reading pipe");
	}
}

void anonymus_pipe_client::close_pipe ()
{
	rx_destry_client_side_pipe(&handles_);
}


// Class host::pipe::anonymus_pipe_endpoint 

anonymus_pipe_endpoint::anonymus_pipe_endpoint (rx_pipe_host* host, local_pipe_port* my_port)
      : host_(host),
        my_port_(my_port),
        binded(false)
	, pipe_sender_("Pipe Writer", RX_DOMAIN_EXTERN)
{
	rx_init_stack_entry(&stack_entry_, this);

	stack_entry_.send_function = &anonymus_pipe_endpoint::send_function;
	stack_entry_.close_function = [](rx_protocol_stack_endpoint* reference, rx_protocol_result_t reason)->rx_protocol_result_t
		{
			anonymus_pipe_endpoint* whose = (anonymus_pipe_endpoint*)reference->user_data;
			
			whose->close();

			rx_notify_closed(reference, RX_PROTOCOL_OK);

			return RX_PROTOCOL_OK;
		};
	stack_entry_.closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
		{
			anonymus_pipe_endpoint* whose = (anonymus_pipe_endpoint*)entry->user_data;
			whose->my_port_->unbind_stack_endpoint(entry);
		};
}


anonymus_pipe_endpoint::~anonymus_pipe_endpoint()
{
}



void anonymus_pipe_endpoint::receive_loop (std::function<void(int64_t)> received_func)
{
	rx_result result;

	while (true)
	{
		rx_const_packet_buffer buffer{};
		result = pipes_->read_pipe(&buffer);
		if (!result)
		{
			HOST_LOG_ERROR("rx_pipe_host", 900, "Error reading pipe, exiting!");
			break;
		}
		received_func(buffer.size);

		auto res = rx_move_packet_up(&stack_entry_, rx_create_recv_packet(0, &buffer, 0, 0));
		if (res != RX_PROTOCOL_OK)
		{
			std::ostringstream ss;
			ss << "Error code " << (int)res << "(" << rx_protocol_error_message(res) << ") returned by stack!\r\n";
			std::cout << ss.str();

			HOST_LOG_ERROR("rx_pipe_host", 900, "Error reading pipe, exiting!");
			break;
		}
	}
	auto session = rx_create_session(nullptr, nullptr, 0, 0, nullptr);
	rx_notify_disconnected(&stack_entry_, &session, RX_PROTOCOL_OK);
	pipe_sender_.end();
}

rx_result anonymus_pipe_endpoint::open (const pipe_client_t& pipes, std::function<void(int64_t)> sent_func)
{

	pipes_ = std::make_unique<anonymus_pipe_client>(pipes);
	sent_func_ = sent_func;
	pipe_sender_.start();

	return true;
}

rx_protocol_result_t anonymus_pipe_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	if (packet.buffer && packet.buffer->buffer_ptr && packet.buffer->size > 0)
	{
		anonymus_pipe_endpoint* self = reinterpret_cast<anonymus_pipe_endpoint*>(reference->user_data);
		using job_type = rx::jobs::function_job<rx_packet_buffer>;
		auto packet_id = packet.id;

		rx_packet_buffer copy_buff;
		rx_init_packet_buffer(&copy_buff, packet.buffer->size, 0);
		rx_push_to_packet(&copy_buff, packet.buffer->buffer_ptr, packet.buffer->size);

		auto job = rx_create_func_job(rx_reference_ptr(), [self, packet_id](rx_packet_buffer buffer)
			{
				auto result = self->pipes_->write_pipe(&buffer);
				if (result)
				{
					self->sent_func_(buffer.size);
					rx_notify_ack(&self->stack_entry_, packet_id, RX_PROTOCOL_OK);
				}
				else
				{
					for (const auto& one : result.errors())
						std::cout << one << "\r\n";
				}
				rx_deinit_packet_buffer(&buffer);
			}, std::move(copy_buff));


		self->pipe_sender_.append(job);
	}
	return RX_PROTOCOL_OK;
}

void anonymus_pipe_endpoint::close ()
{
	if (pipes_)
	{
		pipes_->close_pipe();
		pipes_.reset();
	}
}


} // namespace pipe
} // namespace host

