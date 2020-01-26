

/****************************************************************************
*
*  host\rx_pipe.h
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


#ifndef rx_pipe_h
#define rx_pipe_h 1


#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_log
#include "lib/rx_log.h"
// rx_anonymus_pipes
#include "host/rx_anonymus_pipes.h"

#define RX_PIPE_BUFFER_SIZE 0x10000 //64 KiB for pipes


namespace host {

namespace pipe {





class rx_pipe_stdout_log_subscriber : public rx::log::log_subscriber  
{
	typedef std::vector<log::log_event_data> pending_events_type;

  public:
      rx_pipe_stdout_log_subscriber();


      void log_event (log::log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when);

      void release_log (bool dump_previous);

      void suspend_log ();


      bool show_traces;


  protected:

  private:


      pending_events_type pending_events_;

      bool running_;

      locks::slim_lock pending_lock_;


};






class rx_pipe_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      rx_pipe_host (hosting::rx_host_storages& storage);

      ~rx_pipe_host();


      void get_host_info (string_array& hosts);

      void server_started_event ();

      bool shutdown (const string_type& msg);

      bool exit () const;

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);

      bool break_host (const string_type& msg);

      int pipe_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins);

      static string_type get_pipe_info ();

      bool is_canceling () const;

      rx_result build_host (rx_directory_ptr root);

      string_type get_host_manual () const;

      string_type get_host_name ();


  protected:

      bool parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config, pipe_client_t& pipes);

      void pipe_loop (configuration_data_t& config, const pipe_client_t& pipes, std::vector<library::rx_plugin_base*>& plugins);


  private:

      virtual void get_stdio_handles (sys_handle_t& in, sys_handle_t& out, sys_handle_t& err) = 0;



      rx_reference<rx_pipe_stdout_log_subscriber> stdout_log_;

      rx_reference<local_pipe_port> pipe_port_;


      bool exit_;

      opcua_transport_protocol_type transport_;

      bool dump_start_log_;

      bool dump_storage_references_;

      bool debug_stop_;


};


} // namespace pipe
} // namespace host



#endif
