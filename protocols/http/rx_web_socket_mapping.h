

/****************************************************************************
*
*  protocols\http\rx_web_socket_mapping.h
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


#ifndef rx_web_socket_mapping_h
#define rx_web_socket_mapping_h 1



// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"
// dummy
#include "dummy.h"
// rx_http_parser
#include "protocols/http/rx_http_parser.h"
// rx_io_buffers
#include "lib/rx_io_buffers.h"

#define WS_HEAD_MAX_SIZE 16 //(actually 14, but we reserve 2 bytes for future use)


namespace protocols {

namespace rx_http {
class rx_web_socket_port;

#define RX_WS_INVALID_OPCODE 0xFF // Invalid opcode for WebSocket
struct web_socket_header
{
    uint8_t opcode;
    bool is_final;
    bool is_masked;
    size_t payload_length;
    uint8_t mask_key[4];

    operator bool() const
    {
        return opcode !=  RX_WS_INVALID_OPCODE;
	}
};




class rx_web_socket_endpoint 
{
    enum class web_socket_state
    {
      initial,
	  idle,
      header_collect,
      payload_collect,
      closing,
      closed
	};

  public:
      rx_web_socket_endpoint (rx_web_socket_port* port);

      ~rx_web_socket_endpoint();


      void close_endpoint ();


      rx_web_socket_port* get_port ()
      {
        return port_;
      }


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }



      rx_protocol_stack_endpoint stack_entry;


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      static rx_protocol_result_t transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address);

      rx_protocol_result_t parse_http_request (http_parsed_request req);

      string_type generate_websocket_accept (const string_type& web_key);

      rx_protocol_result_t received (recv_protocol_packet packet);

      rx_protocol_result_t send (send_protocol_packet packet);

      bool is_header_done ();

      web_socket_header parse_header ();



      http_parser parser_;

      rx::io::rx_io_buffer *receive_buffer_;


      rx_web_socket_port* port_;

      rx_thread_handle_t executer_;

      locks::slim_lock port_lock_;

      web_socket_state state_;

      size_t head_size_;

      uint8_t header_buffer_[WS_HEAD_MAX_SIZE];

      size_t payload_to_read_;


};







typedef rx_platform::runtime::io_types::ports_templates::connection_transport_port_impl< protocols::rx_http::rx_web_socket_endpoint  > rx_web_socket_port_base;






class rx_web_socket_port : public rx_web_socket_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
Web Socket port. Implementation of Web Socket transport.");

    DECLARE_REFERENCE_PTR(rx_web_socket_port);

  public:
      rx_web_socket_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);


      rx_platform::runtime::io_types::simple_port_status status;


  protected:

  private:


};


} // namespace rx_http
} // namespace protocols



#endif
