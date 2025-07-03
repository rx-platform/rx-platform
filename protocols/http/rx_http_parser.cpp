

/****************************************************************************
*
*  protocols\http\rx_http_parser.cpp
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


// rx_http_parser
#include "protocols/http/rx_http_parser.h"



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

// Class protocols::rx_http::http_parser 

http_parser::http_parser()
      : content_left_(0)
{
}



rx_protocol_result_t http_parser::received (recv_protocol_packet packet)
{
	const char* method, * path;
	int pret, minor_version;
	size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
	const char* buffer_to_read = nullptr;
	rx_protocol_result_t result;

	if (content_left_ > 0)
	{
		if (packet.buffer->size < content_left_)
		{
			// we need to read even more data!!!!
			prepared_request_.content.insert(prepared_request_.content.end(), (const std::byte*)packet.buffer->buffer_ptr, (const std::byte*)packet.buffer->buffer_ptr + packet.buffer->size);
			content_left_ -= packet.buffer->size;

			return RX_PROTOCOL_OK;
		}
		else if (packet.buffer->size == content_left_)
		{
			// we read everything
			prepared_request_.content.insert(prepared_request_.content.end(), (const std::byte*)packet.buffer->buffer_ptr, (const std::byte*)packet.buffer->buffer_ptr + content_left_);
			content_left_ = 0;
			send_current_request();

			rx_reinit_packet_buffer(&receive_buffer_);

			return RX_PROTOCOL_OK;
		}
		else
		{
			RX_ASSERT(packet.buffer->size > content_left_);
			// we read everything and even more
			prepared_request_.content.insert(prepared_request_.content.end(), (const std::byte*)packet.buffer->buffer_ptr, (const std::byte*)packet.buffer->buffer_ptr + content_left_);
			content_left_ = 0;
			send_current_request();

			rx_reinit_packet_buffer(&receive_buffer_);

			// now do the even more stuff
			result = rx_push_to_packet(&receive_buffer_
				, packet.buffer->buffer_ptr + content_left_
				, packet.buffer->size - content_left_);
			if (result != RX_PROTOCOL_OK)
				return result;

			buflen = receive_buffer_.size;
			buffer_to_read = (const char*)(receive_buffer_.buffer_ptr);

			return RX_PROTOCOL_OK;
		}
	}
	else
	{
		prevbuflen = receive_buffer_.size;

		result = rx_push_to_packet(&receive_buffer_, packet.buffer->buffer_ptr, packet.buffer->size);
		if (result != RX_PROTOCOL_OK)
			return result;

		buflen = receive_buffer_.size;
		buffer_to_read = (const char*)(receive_buffer_.buffer_ptr);
	}
	num_headers = sizeof(headers_buffer_) / sizeof(headers_buffer_[0]);
	pret = phr_parse_request(buffer_to_read, buflen, &method, &method_len, &path, &path_len,
		&minor_version, headers_buffer_, &num_headers, prevbuflen);
	if (pret > 0)
	{
		size_t parsed = pret;
		std::byte* content_ptr = (std::byte*)buffer_to_read + parsed;
		size_t content_max = receive_buffer_.size - parsed;
		result = create_and_forward_request(method, method_len, path, path_len, num_headers, content_ptr, content_max);

		rx_reinit_packet_buffer(&receive_buffer_);
	}
	else if (pret == -2)// request is incomplete
		result = RX_PROTOCOL_OK;
	else if (pret == -1)
		result = RX_PROTOCOL_PARSING_ERROR;
	else
		result = RX_PROTOCOL_COLLECT_ERROR;

	return result;
}

rx_protocol_result_t http_parser::create_and_forward_request (const char* method, size_t method_len, const char* path, size_t path_len, size_t num_headers, std::byte* content_ptr, size_t content_max_size)
{
	http_parsed_request request;
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
	if (callback)
		return callback(std::move(std::move(request)));
	else
		return RX_PROTOCOL_OK;
}

void http_parser::send_current_request ()
{
	if(callback)
		callback(std::move(prepared_request_));
}


// Class protocols::rx_http::http_parsed_request 


} // namespace rx_http
} // namespace protocols

