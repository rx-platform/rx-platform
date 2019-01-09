

/****************************************************************************
*
*  system\runtime\rx_objbase.h
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


#ifndef rx_objbase_h
#define rx_objbase_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_logic
#include "system/logic/rx_logic.h"
// rx_callback
#include "system/callbacks/rx_callback.h"
// rx_checkable
#include "system/meta/rx_checkable.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace runtime {
namespace object_types {
class domain_runtime;
class application_runtime;

} // namespace object_types
} // namespace runtime
} // namespace rx_platform


#include "system/callbacks/rx_callback.h"
using namespace rx;
using namespace rx_platform::ns;
using rx::values::rx_value;
using rx::values::rx_simple_value;


namespace rx_platform {

typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
typedef std::stack<buffer_ptr, std::vector<buffer_ptr> > buffers_type;

namespace runtime {
typedef rx_reference<object_types::domain_runtime> rx_domain_ptr;
typedef rx_reference<object_types::domain_runtime> rx_port_ptr;
typedef rx_reference<object_types::domain_runtime> rx_object_ptr;
typedef rx_reference<object_types::application_runtime> rx_application_ptr;





struct application_creation_data 
{


      string_type name;

      rx_node_id id;

      rx_node_id type_id;

      bool system;

  public:

  protected:

  private:


};






struct domain_creation_data 
{


      string_type name;

      rx_node_id id;

      rx_node_id type_id;

      bool system;

      rx_application_ptr application;

  public:

  protected:

  private:


};






struct port_creation_data 
{


      string_type name;

      rx_node_id id;

      rx_node_id type_id;

      rx_application_ptr application;

  public:

  protected:

  private:


};






struct object_creation_data 
{


      string_type name;

      rx_node_id id;

      rx_node_id type_id;

      bool system;

      rx_application_ptr application;

      rx_domain_ptr domain;

  public:

  protected:

  private:


};


namespace object_types {






class object_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 2, 0, "\
object class. basic implementation of an object");

	DECLARE_REFERENCE_PTR(object_runtime);

	//typedef std::vector<runtime_item::smart_ptr> items_order_type;
	typedef std::map<string_type, size_t> items_cache_type;
	typedef blocks::complex_runtime_item_ptr items_type;
	typedef std::vector<logic::program_runtime_ptr> programs_type;

	friend class meta::checkable_data;

  public:
      object_runtime (object_creation_data&& data);

      virtual ~object_runtime();


      rx_value get_value (const string_type path) const;

      void turn_on ();

      void turn_off ();

      void set_blocked ();

      void set_test ();

      values::rx_value get_value () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual bool connect_domain (rx_domain_ptr&& domain);

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      bool is_browsable () const;

      virtual void get_content (server_items_type& sub_items, const string_type& pattern) const;

      platform_item_ptr get_item_ptr ();

      rx_time get_created_time () const;

      rx_time get_modified_time () const;

      string_type get_name () const;

      size_t get_size () const;

      blocks::complex_runtime_item& get_complex_item ();

      meta::checkable_data& meta_data ();

      virtual rx_thread_handle_t get_executer () const;

      virtual bool connect_application (rx_application_ptr&& app);


      blocks::complex_runtime_item& get_runtime_item ()
      {
        return runtime_item_;
      }


      const meta::checkable_data& meta_data () const;


      const rx_mode_type& get_mode () const
      {
        return mode_;
      }


      static string_type get_type_name ()
      {
        return type_name;
      }


      const rx_time get_change_time () const
      {
        return change_time_;
      }



      static string_type type_name;


  protected:
      object_runtime();


      bool init_object ();


      rx_application_ptr my_application_;

      rx_domain_ptr my_domain_;


  private:


      blocks::complex_runtime_item runtime_item_;

      programs_type programs_;

      meta::checkable_data meta_data_;


      rx_mode_type mode_;

      rx_time change_time_;


};






class user_object : public object_runtime  
{
	DECLARE_CODE_INFO("rx", 0,1,0, "\
user object class. basic implementation of a user object");

	DECLARE_REFERENCE_PTR(user_object);

  public:
      user_object();

      user_object (object_creation_data&& data);

      virtual ~user_object();


  protected:

  private:


};






class server_object : public object_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system object class. basic implementation of a system object");

	DECLARE_REFERENCE_PTR(server_object);

  public:
      server_object (object_creation_data&& data);

      virtual ~server_object();


  protected:

  private:


};







class domain_runtime : public object_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system domain class. basic implementation of a domain");

	DECLARE_REFERENCE_PTR(domain_runtime);
	typedef std::vector<object_runtime::smart_ptr> objects_type;

public:

  public:
      domain_runtime (domain_creation_data&& data);

      virtual ~domain_runtime();


      string_type get_type_name () const;

      rx_thread_handle_t get_executer () const;

      bool connect_domain (rx_domain_ptr&& domain);

      platform_item_ptr get_item_ptr ();


      static string_type type_name;


  protected:
      domain_runtime();


  private:


      objects_type objects_;


      rx_thread_handle_t executer_;


};







class port_runtime : public object_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system port class. basic implementation of a port");

	DECLARE_REFERENCE_PTR(port_runtime);

	friend class meta::checkable_data;

  public:
      port_runtime (port_creation_data&& data);

      virtual ~port_runtime();


      string_type get_type_name () const;

      bool write (buffer_ptr what);

      platform_item_ptr get_item_ptr ();


      static string_type type_name;


  protected:

      virtual bool readed (buffer_ptr what, rx_thread_handle_t destination);


  private:
      port_runtime();



};







class application_runtime : public domain_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system application class. contains system default application");

	DECLARE_REFERENCE_PTR(application_runtime);
	typedef std::vector<domain_runtime::smart_ptr> domains_type;
	typedef std::vector<port_runtime::smart_ptr> ports_type;

	friend class meta::checkable_data;

  public:
      application_runtime (application_creation_data&& data);

      virtual ~application_runtime();


      string_type get_type_name () const;

      bool connect_application (rx_application_ptr&& app);

      bool connect_domain (rx_domain_ptr&& domain);

      platform_item_ptr get_item_ptr ();


      static string_type type_name;


  protected:

  private:
      application_runtime();



      domains_type domains_;

      ports_type ports_;


};


} // namespace object_types
} // namespace runtime
} // namespace rx_platform



#endif
