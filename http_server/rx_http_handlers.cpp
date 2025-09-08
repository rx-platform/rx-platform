

/****************************************************************************
*
*  http_server\rx_http_handlers.cpp
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


// rx_http_handlers
#include "http_server/rx_http_handlers.h"

#include "http_server/rx_http_server.h"
#include "rx_http_items.h"
#include "system/server/rx_file_helpers.h"
#include "rx_http_query.h"
#include "rx_http_server.h"


namespace rx_internal {

namespace rx_http_server {

// Class rx_internal::rx_http_server::http_handlers_repository 


rx_result http_handlers_repository::initialize (hosting::rx_platform_host* host, configuration_data_t& config)
{
	register_standard_handlers();
	return true;
}

void http_handlers_repository::register_standard_handlers ()
{
	// standard file handlers
	std::unique_ptr<http_handler> handler_ptr = std::make_unique<png_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<jpg_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<html_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<css_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<js_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<jsx_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<ico_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<svg_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	
	// excluded object handlers (moved to enterprise)
	//handler_ptr = std::make_unique<http_json_object_reader>();
	//handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	//handler_ptr = std::make_unique<http_object_writer>();
	//handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	//handler_ptr = std::make_unique<http_json_object_executer>();
	//handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));

	// display handlers
	handler_ptr = std::make_unique<http_display_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));

	// enterprise interfaces
	auto ent_itfs = enterprise::enterprise_manager::instance().list_interfaces();
	for (const auto& one : ent_itfs)
	{
		handler_ptr = std::make_unique<http_enterprise_handler>(one);
		if (handler_ptr->get_extension())
			handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	}
}

http_handler* http_handlers_repository::get_handler (const string_type& ext)
{
	if(ext.empty())
	{
		auto it = handlers_.find("html");
		if (it != handlers_.end())
			return it->second.get();
		else
			return nullptr;
	}
	else
	{
		auto it = handlers_.find(ext);
		if (it != handlers_.end())
			return it->second.get();
		else
			return nullptr;
	}
}


// Class rx_internal::rx_http_server::http_file_handler 


rx_result http_file_handler::handle_request (http_request& req, http_response& resp)
{
	if (req.method != rx_http_method::get)
	{
		resp.result = 405;
		return true;
	}
	auto paths = rx_internal::rx_http_server::http_server::instance().get_static_paths();
	for (const auto& one_dir : paths)
	{
		auto file_path = rx_combine_paths(one_dir, req.path);
		auto file = rx_file(file_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		if (file)
		{
			resp.result = 403;// Forbidden
			uint64_t size = 0;
			auto result = rx_file_get_size(file, &size);
			if (result == RX_OK)
			{
				resp.content.assign((size_t)size, std::byte{ 0 });
				uint32_t readed = 0;
				result = rx_file_read(file, &resp.content[0], (uint32_t)size, &readed);
				if (result == RX_OK)
				{
					resp.headers["Content-Type"] = get_content_type();
					resp.cache_me = true;
					resp.result = 200;// OK
				}
				else
				{// clear this buffer
					resp.content.clear();
				}
			}
			if (resp.result != 200)
			{
				char buff[0x100];
				rx_last_os_error("Error processing request:", buff, sizeof(buff));
				resp.set_string_content(buff);
			}
			rx_file_close(file);
			return true;
		}
	}

	resp.result = 404;// Not Found
	char buff[0x100];
	rx_last_os_error("Error processing request:", buff, sizeof(buff));
	resp.set_string_content(buff);

	return true;
}


// Class rx_internal::rx_http_server::png_file_handler 


const char* png_file_handler::get_extension ()
{
	return "png";
}

const char* png_file_handler::get_content_type ()
{
	return "image/png";
}


// Class rx_internal::rx_http_server::html_file_handler 


const char* html_file_handler::get_extension ()
{
	return "html";
}

const char* html_file_handler::get_content_type ()
{
	return "text/html";
}

rx_result html_file_handler::handle_request (http_request& req, http_response& resp)
{
	if (req.method != rx_http_method::get)
	{
		resp.result = 405;
		return true;
	}
	auto result = http_server::instance().get_parsed_html_content(req.path, req.rel_root, resp.content);
	if (result)
	{
		resp.headers["Content-Type"] = get_content_type();
		resp.cache_me = true;
		resp.result = 200;// OK
	}
	else
	{
		resp.result = 404;// Not Found
		char buff[0x100];
		rx_last_os_error("Error processing request:", buff, sizeof(buff));
		resp.set_string_content(buff);
	}

	return true;
}


// Class rx_internal::rx_http_server::css_file_handler 


const char* css_file_handler::get_extension ()
{
	return "css";
}

const char* css_file_handler::get_content_type ()
{
	return "text/css";
}


// Class rx_internal::rx_http_server::text_file_handler 


// Class rx_internal::rx_http_server::js_file_handler 


const char* js_file_handler::get_extension ()
{
	return "js";
}

const char* js_file_handler::get_content_type ()
{
	return "text/javascript";
}


// Class rx_internal::rx_http_server::jpg_file_handler 


const char* jpg_file_handler::get_extension ()
{
	return "jpg";
}

const char* jpg_file_handler::get_content_type ()
{
	return "image/jpeg";
}


// Class rx_internal::rx_http_server::ico_file_handler 


const char* ico_file_handler::get_extension ()
{
	return "ico";
}

const char* ico_file_handler::get_content_type ()
{
	//return "image/vnd.microsoft.icon";
	return "image/x-icon";
}


// Class rx_internal::rx_http_server::svg_file_handler 


const char* svg_file_handler::get_extension ()
{
	return "svg";
}

const char* svg_file_handler::get_content_type ()
{
	return "image/svg+xml";
}


// Class rx_internal::rx_http_server::jsx_file_handler 


const char* jsx_file_handler::get_extension ()
{
	return "jsx";
}

const char* jsx_file_handler::get_content_type ()
{
	return "text/js";
}


} // namespace rx_http_server
} // namespace rx_internal

