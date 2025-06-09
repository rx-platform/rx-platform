

/****************************************************************************
*
*  http_server\rx_aspnet.h
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


#ifndef rx_aspnet_h
#define rx_aspnet_h 1



// rx_http
#include "system/http_support/rx_http.h"
// rx_http_displays
#include "http_server/rx_http_displays.h"

using namespace rx_platform::http;


namespace rx_internal {

namespace rx_http_server {

namespace aspnet {





class aspnet_authorizer : public rx_platform::http::http_request_filter  
{

  public:

      rx_result handle_request_after (http_request& req, http_response& resp);

      rx_result handle_request_before (http_request& req, http_response& resp);


  protected:

  private:


};






class aspnet_logon_display : public http_displays::rx_http_static_display  
{

  public:
      aspnet_logon_display();

      ~aspnet_logon_display();


      rx_result initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path);

      rx_result deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path);

      rx_result handle_request (rx_platform::http::http_request& req, rx_platform::http::http_response& resp);


  protected:

  private:


};


} // namespace aspnet
} // namespace rx_http_server
} // namespace rx_internal



#endif
