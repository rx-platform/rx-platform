

/****************************************************************************
*
*  system\server\rx_server.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef rx_server_h
#define rx_server_h 1




#include "version/rx_version.h"

#include "os_itf/rx_ositf.h"
#include "system/rx_platform_version.h"
#include "lib/rx_lib.h"
#include "rx_configuration.h"

// rx_host
#include "host/rx_host.h"
// rx_security
#include "lib/security/rx_security.h"
// rx_mngt
#include "system/server/rx_mngt.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_inf
#include "system/server/rx_inf.h"


#include "lib/rx_log.h"

#define ANSI_COLOR_BOLD	   "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_CLS "\x1b[2J"
#define ANSI_CUR_HOME "\033[0;0H"

#define RX_OBJECT_DELIMETER '.'
using namespace rx;


namespace server {

struct configuration_data_t
{
	runtime::runtime_data_t runtime_data;
	mngt::managment_data_t managment_data;
	ns::namespace_data_t namespace_data;
};




class rx_server : public rx::security::secured_object  
{
	typedef std::map<string_type,prog::server_script_host*> scripts_type;

  public:

      static rx_server& instance ();

      void cleanup ();

      dword initialize (host::rx_server_host* host, configuration_data_t& data);

      dword deinitialize ();

      dword start (host::rx_server_host* host, const configuration_data_t& data);

      dword stop ();

      server_directory_ptr get_root_directory ();

      bool shutdown (const string_type& msg, std::ostream& err);

      bool read_log (const log::log_query_type& query, log::log_events_type& result);

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);


      runtime::server_rt& get_runtime ()
      {
        return _runtime;
      }


      mngt::server_manager& get_manager ()
      {
        return _manager;
      }



      const rx_time get_started () const
      {
        return _started;
      }


      const string_type& get_os_info () const
      {
        return _os_info;
      }


      const string_type& get_rx_version () const
      {
        return _rx_version;
      }


      const string_type& get_lib_version () const
      {
        return _lib_version;
      }


      const string_type& get_host_info () const
      {
        return _host_info;
      }


      const string_type& get_rx_name () const
      {
        return _rx_name;
      }


      const string_type& get_comp_version () const
      {
        return _comp_version;
      }


      const string_type& get_hal_version () const
      {
        return _hal_version;
      }


      const rx_pid_t get_pid () const
      {
        return _pid;
      }



  protected:

      void interface_bind ();

      void interface_release ();


  private:
      rx_server();

      virtual ~rx_server();



      runtime::server_rt _runtime;

      ns::rx_server_directory::smart_ptr _root;

      mngt::server_manager _manager;

      host::rx_server_host *_host;

      scripts_type _scripts;


      static rx_server* g_instance;

      rx_time _started;

      string_type _os_info;

      string_type _rx_version;

      string_type _lib_version;

      string_type _host_info;

      string_type _rx_name;

      string_type _comp_version;

      string_type _hal_version;

      rx_pid_t _pid;


};


} // namespace server



#endif
