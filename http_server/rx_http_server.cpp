

/****************************************************************************
*
*  http_server\rx_http_server.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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
#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "lib/rx_ser_json.h"
#include "sys_internal/rx_async_functions.h"
#include "system/server/rx_file_helpers.h"


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
	http_displays::rx_http_display_base::fill_globals();
	string_type master_path = config.other.http_path;
	if (!master_path.empty())
	{
		static_path_ = rx_combine_paths(master_path, "_static");
		global_path_ = rx_combine_paths(master_path, "_global");
		dynamic_path_ = rx_combine_paths(master_path, "_dynamic");
		handlers_.initialize(host, config);
	}
	auto result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		RX_STATIC_HTTP_DISPLAY_TYPE_ID, [] {
			return rx_create_reference<http_displays::rx_http_static_display>();
		});
	result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		RX_STANDARD_HTTP_DISPLAY_TYPE_ID, [] {
			return rx_create_reference<http_displays::rx_http_standard_display>();
		});
	result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		RX_SIMPLE_HTTP_DISPLAY_TYPE_ID, [] {
			return rx_create_reference<http_displays::rx_http_simple_display>();
		});
	return true;
}

rx_result http_server::handle_request (http_request req)
{
	if (req.path.empty() || req.path == "/")
		req.path = "/sys/runtime/system/System.index.disp";
	HTTP_LOG_TRACE("http_server", 100, "HTTP request received for "s + req.path);
	http_response response;
	response.cache_me = false;
#ifndef _DEBUG
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
#endif

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
			response.set_string_content(result.errors_line());
			response.headers["Content-Type"] = "text/plain";
			response.result = 500;
		}
	}
	else
	{
		response.set_string_content("No handler registered for this type");
		response.headers["Content-Type"] = "text/plain";
		response.result = 501;
	}
	if (response.result > 0)
		send_response(req, std::move(response));
	return true;
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

string_type http_server::get_global_content (const string_type& path)
{
	{
		locks::auto_lock_t _(&cache_lock_);
		auto it = cached_globals_.find(path);
		if (it != cached_globals_.end())
			return it->second;
	}
	string_type ret;
	auto file_path = rx_combine_paths(global_path_, path);
	rx::rx_source_file src;
	auto result = src.open(file_path.c_str());
	if (result)
	{
		string_type buffer;
		result = src.read_string(buffer);
		if (result)
		{
			ret = std::move(buffer);
			// put in cache if not there
			locks::auto_lock_t _(&cache_lock_);
			cached_globals_.emplace(path, ret);

		}
	}
	return ret;
}

string_type http_server::get_dynamic_content (const string_type& path)
{
	string_type ret;
	auto file_path = rx_combine_paths(dynamic_path_, path);
	rx::rx_source_file src;
	auto result = src.open(file_path.c_str());
	if (result)
	{
		string_type buffer;
		result = src.read_string(buffer);
		if (result)
		{
			ret = std::move(buffer);
			locks::auto_lock_t _(&cache_lock_);
			cached_dynamic_.emplace(path, ret);
		}
	}
	return ret;
}

void http_server::register_standard_filters ()
{
	filters_.emplace_back(std::make_unique<standard_request_filter>());
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
	case 100:
		resp.result_string = "100 Continue";
		break;
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


// Class rx_internal::rx_http_server::http_display_handler 


rx_result http_display_handler::handle_request (http_request& req, http_response& resp)
{
	resp.result = 0;

	auto idx = req.path.rfind('.');
	if (idx != string_type::npos)
	{
		req.path = req.path.substr(0, idx);
	}

	rx_thread_handle_t executer;

	auto disp_res = http_server::instance().get_displays().get_display(req.path, executer);
	if (disp_res)
	{
		
		disp_res.value()->req_ += 1;
		auto ticks1 = rx_get_us_ticks();

		rx_platform::rx_post_function_to(executer, req.whose
			, [ticks1](http_displays::rx_http_display_base::smart_ptr disp, http_request&& req, http_response&& resp) mutable
			{
				auto result = disp->handle_request(req, resp);


				float time_val = (float)(rx_get_us_ticks() - ticks1) / 1000.0f;
				disp->last_req_time_ = time_val;
				if (disp->max_req_time_ <= time_val)
					disp->max_req_time_ = time_val;

				if (!result)
				{
					disp->failed_ += 1;

					resp.headers["Content-Type"] = "text/plain";
					if (resp.result == 0 || resp.result == 200)
					{
						resp.result = 501;
						resp.set_string_content(result.errors_line());
					}

				}
				if (req.whose)
					http_server::instance().send_response(req, resp);

			}, disp_res.move_value(), std::move(req), std::move(resp));
		return true;
	}
	else
	{
		return "Invalid path";
	}

}

const char* http_display_handler::get_extension ()
{
	return "disp";
}


} // namespace rx_http_server
} // namespace rx_internal

