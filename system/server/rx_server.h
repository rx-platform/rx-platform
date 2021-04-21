

/****************************************************************************
*
*  system\server\rx_server.h
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


#ifndef rx_server_h
#define rx_server_h 1



#define RX_DOMAIN_EXTERN 0xfffb
#define RX_DOMAIN_META 0xfffc
#define RX_DOMAIN_SLOW 0xfffd
#define RX_DOMAIN_IO 0xfffe
#define RX_DOMAIN_UNASSIGNED 0xffff

#include "version/rx_version.h"

#include "os_itf/rx_ositf.h"
#include "system/rx_platform_version.h"
#include "lib/rx_lib.h"
#include "rx_configuration.h"
#include "system/libraries/rx_plugin.h"
#include "lib/security/rx_security.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_ns
#include "system/server/rx_ns.h"

namespace rx_platform {
namespace prog {
class server_script_host;
} // namespace prog
} // namespace rx_platform

namespace rx_internal {
namespace interfaces {
namespace io_endpoints {
class rx_io_manager;

} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal



#include "lib/rx_log.h"
using namespace rx;


namespace rx_platform {


struct io_manager_data_t
{
	uint32_t io_timer_period = 200;
};

struct general_data_t
{
	string_type manuals_path;
};
struct runtime_data_t
{
    bool real_time = false;
    bool no_hd_timer = true;
    int io_pool_size = -1;
    bool has_unassigned_pool = true;
    int workers_pool_size = -1;
    int slow_pool_size = -1;
    bool has_calculation_timer = false;
    threads::job_thread* extern_executer = nullptr;
};

struct management_data_t
{
    string_type telnet_addr = "127.0.0.1";
    uint16_t telnet_port = 0;
    string_type logs_directory;
    bool test_log = false;
    string_type startup_script;
};

struct meta_configuration_data_t
{
    bool build_system_from_code = false;
    string_type instance_name;
    uint32_t wd_timer_period = 1000;
};


struct configuration_data_t
{
	runtime_data_t processor;
	management_data_t management;
	ns::namespace_data_t storage;
	meta_configuration_data_t meta_configuration;
	io_manager_data_t io;
	general_data_t other;
};

enum class rx_platform_status
{
	initializing,
	starting,
	running,
	stopping,
	deinitializing
};





class rx_gate 
{
	typedef std::map<string_type,prog::server_script_host*> scripts_type;

  public:

      static rx_gate& instance ();

      void cleanup ();

      rx_result_with<security::security_context_ptr> initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result deinitialize (security::security_context_ptr sec_ctx);

      rx_result start (hosting::rx_platform_host* host, const configuration_data_t& data);

      rx_result stop ();

      rx_directory_ptr get_root_directory ();

      bool shutdown (const string_type& msg);

      bool read_log (const string_type& log, const log::log_query_type& query, std::function<void(rx_result_with<log::log_events_type>&&)> callback);

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      runtime_data_t get_cpu_data ();


      hosting::rx_platform_host * get_host ()
      {
        return host_;
      }



      rx_time get_started () const
      {
        return started_;
      }


      const string_type& get_os_info () const
      {
        return os_info_;
      }


      const string_type& get_rx_version () const
      {
        return rx_version_;
      }


      const string_type& get_lib_version () const
      {
        return lib_version_;
      }


      const string_type& get_rx_name () const
      {
        return rx_name_;
      }


      const string_type& get_comp_version () const
      {
        return comp_version_;
      }


      const string_type& get_hal_version () const
      {
        return hal_version_;
      }


      rx_pid_t get_pid () const
      {
        return pid_;
      }


      const bool is_shutting_down () const
      {
        return shutting_down_;
      }


      rx_platform_status get_platform_status () const
      {
        return platform_status_;
      }



	  template <class typeT>
	  rx_result register_constructor(const rx_node_id& id, std::function<typename typeT::RImplPtr()> f);
  protected:

  private:
      rx_gate();

      ~rx_gate();

      template <class typeT>
      rx_result register_constructor_internal(const rx_node_id& id, std::function<typename typeT::RImplPtr()> f);

      rx_directory_ptr root_;

      hosting::rx_platform_host *host_;

      scripts_type scripts_;

      std::unique_ptr<rx_internal::interfaces::io_endpoints::rx_io_manager> io_manager_;


      static rx_gate* g_instance;

      rx_time started_;

      string_type os_info_;

      string_type rx_version_;

      string_type lib_version_;

      string_type rx_name_;

      string_type comp_version_;

      string_type hal_version_;

      rx_pid_t pid_;

      security::security_guard_ptr security_guard_;

      bool shutting_down_;

      rx_platform_status platform_status_;


};


} // namespace rx_platform



#endif
