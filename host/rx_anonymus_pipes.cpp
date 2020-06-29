

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
      : pipe_handles_(pipes)
{
}



rx_result local_pipe_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = local_pipe_port_type::initialize_runtime(ctx);
	if (result)
	{
		ctx.tags->set_item_static("Pipe.InPipe"s, (uint64_t)(pipe_handles_.client_read), ctx);
		ctx.tags->set_item_static("Pipe.OutPipe"s, (uint64_t)(pipe_handles_.client_write) , ctx);
	}
	return result;
}

rx_result local_pipe_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = local_pipe_port_type::deinitialize_runtime(ctx);
	if (result)
	{
	}
	return result;
}

rx_result local_pipe_port::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = local_pipe_port_type::start_runtime(ctx);
	if (result)
	{
	}
	return result;
}

rx_result local_pipe_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = local_pipe_port_type::stop_runtime(ctx);
	if (result)
	{
	}
	return result;
}

void local_pipe_port::receive_loop ()
{

	pipes_.receive_loop([this] (size_t count)
		{
			update_received_counters(count);
		});
}

rx_result local_pipe_port::open ()
{
	auto result = pipes_.open(pipe_handles_, [this](size_t count)
		{
			update_sent_counters(count);
		});
	return result;
}

void local_pipe_port::close ()
{
	pipes_.close();
}

rx_protocol_stack_entry* local_pipe_port::get_stack_entry ()
{
	return &pipes_;
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
	: pipe_sender_("Pipe Writer", RX_DOMAIN_EXTERN)
{
	rx_protocol_stack_entry* mine_entry = this;

	rx_init_stack_entry(mine_entry);

	mine_entry->send_function = &anonymus_pipe_endpoint::send_function;

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
		auto res = rx_move_packet_up(this, &buffer, 0);
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

rx_protocol_result_t anonymus_pipe_endpoint::send_function (rx_protocol_stack_entry* reference, rx_packet_buffer* buffer, rx_packet_id_type packet_id)
{
	anonymus_pipe_endpoint* self = reinterpret_cast<anonymus_pipe_endpoint*>(reference);
	using job_type = rx::jobs::function_job<rx_reference_ptr, rx_packet_buffer>;

	rx::function_to_go<rx_reference_ptr, rx_packet_buffer> send_func(rx_reference_ptr(), [self](rx_packet_buffer buffer)
		{
			auto result = self->pipes_->write_pipe(&buffer);
			if (result)
			{
				self->sent_func_(buffer.size);
				rx_move_result_up(self, RX_PROTOCOL_OK);
			}
			else
			{
				for (const auto& one : result.errors())
					std::cout << one << "\r\n";
			}
			rx_deinit_packet_buffer(&buffer);
		});

	send_func.set_arguments(std::move(*buffer));
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


} // namespace pipe
} // namespace host

