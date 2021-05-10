

/****************************************************************************
*
*  protocols\http\rx_http_handlers.cpp
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


// rx_http_handlers
#include "protocols/http/rx_http_handlers.h"

#include "rx_http_mapping.h"


namespace protocols {

namespace rx_http {

// Class protocols::rx_http::http_handler 


// Class protocols::rx_http::http_file_handler 


rx_result http_file_handler::handle_request (http_request req, http_response& resp)
{
	if (req.method != rx_http_method::get)
	{
		resp.result = 405;
		return true;
	}
	auto file_path = rx_combine_paths(http_server::instance().get_resources_path(), req.path);
	auto file = rx_file(file_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	if (file)
	{
		resp.result = 403;// Forbidden
		uint64_t size = 0;
		auto result = rx_file_get_size(file, &size);
		if (result == RX_OK)
		{
			resp.content.assign(size, 0);
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
		rx_file_close(file);
	}
	else
	{
		resp.result = 404;// Not Found
	}


	return true;
}


// Class protocols::rx_http::png_file_handler 


const char* png_file_handler::get_extension ()
{
	return "png";
}

const char* png_file_handler::get_content_type ()
{
	return "image/png";
}


// Class protocols::rx_http::html_file_handler 


const char* html_file_handler::get_extension ()
{
	return "html";
}

const char* html_file_handler::get_content_type ()
{
	return "text/html";
}


// Class protocols::rx_http::css_file_handler 


const char* css_file_handler::get_extension ()
{
	return "css";
}

const char* css_file_handler::get_content_type ()
{
	return "text/css";
}


// Class protocols::rx_http::text_file_handler 


// Class protocols::rx_http::http_handlers_repository 


rx_result http_handlers_repository::initialize (hosting::rx_platform_host* host, configuration_data_t& config)
{
	register_standard_handlers();
	return true;
}

void http_handlers_repository::register_standard_handlers ()
{
	std::unique_ptr<http_handler> handler_ptr = std::make_unique<png_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<html_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
	handler_ptr = std::make_unique<css_file_handler>();
	handlers_.emplace(string_type(handler_ptr->get_extension()), std::move(handler_ptr));
}

http_handler* http_handlers_repository::get_handler (const string_type& ext)
{
	auto it = handlers_.find(ext);
	if (it != handlers_.end())
		return it->second.get();
	else
		return nullptr;
}


} // namespace rx_http
} // namespace protocols

