

/****************************************************************************
*
*  first_plugin\first_ports.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

#include "first_plugin_version.h"
#include "version/rx_code_macros.h"

// first_ports
#include "first_plugin/first_ports.h"



// Class first_transport_port 

first_transport_port::first_transport_port()
      : timer_(0)
{
}


first_transport_port::~first_transport_port()
{
}



rx_result first_transport_port::initialize_port (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, _rx_func_);
	return true;
}

rx_result first_transport_port::start_port (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, _rx_func_);
	timer_ = create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_transport_port::stop_port ()
{
	RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_transport_port::deinitialize_port ()
{
	RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, _rx_func_);
	return true;
}

void first_transport_port::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, _rx_func_);
	this->post_job([this]()
		{
			RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, "Usao u posted function");
		});
	destroy_timer(timer_);
	timer_ = 0;
}

std::unique_ptr<first_transport_port_endpoint> first_transport_port::construct_endpoint ()
{
	auto ep = std::make_unique< first_transport_port_endpoint>(this);
	return ep;
}


// Class first_transport_port_endpoint 

first_transport_port_endpoint::first_transport_port_endpoint (first_transport_port* port)
      : port_(port)
{
	RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, _rx_func_);
	rx_init_stack_entry(&stack_, this);
}


first_transport_port_endpoint::~first_transport_port_endpoint()
{
	RX_PLUGIN_LOG_DEBUG("first_transport_port", 100, _rx_func_);
}



rx_protocol_stack_endpoint* first_transport_port_endpoint::get_endpoint ()
{
	return &stack_;
}

rx_protocol_result_t first_transport_port_endpoint::received (rx_protocol_stack_endpoint* stack, recv_protocol_packet packet)
{
	auto buffer = port_->alloc_io_buffer();
	if (buffer)
	{
		rx_protocol_result_t result = rx_push_to_packet(&buffer.value(), packet.buffer->buffer_ptr, rx_get_packet_available_data(packet.buffer));
		if (result != RX_PROTOCOL_OK)
		{
			port_->release_io_buffer(buffer.move_value());
			return result;
		}
		else
		{
			char* ptr = (char*)buffer.value().buffer_ptr;
			for (size_t i = 0; i < buffer.value().size; i++)
			{
				if (ptr[i] >= 'a' && ptr[i] < 'z')
					ptr[i] = ptr[i] + 1;
				else if (ptr[i] == 'z')
					ptr[i] = 'a';
			}
			rx_const_packet_buffer send_buff{};
			rx_init_const_from_packet_buffer(&send_buff, &buffer.value());
			auto send_packet = packet;
			send_packet.buffer = &send_buff;
			result = rx_move_packet_up(&stack_, send_packet);

			port_->release_io_buffer(buffer.move_value());

			return result;
		}

	}
	else
	{
		return RX_PROTOCOL_OUT_OF_MEMORY;
	}
}


first_transport_port * first_transport_port_endpoint::get_port ()
{
  return port_;
}


// Class first_server_port 

first_server_port::first_server_port()
      : timer_(0)
{
}


first_server_port::~first_server_port()
{
}



rx_result first_server_port::initialize_port (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_server_port", 100, _rx_func_);
	return true;
}

rx_result first_server_port::start_port (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_server_port", 100, _rx_func_);
	timer_ = create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_server_port::stop_port ()
{
	RX_PLUGIN_LOG_DEBUG("first_server_port", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_server_port::deinitialize_port ()
{
	RX_PLUGIN_LOG_DEBUG("first_server_port", 100, _rx_func_);
	return true;
}

void first_server_port::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_server_port", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}

rx_reference<first_server_endpoint> first_server_port::construct_endpoint ()
{
	return rx_create_reference<first_server_endpoint>(this);
}

std::map<rx_node_id, first_server_port::smart_ptr> first_server_port::runtime_instances;

// Class first_client_port 

first_client_port::first_client_port()
      : timer_(0),
        command(""),
        response("")
{
}


first_client_port::~first_client_port()
{
}



rx_result first_client_port::initialize_port (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_client_port", 100, _rx_func_);
	rx_result result = command.bind("Command", ctx, [this](const string_type& val)
		{
			auto ep = active_endpoint();
			if (ep)
			{
				ep->send_command(val);
			}
		});
	if (!result)
		return result;
	result = response.bind("Resp", ctx);
	if (!result)
		return result;

	return true;
}

rx_result first_client_port::start_port (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_client_port", 100, _rx_func_);
	timer_ = create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_client_port::stop_port ()
{
	RX_PLUGIN_LOG_DEBUG("first_client_port", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_client_port::deinitialize_port ()
{
	RX_PLUGIN_LOG_DEBUG("first_client_port", 100, _rx_func_);
	return true;
}

void first_client_port::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_client_port", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}

rx_reference<first_client_endpoint> first_client_port::construct_endpoint ()
{
	return rx_create_reference<first_client_endpoint>(this);
}


// Class first_server_endpoint 

first_server_endpoint::first_server_endpoint (first_server_port* port)
      : port_(port)
{
	RX_PLUGIN_LOG_DEBUG("first_server_endpoint", 100, _rx_func_);
	rx_init_stack_entry(&stack_, this);
}


first_server_endpoint::~first_server_endpoint()
{
	RX_PLUGIN_LOG_DEBUG("first_server_endpoint", 100, _rx_func_);
}



rx_protocol_stack_endpoint* first_server_endpoint::get_endpoint ()
{
	return &stack_;
}

rx_protocol_result_t first_server_endpoint::received (rx_protocol_stack_endpoint* stack, recv_protocol_packet packet)
{
	string_type line;
	if (packet.buffer)
	{
		while (!rx_buffer_eof(packet.buffer))
		{
			char ch;
			rx_pop_from_packet(packet.buffer, &ch, sizeof(ch));
			if (ch >=32 && ch<128)
			{
				line += ch;
			}
		}
		if (!line.empty())
		{
			string_type response;
			if (line == "koliko je sati?")
				response = "Sati je "s + rx_time::now().get_string(false);
			else
				response = "Ne razumem?!?";

			if(!response.empty())
			{
				response += "\r\n";
				auto buff = port_->alloc_io_buffer();
				if (buff)
				{
					rx_push_to_packet(&buff.value(), response.c_str(), response.size());
					auto packet = rx_create_send_packet(0, &buff.value(), 0, 0);
					auto result = rx_move_packet_down(&stack_, packet);

					port_->release_io_buffer(buff.move_value());

					return result;
				}
			}
		}
	}
	return RX_PROTOCOL_OK;
}


first_server_port * first_server_endpoint::get_port ()
{
  return port_;
}


// Class first_client_endpoint 

first_client_endpoint::first_client_endpoint (first_client_port* port)
      : port_(port)
{
	RX_PLUGIN_LOG_DEBUG("first_client_endpoint", 100, _rx_func_);
	rx_init_stack_entry(&stack_, this);
}


first_client_endpoint::~first_client_endpoint()
{
	RX_PLUGIN_LOG_DEBUG("first_client_endpoint", 100, _rx_func_);
}



rx_protocol_stack_endpoint* first_client_endpoint::get_endpoint ()
{
	return &stack_;
}

void first_client_endpoint::close_endpoint ()
{
}

rx_protocol_result_t first_client_endpoint::received (rx_protocol_stack_endpoint* stack, recv_protocol_packet packet)
{
	string_type line;
	if (packet.buffer)
	{
		while (!rx_buffer_eof(packet.buffer))
		{
			char ch;
			rx_pop_from_packet(packet.buffer, &ch, sizeof(ch));
			if (ch >= 32 && ch < 128)
			{
				line += ch;
			}
		}
		port_->response = line;
	}
	return RX_PROTOCOL_OK;
}

void first_client_endpoint::send_command (const string_type& val)
{
	auto buff = port_->alloc_io_buffer();
	if (buff)
	{
		rx_push_to_packet(&buff.value(), val.c_str(), val.size());
		auto packet = rx_create_send_packet(0, &buff.value(), 0, 0);
		auto result = rx_move_packet_down(&stack_, packet);

		port_->release_io_buffer(buff.move_value());
	}
}


first_client_port * first_client_endpoint::get_port ()
{
  return port_;
}


