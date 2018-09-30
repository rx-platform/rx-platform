

/****************************************************************************
*
*  system\hosting\rx_host.h
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


#ifndef rx_host_h
#define rx_host_h 1


#include "system/meta/rx_objbase.h"
#include "system/meta/rx_obj_classes.h"
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



namespace rx {
namespace security
{
typedef rx::pointers::reference<security_context> execute_context_ptr;
}
}


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

      virtual ~host_security_context();


      bool is_system () const;


  protected:

  private:


};






class rx_platform_file : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_file);

	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
file class. basic implementation of a file");

  public:
      rx_platform_file();

      virtual ~rx_platform_file();


      string_type get_type_name () const;

      namespace_item_attributes get_attributes () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool is_browsable () const;

      virtual values::rx_value get_value () const = 0;

      virtual rx_time get_created_time () const = 0;

      virtual string_type get_name () const = 0;

      virtual size_t get_size () const;

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      platform_item_ptr get_item_ptr ();


  protected:

  private:


};






class rx_platform_storage : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_storage);
	typedef std::map<string_type, rx_platform_file::smart_ptr> files_type;

  public:
      rx_platform_storage();

      virtual ~rx_platform_storage();


      virtual void get_storage_info (string_type& info) = 0;

      virtual sys_handle_t get_host_test_file (const string_type& path);

      virtual sys_handle_t get_host_console_script_file (const string_type& path);

      virtual const string_type& get_license () = 0;

      virtual void init_storage ();

      virtual void deinit_storage ();

      virtual void list_storage (const string_type& path, server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern) = 0;


  protected:

  private:


      files_type files_;


};







class rx_platform_host 
{

	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      rx_platform_host (rx_platform_storage::smart_ptr storage);

      virtual ~rx_platform_host();


      virtual void get_host_info (string_array& hosts);

      virtual void server_started_event ();

      virtual void server_stopping_event ();

      virtual bool shutdown (const string_type& msg) = 0;

      virtual void get_host_objects (std::vector<rx_platform::objects::object_runtime_ptr>& items) = 0;

      virtual void get_host_classes (std::vector<rx_platform::meta::object_class_ptr>& items) = 0;

      virtual bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual bool start (const string_array& args) = 0;

      virtual bool break_host (const string_type& msg) = 0;

      int console_main (int argc, char* argv[]);

      virtual string_type get_startup_script ();

      virtual std::vector<ETH_interfaces> get_ETH_interfacesf (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      virtual std::vector<IP_interfaces> get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);


      rx_platform_host * get_parent ()
      {
        return parent_;
      }


      rx_reference<rx_platform_storage> get_storage () const
      {
        return storage_;
      }



  protected:

  private:
      rx_platform_host(const rx_platform_host &right);

      rx_platform_host & operator=(const rx_platform_host &right);



      rx_platform_host *parent_;

      rx_reference<rx_platform_storage> storage_;


};


} // namespace hosting
} // namespace rx_platform



#endif
