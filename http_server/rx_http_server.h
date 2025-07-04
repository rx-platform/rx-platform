

/****************************************************************************
*
*  http_server\rx_http_server.h
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


#ifndef rx_http_server_h
#define rx_http_server_h 1


#include "system/server/rx_server.h"

// rx_http_request
#include "system/http_support/rx_http_request.h"
// rx_http
#include "system/http_support/rx_http.h"
// rx_http_mapping
#include "protocols/http/rx_http_mapping.h"
// rx_http_displays
#include "http_server/rx_http_displays.h"
// rx_http_handlers
#include "http_server/rx_http_handlers.h"

/////////////////////////////////////////////////////////////
// logging macros for http library
#define HTTP_LOG_INFO(src,lvl,msg) RX_LOG_INFO("HTTP",src,lvl,msg)
#define HTTP_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("HTTP",src,lvl,msg)
#define HTTP_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("HTTP",src,lvl,msg)
#define HTTP_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("HTTP",src,lvl,msg)
#define HTTP_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("HTTP",src,lvl,msg)
#define HTTP_LOG_TRACE(src,lvl,msg) RX_TRACE("HTTP",src,lvl,msg)


namespace rx_internal {

namespace rx_http_server {





class standard_request_filter : public rx_platform::http::http_request_filter  
{

  public:

      rx_result handle_request_after (http_request& req, http_response& resp);

      rx_result handle_request_before (http_request& req, http_response& resp);


  protected:

  private:


};






class http_display_handler : public rx_platform::http::http_handler  
{
    struct display_result_type
    {
        http_request request;
        http_response response;
        operator bool() const
        {
            return request.whose;
        }
        display_result_type() = default;
        display_result_type(display_result_type&&) noexcept = default;
        display_result_type(const display_result_type&) = delete;
        display_result_type& operator=(display_result_type&&) noexcept = default;
        display_result_type& operator=(const display_result_type&) = delete;
    };

  public:

      rx_result handle_request (http_request& req, http_response& resp);

      const char* get_extension ();


  protected:

  private:


};







class http_server 
{
    typedef std::unique_ptr<http_request_filter> filter_ptr_t;
    typedef std::vector<filter_ptr_t> filters_type;
    typedef std::map<string_type, http_response> cached_items_type;
    typedef std::map<string_type, string_type> cached_content_type;

  public:

      static http_server& instance ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& config);

      rx_result handle_request (http_request req);

      void send_response (http_request& request, http_response response);

      static string_type get_server_info ();

      static string_type get_server_header_info ();

      void deinitialize ();

      string_type get_global_content1 (const string_type& path);

      string_type get_dynamic_content (const string_type& path);

      void dump_http_references (std::ostream& out);


      http_displays::http_displays_repository& get_displays ()
      {
        return displays_;
      }



      const string_array& get_static_paths () const
      {
        return static_paths_;
      }



  protected:

  private:
      http_server();


      void register_standard_filters ();



      http_handlers_repository handlers_;

      filters_type filters_;

      http_displays::http_displays_repository displays_;


      string_array static_paths_;

      string_type global_path_;

      string_array dynamic_paths_;

      cached_items_type cached_items_;

      locks::slim_lock cache_lock_;

      cached_content_type cached_globals_;

      cached_content_type cached_dynamic_;


};






class http_path_filter : public rx_platform::http::http_request_filter  
{

  public:

      rx_result handle_request_after (http_request& req, http_response& resp);

      rx_result handle_request_before (http_request& req, http_response& resp);


  protected:

  private:


};


} // namespace rx_http_server
} // namespace rx_internal



#endif
