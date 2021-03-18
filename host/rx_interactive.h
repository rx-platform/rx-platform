

/****************************************************************************
*
*  host\rx_interactive.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_interactive_h
#define rx_interactive_h 1



#include "system/server/rx_server.h"

// rx_host
#include "system/hosting/rx_host.h"
// dummy
#include "dummy.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_thread
#include "lib/rx_thread.h"

namespace host {
namespace interactive {
class interactive_console_port;

} // namespace interactive
} // namespace host


using namespace rx;
using namespace rx_platform;



namespace host {

namespace interactive {





class interactive_console_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      interactive_console_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~interactive_console_host();


      void get_host_info (string_array& hosts);

      void server_started_event ();

      bool shutdown (const string_type& msg);

      bool exit () const;

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);

      std::vector<ETH_interface> get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      std::vector<IP_interface> get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      int console_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins);

      virtual rx_result setup_console (int argc, char* argv[]);

      virtual void restore_console ();

      static string_type get_interactive_info ();

      rx_result build_host (hosting::host_platform_builder& builder);

      string_type get_host_manual () const;

      string_type get_host_name ();

      virtual bool read_stdin (std::array<char,0x100>& chars, size_t& count) = 0;

      virtual bool write_stdout (const void* data, size_t size) = 0;

      bool write_stdout (const string_type& lines);

      void console_run_result (rx_result result);


  protected:

      rx_result console_loop (configuration_data_t& config, std::vector<library::rx_plugin_base*>& plugins);

      bool parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config);


  private:


      rx_reference<interactive_console_port> interactive_port_;


      bool exit_;


};






class interactive_console_endpoint 
{

  public:
      interactive_console_endpoint (interactive_console_host* host);


      rx_result run_interactive (std::function<void(int64_t)> received_func);

      void close ();

      rx_result open (std::function<void(int64_t)> sent_func);


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      interactive_console_host *host_;

      rx::threads::physical_job_thread std_out_sender_;

      rx_protocol_stack_endpoint stack_entry_;


      std::function<void(int64_t)> sent_func_;

      friend class interactive_console_port;
};






class interactive_console_port : public rx_platform::runtime::items::port_runtime  
{
	DECLARE_CODE_INFO("rx", 0, 0, 1, "\
Standard IO class. implementation of an standard IO console port");

	DECLARE_REFERENCE_PTR(interactive_console_port);

  public:
      interactive_console_port (interactive_console_host* host);


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result run_interactive (interactive_console_host* host);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      void remove_endpoint(rx_protocol_stack_endpoint* what)
      {

      }
  protected:

  private:


      interactive_console_endpoint endpoint_;


      std::atomic<bool> listening_;


};


} // namespace interactive
} // namespace host



#endif
