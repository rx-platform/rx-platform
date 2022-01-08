

/****************************************************************************
*
*  system\http_support\rx_http_request.h
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


#ifndef rx_http_request_h
#define rx_http_request_h 1




namespace protocols {
namespace rx_http {
class rx_http_endpoint;

} // namespace rx_http
} // namespace protocols


using protocols::rx_http::rx_http_endpoint;


namespace rx_platform {

namespace http {
typedef rx_reference<rx_http_endpoint> rx_http_endpoint_ptr;

enum class rx_http_method
{
    null,
    get,
    post,
    put,
    del
};

typedef std::map<string_type, string_type> rx_http_params_type;
typedef std::map<string_type, string_type> rx_http_headers_type;
typedef std::vector<string_type> rx_acceptable_types_type;






struct http_request 
{


      rx_http_endpoint_ptr whose;

      rx_http_method method;

      string_type path;

      string_type extension;

      rx_http_headers_type headers;

      rx_acceptable_types_type acceptable_types;

      rx_http_params_type params;

  public:

  protected:

  private:


};






struct http_response 
{


      void set_string_content (const string_type& str);


      int result;

      rx_http_headers_type headers;

      byte_string content;

      string_type result_string;

      bool cache_me;

  public:

  protected:

  private:


};


} // namespace http
} // namespace rx_platform



#endif
