

/****************************************************************************
*
*  protocols\http\rx_http_path_mapping.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


// rx_http_path_mapping
#include "protocols/http/rx_http_path_mapping.h"

#include "http_server/rx_http_server.h"


namespace protocols {

namespace rx_http {
namespace
{

rx_protocol_result_t parse_data(string_view_type data, string_type& path, size_t& used, bool use_host_name)
{
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	bool had_end = false;
	string_type res_path;
	string_type host_name;
	size_t data_size = data.size();
	size_t content_size = 0;
	size_t start_idx = 0;
	size_t header_end_idx = 0;
	size_t end_idx = data.find("\r\n");
	while (end_idx != string_view_type::npos && end_idx < data_size)
	{
		if (res_path.empty())
		{
			auto idx1 = data.find_first_of(" \t", start_idx);
			if (idx1 == string_view_type::npos)
				break;
			auto idx2 = data.find(" HTTP", idx1);
			if (idx2 == string_view_type::npos)
				break;
			if (idx2 - idx1 == 0)
				res_path = "/";
			else
				res_path = string_type(data.substr(idx1, idx2 - idx1));
		}
		else if (data.substr(start_idx, 15) == "Content-Length:")
		{
			auto idx1 = data.find_first_not_of(" \t", start_idx + 15);
			if (idx1 == string_view_type::npos)
				break;
			auto idx2 = data.find_first_of(" \t\r\n", idx1);
			if (idx2 == string_view_type::npos)
				break;
			if (idx2 - idx1 > 0)
			{
				size_t temp_size;
				auto res = std::from_chars(&data[idx1], &data[idx2], temp_size);
				if (res.ec == std::errc())
					content_size = temp_size;
			}
		}
		else if (data.substr(start_idx, 5) == "Host:")
		{
			auto idx1 = data.find_first_not_of(" \t", start_idx + 5);
			if (idx1 == string_view_type::npos)
				break;
			auto idx2 = data.find_first_of(" \t\r\n", idx1);
			if (idx2 == string_view_type::npos)
				break;
			if (idx2 - idx1 > 0)
			{
				host_name = string_type(data.substr(idx1, idx2 - idx1));
			}
		}
		else
		{
			auto idx = data.find("\r\n", start_idx);
			if (idx == start_idx)
			{
				header_end_idx = idx + 2;
				break;
			}
		}
		start_idx = end_idx + 2;
		end_idx = data.find("\r\n", start_idx);
	}
	if (header_end_idx > 0)
	{
		size_t total_size = header_end_idx + content_size;
		if (total_size == data.size())
		{
			had_end = true;
			used = 0;
		}
		else if (total_size > data.size())
		{
			had_end = false;
			used = 0;
		}
		else// if(total_size < data.size())
		{
			had_end = true;
			used = total_size;
		}
	}
	if (result == RX_PROTOCOL_OK && had_end && !res_path.empty())
	{
		if (use_host_name)
		{
			path = host_name;
			if (res_path[0] != '/')
				path += "/";
		}
		path += res_path;
	}
	return result;
}
}

// Class protocols::rx_http::rx_http_addressing_port 

rx_http_addressing_port::rx_http_addressing_port()
      : max_packet_size_(0x10000),
        use_host_name_(false)
{
	construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
		{
			auto rt = std::make_unique<rx_http_path_endpoint>(smart_this(), use_host_name_);
			rx_protocol_stack_endpoint* stack = &rt->stack_entry;
			return construct_func_type::result_type{ stack, std::move(rt) };
		};
}


rx_http_addressing_port::~rx_http_addressing_port()
{
}



rx_result rx_http_addressing_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = status.initialize(ctx);
	if (!result)
		return result;
	result = max_packet_size_.bind("Options.MaxPacket", ctx);
	if (!result)
		return result;

	use_host_name_ = ctx.get_item_static("Options.UseHostName", false);

	return true;
}


// Class protocols::rx_http::rx_http_path_endpoint 

rx_http_path_endpoint::rx_http_path_endpoint (rx_reference<rx_http_addressing_port> port, bool host_name)
      : my_port_(port),
        use_host_name_(host_name)
	, parser_(true)
{
	HTTP_LOG_DEBUG("rx_http_path_endpoint", 200, "HTTP Addressing endpoint created.");

	rx_init_stack_entry(&stack_entry, this);
	stack_entry.received_function = &rx_http_path_endpoint::received_function;
	//my_addr_ = rx_uuid::create_new().to_string();
	parser_.callback = [this](http_parsed_request req)
		{
			return parsed_request(std::move(req));
		};
}


rx_http_path_endpoint::~rx_http_path_endpoint()
{
	HTTP_LOG_DEBUG("rx_http_path_endpoint", 200, "HTTP Addressing endpoint destroyed.");
}



rx_protocol_result_t rx_http_path_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_http_path_endpoint* self = reinterpret_cast<rx_http_path_endpoint*>(reference->user_data);
	self->my_port_->status.received_packet();
	if (self->upgrade_address_.empty())
	{
		return self->parser_.received(packet);
	}
	else
	{
		io::string_address addr_to(self->upgrade_address_);
		io::string_address addr_from(self->my_addr_);

		auto forward_packet = rx_create_recv_packet(99, packet.buffer, 0, 0);
		forward_packet.to_addr = &addr_to;
		forward_packet.from_addr = &addr_from;
		return rx_move_packet_up(&self->stack_entry, forward_packet);
	}
}

rx_reference<rx_http_addressing_port> rx_http_path_endpoint::get_port ()
{
	return my_port_;
}

rx_result rx_http_path_endpoint::send_response (http_response response)
{
	rx_reference<rx_http_addressing_port> temp_port;
	{
		std::scoped_lock _(port_lock_);
		temp_port = my_port_;
	}
	if (!temp_port)
		return "Already closed!";

	std::ostringstream ss;
	ss << "HTTP/1.1 " << response.result_string << "\r\n";
	for (auto& one : response.headers)
	{

		ss << one.first
			<< ": "
			<< one.second
			<< "\r\n";
	}
	ss << "\r\n";


	string_type pack_text(ss.str());

	auto buffer = temp_port->alloc_io_buffer();
	if (!buffer)
	{
		buffer.register_error(RX_OUT_OF_MEMORY);
		return buffer.errors();
	}

	auto result = buffer.value().write_chars(pack_text);
	if (!response.content.empty())
		result = buffer.value().write(&response.content[0], response.content.size());
	if (!result)
	{
		result.register_error("Error writing to buffer");
		return result;
	}
	send_protocol_packet ret_packet = rx_create_send_packet(0, &buffer.value(), 0, 0);
	auto proto_result = rx_move_packet_down(&stack_entry, ret_packet);
	temp_port->release_io_buffer(buffer.move_value());
	if (proto_result != RX_PROTOCOL_OK)
	{
		return "Error sending packet:"s + rx_protocol_error_message(proto_result);
	}
	else
	{
		RX_ASSERT(temp_port);// checked earlier but...
		if (temp_port)
			temp_port->status.sent_packet();
	}
	return true;
}

rx_protocol_result_t rx_http_path_endpoint::parsed_request (http_parsed_request req)
{
	std::vector<string_view_type> paths;
	if (req.path.empty())
	{
		paths.push_back("/");
	}
	else
	{
		size_t start_idx = 0;
		size_t idx1 = 0;
		size_t idx2 = req.path.find('/');
		if (idx2 == 0)
		{
			idx1 = 1;
			start_idx = 1;
		}
		idx2 = req.path.find('/', idx1);
		while (idx2 != string_view_type::npos)
		{
			if(idx2 > idx1)
				paths.emplace_back(&req.path[start_idx], idx2 - 1);
			idx1 = idx2 + 1;
			idx2 = req.path.find('/', idx1);
		}
		if(idx1 < req.path.size() - 1)
			paths.emplace_back(&req.path[start_idx], req.path.size() - idx1 -1);
	}
	for(auto& one : paths)
	{
		string_type temp(one);
		io::string_address addr_to(temp);
		io::string_address addr_from(my_addr_);

		rx_const_packet_buffer buffer = parser_.create_const_buffer();
		auto forward_packet = rx_create_recv_packet(99, &buffer, 0, 0);
		forward_packet.to_addr = &addr_to;
		forward_packet.from_addr = &addr_from;
		auto ret = rx_move_packet_up(&stack_entry, forward_packet);
		if (req.method == rx_http_method::get)
		{
			string_view_type conn = req.headers["Connection"];
			string_view_type upgrade = req.headers["Upgrade"];
			string_view_type web_key = req.headers["Sec-WebSocket-Key"];
			if (!conn.empty() && !upgrade.empty() && !web_key.empty())
			{
				if (conn.find("Upgrade") != string_view_type::npos && upgrade.compare("websocket") == 0)
				{
					upgrade_address_ = temp;
				}
			}
		}
		return ret;


	}
	return RX_PROTOCOL_OK;
}


// Class protocols::rx_http::rx_http_host_name_port 

rx_http_host_name_port::rx_http_host_name_port()
      : max_packet_size_(0x100),
        use_host_name_(false)
{
	construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
		{
			auto rt = std::make_unique<rx_http_host_name_endpoint>(smart_this(), use_host_name_);
			rx_protocol_stack_endpoint* stack = &rt->stack_entry;
			return construct_func_type::result_type{ stack, std::move(rt) };
		};
}



rx_result rx_http_host_name_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = status.initialize(ctx);
	if (!result)
		return result;
	result = max_packet_size_.bind("Options.MaxPacket", ctx);
	if (!result)
		return result;

	use_host_name_ = ctx.get_item_static("Options.UseHostName", false);

	return true;
}

void rx_http_host_name_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
}


// Class protocols::rx_http::rx_http_host_name_endpoint 

rx_http_host_name_endpoint::rx_http_host_name_endpoint (rx_reference<rx_http_host_name_port> port, bool host_name)
      : my_port_(port),
        use_host_name_(host_name),
        connected_(false)
{
	HTTP_LOG_DEBUG("rx_http_host_name_endpoint", 200, "HTTP Host Name endpoint created.");	

	rx_init_stack_entry(&stack_entry, this);
	stack_entry.received_function = &rx_http_host_name_endpoint::received_function;

}


rx_http_host_name_endpoint::~rx_http_host_name_endpoint()
{
	HTTP_LOG_DEBUG("rx_http_host_name_endpoint", 200, "HTTP Host Name endpoint destroyed.");
}



rx_protocol_result_t rx_http_host_name_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_http_host_name_endpoint* self = reinterpret_cast<rx_http_host_name_endpoint*>(reference->user_data);
	if (self->connected_)
	{
		// just forward
		return rx_move_packet_up(reference, packet);
	}
	self->my_port_->status.received_packet();
	self->my_port_->alloc_io_buffer();
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	size_t data_size = 0;
	if (packet.buffer)
		data_size = rx_get_packet_available_data(packet.buffer);
	if (data_size)
	{
		const char* buff = (const char*)rx_get_from_packet(packet.buffer, data_size, &result);
		if (self->my_buffer_.empty())
		{
			if (result == RX_PROTOCOL_OK)
			{
				RX_ASSERT(buff);
				string_view_type data(buff, data_size);
				RX_ASSERT(!data.empty());
				string_type path;
				size_t used = 0;
				result = parse_data(data, path, used, self->use_host_name_);
				if (result == RX_PROTOCOL_OK)
				{
					if (path.empty())
					{
						// didn't found it yet
						auto ret = self->my_buffer_.write(buff, data_size);
						if (!ret)
							result = RX_PROTOCOL_OUT_OF_MEMORY;

					}
					else
					{
						io::string_address addr(path);
						io::any_address remote;
						if (packet.from_addr)
							remote = io::any_address(packet.from_addr);
						auto result = self->my_port_->stack_endpoint_connected(reference, &addr, remote);
						if (!result)
						{
							std::ostringstream ss;
							ss << "Error binding connected endpoint ";
							ss << result.errors_line();
							HTTP_LOG_ERROR("rx_http_host_name_endpoint", 200, ss.str().c_str());
							return RX_PROTOCOL_INVALID_ADDR;
						}
						packet.to_addr = &addr;
						result = rx_move_packet_up(reference, packet);

						self->connected_ = true;
						return RX_PROTOCOL_OK;
					}
				}

			}
		}
		else
		{
			auto ret = self->my_buffer_.write(buff, data_size);
			if (ret)
			{
				data_size = self->my_buffer_.size;
				string_view_type data((const char*)self->my_buffer_.buffer_ptr, data_size);
				RX_ASSERT(!data.empty());
				string_type path;
				size_t used = 0;
				result = parse_data(data, path, used, self->use_host_name_);
				if (result == RX_PROTOCOL_OK)
				{
					if (!path.empty())
					{
						io::string_address addr(path);
						io::any_address remote;
						if (packet.from_addr)
							remote = io::any_address(packet.from_addr);
						auto result = self->my_port_->stack_endpoint_connected(reference, &addr, remote);
						if (!result)
						{
							std::ostringstream ss;
							ss << "Error binding connected endpoint ";
							ss << result.errors_line();
							HTTP_LOG_ERROR("rx_http_host_name_endpoint", 200, ss.str().c_str());
							return RX_PROTOCOL_INVALID_ADDR;
						}
						packet.to_addr = &addr;
						result = rx_move_packet_up(reference, packet);
						self->connected_ = true;
						
						return result;
					}
				}
			}
			else
			{
				result = RX_PROTOCOL_OUT_OF_MEMORY;
			}
		}
	}
	return result;
}

rx_reference<rx_http_host_name_port> rx_http_host_name_endpoint::get_port ()
{
	return my_port_;
}

void rx_http_host_name_endpoint::close_endpoint ()
{
}


} // namespace rx_http
} // namespace protocols

