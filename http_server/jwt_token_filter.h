

/****************************************************************************
*
*  http_server\jwt_token_filter.h
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


#ifndef jwt_token_filter_h
#define jwt_token_filter_h 1



// rx_http
#include "system/http_support/rx_http.h"

using namespace rx_platform;


namespace rx_internal {

namespace rx_http_server {

namespace jwt_authorize {





class jwt_authorizer : public rx_platform::http::http_request_filter  
{

  public:

      rx_result handle_request_after (http::http_request& req, http::http_response& resp);

      rx_result handle_request_before (http::http_request& req, http::http_response& resp);


  protected:

  private:


};


} // namespace jwt_authorize
} // namespace rx_http_server
} // namespace rx_internal



#endif
