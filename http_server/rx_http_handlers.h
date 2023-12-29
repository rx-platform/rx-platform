

/****************************************************************************
*
*  http_server\rx_http_handlers.h
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


#ifndef rx_http_handlers_h
#define rx_http_handlers_h 1



// rx_http_request
#include "system/http_support/rx_http_request.h"
// rx_http
#include "system/http_support/rx_http.h"

#include "system/server/rx_server.h"
using namespace rx_platform::http;


namespace rx_internal {

namespace rx_http_server {






class http_handlers_repository 
{
    typedef std::map<string_type, std::unique_ptr<http_handler> > handlers_type;

  public:

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& config);

      http_handler* get_handler (const string_type& ext);


  protected:

  private:

      void register_standard_handlers ();



      handlers_type handlers_;


};






class http_file_handler : public rx_platform::http::http_handler  
{

  public:

      rx_result handle_request (http_request& req, http_response& resp);

      virtual const char* get_content_type () = 0;


  protected:

  private:


};






class png_file_handler : public http_file_handler  
{

  public:

      const char* get_extension ();

      const char* get_content_type ();


  protected:

  private:


};






class text_file_handler : public http_file_handler  
{

  public:

  protected:

  private:


};






class html_file_handler : public text_file_handler  
{

  public:

      const char* get_extension ();

      const char* get_content_type ();


  protected:

  private:


};






class css_file_handler : public text_file_handler  
{

  public:

      const char* get_extension ();

      const char* get_content_type ();


  protected:

  private:


};






class js_file_handler : public text_file_handler  
{

  public:

      const char* get_extension ();

      const char* get_content_type ();


  protected:

  private:


};


} // namespace rx_http_server
} // namespace rx_internal



#endif
