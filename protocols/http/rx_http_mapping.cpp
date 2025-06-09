

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
string_type rx_url_decode(const char* path, size_t path_len)
{
	string_type path_conv;
	path_conv.reserve(path_len);
	char conv_buffer[0x3];
	conv_buffer[2] = '\0';
	size_t idx = 0;
	while (idx < path_len)
	{
		if (path[idx] != '%')
		{
			path_conv.push_back(path[idx]);
		}
		else
		{
			idx++;
			if (idx < path_len)
			{
				conv_buffer[0] = path[idx];
				idx++;
				if (idx < path_len)
				{
					conv_buffer[1] = path[idx];
					char* end = nullptr;
					char enc = (char)strtol(conv_buffer, &end, 16);
					if (enc)
						path_conv.push_back(enc);
				}
			}
		}
		idx++;
	}
	return path_conv;
}

// Class protocols::rx_http::rx_http_endpoint 

rx_http_endpoint::rx_http_endpoint (rx_reference<rx_http_port> port)
      : executer_(-1),
        port_(port),
        content_left_(0)
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
	const char* method, * path;
	int pret, minor_version;
	size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
	const char* buffer_to_read = nullptr;
	rx_protocol_result_t result;

	if (self->content_left_ > 0)
	{
		if (packet.buffer->size < self->content_left_)
		{
			// we need to read even more data!!!!
			self->prepared_request_.content.insert(self->prepared_request_.content.end(), (const std::byte*)packet.buffer->buffer_ptr, (const std::byte*)packet.buffer->buffer_ptr + packet.buffer->size);
			self->content_left_ -= packet.buffer->size;

			return RX_PROTOCOL_OK;
		}
		else if (packet.buffer->size == self->content_left_)
		{
			// we read everything
			self->prepared_request_.content.insert(self->prepared_request_.content.end(), (const std::byte*)packet.buffer->buffer_ptr, (const std::byte*)packet.buffer->buffer_ptr + self->content_left_);
			self->content_left_ = 0;
			self->send_current_request();

			rx_reinit_packet_buffer(&self->receive_buffer_);

			return RX_PROTOCOL_OK;
		}
		else
		{
			RX_ASSERT(packet.buffer->size > self->content_left_);
			// we read everything and even more
			self->prepared_request_.content.insert(self->prepared_request_.content.end(), (const std::byte*)packet.buffer->buffer_ptr, (const std::byte*)packet.buffer->buffer_ptr + self->content_left_);
			self->content_left_ = 0;
			self->send_current_request();

			rx_reinit_packet_buffer(&self->receive_buffer_);

			// now do the even more stuff
			result = rx_push_to_packet(&self->receive_buffer_
				, packet.buffer->buffer_ptr + self->content_left_
				, packet.buffer->size - self->content_left_);
			if (result != RX_PROTOCOL_OK)
				return result;

			buflen = self->receive_buffer_.size;
			buffer_to_read = (const char*)(self->receive_buffer_.buffer_ptr);

			return RX_PROTOCOL_OK;
		}
	}
	else
	{
		prevbuflen = self->receive_buffer_.size;

		result = rx_push_to_packet(&self->receive_buffer_, packet.buffer->buffer_ptr, packet.buffer->size);
		if (result != RX_PROTOCOL_OK)
			return result;

		buflen = self->receive_buffer_.size;
		buffer_to_read = (const char*)(self->receive_buffer_.buffer_ptr);
	}
	num_headers = sizeof(self->headers_buffer_) / sizeof(self->headers_buffer_[0]);
	pret = phr_parse_request(buffer_to_read, buflen, &method, &method_len, &path, &path_len,
		&minor_version, self->headers_buffer_, &num_headers, prevbuflen);
	if (pret > 0)
	{
		size_t parsed = pret;
		std::byte* content_ptr = (std::byte*)buffer_to_read + parsed;
		size_t content_max = self->receive_buffer_.size - parsed;
		result = self->create_and_forward_request(method, method_len, path, path_len, num_headers, content_ptr, content_max);

		rx_reinit_packet_buffer(&self->receive_buffer_);
	}
	else if (pret == -2)// request is incomplete
		result = RX_PROTOCOL_OK;
	else if (pret == -1)
		result = RX_PROTOCOL_PARSING_ERROR;
	else
		result = RX_PROTOCOL_COLLECT_ERROR;

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
	std::scoped_lock _(port_lock_);
	port_=rx_reference<rx_http_port>::null_ptr;
}

rx_protocol_result_t rx_http_endpoint::create_and_forward_request (const char* method, size_t method_len, const char* path, size_t path_len, size_t num_headers, std::byte* content_ptr, size_t content_max_size)
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
	size_t idx = 0;
	for (; idx < path_len; idx++)
	{
		if (path[idx] == '?')
			break;
		str_path += path[idx];
	}
	request.path = rx_url_decode(str_path.c_str(), str_path.size());
	idx++;
	if (idx < path_len)
	{
		size_t idx_start = idx;
		string_type param_name;
		for (; idx < path_len; idx++)
		{
			if (param_name.empty())
			{
				if (path[idx] == '=')
				{
					if (idx_start == idx)
						break;

					param_name = rx_url_decode(&path[idx_start], idx - idx_start);
					idx++;
					idx_start = idx;
				}
				else if (path[idx] == '&')
				{
					if (idx_start == idx)
						break;

					request.params[rx_url_decode(&path[idx_start], idx - idx_start)] = "";
					param_name.clear();
					idx++;
					idx_start = idx;
				}
			}
			else
			{
				if (path[idx] == '&')
				{
					if (idx_start == idx)
						request.params[param_name] = "";
					else
						request.params[param_name] = rx_url_decode(&path[idx_start], idx - idx_start);
					idx++;
					idx_start = idx;
					param_name.clear();
				}
			}
		}
		if (param_name.empty())
		{
			if (idx_start < idx)
			{
				request.params[rx_url_decode(&path[idx_start], idx - idx_start)] = "";
			}
		}
		else
		{
			if (idx_start == idx)
				request.params[param_name] = "";
			else
				request.params[param_name] = rx_url_decode(&path[idx_start], idx - idx_start);
		}
	}
	size_t content_len = 0;
	for (size_t i = 0; i < num_headers; i++)
	{
		string_type name;
		string_type val;
		if (headers_buffer_[i].name_len > 0)
			name.assign(headers_buffer_[i].name, headers_buffer_[i].name_len);
		if (headers_buffer_[i].value_len > 0)
			val.assign(headers_buffer_[i].value, headers_buffer_[i].value_len);
		if (name == "Content-Length")
		{
			std::from_chars(headers_buffer_[i].value, headers_buffer_[i].value + headers_buffer_[i].value_len, content_len);
		}
		request.headers.emplace(std::move(name), std::move(val));
	}

	if (content_len > 0 && content_max_size > 0)
	{
		request.content = byte_string(content_ptr, content_ptr + std::min(content_len, content_max_size));
	}

	if (content_len > content_max_size)
	{
		prepared_request_ = std::move(request);
		content_left_ = content_len - content_max_size;
		return RX_PROTOCOL_OK;
	}
	request.whose = smart_this();
	std::scoped_lock _(port_lock_);
	if (port_)
	{
		port_->send_function([](http_request&& request)
			{
				auto result = rx_internal::rx_http_server::http_server::instance().handle_request(request);

			}, std::move(request));
	}
	return RX_PROTOCOL_OK;
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

void rx_http_endpoint::send_current_request ()
{
	prepared_request_.whose = smart_this();
	{
		std::scoped_lock _(port_lock_);
		if (port_)
		{
			port_->send_function([](http_request&& request)
				{
					auto result = rx_internal::rx_http_server::http_server::instance().handle_request(request);

				}, std::move(prepared_request_));
		}
	}
	prepared_request_ = http_request();
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

