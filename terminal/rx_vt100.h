

/****************************************************************************
*
*  terminal\rx_vt100.h
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


#ifndef rx_vt100_h
#define rx_vt100_h 1



// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"
// dummy
#include "dummy.h"



namespace rx_internal {

namespace terminal {

namespace term_transport {





class vt100_transport 
{
  public:
	vt100_transport(const vt100_transport &right) = delete;
	vt100_transport & operator=(const vt100_transport &right) = delete;
	vt100_transport(vt100_transport &&right) = delete;
	vt100_transport & operator=(vt100_transport &&right) = delete;
  private:
	enum parser_state
	{
		parser_normal,
		parser_in_end_line,
		parser_had_escape,
		parser_had_bracket,
		parser_had_bracket_number
	};
	typedef std::list<string_type> history_type;

  public:
      vt100_transport (runtime::items::port_runtime* port, bool to_echo = true);


      bool char_received (const char ch, bool eof, string_type& to_echo, string_type& line);

      void add_to_history (const string_type& line);

      rx_protocol_stack_endpoint* bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);

      void set_echo (bool val);


      void set_password_mode (bool value)
      {
        password_mode_ = value;
      }


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



  protected:

  private:

      bool move_cursor_left ();

      bool move_cursor_right ();

      bool char_received_normal (const char ch, bool eof, string_type& to_echo, string_type& line);

      bool char_received_in_end_line (char ch, string_type& to_echo, string_type& line);

      bool char_received_had_escape (const char ch, string_type& to_echo);

      bool char_received_had_bracket (char ch, string_type& to_echo);

      bool char_received_had_bracket_number (const char ch, string_type& to_echo);

      bool move_history_up (string_type& to_echo);

      bool move_history_down (string_type& to_echo);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);



      rx_protocol_stack_endpoint stack_entry_;


      parser_state state_;

      string_type current_line_;

      string_type::size_type current_idx_;

      bool password_mode_;

      history_type history_;

      history_type::iterator history_it_;

      bool had_first_;

      int opened_brackets_;

      std::function<void(int64_t)> sent_func_;

      std::function<void(int64_t)> received_func_;

      bool send_echo_;

      runtime::items::port_runtime* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< vt100_transport  > vt100_port_base;






class vt100_transport_port : public vt100_port_base  
{
	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
VT100 terminal. implementation of VT100 transport protocol port.");

	DECLARE_REFERENCE_PTR(vt100_transport_port);
    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<vt100_transport> > active_endpoints_type;

  public:
      vt100_transport_port();


  protected:

  private:


};


} // namespace term_transport
} // namespace terminal
} // namespace rx_internal



#endif
