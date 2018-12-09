

/****************************************************************************
*
*  system\meta\rx_blocks.h
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


#ifndef rx_blocks_h
#define rx_blocks_h 1



// rx_values
#include "lib/rx_values.h"
// rx_classes
#include "system/meta/rx_classes.h"

namespace rx_platform {
namespace objects {
namespace object_types {
class object_runtime;
} // namespace object_types

namespace blocks {
class complex_runtime_item;

} // namespace blocks
} // namespace objects
} // namespace rx_platform


#include "system/server/rx_ns.h"
#include "system/callbacks/rx_callback.h"
#include "system/meta/rx_def_blocks.h"
using namespace rx_platform::ns;
using namespace rx::values;


namespace rx_platform {

namespace objects {
struct object_state_data
{
	rx_time ts;
	rx_mode_type mode;
};
typedef rx::pointers::reference<object_types::object_runtime> object_runtime_ptr;
typedef rx::pointers::reference<object_types::application_runtime> application_runtime_ptr;
typedef rx::pointers::reference<object_types::domain_runtime> domain_runtime_ptr;
typedef rx::pointers::reference<object_types::port_runtime> port_runtime_ptr;
typedef callback::callback_functor_container<locks::lockable, rx::values::rx_value> value_callback_t;

namespace blocks {

extern const char* g_const_simple_class_name;
typedef uint32_t runtime_item_id_t;
class variable_runtime;
class struct_runtime;
class complex_runtime_item;

typedef complex_runtime_item* complex_runtime_item_ptr;
typedef struct_runtime* struct_runtime_ptr;








class const_value_item 
{
public:
	const_value_item(const const_value_item& right) = default;
	const_value_item(const_value_item&& right) = default;
	const_value_item() = default;
	~const_value_item() = default;

	const_value_item(rx_simple_value&& val)
		: storage_(std::move(val))
	{
	}

  public:

      bool serialize_definition (base_meta_writer& stream, uint8_t type, const object_state_data& data) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode);

      bool has_own_time () const;

      namespace_item_attributes get_attributes () const;

      void item_lock ();

      void item_unlock ();

      void item_lock () const;

      void item_unlock () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      static string_type get_type_name ();

      rx_value get_value (const object_state_data& data) const;

      uint32_t register_value (const string_type& name, const rx_value& val);


  protected:

  private:


      rx::values::rx_simple_value storage_;


      static const uint32_t type_id_;


};






class filter_runtime 
{
	typedef rx_platform::meta::basic_defs::struct_class definition_t;
	typedef objects::blocks::struct_runtime RType;
	typedef objects::blocks::struct_runtime* RTypePtr;
public:
	typedef std::unique_ptr<filter_runtime> smart_ptr;

  public:
      filter_runtime();


      string_type get_type_name () const;


  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


      static string_type type_name;


};






class mapper_runtime 
{
	friend class meta::def_blocks::complex_data_type;
public:
	typedef rx_platform::meta::basic_defs::mapper_class definition_t;
	typedef objects::blocks::mapper_runtime RType;
	typedef objects::blocks::mapper_runtime* RTypePtr;
	typedef std::unique_ptr<mapper_runtime> smart_ptr;

  public:
      mapper_runtime();

      virtual ~mapper_runtime();


  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


};






class source_runtime 
{
	friend class meta::def_blocks::complex_data_type;
public:
	typedef rx_platform::meta::basic_defs::source_class definition_t;
	typedef objects::blocks::source_runtime RType;
	typedef objects::blocks::source_runtime* RTypePtr;
	typedef std::unique_ptr<source_runtime> smart_ptr;

  public:
      source_runtime();

      virtual ~source_runtime();


      static string_type get_type_name ()
      {
        return type_name;
      }



  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


      static string_type type_name;


};






class value_item 
{
public:
	value_item(const value_item& right) = default;
	value_item(value_item&& right) = default;
	~value_item() = default;


	value_item(rx_timed_value&& val)
		: change_time_(val.get_time()),
		readonly_(false),
		storage_(std::move(val))
	{
	}


  public:

      bool serialize_definition (base_meta_writer& stream, uint8_t type, const object_state_data& data) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type, const rx_mode_type& mode);

      rx_value get_value (const object_state_data& data) const;

      static string_type get_type_name ();


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


      rx::values::rx_timed_value storage_;


      static const uint32_t type_id_;

      rx_time change_time_;

      bool readonly_;


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
	typedef std::vector<value_item> values_type;
	typedef std::vector<const_value_item> const_values_type;
	typedef std::vector<value_callback_t*> const_values_callbacks_type;

	typedef std::map<string_type, uint32_t > names_cahce_type;
	typedef std::map<uint32_t, string_type > indexes_cache_type;

	friend class meta::def_blocks::complex_data_type;

  public:
      complex_runtime_item();

      complex_runtime_item (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~complex_runtime_item();


      rx_value get_value (const string_type path) const;

      uint32_t set_hosting_object (object_runtime_ptr obj);

      virtual void object_state_changed (const rx_time& now);

      uint32_t register_sub_item (const string_type& name, complex_runtime_item* val);

      virtual rx_value get_value ();

      virtual bool serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      virtual bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      string_type get_const_name (uint32_t name_idx) const;

      virtual void get_sub_items (server_items_type& items, const string_type& pattern) const;

      uint32_t register_const_value (const string_type& name, rx_simple_value&& val);

      uint32_t register_value (const string_type& name, rx_timed_value&& val);

      virtual string_type get_type_name () const;


      const rx_node_id& get_parent () const
      {
        return parent_;
      }


	  template<typename T>
	  uint32_t register_const_value_direct(const string_type& name, T&& val)
	  {
		  return register_const_value(name, val);
	  }

	  template<typename T>
	  uint32_t register_value_direct(const string_type& name, T&& val)
	  {
		  rx_timed_value temp;
		  temp.assign_static(std::forward<T>(val));
		  return register_value(name, std::move(temp));
	  }
	  callback::callback_handle_t register_callback(const string_type& path, void* p, value_callback_t::callback_function_t func)
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

      rx_reference<object_types::object_runtime> my_object_;


      names_cahce_type names_cache_;

      const_values_callbacks_type const_values_callbacks_;

      rx_node_id parent_;

      indexes_cache_type indexes_cache_;


};






class struct_runtime : public complex_runtime_item  
{

	friend class meta::def_blocks::complex_data_type;

public:
	typedef rx_platform::meta::basic_defs::struct_class definition_t;
	typedef objects::blocks::struct_runtime RType;
	typedef objects::blocks::struct_runtime* RTypePtr;

  public:
      struct_runtime();

      struct_runtime (const string_type& name, const rx_node_id& id, bool system = false);


      bool serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      meta::checkable_data& meta_data ();

      string_type get_type_name () const;


      const meta::checkable_data& meta_data () const;


      static string_type type_name;


  protected:

  private:


      meta::checkable_data meta_data_;


};






class event_runtime 
{
	friend class meta::def_blocks::complex_data_type;
public:
	typedef rx_platform::meta::basic_defs::event_class definition_t;
	typedef objects::blocks::event_runtime RType;
	typedef objects::blocks::event_runtime* RTypePtr;
	typedef std::unique_ptr<event_runtime> smart_ptr;

  public:
      event_runtime();


      string_type get_type_name () const;


  protected:

  private:


      std::unique_ptr<complex_runtime_item> my_item_;


      static string_type type_name;


};







class variable_runtime 
{
	typedef std::vector<filter_runtime::smart_ptr> filters_type;
	typedef std::vector<source_runtime::smart_ptr> sources_type;
	typedef std::vector<event_runtime::smart_ptr> events_type;


	friend class meta::def_blocks::complex_data_type;

public:
	typedef rx_platform::meta::basic_defs::variable_class definition_t;
	typedef objects::blocks::variable_runtime RType;

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

      events_type events_;


};


} // namespace blocks
} // namespace objects
} // namespace rx_platform



#endif