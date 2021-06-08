

/****************************************************************************
*
*  http_server\rx_http_server.cpp
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


// rx_http_server
#include "http_server/rx_http_server.h"

#include "rx_http_version.h"


namespace rx_internal {

namespace rx_http_server {
namespace {
http_server* g_inst = nullptr;
}

// Class rx_internal::rx_http_server::http_server 

http_server::http_server()
{
}



http_server& http_server::instance ()
{
	if (g_inst == nullptr)
		g_inst = new http_server();
	return *g_inst;
}

rx_result http_server::initialize (hosting::rx_platform_host* host, configuration_data_t& config)
{
	register_standard_filters();
	resources_path_ = config.other.http_path;
	if (!resources_path_.empty())
	{
		handlers_.initialize(host, config);
	}
	return true;
}

rx_result http_server::handle_request (http_request req)
{
	if (req.path.empty() || req.path == "/")
		req.path = "/index.html";
	HTTP_LOG_TRACE("http_server", 100, "HTTP request received for "s + req.path);
	http_response response;
	response.cache_me = false;
	if (req.method == rx_http_method::get)
	{
		locks::auto_lock_t _(&cache_lock_);
		auto it = cached_items_.find(req.path);
		if (it != cached_items_.end())
		{
			response = it->second;
		}
	}
	if (response.cache_me)
	{
		HTTP_LOG_TRACE("http_server", 100, "Cache sending HTTP response "s + response.result_string + " for "s + req.path);
		req.whose->send_response(std::move(response));
		return true;// done with the cache
	}
	
	for (auto& one : filters_)
	{
		one->handle_request_before(req, response);
	}
	auto handler = handlers_.get_handler(req.extension);
	if (handler)
	{		
		auto result = handler->handle_request(req, response);
		if (!result)
		{
			response.result = 500;
		}
	}
	else
	{
		response.result = 501;
	}
	send_response(req, std::move(response));
	return true;
}

void http_server::register_standard_filters ()
{
	filters_.emplace_back(std::make_unique<standard_request_filter>());
}

void http_server::send_response (http_request& request, http_response response)
{
	for (auto& one : filters_)
	{
		one->handle_request_after(request, response);
	}
	if (response.cache_me && request.method == rx_http_method::get)
	{
		locks::auto_lock_t _(&cache_lock_);
		cached_items_.emplace(request.path, response);
	}
	HTTP_LOG_TRACE("http_server", 100, "Sending HTTP response "s + response.result_string + " for "s + request.path);
	request.whose->send_response(std::move(response));
}

string_type http_server::get_server_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_HTTP_NAME, RX_HTTP_MAJOR_VERSION, RX_HTTP_MINOR_VERSION, RX_HTTP_BUILD_NUMBER);
	}
	return ret;
}

string_type http_server::get_server_header_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		std::ostringstream ss;
		ss << "{rx-platform}/"
			<< RX_HTTP_MAJOR_VERSION << '.'
			<< RX_HTTP_MINOR_VERSION;
		ret = ss.str();
	}
	return ret;
}

void http_server::deinitialize ()
{
	delete this;
}


// Class rx_internal::rx_http_server::standard_request_filter 


rx_result standard_request_filter::handle_request_after (http_request& req, http_response& resp)
{
	resp.headers.emplace("Connection", "Keep-Alive");
	resp.headers.emplace("Keep-Alive", "timeout=32");
	resp.headers.emplace("Content-Language", "en");
	resp.headers.emplace("Server", http_server::get_server_header_info());
	char buff[0x20];
	auto[ptr, ep] = std::to_chars(buff, buff + 0x1f, resp.content.size());
	*ptr = '\0';
	resp.headers.emplace("Content-Length", buff);
	switch (resp.result)
	{
	case 200:
		resp.result_string = "200 OK";
		break;
	case  403:
		resp.result_string = "403 Forbidden";
		break;
	case  404:
		resp.result_string = "404 Not Found";
		break;
	case 405:
		resp.result_string = "405 Method Not Allowed";
		break;
	case 500:
		resp.result_string = "500 Internal Server Error";
		break;
	case 501:
		resp.result_string = "501 Not Implemented";
		break;
	default:
		{
			char buff[0x20];
			auto [ptr, ec] = std::to_chars(buff, buff + 0x1f, resp.result);
			*ptr = '\0';
			resp.result_string = buff;
			resp.result_string += " Unknown";
		}
	}
	return true;
}

rx_result standard_request_filter::handle_request_before (http_request& req, http_response& resp)
{
	size_t idx = req.path.rfind('.');
	if (idx != !string_type::npos)
	{
		req.extension = req.path.substr(idx + 1);
	}
	return true;
}


} // namespace rx_http_server
} // namespace rx_internal

