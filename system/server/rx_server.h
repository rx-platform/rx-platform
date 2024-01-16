

/****************************************************************************
*
*  system\server\rx_server.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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



#define RX_DOMAIN_PYTHON 0xfffa
#define RX_DOMAIN_EXTERN 0xfffb
#define RX_DOMAIN_META 0xfffc
#define RX_DOMAIN_SLOW 0xfffd
#define RX_DOMAIN_IO 0xfffe
#define RX_DOMAIN_UNASSIGNED 0xffff

#include "version/rx_version.h"

#include "os_itf/rx_ositf.h"
#include "lib/rx_lib.h"
#include "rx_configuration.h"
#include "security/rx_security.h"
#include "rx_ns.h"

// rx_host
#include "system/hosting/rx_host.h"

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



#include "system/server/rx_log.h"
using namespace rx;


namespace rx_platform {


struct io_manager_data_t
{
	uint32_t io_timer_period = 200;
};


struct general_data_t
{
	string_type manuals_path;
    string_type http_path;
    uint16_t http_port;
    uint16_t opcua_port;
    uint16_t rx_port;
    string_type rx_security;
    string_type py_path;
    std::map<string_type, string_type> certificates;
};
struct runtime_data_t
{
    bool real_time = false;
    bool no_hd_timer = false;
    int io_pool_size = -1;
    bool has_unassigned_pool = true;
    int workers_pool_size = -1;
    int slow_pool_size = -1;
    bool has_calculation_timer = false;
    threads::job_thread* extern_executer = nullptr;
};

struct management_data_t
{
    bool debug = false;
    uint32_t wd_timer_period = 1000;
    string_type telnet_addr = "127.0.0.1";
    uint16_t telnet_port = 0;
    string_type startup_script;
};

struct meta_configuration_data_t
{
    bool build_system_from_code = false;
    string_type instance_name;
    string_type plugin;
};


struct namespace_data_t
{
    string_type system_storage_reference;
    string_type user_storage_reference;
    string_type test_storage_reference;
};

struct log_data_t
{
    string_type directory;
    int log_level = -1;
    uint32_t cache_size = 0;
    bool test_log = false;
};

struct configuration_data_t
{
	runtime_data_t processor;
	management_data_t management;
	namespace_data_t storage;
	meta_configuration_data_t meta_configuration;
	io_manager_data_t io;
	general_data_t other;
    log_data_t log;
    std::map<string_type, string_type> user_storages;
    std::map<string_type, string_type> system_storages;
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

      void set_host (hosting::rx_platform_host* host);

      void cleanup ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const configuration_data_t& data);

      rx_result stop ();

      rx_directory_ptr get_directory (const string_type& path, ns::rx_directory_resolver* dirs = nullptr);

      ns::rx_namespace_item get_namespace_item (const string_type& path, ns::rx_directory_resolver* dirs = nullptr);

      rx_result_with<rx_directory_ptr> add_directory (const string_type& path);

      bool shutdown (const string_type& msg);

      bool read_log (const string_type& log, const log::log_query_type& query, std::function<void(rx_result_with<log::log_events_type>&&)> callback);

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      runtime_data_t get_cpu_data ();

      string_type resolve_ip4_alias (const string_type& what) const;

      string_type resolve_ip6_alias (const string_type& what) const;

      string_type resolve_serial_alias (const string_type& what) const;

      string_type resolve_ethernet_alias (const string_type& what) const;


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


      const string_type& get_instance_name () const
      {
        return rx_name_;
      }


      const string_type& get_node_name () const
      {
        return node_name_;
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


      const configuration_data_t& get_configuration () const
      {
        return configuration_;
      }


      const string_type& get_abi_version () const
      {
        return abi_version_;
      }


      const string_type& get_common_version () const
      {
        return common_version_;
      }



  protected:

  private:
      rx_gate();

      ~rx_gate();



      hosting::rx_platform_host *host_;

      scripts_type scripts_;

      std::unique_ptr<rx_internal::interfaces::io_endpoints::rx_io_manager> io_manager_;


      static rx_gate* g_instance;

      rx_time started_;

      string_type os_info_;

      string_type rx_version_;

      string_type lib_version_;

      string_type rx_name_;

      string_type node_name_;

      string_type comp_version_;

      string_type hal_version_;

      rx_pid_t pid_;

      security::security_guard_ptr security_guard_;

      bool shutting_down_;

      rx_platform_status platform_status_;

      configuration_data_t configuration_;

      string_type abi_version_;

      string_type common_version_;


};


} // namespace rx_platform



#endif
