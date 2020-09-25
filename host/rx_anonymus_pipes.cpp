

/****************************************************************************
*
*  host\rx_anonymus_pipes.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_anonymus_pipes
#include "host/rx_anonymus_pipes.h"

#include "system/runtime/rx_operational.h"
#include "system/server/rx_server.h"
#include "lib/rx_func_to_go.h"
#include "lib/rx_job.h"


namespace host {

namespace pipe {

// Class host::pipe::local_pipe_port 

local_pipe_port::local_pipe_port (const pipe_client_t& pipes)
      : pipe_handles_(pipes),
        active_(false)
{
}



rx_result local_pipe_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.tags->set_item_static("Pipe.InPipe"s, (uint64_t)(pipe_handles_.client_read), ctx);
	ctx.tags->set_item_static("Pipe.OutPipe"s, (uint64_t)(pipe_handles_.client_write) , ctx);
	return true;
}

void local_pipe_port::receive_loop ()
{
	while (!active_)
		rx_ms_sleep(50);

	pipes_.receive_loop([this] (size_t count)
		{
		});
}

rx_result local_pipe_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
	if (active_)
	{
		RX_ASSERT(false);
		return "Already started.";
	}
	auto result = pipes_.open(pipe_handles_, [this](size_t count)
		{
		});
	if (!result)
		return result;

	bind_stack_endpoint(&pipes_.stack_entry_, nullptr, nullptr);	
	active_ = true;

	return true;
}

void local_pipe_port::stack_disassembled ()
{
	if(active_)
		pipes_.close();
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

anonymus_pipe_endpoint::anonymus_pipe_endpoint()
      : binded(false)
	, pipe_sender_("Pipe Writer", RX_DOMAIN_EXTERN)
{

	rx_init_stack_entry(&stack_entry_, this);

	stack_entry_.send_function = &anonymus_pipe_endpoint::send_function;
	stack_entry_.stack_changed_function= &anonymus_pipe_endpoint::stack_changed_function;

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
	anonymus_pipe_endpoint* self = reinterpret_cast<anonymus_pipe_endpoint*>(reference->user_data);
	using job_type = rx::jobs::function_job<rx_reference_ptr, rx_packet_buffer>;
	auto packet_id = packet.id;
	rx::function_to_go<rx_reference_ptr, rx_packet_buffer> send_func(rx_reference_ptr(), [self, packet_id](rx_packet_buffer buffer)
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
		});

	send_func.set_arguments(std::move(*packet.buffer));
	auto job = rx_create_reference<job_type>(std::move(send_func));

	self->pipe_sender_.append(job);

	return RX_PROTOCOL_OK;
}

void anonymus_pipe_endpoint::close ()
{
	pipe_sender_.end();
	pipes_->close_pipe();
	pipes_.release();
}

void anonymus_pipe_endpoint::stack_changed_function (rx_protocol_stack_endpoint* reference)
{
	rx_notify_connected(reference, nullptr);
}


} // namespace pipe
} // namespace host

