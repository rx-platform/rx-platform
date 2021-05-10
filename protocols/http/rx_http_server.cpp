

/****************************************************************************
*
*  protocols\http\rx_http_server.cpp
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
#include "protocols/http/rx_http_server.h"

#include "rx_http_mapping.h"


namespace protocols {

namespace rx_http {

// Class protocols::rx_http::http_server 

http_server::http_server()
{
}



http_server& http_server::instance ()
{
	static http_server g_inst;
	return g_inst;
}

rx_result http_server::initialize (hosting::rx_platform_host* host, configuration_data_t& config)
{
	resources_path_ = config.other.http_path;
	if (!resources_path_.empty())
	{
		handlers_.initialize(host, config);
		register_standard_filters();
	}
	return true;
}

rx_result http_server::handle_request (http_request req)
{
	if (req.path.empty() || req.path == "/")
		req.path = "/index.html";
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
		req.whose->send_response(std::move(response));
		return true;// done with the cache
	}
	size_t idx = req.path.rfind('.');
	if (idx !=! string_type::npos)
	{
		req.extension = req.path.substr(idx + 1);
	}
	auto handler = handlers_.get_handler(req.extension);
	if (handler)
	{
		for (auto& one : filters_)
		{
			one->handle_request_before(req, response);
		}
		auto result = handler->handle_request(req, response);
		if (result)
		{
			for (auto& one : filters_)
			{
				one->handle_request_after(req, response);
			}
			if (response.cache_me && req.method == rx_http_method::get)
			{
				locks::auto_lock_t _(&cache_lock_);
				cached_items_.emplace(req.path, response);
			}
			req.whose->send_response(std::move(response));
		}
	}
	return true;
}

void http_server::register_standard_filters ()
{
	filters_.emplace_back(std::make_unique<standard_request_filter>());
}


// Class protocols::rx_http::http_request_filter 


// Class protocols::rx_http::standard_request_filter 


rx_result standard_request_filter::handle_request_after (http_request req, http_response& resp)
{
	resp.headers.emplace("Connection", "Keep-Alive");
	resp.headers.emplace("Keep-Alive", "timeout=32");
	resp.headers.emplace("Server", "{rx-platform}");
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

rx_result standard_request_filter::handle_request_before (http_request req, http_response& resp)
{
	return true;
}


} // namespace rx_http
} // namespace protocols

