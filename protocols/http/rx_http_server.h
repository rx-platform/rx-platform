

/****************************************************************************
*
*  protocols\http\rx_http_server.h
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


#ifndef rx_http_server_h
#define rx_http_server_h 1


#include "system/server/rx_server.h"

// rx_http_request
#include "protocols/http/rx_http_request.h"
// rx_http_handlers
#include "protocols/http/rx_http_handlers.h"



namespace protocols {

namespace rx_http {





class http_request_filter 
{

  public:

      virtual rx_result handle_request_before (http_request req, http_response& resp) = 0;

      virtual rx_result handle_request_after (http_request req, http_response& resp) = 0;

      virtual ~http_request_filter() = default;
  protected:

  private:


};







class http_server 
{
    typedef std::unique_ptr<http_request_filter> filter_ptr_t;
    typedef std::vector<filter_ptr_t> filters_type;
    typedef std::map<string_type, http_response> cached_items_type;

  public:

      static http_server& instance ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& config);

      rx_result handle_request (http_request req);


      const string_type& get_resources_path () const
      {
        return resources_path_;
      }



  protected:

  private:
      http_server();


      void register_standard_filters ();



      http_handlers_repository handlers_;

      filters_type filters_;


      string_type resources_path_;

      cached_items_type cached_items_;

      locks::slim_lock cache_lock_;


};






class standard_request_filter : public http_request_filter  
{

  public:

      rx_result handle_request_after (http_request req, http_response& resp);

      rx_result handle_request_before (http_request req, http_response& resp);


  protected:

  private:


};


} // namespace rx_http
} // namespace protocols



#endif
