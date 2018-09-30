

/****************************************************************************
*
*  system\meta\rx_objbase.h
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
#include "system/meta/rx_blocks.h"
// rx_logic
#include "system/logic/rx_logic.h"
// rx_callback
#include "system/callbacks/rx_callback.h"
// rx_classes
#include "system/meta/rx_classes.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace objects {
namespace object_types {
class application_runtime;
class domain_runtime;

} // namespace object_types
} // namespace objects
} // namespace rx_platform


#include "system/meta/rx_obj_classes.h"
#include "system/callbacks/rx_callback.h"
using namespace rx;
using namespace rx_platform::ns;
using rx::values::rx_value;
using rx::values::rx_simple_value;


namespace rx_platform {

namespace objects {

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
	typedef rx_platform::meta::object_defs::object_class definition_t;
	typedef objects::object_types::object_runtime RType;

  public:
      object_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~object_runtime();


      rx_value get_value (const string_type path) const;

      void turn_on ();

      void turn_off ();

      void set_blocked ();

      void set_test ();

      values::rx_value get_value () const;

      namespace_item_attributes get_attributes () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      bool is_browsable () const;

      virtual void get_content (server_items_type& sub_items, const string_type& pattern) const;

      platform_item_ptr get_item_ptr ();

      rx_time get_created_time () const;

      rx_time get_modified_time () const;

      string_type get_name () const;

      size_t get_size () const;

      blocks::complex_runtime_item_ptr get_complex_item ();


      blocks::complex_runtime_item& get_runtime_item ()
      {
        return runtime_item_;
      }


      meta::checkable_data& get_meta_data ()
      {
        return meta_data_;
      }



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


      rx_reference<application_runtime> my_application_;

      rx_reference<domain_runtime> my_domain_;


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

      user_object (const string_type& name, const rx_node_id& id);

      virtual ~user_object();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class server_object : public object_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system object class. basic implementation of a system object");

	DECLARE_REFERENCE_PTR(server_object);

  public:
      server_object (const string_type& name, const rx_node_id& id);

      virtual ~server_object();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class domain_runtime : public object_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system domain class. basic implementation of a domain");

	DECLARE_REFERENCE_PTR(domain_runtime);
	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;
	typedef std::vector<object_runtime::smart_ptr> objects_type;

  public:
      domain_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~domain_runtime();


      string_type get_type_name () const;

      namespace_item_attributes get_attributes () const;


      static string_type type_name;


  protected:
      domain_runtime();


  private:


      objects_type objects_;


};






class port_runtime : public object_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system port class. basic implementation of a port");

	DECLARE_REFERENCE_PTR(port_runtime);

  public:
      port_runtime (const string_type& name, const rx_node_id& id);

      virtual ~port_runtime();


      string_type get_type_name () const;

      namespace_item_attributes get_attributes () const;


      static string_type type_name;


  protected:

  private:
      port_runtime();



};






class application_runtime : public domain_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system application class. contains system default application");

	DECLARE_VIRTUAL_REFERENCE_PTR(application_runtime);
	typedef std::vector<domain_runtime::smart_ptr> domains_type;
	typedef std::vector<port_runtime::smart_ptr> ports_type;

  public:
      application_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~application_runtime();


      string_type get_type_name () const;

      namespace_item_attributes get_attributes () const;


      static string_type type_name;


  protected:

  private:
      application_runtime();



      domains_type domains_;

      ports_type ports_;


};


} // namespace object_types
} // namespace objects
} // namespace rx_platform


namespace rx_platform 
{
namespace objects 
{
typedef reference<object_types::object_runtime> object_runtime_ptr;
typedef reference<object_types::port_runtime> port_runtime_ptr;
typedef reference<object_types::domain_runtime> domain_runtime_ptr;
typedef reference<object_types::application_runtime> application_runtime_ptr;
}
}


#endif
