

/****************************************************************************
*
*  protocols\http\rx_http_parser.h
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


#ifndef rx_http_parser_h
#define rx_http_parser_h 1



// rx_io_buffers
#include "lib/rx_io_buffers.h"

#include "protocols/ansi_c/http_c/rx_http_c_impl.h"
#include "system/http_support/rx_http_request.h"

using namespace rx_platform::http;



namespace protocols {

namespace rx_http {





struct http_parsed_request 
{


      rx_http_method method;

      string_type path;

      string_view_type extension;

      rx_http_parsed_headers_type headers;

      rx_parsed_acceptable_types_type acceptable_types;

      rx_http_parsed_params_type params;

      void* content_ptr;

      size_t content_size;

  public:

  protected:

  private:


};






struct http_parsed_response 
{


      int status;

      string_view_type message;

      rx_http_parsed_headers_type headers;

      void* content_ptr;

      size_t content_size;

  public:

  protected:

  private:


};







class http_parser 
{
    
    typedef std::function<rx_protocol_result_t(http_parsed_request)> callback_type;
    typedef std::function<rx_protocol_result_t(http_parsed_response)> client_callback_type;

  public:
      http_parser (bool simple);


      rx_protocol_result_t received (recv_protocol_packet packet);

      rx_protocol_result_t received_response (recv_protocol_packet packet);

      rx_const_packet_buffer create_const_buffer ();


      callback_type callback;

      client_callback_type client_callback;


  protected:

  private:

      rx_protocol_result_t create_and_forward_request (const char* method, size_t method_len, const char* path, size_t path_len, size_t num_headers, std::byte* content_ptr, size_t content_max_size);

      rx_protocol_result_t create_and_forward_response (int status_code, const char* msg, size_t msg_len, size_t num_headers, std::byte* content_ptr, size_t content_max_size);

      void send_current_request ();

      void send_current_response ();



      rx::io::rx_io_buffer receive_buffer_;


      size_t content_left_;

      http_parsed_request prepared_request_;

      http_parsed_response prepared_response_;

      phr_header headers_buffer_[0x80];

      bool simple_;


};


} // namespace rx_http
} // namespace protocols



#endif
