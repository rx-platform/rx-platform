

/****************************************************************************
*
*  terminal\rx_telnet.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_telnet_h
#define rx_telnet_h 1



// dummy
#include "dummy.h"
// rx_vt100
#include "terminal/rx_vt100.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"



namespace rx_internal {

namespace terminal {

namespace term_transport {





class telnet_transport 
{
public:
    telnet_transport(const telnet_transport& right) = delete;
    telnet_transport& operator=(const telnet_transport& right) = delete;
    telnet_transport(telnet_transport&& right) = delete;
    telnet_transport& operator=(telnet_transport&& right) = delete;
private:
    enum telnet_parser_state
    {
        telnet_parser_idle,
        telnet_parser_had_escape,
        telnet_parser_had_will,
        telnet_parser_had_wont,
        telnet_parser_had_do,
        telnet_parser_had_dont,
        telnet_parser_had_sb,
        telnet_parser_had_sb2
    };

  public:
      telnet_transport (runtime::items::port_runtime* port);

      ~telnet_transport();


      rx_protocol_stack_endpoint* bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);

      bool char_received (const char ch, bool eof, string_type& to_echo, string_type& line);


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      bool handle_telnet (const char ch, string_type& to_echo);



      vt100_transport vt100_;

      rx_protocol_stack_endpoint stack_entry_;


      std::function<void(int64_t)> sent_func_;

      std::function<void(int64_t)> received_func_;

      bool send_echo_;

      telnet_parser_state telnet_state_;

      runtime::items::port_runtime* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< telnet_transport  > telnet_port_base;






class telnet_transport_port : public telnet_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
VT100 terminal. implementation of telnet and VT100 transport protocol port.");

    DECLARE_REFERENCE_PTR(telnet_transport_port);

  public:
      telnet_transport_port();


  protected:

  private:


};


} // namespace term_transport
} // namespace terminal
} // namespace rx_internal



#endif
