

/****************************************************************************
*
*  terminal\rx_vt100.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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



// dummy
#include "dummy.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_console
#include "terminal/rx_console.h"
// rx_ptr
#include "lib/rx_ptr.h"



namespace rx_internal {

namespace terminal {

namespace term_ports {





class vt100_endpoint : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(vt100_endpoint);
  private:
	enum parser_state
	{
		parser_normal,
		parser_in_end_line,
		parser_had_escape,
		parser_had_bracket,
        parser_had_os_command,
		parser_had_bracket_number
	};
	typedef std::list<string_type> history_type;

  public:
      vt100_endpoint (runtime::items::port_runtime* port, security::security_guard_ptr guard, bool to_echo = true);

      ~vt100_endpoint();


      bool char_received (const char ch, bool eof, string_type& to_echo, string_type& line);

      void add_to_history (const string_type& line);

      void set_echo (bool val);

      bool do_command (string_type&& line, security::security_context_ptr ctx);

      bool do_commands (string_array&& lines, security::security_context_ptr ctx);

      static string_type get_terminal_info ();

      void get_prompt (string_type& prompt);

      void get_wellcome (string_type& wellcome);

      void close_endpoint ();


      void set_password_mode (bool value)
      {
        password_mode_ = value;
      }


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



      rx_protocol_stack_endpoint stack_entry;


      bool send_echo;


  protected:

  private:

      bool move_cursor_left ();

      bool move_cursor_right ();

      bool char_received_normal (const char ch, bool eof, string_type& to_echo, string_type& line);

      bool char_received_in_end_line (char ch, string_type& to_echo, string_type& line);

      bool char_received_had_escape (const char ch, string_type& to_echo);

      bool char_received_had_bracket (char ch, string_type& to_echo);

      bool char_received_had_os (char ch, string_type& to_echo);

      bool char_received_had_bracket_number (const char ch, string_type& to_echo);

      bool move_history_up (string_type& to_echo);

      bool move_history_down (string_type& to_echo);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      void synchronized_cancel_command (security::security_context_ptr ctx);

      void synchronized_do_command (const string_type& in_line, security::security_context_ptr ctx);

      void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done);



      rx_reference<console::console_runtime> console_program_;


      parser_state state_;

      string_type current_line_;

      string_type::size_type current_idx_;

      bool password_mode_;

      history_type history_;

      history_type::iterator history_it_;

      bool had_first_;

      int opened_brackets_;

      runtime::items::port_runtime* port_;

      string_type os_command_;

      rx_timer_ptr pull_timer_;

};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< rx_internal::terminal::term_ports::vt100_endpoint  > vt100_port_base;






class vt100_port : public vt100_port_base  
{
	DECLARE_CODE_INFO("rx", 2, 0, 1, "\
VT100 terminal, implementation of VT100 terminal server that\r\n\
executes input based script program.");

	DECLARE_REFERENCE_PTR(vt100_port);
    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<vt100_endpoint> > active_endpoints_type;

  public:
      vt100_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime_init_context& ctx);


  protected:

  private:


      security::security_guard_ptr security_guard_;


};


} // namespace term_ports
} // namespace terminal
} // namespace rx_internal



#endif
