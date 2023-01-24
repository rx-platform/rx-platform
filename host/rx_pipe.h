

/****************************************************************************
*
*  host\rx_pipe.h
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


#ifndef rx_pipe_h
#define rx_pipe_h 1



#include "system/server/rx_server.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_log
#include "system/server/rx_log.h"
// rx_anonymus_pipes
#include "host/rx_anonymus_pipes.h"

#define RX_PIPE_BUFFER_SIZE 0x10000 //64 KiB for pipes


namespace host {

namespace pipe {





class rx_pipe_stdout_log_subscriber : public rx_platform::log::log_subscriber  
{
	typedef std::vector<log::log_event_data> pending_events_type;

  public:
      rx_pipe_stdout_log_subscriber (bool supports_ansi = true);


      void log_event (log::log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when);

      void release_log (bool dump_previous);

      void suspend_log ();

      rx_result read_log (const log::log_query_type& query, log::log_events_type& result);

      string_type get_name () const;


      void set_supports_ansi (bool value)
      {
        supports_ansi_ = value;
      }



      log::rx_log_query_type log_query;


  protected:

  private:


      pending_events_type pending_events_;

      bool running_;

      locks::slim_lock pending_lock_;

      bool supports_ansi_;


};






class rx_pipe_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      rx_pipe_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

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

      rx_result build_host (hosting::host_platform_builder& builder);

      string_type get_host_manual () const;

      string_type get_host_name ();

      virtual void restore_console ();

      virtual rx_result setup_console (int argc, char* argv[]);

      void pipe_run_result (rx_result result);


  protected:

      void pipe_loop (configuration_data_t& config, const pipe_client_t& pipes, std::vector<library::rx_plugin_base*>& plugins);

      void add_command_line_options (hosting::command_line_options_t& options, rx_platform::configuration_data_t& config);

      virtual void read_config_options (const std::map<string_type, string_type>& options, rx_platform::configuration_data_t& config);


  private:

      virtual void get_stdio_handles (sys_handle_t& in, sys_handle_t& out, sys_handle_t& err) = 0;



      rx_reference<rx_pipe_stdout_log_subscriber> stdout_log_;

      rx_reference<local_pipe_port> pipe_port_;


      bool exit_;

      bool dump_start_log_;

      bool dump_storage_references_;

      bool debug_stop_;


};


} // namespace pipe
} // namespace host



#endif
