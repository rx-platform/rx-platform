

/****************************************************************************
*
*  http_server\rx_http_items.h
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


#ifndef rx_http_items_h
#define rx_http_items_h 1



// rx_platform_item
#include "system/server/rx_platform_item.h"
// rx_http_request
#include "system/http_support/rx_http_request.h"
// rx_http
#include "system/http_support/rx_http.h"
// rx_http_mapping
#include "protocols/http/rx_http_mapping.h"

using namespace rx_platform::http;


namespace rx_internal {

namespace rx_http_server {






class http_rx_item_handler : public rx_platform::http::http_handler  
{
    struct item_result_type
    {
        http_request request;
        http_response response;
        operator bool() const
        {
            return request.whose;
        }
        item_result_type() = default;
        item_result_type(item_result_type&&) noexcept = default;
        item_result_type(const item_result_type&) = delete;
        item_result_type& operator=(item_result_type&&) noexcept = default;
        item_result_type& operator=(const item_result_type&) = delete;
    };

  public:

      virtual const char* get_content_type () = 0;

      rx_result handle_request (http_request& req, http_response& resp);


  protected:

  private:

      virtual rx_result do_with_item (string_view_type sub_item, rx_item_type type_type, rx_node_id type_id, http_request& req, http_response& resp, platform_item_ptr item) = 0;

      virtual rx_result do_with_directory (string_view_type sub_item, rx_item_type type_type, rx_node_id type_id, http_request& req, http_response& resp, rx_directory_ptr item) = 0;



};


} // namespace rx_http_server
} // namespace rx_internal



#endif
