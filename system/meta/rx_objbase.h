

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



// rx_ptr
#include "lib/rx_ptr.h"
// rx_values
#include "lib/rx_values.h"
// rx_logic
#include "system/logic/rx_logic.h"
// rx_callback
#include "system/callbacks/rx_callback.h"
// rx_classes
#include "system/meta/rx_classes.h"

namespace rx_platform {
namespace objects {
class application_runtime;
class domain_runtime;
class object_runtime;
class complex_runtime_item;

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
extern const char* g_const_simple_class_name;
typedef uint32_t runtime_item_id_t;
class variable_runtime;

typedef callback::callback_functor_container<locks::lockable,rx::values::rx_value> value_callback_t;
class complex_runtime_item;
typedef pointers::reference<object_runtime> object_runtime_ptr;
typedef complex_runtime_item* complex_runtime_item_ptr;
typedef pointers::reference<domain_runtime> domain_runtime_ptr;
typedef pointers::reference<application_runtime> application_runtime_ptr;
typedef struct_runtime* struct_runtime_ptr;

struct object_state_data
{
	rx_time ts;
	rx_mode_type mode;
};






class value_item 
{
public:
	value_item(const value_item& right) = default;
	value_item(value_item&& right) = default;
	~value_item() = default;

	template<typename T>
	value_item(const T& t)
		: change_time_(rx_time::now()),
		readonly_(false),
		storage_(t)
	{
	}


  public:

      bool serialize_definition (base_meta_writter& stream, uint8_t type, const object_state_data& data) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type, const rx_mode_type& mode);

      rx_value get_value (const object_state_data& data) const;


      const rx_time get_change_time () const
      {
        return change_time_;
      }


      bool is_readonly () const
      {
        return readonly_;
      }



  protected:

  private:


      rx::values::rx_simple_value storage_;


      static const uint32_t type_id_;

      rx_time change_time_;

      bool readonly_;


};






class filter_runtime 
{
  public:
	typedef std::unique_ptr<filter_runtime> smart_ptr;

  public:
      filter_runtime();

      virtual ~filter_runtime();


  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


};






class source 
{
  public:
	typedef std::unique_ptr<source> smart_ptr;

  public:
      source();

      virtual ~source();


  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


};






class mapper 
{

  public:
      mapper();

      virtual ~mapper();


  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


};







class variable_runtime 
{
	typedef std::vector<filter_runtime::smart_ptr> filters_type;
	typedef std::vector<source::smart_ptr> sources_type;


	friend class meta::complex_data_type;

public:
	typedef rx_platform::meta::variable_class definition_t;
	typedef objects::variable_runtime RType;

  public:
      variable_runtime();

      variable_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~variable_runtime();


      static string_type type_name;


  protected:

  private:


      filters_type filters_;

      sources_type input_sources_;

      sources_type output_sources_;

      std::unique_ptr<complex_runtime_item> my_item_;


};






class struct_runtime 
{

	friend class meta::complex_data_type;

public:
	typedef rx_platform::meta::struct_class definition_t;
	typedef objects::struct_runtime RType;
	typedef objects::struct_runtime* RTypePtr;

  public:
      struct_runtime();

      struct_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~struct_runtime();


      bool serialize_definition (base_meta_writter& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type type_name;


  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


};







class const_value_item 
{
  public:
	const_value_item(const const_value_item& right) = default;
	const_value_item(const_value_item&& right) = default;
	const_value_item() = default;
	~const_value_item() = default;

	template<typename T>
	const_value_item(const T& t)
	: storage_(t)
	{
	}

  public:

      bool serialize_definition (base_meta_writter& stream, uint8_t type, const object_state_data& data) const;

      bool deserialize_definition (base_meta_writter& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode);

      bool has_own_time () const;

      namespace_item_attributes get_attributes () const;

      void item_lock ();

      void item_unlock ();

      void item_lock () const;

      void item_unlock () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      rx_value get_value (const object_state_data& data) const;

      uint32_t register_value (const string_type& name, const rx_value& val);


  protected:

  private:


      rx::values::rx_simple_value storage_;


      static const uint32_t type_id_;


};


#define RT_CONST_IDX_MASK	0xc0000000
#define RT_VALUE_IDX_MASK	0x80000000
#define RT_COMPLEX_IDX_MASK 0x40000000
#define RT_TYPE_MASK		0xc0000000
#define RT_INDEX_MASK		0x0000ffff
#define RT_CALLBACK_MASK	0x3fff0000
#define RT_CALLBACK_INDEX(idx)	(((idx>>16)||0x3fff)-1)




class complex_runtime_item 
{
public:
	typedef std::unique_ptr<complex_runtime_item > smart_ptr;

	typedef std::vector<smart_ptr> sub_items_type;
	typedef std::vector<std::unique_ptr<value_item> > values_type;
	typedef std::vector<const_value_item> const_values_type;
	typedef std::vector<value_callback_t*> const_values_callbacks_type;

	typedef std::map<string_type, uint32_t > names_cahce_type;
	typedef std::map<uint32_t, string_type > indexes_cache_type;

	friend class meta::complex_data_type;

  public:
      complex_runtime_item (const string_type& name, const rx_node_id& id, bool system = false);

      ~complex_runtime_item();


      rx_value get_value (const string_type path) const;

      uint32_t set_hosting_object (object_runtime_ptr obj);

      virtual void object_state_changed (const rx_time& now);

      uint32_t register_sub_item (const string_type& name, complex_runtime_item* val);

      virtual rx_value get_value ();

      virtual bool serialize_definition (base_meta_writter& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      virtual bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      string_type get_const_name (uint32_t name_idx) const;

      virtual void get_sub_items (server_items_type& items, const string_type& pattern) const;


      const rx_node_id& get_parent () const
      {
        return parent_;
      }


	  template<typename T>
	  uint32_t register_const_value(const string_type& name, const T& val)
	  {
		  auto it = names_cache_.find(name);
		  if (it == names_cache_.end())
		  {
			  uint32_t idx = (uint32_t)(const_values_.size());
			  idx |= RT_CONST_IDX_MASK;
			//  const_values_.emplace_back(val);
			  names_cache_.emplace(name,idx);
			  return RX_OK;
		  }
		  return RX_ERROR;
	  }

	  template<typename T>
	  uint32_t register_value(const string_type& name, const T& val)
	  {
		  auto it = names_cache_.find(name);
		  if (it == names_cache_.end())
		  {
			  uint32_t idx = (uint32_t)(values_.size());
			  idx |= RT_VALUE_IDX_MASK;
			 // values_.emplace_back(value_item(val));
			  names_cache_.emplace(name, idx);
			  return RX_OK;
		  }
		  return RX_ERROR;
	  }
	  uint32_t register_value(const string_type& name, const rx_value& val)
	  {
		  switch (val.get_type())
		  {
		  case RX_BOOL_TYPE:
			  return register_value<bool>(name, val);
		  case RX_SDWORD_TYPE:
			  return register_value<int>(name, val);

		  }
		  return RX_OK;
	  }

	  uint32_t register_const_value(const string_type& name, const rx_value& val)
	  {
		  switch (val.get_type())
		  {
		  case RX_BOOL_TYPE:
			  return register_const_value<bool>(name, val);
		  case RX_SDWORD_TYPE:
			  return register_const_value<int>(name, val);

		  }
		  return RX_OK;
	  }
	  callback::callback_handle_t register_callback(const string_type& path, void* p, rx_platform::objects::value_callback_t::callback_function_t func)
	  {
		  rx_value val;
		  value_callback_t* call_obj = get_callback(path, val);
		  if (call_obj)
			  return call_obj->register_callback(func);
		  else
			  return 0;
	  }
	  /*
	  template<typename callbackT>
	  callback::callback_handle_t register_callback(const string_type& path, void* p, void(callbackT::*func)(const rx_value&, callback::callback_state_t))
	  {
		  rx_value val;
		  value_callback_t* call_obj = get_callback(path, val);
		  if (call_obj)
			  return call_obj->register_callback(p, func,val);
		  else
			  return 0;
	  }
	  callback::callback_handle_t register_callback(const string_type& path, void(func)(const rx_value&, callback::callback_state_t))
	  {
		  rx_value val;
		  value_callback_t* call_obj = get_callback(path,val);
		  if (call_obj)
			  return call_obj->register_callback(func,val);
		  else
			  return 0;
	  }*/
  protected:

      object_runtime_ptr& get_hosting_object ();

      value_callback_t* get_callback (const string_type& path, rx_value& val);


  private:


      values_type values_;

      const_values_type const_values_;

      sub_items_type sub_items_;

      rx_reference<object_runtime> my_object_;


      names_cahce_type names_cache_;

      const_values_callbacks_type const_values_callbacks_;

      rx_node_id parent_;

      indexes_cache_type indexes_cache_;


};







class object_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 2, 0, "\
object class. basic implementation of an object");

	DECLARE_REFERENCE_PTR(object_runtime);

	//typedef std::vector<runtime_item::smart_ptr> items_order_type;
	typedef std::map<string_type, size_t> items_cache_type;
	typedef complex_runtime_item_ptr items_type;
	typedef std::vector<logic::program_runtime_ptr> programs_type;

	friend class meta::object_data_type;

public:
	typedef rx_platform::meta::object_class definition_t;
	typedef objects::variable_runtime RType;

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

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      bool is_browsable () const;

      virtual void get_content (server_items_type& sub_items, const string_type& pattern) const;

      platform_item_ptr get_item_ptr ();

      rx_time get_created_time () const;

      rx_time get_modified_time () const;

      string_type get_name () const;

      size_t get_size () const;

      complex_runtime_item_ptr get_complex_item ();


      complex_runtime_item& get_runtime_item ()
      {
        return runtime_item_;
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


      complex_runtime_item runtime_item_;

      programs_type programs_;

      meta::checkable_data meta_data_;


      rx_mode_type mode_;

      rx_time change_time_;


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


} // namespace objects
} // namespace rx_platform



#endif
