

/****************************************************************************
*
*  system\hosting\rx_host.h
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


#ifndef rx_host_h
#define rx_host_h 1


#include "system/rx_platform_typedefs.h"
#include "system/meta/rx_obj_types.h"
#include "system/storage_base/rx_storage.h"


// adding command line parsing library
// see <https://github.com/jarro2783/cxxopts>
#include "third-party/cxxopts/include/cxxopts.hpp"


namespace rx_platform
{
struct configuration_data_t;
}
/////////////////////////////////////////////////////////////
// logging macros for host library
#define HOST_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Host",src,lvl,msg)
#define HOST_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Host",src,lvl,msg)
#define HOST_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Host",src,lvl,msg)
#define HOST_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Host",src,lvl,msg)
#define HOST_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Host",src,lvl,msg)
#define HOST_LOG_TRACE(src,lvl,msg) RX_TRACE("Host",src,lvl,msg)


#define SAFE_ANSI_STATUS_ERROR (supports_ansi() ? ANSI_STATUS_ERROR : "ERROR")
#define SAFE_ANSI_STATUS_OK (supports_ansi() ? ANSI_STATUS_OK : "OK")


// rx_log
#include "system/server/rx_log.h"

namespace rx_platform {
namespace library {
class rx_plugin_base;
} // namespace library

namespace security {
class security_context;

} // namespace security
} // namespace rx_platform




namespace rx_platform {
template<typename typeT>
rx_result register_host_constructor(const rx_node_id& id, std::function<typename typeT::RImplPtr()> f);
template<typename typeT>
rx_result register_host_simple_constructor(const rx_node_id& id, std::function<typename typeT::RTypePtr()> f);

template<typename typeT>
rx_result register_host_type(typename typeT::smart_ptr what);
template<typename typeT>
rx_result register_host_simple_type(typename typeT::smart_ptr what);

rx_result register_host_relation_type(relation_type_ptr what);
rx_result register_host_data_type(data_type_ptr what);

template<typename typeT>
rx_result register_host_runtime(const typename typeT::instance_data_t& instance_data, const data::runtime_values_data* data);

namespace storage_base
{
class rx_platform_storage_type;
}

namespace hosting {
///////////////////////////////////////////////////////////////////////////////////////////////
// IP addresses
int rx_add_ip_address(uint32_t addr, uint32_t mask, int itf, ip_addr_ctx_t* ctx);
int rx_remove_ip_address(ip_addr_ctx_t ctx);
int rx_is_valid_ip_address(uint32_t addr, uint32_t mask);





class configuration_reader 
{

  public:

      virtual rx_result parse_configuration (const string_type& input_data, std::map<string_type, string_type>& config_values) = 0;


  protected:

  private:


};






class host_platform_builder 
{

  public:

      string_type host_root;


  protected:

  private:


};






class startup_log_subscriber : public log::log_subscriber  
{
    typedef std::vector<log::log_event_data> pending_events_type;

  public:

      void log_event (log::log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when);

      rx_result read_log (const log::log_query_type& query, log::log_events_type& result);

      string_type get_name () const;

      void started ();


  protected:

  private:


      pending_events_type pending_events_;

      locks::slim_lock pending_lock_;

      bool started_;


};




struct configured_storage_t
{
    string_type storage_id;
    string_type reference;
    string_type name;
    rx_storage_ptr connection;
};

struct rx_host_storages
{
	std::map<string_type, storage_base::rx_platform_storage_type*> storage_types;
    std::map < string_type, storage_base::rx_storage_connection::smart_ptr> registered_connections;
    std::vector<configured_storage_t> configured_storages;
};


typedef cxxopts::Options command_line_options_t;
struct rx_host_directories
{
	// configuration files locations
	string_type system_config;
	string_type user_config;
	string_type local_folder;
    string_type copyright_file;
    string_type license_file;
	// storage files locations
	string_type system_storage;
	string_type user_storage;
	// manual files location
	string_type manuals;
	// log files location
	string_type logs;
    // web files location
    string_type http;

	void fill_missing(rx_host_directories&& from)
	{
		if (system_config.empty())
			system_config = std::move(from.system_config);
		if (user_config.empty())
			user_config = std::move(from.user_config);
		if (local_folder.empty())
			local_folder = std::move(from.local_folder);

		if (system_storage.empty())
			system_storage = std::move(from.system_storage);
		if (user_storage.empty())
			user_storage = std::move(from.user_storage);

		if (manuals.empty())
			manuals = std::move(from.manuals);

        if (http.empty())
            http = std::move(from.http);

		if (logs.empty())
			logs = std::move(from.logs);
	}
};
using hosts_type = string_array;







class rx_platform_host 
{
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      rx_platform_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      virtual ~rx_platform_host();


      virtual void get_host_info (hosts_type& hosts);

      virtual void server_started_event ();

      virtual void server_stopping_event ();

      virtual bool shutdown (const string_type& msg) = 0;

      virtual bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual bool break_host (const string_type& msg) = 0;

      virtual string_type get_default_user_storage () const;

      virtual bool is_canceling () const = 0;

      virtual bool exit () const = 0;

      virtual rx_result build_host (host_platform_builder& builder) = 0;

      string_type get_manual (string_type what) const;

      virtual string_type get_host_manual () const = 0;

      virtual string_type get_host_name () = 0;

      void print_offline_manual (const string_type& host, const rx_host_directories& dirs);

      rx_result_with<rx_storage_ptr> get_system_storage (const string_type& name);

      rx_result_with<rx_storage_ptr> get_user_storage (const string_type& name = "");

      rx_result_with<rx_storage_ptr> get_test_storage (const string_type& name = "");

      static void dump_log_items (const log::log_events_type& items, std::ostream& out);

      virtual string_type get_full_path (const string_type& path) = 0;

      virtual bool supports_ansi () const = 0;

      void dump_storage_references (std::ostream& out);

      const string_type& get_license ();

      const string_type& get_copyright ();

      void host_started ();

      std::vector<std::map<string_type, string_type> > read_config_files (const string_type& file_name);

      virtual void fill_plugin_libs (string_array& paths);

      std::vector<std::pair<string_type, rx_storage_ptr> > get_configured_storages ();

      rx_result register_storage_type (const string_type& prefix, storage_base::rx_platform_storage_type* what);

      virtual string_type get_default_log_directory () const;


      rx_platform_host * get_parent ()
      {
        return parent_;
      }



      const rx_host_storages& get_storages () const
      {
        return storages_;
      }



  protected:

      rx_result parse_config_files (rx_platform::configuration_data_t& config);

      virtual void read_config_options (const std::map<string_type, string_type>& options, rx_platform::configuration_data_t& config);

      bool parse_command_line (int argc, char* argv[], const char* help_name, rx_platform::configuration_data_t& config);

      virtual void add_command_line_options (command_line_options_t& options, rx_platform::configuration_data_t& config);

      rx_result initialize_storages (rx_platform::configuration_data_t& config, const std::vector<library::rx_plugin_base*>& plugins);

      void deinitialize_storages ();

      rx_result register_plugins (std::vector<library::rx_plugin_base*>& plugins);

      virtual rx_result fill_host_directories (rx_host_directories& data) = 0;

      void dump_startup_log (std::ostream& out);

      rx_result start_log (rx_platform::configuration_data_t& config);


  private:
      rx_platform_host(const rx_platform_host &right);

      rx_platform_host & operator=(const rx_platform_host &right);


      rx_result init_storage (const string_type& name, const string_type& full_reference);



      rx_platform_host *parent_;

      rx_reference<startup_log_subscriber> startup_log_;


      string_type manuals_path_;

      rx_host_storages storages_;

      string_type copyright_cache_;

      string_type lic_path_;

      rx_host_directories host_directories_;


};


} // namespace hosting
} // namespace rx_platform



#endif
