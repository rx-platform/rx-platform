

/****************************************************************************
*
*  host\rx_interactive.h
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


#ifndef rx_interactive_h
#define rx_interactive_h 1



#include "system/server/rx_server.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_security
#include "lib/security/rx_security.h"
// rx_vt100
#include "host/rx_vt100.h"



using namespace rx;
using namespace rx_platform;



namespace host {

namespace interactive {





class interactive_security_context : public rx::security::built_in_security_context  
{
	DECLARE_REFERENCE_PTR(interactive_security_context);


  public:
      interactive_security_context();

      ~interactive_security_context();


      bool has_console () const;

      bool is_system () const;

      bool is_interactive () const;


  protected:

  private:


};






class interactive_console_client : public rx_platform::prog::console_client  
{
	DECLARE_REFERENCE_PTR(interactive_console_client);


  public:
      interactive_console_client (hosting::rx_platform_host* host);

      ~interactive_console_client();


      const string_type& get_console_name ();

      rx_result run_interactive (configuration_data_t& config);

      security::security_context::smart_ptr get_current_security_context ();


      static string_type license_message;


  protected:

      void exit_console ();

      void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer);

      bool readed (const void* data, size_t count, rx_thread_handle_t destination);


  private:


      rx_reference<interactive_security_context> security_context_;

      rx_platform::hosting::rx_platform_host *host_;

      rx_vt100::vt100_transport vt100_transport_;


      bool exit_;


};







class interactive_console_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      interactive_console_host (hosting::rx_host_storages& storage);

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

      rx_result build_host (rx_directory_ptr root);

      string_type get_host_manual () const;

      string_type get_host_name ();

      virtual bool supports_ansi () const = 0;


  protected:
	  
      rx_result console_loop (configuration_data_t& config, std::vector<library::rx_plugin_base*>& plugins);

      bool parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config);


  private:


      bool exit_;


};


} // namespace interactive
} // namespace host



#endif
