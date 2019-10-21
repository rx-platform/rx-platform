

/****************************************************************************
*
*  system\hosting\rx_host.h
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


#ifndef rx_host_h
#define rx_host_h 1


#include "system/runtime/rx_objbase.h"
#include "system/meta/rx_obj_types.h"
#include "system/libraries/rx_plugin.h"



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

// rx_storage
#include "system/storage_base/rx_storage.h"
// rx_security
#include "lib/security/rx_security.h"





namespace rx_platform {

namespace hosting {
///////////////////////////////////////////////////////////////////////////////////////////////
// IP addresses
int rx_add_ip_address(uint32_t addr, uint32_t mask, int itf, ip_addr_ctx_t* ctx);
int rx_remove_ip_address(ip_addr_ctx_t ctx);
int rx_is_valid_ip_address(uint32_t addr, uint32_t mask);





class host_security_context : public rx::security::built_in_security_context  
{
	DECLARE_REFERENCE_PTR(host_security_context);

  public:
      host_security_context();

      ~host_security_context();


      bool is_system () const;


  protected:

  private:


};






class configuration_reader 
{

  public:

      virtual rx_result parse_configuration (const string_type& input_data, std::map<string_type, string_type>& config_values) = 0;


  protected:

  private:


};


struct rx_host_storages
{
	storage_base::rx_platform_storage::smart_ptr system_storage;
	storage_base::rx_platform_storage::smart_ptr user_storage;
	storage_base::rx_platform_storage::smart_ptr test_storage;
};
typedef cxxopts::Options command_line_options_t;





class rx_platform_host 
{

	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      rx_platform_host (rx_host_storages& storage);

      virtual ~rx_platform_host();


      virtual void get_host_info (string_array& hosts);

      virtual void server_started_event ();

      virtual void server_stopping_event ();

      virtual bool shutdown (const string_type& msg) = 0;

      virtual bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual bool break_host (const string_type& msg) = 0;

      virtual std::vector<ETH_interface> get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual std::vector<IP_interface> get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual string_type get_config_path () const = 0;

      virtual string_type get_default_name () const = 0;

      virtual string_type defualt_system_storage_reference () const;

      virtual bool is_canceling () const = 0;

      virtual bool read_stdin (std::array<char,0x100>& chars, size_t& count) = 0;

      virtual bool write_stdout (const void* data, size_t size) = 0;

      bool write_stdout (const string_type& lines);

      virtual bool exit () const = 0;

      virtual void add_command_line_options (command_line_options_t& options, rx_platform::configuration_data_t& config);

      virtual rx_result build_host (rx_directory_ptr root) = 0;

      virtual storage_base::rx_platform_storage::smart_ptr get_storage () = 0;

      static string_type get_manual (string_type what);

      virtual string_type get_host_manual () const = 0;

      static string_type get_manual_explicit (string_type what, string_type man_folder);


      rx_platform_host * get_parent ()
      {
        return parent_;
      }


      rx_reference<storage_base::rx_platform_storage> get_system_storage () const
      {
        return system_storage_;
      }


      rx_reference<storage_base::rx_platform_storage> get_user_storage () const
      {
        return user_storage_;
      }


      rx_reference<storage_base::rx_platform_storage> get_test_storage () const
      {
        return test_storage_;
      }



  protected:

      rx_result read_config_file (configuration_reader& reader, rx_platform::configuration_data_t& config);

      rx_result initialize_storages (rx_platform::configuration_data_t& config);

      rx_result deinitialize_storages ();

      rx_result register_plugins (std::vector<library::rx_plugin_base*>& plugins);

      virtual string_type get_default_manual_path () const = 0;


  private:
      rx_platform_host(const rx_platform_host &right);

      rx_platform_host & operator=(const rx_platform_host &right);



      rx_platform_host *parent_;

      rx_reference<storage_base::rx_platform_storage> system_storage_;

      rx_reference<storage_base::rx_platform_storage> user_storage_;

      rx_reference<storage_base::rx_platform_storage> test_storage_;


      static string_type manuals_path_;


};


} // namespace hosting
} // namespace rx_platform



#endif
