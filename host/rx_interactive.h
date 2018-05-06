

/****************************************************************************
*
*  host\rx_interactive.h
*
*  Copyright (c) 2018 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_interactive_h
#define rx_interactive_h 1



#include "system/server/rx_server.h"

// rx_vt100
#include "host/rx_vt100.h"
// rx_host
#include "system/hosting/rx_host.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_security
#include "lib/security/rx_security.h"

namespace host {
namespace interactive {
class interactive_console_host;

} // namespace interactive
} // namespace host


using namespace rx;
using namespace rx_platform;



namespace host {

namespace interactive {





class interactive_security_context : public rx::security::built_in_security_context  
{
	DECLARE_REFERENCE_PTR(interactive_security_context);


  public:
      interactive_security_context();

      virtual ~interactive_security_context();


      bool has_console () const;

      bool is_system () const;

      bool is_interactive () const;


  protected:

  private:


};






class interactive_console_client : public rx_platform::prog::console_client  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(interactive_console_client);


  public:
      interactive_console_client (interactive_console_host* host);

      virtual ~interactive_console_client();


      const string_type& get_console_name ();

      void run_interactive ();

      security::security_context::smart_ptr get_current_security_context ();


  protected:

      void virtual_bind ();

      void virtual_release ();

      void exit_console ();

      void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer);


  private:


      rx_reference<interactive_security_context> security_context_;

      interactive_console_host *host_;

      rx_vt100::vt100_transport vt100_transport_;


      bool exit_;


};







class interactive_console_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      interactive_console_host (rx_platform::hosting::rx_platform_storage::smart_ptr storage);

      virtual ~interactive_console_host();


      void get_host_info (string_array& hosts);

      void server_started_event ();

      bool shutdown (const string_type& msg);

      bool exit () const;

      void get_host_objects (std::vector<rx_platform::objects::object_runtime_ptr>& items);

      void get_host_classes (std::vector<rx_platform::meta::object_class_ptr>& items);

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);

      virtual bool is_canceling () const = 0;

      virtual bool read_stdin (std::array<char,0x100>& chars, size_t& count) = 0;

      virtual bool write_stdout (const void* data, size_t size) = 0;

      bool write_stdout (const string_type& lines);


  protected:

      void console_loop (configuration_data_t& config);


  private:


      bool exit_;


};


} // namespace interactive
} // namespace host



#endif
