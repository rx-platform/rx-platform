

/****************************************************************************
*
*  system\http_support\rx_http.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_http_h
#define rx_http_h 1



// rx_http_request
#include "system/http_support/rx_http_request.h"



namespace rx_platform {

namespace http {






class http_request_filter 
{

  public:

      virtual rx_result handle_request_before (http_request& req, http_response& resp) = 0;

      virtual rx_result handle_request_after (http_request& req, http_response& resp) = 0;

      virtual ~http_request_filter() = default;
  protected:

  private:


};







class http_handler 
{

  public:

      virtual rx_result handle_request (http_request& req, http_response& resp) = 0;

      virtual const char* get_extension () = 0;

      virtual ~http_handler() = default;
  protected:

  private:


};


} // namespace http
} // namespace rx_platform



#endif
