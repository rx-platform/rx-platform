

/****************************************************************************
*
*  system\http_support\rx_http_request.cpp
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
// rx_http_request
#include "system/http_support/rx_http_request.h"



namespace rx_platform {

namespace http {

// Class rx_platform::http::http_request 


string_view_type http_request::get_content_as_string ()
{
	if (content.empty())
		return string_view_type();
	else
		return string_view_type((const char*)&content[0], content.size());
}


// Class rx_platform::http::http_response 


void http_response::set_string_content (const string_type& str)
{
	if (str.empty())
		content = byte_string{ 0 };
	else
		content = byte_string((const std::byte*)str.c_str(), (const std::byte*)(str.c_str() + str.size()));
}


} // namespace http
} // namespace rx_platform

