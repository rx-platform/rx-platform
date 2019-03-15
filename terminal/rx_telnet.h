

/****************************************************************************
*
*  terminal\rx_telnet.h
*
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



// rx_io
#include "lib/rx_io.h"
// rx_commands
#include "terminal/rx_commands.h"
// rx_vt100
#include "host/rx_vt100.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_security
#include "lib/security/rx_security.h"
// rx_ptr
#include "lib/rx_ptr.h"



namespace terminal {

namespace console {





class telnet_security_context : public rx::security::security_context  
{
	DECLARE_REFERENCE_PTR(telnet_security_context);

  public:
      telnet_security_context();

      telnet_security_context (const sockaddr_in& addr, const sockaddr_in& local_addr);

      ~telnet_security_context();


      bool has_console () const;

      bool is_system () const;


  protected:

  private:


};






class telnet_client : public rx_platform::prog::console_client  
{
	DECLARE_REFERENCE_PTR(telnet_client);
	typedef std::queue<buffer_ptr> running_buffers_type;

  public:
      telnet_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr);

      ~telnet_client();


      bool on_startup (rx_thread_handle_t destination);

      void on_shutdown (rx_thread_handle_t destination);

      const string_type& get_console_name ();

      bool get_next_line (string_type& line);


      rx_reference<rx::io::tcp_socket_std_buffer> socket ()
      {
        return my_socket_;
      }



  protected:

      void exit_console ();

      bool readed (const void* data, size_t count, rx_thread_handle_t destination);

      void release_buffer (buffer_ptr what);

      void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer);


  private:

      buffer_ptr get_free_buffer ();

      bool new_recive (const char* buff, size_t& idx);

      void send_string_response (const string_type& line, bool with_prompt = true);

      void lines_received (string_array&& lines);

      void cancel_current ();



      rx_reference<telnet_security_context> security_context_;

      host::rx_vt100::vt100_transport vt100_parser_;

      rx_reference<rx::io::tcp_socket_std_buffer> my_socket_;


      locks::slim_lock buffers_lock_;

      buffers_type buffers_;

      string_type receiving_string_;

      bool send_echo_;

      bool cancel_current_;

      bool verified_;

      bool exit_;


};







class server_telnet_socket : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(server_telnet_socket);

  public:
      server_telnet_socket();

      ~server_telnet_socket();


  protected:

      io::tcp_socket_std_buffer::smart_ptr make_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, rx_thread_handle_t destination);


  private:


      rx_reference<rx::io::tcp_listent_std_buffer> my_socket_;


};


} // namespace console
} // namespace terminal



#endif
