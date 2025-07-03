

/****************************************************************************
*
*  protocols\http\rx_http_mapping.cpp
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


// rx_http_mapping
#include "protocols/http/rx_http_mapping.h"

#include "security/rx_security.h"


namespace protocols {

namespace rx_http {

// Class protocols::rx_http::rx_http_endpoint 

rx_http_endpoint::rx_http_endpoint (rx_reference<rx_http_port> port)
      : executer_(-1),
        port_(port)
{
	HTTP_LOG_DEBUG("rx_http_endpoint", 200, "HTTP communication server endpoint created.");
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &rx_http_endpoint::received_function;
	executer_ = port->get_executer();
}


rx_http_endpoint::~rx_http_endpoint()
{
	
	HTTP_LOG_DEBUG("rx_http_endpoint", 200, "HTTP communication server endpoint destroyed.");
}



rx_protocol_result_t rx_http_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	rx_http_endpoint* self = reinterpret_cast<rx_http_endpoint*>(reference->user_data);
	self->port_->status.received_packet();

	return self->parser_.received(packet);
}

rx_protocol_result_t rx_http_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	return RX_PROTOCOL_OK;
}

rx_protocol_stack_endpoint* rx_http_endpoint::bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	parser_.callback = [this](http_parsed_request req)
	{
			http_request request;
			request.method = req.method;
			request.path = std::move(req.path);
			request.headers = std::move(req.headers);
			request.content = std::move(req.content);
			request.acceptable_types = req.acceptable_types;
			request.extension = std::move(req.extension);
			request.params = std::move(req.params);
			request.whose = smart_this();
			request.identity = security::active_security();
			std::scoped_lock _(port_lock_);
			if (port_)
			{
				port_->send_function([](http_request&& request)
					{
						auto result = rx_internal::rx_http_server::http_server::instance().handle_request(std::move(request));

					}, std::move(request));
			}
			return RX_PROTOCOL_OK;
	};
	return &stack_entry_;
}

void rx_http_endpoint::close_endpoint ()
{
	std::scoped_lock _(port_lock_);
	port_=rx_reference<rx_http_port>::null_ptr;
}

rx_result rx_http_endpoint::send_response (http_response response)
{
	rx_reference<rx_http_port> temp_port;
	{
		std::scoped_lock _(port_lock_);
		temp_port = port_;
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
	auto proto_result = rx_move_packet_down(&stack_entry_, ret_packet);
	temp_port->release_io_buffer(buffer.move_value());
	if (proto_result != RX_PROTOCOL_OK)
	{
		return "Error sending packet:"s + rx_protocol_error_message(proto_result);
	}
	else
	{
		if(port_)
			port_->status.sent_packet();
	}
	return true;
}

void rx_http_endpoint::set_identity (security::security_context_ptr ctx)
{
}


// Class protocols::rx_http::rx_http_port 

rx_http_port::rx_http_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<rx_http_endpoint>(smart_this());
		auto entry = rt->bind_endpoint([this](int64_t count)
			{
			},
			[this](int64_t count)
			{
			});
		return construct_func_type::result_type{ entry, rt };
	};
}



void rx_http_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}

rx_result rx_http_port::initialize_runtime (runtime::runtime_init_context& ctx)
{

	auto result = status.initialize(ctx);
	if (!result)
		return result;


	return result;
}


} // namespace rx_http
} // namespace protocols

