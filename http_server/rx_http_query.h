

/****************************************************************************
*
*  http_server\rx_http_query.h
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


#ifndef rx_http_query_h
#define rx_http_query_h 1



// rx_http_request
#include "system/http_support/rx_http_request.h"
// rx_http
#include "system/http_support/rx_http.h"
// rx_enterprise
#include "enterprise/rx_enterprise.h"

namespace protocols {
namespace rx_http {
class rx_http_endpoint;

} // namespace rx_http
} // namespace protocols


using namespace rx_platform::http;


namespace rx_internal {

namespace rx_http_server {

struct enterprise_request_type
{
    http_request request;
    http_response response;
    enterprise_request_type() = default;
    enterprise_request_type(enterprise_request_type&&) noexcept = default;
    enterprise_request_type(const enterprise_request_type&) = delete;
    enterprise_request_type& operator=(enterprise_request_type&&) noexcept = default;
    enterprise_request_type& operator=(const enterprise_request_type&) = delete;
};




class enterprise_handler_callback : public enterprise::enterprise_callback  
{
    typedef std::map<uint64_t, enterprise_request_type> requests_type;

  public:
      enterprise_handler_callback();

      ~enterprise_handler_callback();


      void read_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      void write_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      void execute_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      void query_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      uint64_t register_request (http_request& req, http_response& resp);


  protected:

  private:

      void request_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);



      locks::slim_lock requests_lock_;

      requests_type pending_requests_;


};







class http_enterprise_handler : public rx_platform::http::http_handler  
{

  public:
      http_enterprise_handler (string_view_type itf_type);


      rx_result handle_request (http_request& req, http_response& resp);

      const char* get_extension ();


  protected:

  private:


      enterprise::enterprise_interface *my_interface_;

      enterprise_handler_callback callback_;


      string_type name_;


};


} // namespace rx_http_server
} // namespace rx_internal



#endif
