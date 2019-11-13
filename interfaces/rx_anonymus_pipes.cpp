

/****************************************************************************
*
*  interfaces\rx_anonymus_pipes.cpp
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/


#include "pch.h"


// rx_anonymus_pipes
#include "interfaces/rx_anonymus_pipes.h"



namespace interfaces {

namespace anonymus_pipe {

// Class interfaces::anonymus_pipe::anonymus_pipe_client

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

rx_result anonymus_pipe_client::close_pipe ()
{
	rx_destry_client_side_pipe(&handles_);
	return true;
}


// Class interfaces::anonymus_pipe::anonymus_pipe_endpoint

anonymus_pipe_endpoint::anonymus_pipe_endpoint()
	: pipe_sender_("Pipe Writer", RX_DOMAIN_EXTERN)
{
}



void anonymus_pipe_endpoint::receive_loop ()
{
	rx_result result;
	
	while (true)
	{
		rx_const_packet_buffer buffer{};
		result = pipes_->read_pipe(&buffer);
		if (!result)
		{
			ITF_LOG_ERROR("rx_pipe_host", 900, "Error reading pipe, exiting!");
			break;
		}
		auto res = rx_move_packet_up(this, nullptr, &buffer);
		if (res != RX_PROTOCOL_OK)
		{
			std::cout << "Error code " << (int)res << " returned by stack!\r\n";
			break;
		}
	}
}

rx_result anonymus_pipe_endpoint::open (const pipe_client_t& pipes)
{
	rx_protocol_stack_entry* mine_entry = this;

	pipes_ = std::make_unique<anonymus_pipe_client>(pipes);
	pipe_sender_.start();
	mine_entry->downward = nullptr;
	mine_entry->upward = nullptr;

	mine_entry->send_function = &anonymus_pipe_endpoint::send_function;
	mine_entry->sent_function = nullptr;
	mine_entry->received_function = nullptr;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;


	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;

	return true;
}

rx_protocol_result_t anonymus_pipe_endpoint::send_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer)
{
	anonymus_pipe_endpoint* self = reinterpret_cast<anonymus_pipe_endpoint*>(reference);

	self->pipe_sender_.append(
		rx_create_reference<jobs::lambda_job<rx_packet_buffer> >(
			[self](rx_packet_buffer buffer)
			{
				auto result = self->pipes_->write_pipe(&buffer);
				if (result)
				{
					rx_move_result_up(self, NULL, RX_PROTOCOL_OK);
				}
				else
				{
					for (const auto& one : result.errors())
						std::cout << one << "\r\n";
				}
				rx_deinit_packet_buffer(&buffer);
			},
			*buffer
		)
	);

	return RX_PROTOCOL_OK;
}

rx_result anonymus_pipe_endpoint::close ()
{
	auto result = pipes_->close_pipe();
	if(result)
		pipes_.release();
	return result;
}


} // namespace anonymus_pipe
} // namespace interfaces

