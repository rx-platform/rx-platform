

/****************************************************************************
*
*  protocols\http\rx_http_mapping.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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



namespace protocols {

namespace rx_http {

// Class protocols::rx_http::rx_http_endpoint 

rx_http_endpoint::rx_http_endpoint (runtime::items::port_runtime* port)
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

	const char* method, * path;
	int pret, minor_version;
	struct phr_header headers[100];
	size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
	rx_protocol_result_t result;

	prevbuflen = self->receive_buffer_.size;
	result = rx_push_to_packet(&self->receive_buffer_, packet.buffer->buffer_ptr, packet.buffer->size);
	if (result == RX_PROTOCOL_OK)
	{
		buflen = self->receive_buffer_.size;
		num_headers = sizeof(headers) / sizeof(headers[0]);
		pret = phr_parse_request((const char*)(self->receive_buffer_.buffer_ptr), buflen, &method, &method_len, &path, &path_len,
			&minor_version, headers, &num_headers, prevbuflen);
		if (pret > 0)
		{
			result = self->create_and_forward_request(method, method_len, path, path_len, headers, num_headers);
			
			rx_reinit_packet_buffer(&self->receive_buffer_);
		}
		else if (pret == -1)
			result = RX_PROTOCOL_PARSING_ERROR;
		else
			result = RX_PROTOCOL_COLLECT_ERROR;
		/* request is incomplete, continue the loop */
		//RX_ASSERT(pret == -2);
	//	if (buflen == sizeof(buf))
	//		return RX_PROTOCOL_BUFFER_SIZE_ERROR;
	}
	/*if (result == RX_PROTOCOL_OK)
	{
		char* msg = "Jeee stiglo mi je";

		std::ostringstream ss;
		ss << "HTTP/1.1 200 OK\r\n";
		ss << "Connection: Keep-Alive\r\n";
		ss << "Server: perica.com\r\n";
		ss << "Content-Type: text\r\n";
		ss << "Content-Length: " << strlen(msg) << "\r\n";
		ss << "Keep-Alive: timeout=8\r\n";
		ss << "Server : {rx-platform}\r\n";
		ss << "\r\n";
		ss << msg;

		string_type pack_text(ss.str());

		rx_packet_buffer buffer;
		rx_init_packet_buffer(&buffer, pack_text.size(), nullptr);
		rx_push_to_packet(&buffer, pack_text.c_str(), pack_text.size());
		send_protocol_packet ret_packet = rx_create_send_packet(0, &buffer, 0, 0);
		auto result = rx_move_packet_down(&self->stack_entry_, ret_packet);
	}*/
	return result;
}

rx_protocol_result_t rx_http_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	return RX_PROTOCOL_OK;
}

rx_protocol_stack_endpoint* rx_http_endpoint::bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	return &stack_entry_;
}

void rx_http_endpoint::close_endpoint ()
{
	port_ = nullptr;
}

rx_protocol_result_t rx_http_endpoint::create_and_forward_request (const char* method, size_t method_len, const char* path, size_t path_len, phr_header* headers, size_t num_headers)
{
	http_request request;
	if (method_len == 3)
	{
		if (0 == memcmp(method, "GET", 3))
		{
			request.method = rx_http_method::get;
		}
		else if (0 == memcmp(method, "PUT", 3))
		{
			request.method = rx_http_method::put;
		}
	}
	else if (method_len == 4 && 0 == memcmp(method, "POST", 4))
	{
		request.method = rx_http_method::post;
	}
	else if (method_len == 6 && 0 == memcmp(method, "DELETE", 4))
	{
		request.method = rx_http_method::del;
	}
	string_type str_path;
	str_path.reserve(path_len);
	for (size_t i = 0; i < path_len; i++)
	{
		if (path[i] == '?')
			break;
		str_path += path[i];
	}
	request.path = str_path;
	request.whose = smart_this();
	port_->send_function([](http_request&& request)
		{
			auto result = rx_internal::rx_http_server::http_server::instance().handle_request(request);

		}, std::move(request));
	return RX_PROTOCOL_OK;
}

rx_result rx_http_endpoint::send_response (http_response response)
{
	if (port_ == nullptr)
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

	auto buffer = port_->alloc_io_buffer();
	if (!buffer)
	{
		buffer.register_error("Out of memory");
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
	port_->release_io_buffer(buffer.move_value());
	if (proto_result != RX_PROTOCOL_OK)
	{
		return "Error sending packet:"s + rx_protocol_error_message(proto_result);
	}
	return true;
}


// Class protocols::rx_http::rx_http_port 

rx_http_port::rx_http_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<rx_http_endpoint>(this);
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


} // namespace rx_http
} // namespace protocols

