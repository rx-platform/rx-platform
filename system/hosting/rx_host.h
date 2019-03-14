

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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_host_h
#define rx_host_h 1


#include "system/runtime/rx_objbase.h"
#include "system/meta/rx_obj_types.h"
namespace rx_platform
{
struct configuration_data_t;
}
/////////////////////////////////////////////////////////////
// logging macros for host library
#define HOST_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Host",src,lvl,msg)
#define HOST_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Host",src,lvl,msg)
#define HOST_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Host",src,lvl,msg)
#define HOST_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Host",src,lvl,msg)
#define HOST_LOG_TRACE(src,lvl,msg) RX_TRACE("Host",src,lvl,msg)

// rx_security
#include "lib/security/rx_security.h"
// rx_ptr
#include "lib/rx_ptr.h"





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


class rx_storage_item;
typedef std::unique_ptr<rx_storage_item> rx_storage_item_ptr;




class rx_storage_item 
{

  public:
      rx_storage_item (const string_type& path);

      virtual ~rx_storage_item();


      virtual rx_result open_for_read () = 0;

      virtual rx_result open_for_write () = 0;

      virtual rx_result close () = 0;

      virtual base_meta_reader& read_stream () = 0;

      virtual base_meta_writer& write_stream () = 0;

      virtual rx_result delete_item () = 0;


      const string_type& get_path () const
      {
        return path_;
      }


	  rx_storage_item() = delete;
	  rx_storage_item(const rx_storage_item&) = delete;
	  rx_storage_item(rx_storage_item&&) = delete;
	  rx_storage_item& operator=(const rx_storage_item&) = delete;
	  rx_storage_item& operator=(rx_storage_item&&) = delete;
  protected:

  private:


      string_type path_;


};







class rx_platform_storage : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_storage);

  public:
      rx_platform_storage();

      ~rx_platform_storage();


      virtual void get_storage_info (string_type& info) = 0;

      virtual rx_result init_storage (const string_type& storage_reference);

      virtual rx_result deinit_storage ();

      virtual rx_result list_storage (std::vector<rx_storage_item_ptr>& items) = 0;

      virtual void get_storage_reference (string_type& ref) = 0;


  protected:

  private:


};






class configuration_reader 
{

  public:

      virtual rx_result parse_configuration (const string_type& input_data, configuration_data_t& config) = 0;


  protected:

  private:


};


struct rx_host_storages
{
	rx_platform_storage::smart_ptr system_storage;
	rx_platform_storage::smart_ptr user_storage;
	rx_platform_storage::smart_ptr test_storage;
};





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

      virtual void get_host_objects (std::vector<rx_platform::runtime::object_runtime_ptr>& items) = 0;

      virtual void get_host_types (std::vector<rx_platform::meta::object_type_ptr>& items) = 0;

      virtual bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual bool break_host (const string_type& msg) = 0;

      virtual std::vector<ETH_interface> get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual std::vector<IP_interface> get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual string_type get_config_path () const = 0;

      virtual string_type get_default_name () const = 0;


      rx_platform_host * get_parent ()
      {
        return parent_;
      }


      rx_reference<rx_platform_storage> get_system_storage () const
      {
        return system_storage_;
      }


      rx_reference<rx_platform_storage> get_user_storage () const
      {
        return user_storage_;
      }


      rx_reference<rx_platform_storage> get_test_storage () const
      {
        return test_storage_;
      }



  protected:

      bool read_config_file (configuration_reader& reader, rx_platform::configuration_data_t& config);


  private:
      rx_platform_host(const rx_platform_host &right);

      rx_platform_host & operator=(const rx_platform_host &right);



      rx_platform_host *parent_;

      rx_reference<rx_platform_storage> system_storage_;

      rx_reference<rx_platform_storage> user_storage_;

      rx_reference<rx_platform_storage> test_storage_;


};


} // namespace hosting
} // namespace rx_platform



#endif
