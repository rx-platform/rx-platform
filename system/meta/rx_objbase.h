

/****************************************************************************
*
*  system\meta\rx_objbase.h
*
*  Copyright (c) 2017 Dusan Ciric
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
// rx_callback
#include "system/callbacks/rx_callback.h"
// rx_classes
#include "system/meta/rx_classes.h"

namespace server {
namespace objects {
class application_runtime;
class domain_runtime;
class object_runtime;

} // namespace objects
} // namespace server


#include "system/callbacks/rx_callback.h"
using namespace rx;
using namespace server::ns;
using rx::values::rx_value;
using rx::values::simple_const_value;


namespace server {

namespace objects {
extern const char* g_const_simple_class_name;
typedef dword runtime_item_id_t;
class object_runtime;
class variable_runtime;
typedef pointers::virtual_reference<object_runtime> object_runtime_ptr;

typedef callback::callback_functor_cotainer<locks::lockable,rx_value> value_callback_t;
class complex_runtime_item;
typedef pointers::reference<complex_runtime_item> complex_runtime_item_ptr;
typedef pointers::reference<domain_runtime> domain_runtime_ptr;
typedef pointers::reference<application_runtime> application_runtime_ptr;





typedef meta::checkable_type< object_runtime  > object_runtime_t;






class value_item 
{

  public:
      value_item();

      virtual ~value_item();


      bool serialize_definition (base_meta_writter& stream, byte type) const;

      bool deserialize_definition (base_meta_reader& stream, byte type);


  protected:

  private:


      static const dword _type_id;


};






class const_value_item 
{

  public:
      const_value_item();

      virtual ~const_value_item();


      virtual void get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const = 0;

      bool serialize_definition (base_meta_writter& stream, byte type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize_definition (base_meta_reader& stream, byte type);


  protected:

  private:


      static const dword _type_id;


};







template <typename valT>
class server_const_value_item : public const_value_item  
{
	typedef server_const_value_item<valT> item_type;

  public:
      server_const_value_item (const valT& value);

      virtual ~server_const_value_item();


      simple_const_value<valT>& value ();

      const simple_const_value<valT>& value () const;

      void bind ();

      void release ();

      bool has_own_time () const;

      namespace_item_attributes get_attributes () const;

      void item_lock ();

      void item_unlock ();

      void item_lock () const;

      void item_unlock () const;

      void get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;


  protected:

  private:


      simple_const_value<valT> _storage;

      std::atomic<ref_counting_type> _ref_count;


};


#define RT_CONST_IDX_MASK	0xc0000000
#define RT_VALUE_IDX_MASK	0x80000000
#define RT_COMPLEX_IDX_MASK 0x40000000
#define RT_TYPE_MASK		0xc0000000
#define RT_INDEX_MASK		0x0000ffff
#define RT_CALLBACK_MASK	0x3fff0000
#define RT_CALLBACK_INDEX(idx)	(((idx>>16)||0x3fff)-1)




class complex_runtime_item : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(complex_runtime_item);
	typedef std::vector<complex_runtime_item::smart_ptr> sub_items_type;
	typedef std::vector<value_item*> values_type;
	typedef std::vector<const_value_item*> const_values_type;
	typedef std::vector<value_callback_t*> const_values_callbacks_type;

	typedef std::map<string_type, dword > names_cahce_type;


	template<class otherT>
	friend class meta::checkable_type;

  public:
      complex_runtime_item (object_runtime_ptr my_object);

      virtual ~complex_runtime_item();


      dword register_const_value (const string_type& name, const_value_item& val);

      rx_value get_value (const string_type path) const;

      virtual void object_state_changed (const rx_time& now);

      dword register_sub_item (const string_type& name, complex_runtime_item::smart_ptr val);

      virtual rx_value get_value ();

      virtual bool serialize_definition (base_meta_writter& stream, byte type, const rx_time& ts, const rx_mode_type& mode) const;

      virtual bool deserialize_definition (base_meta_reader& stream, byte type);

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
	  }
  protected:

      object_runtime_ptr& get_hosting_object ();

      value_callback_t* get_callback (const string_type& path, rx_value& val);


  private:

      dword set_hosting_object (object_runtime_ptr obj);



      values_type _values;

      const_values_type _const_values;

      sub_items_type _sub_items;

      object_runtime_ptr _my_object;


      names_cahce_type _names_cache;

      const_values_callbacks_type _const_values_callbacks;


};







class object_runtime : public object_runtime_t  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(object_runtime);

	//typedef std::vector<runtime_item::smart_ptr> items_order_type;
	typedef std::map<string_type, size_t> items_cache_type;
	typedef complex_runtime_item_ptr items_type;

  public:
      object_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~object_runtime();


      rx_value get_value (const string_type path) const;

      void turn_on ();

      void turn_off ();

      void set_blocked ();

      void set_test ();

      void get_value (values::rx_value& val) const;

      namespace_item_attributes get_attributes () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      const string_type& get_item_name () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;

      dword register_const_value (const string_type& name, const_value_item& val);


      complex_runtime_item::smart_ptr get_complex_item ()
      {
        return _complex_item;
      }



      const rx_time get_created_time () const
      {
        return _created_time;
      }


      const rx_mode_type& get_mode () const
      {
        return _mode;
      }


      const rx_time get_modified_time () const
      {
        return _modified_time;
      }



      static string_type type_name;

	  template<typename... Args>
	  callback::callback_handle_t register_callback(const string_type& path,Args... args)
	  {
		  return _complex_item->register_callback(path, args...);
	  }
	  void unregister_callback(const string_type& path, callback::callback_handle_t)
	  {
		//  return _items->unregister_callaback(path);
	  }
  protected:
      object_runtime();


      bool serialize_definition (base_meta_writter& stream, byte type) const;

      bool deserialize_definition (base_meta_reader& stream, byte type);

      bool init_object ();


      application_runtime_ptr _my_application;

      domain_runtime_ptr _my_domain;


  private:


      complex_runtime_item::smart_ptr _complex_item;


      rx_time _created_time;

      rx_mode_type _mode;

      rx_time _modified_time;


};






class server_object : public object_runtime, 
                      	public rx::point
{
	DECLARE_CODE_INFO("rx", "0.5.0", "\
system object class. basic implementation of a system object");

	DECLARE_REFERENCE_PTR(server_object);
	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;

  public:
      server_object (const string_type& name, const rx_node_id& id);

      virtual ~server_object();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class filter_runtime : public complex_runtime_item  
{
	DECLARE_REFERENCE_PTR(filter_runtime);

  public:
      filter_runtime (object_runtime_ptr my_object);

      virtual ~filter_runtime();


  protected:

  private:


};






class source : public complex_runtime_item  
{
	DECLARE_REFERENCE_PTR(source);

  public:
      source (object_runtime_ptr my_object);

      virtual ~source();


  protected:

  private:


};






class mapper : public complex_runtime_item  
{
	DECLARE_REFERENCE_PTR(mapper);

  public:
      mapper (object_runtime_ptr my_object);

      virtual ~mapper();


  protected:

  private:


};







class variable_runtime : public complex_runtime_item  
{
	DECLARE_REFERENCE_PTR(variable_runtime);
	typedef std::vector<filter_runtime::smart_ptr> filters_type;
	typedef std::vector<source::smart_ptr> sources_type;

  public:
      variable_runtime (object_runtime_ptr my_object);

      virtual ~variable_runtime();


  protected:

  private:


      filters_type _filters;

      sources_type _input_sources;

      sources_type _output_sources;


};






class domain_runtime : public object_runtime, 
                       	public rx::poin
{
	DECLARE_CODE_INFO("rx", "0.5.0", "\
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


      objects_type _objects;


};






class struct_runtime : public complex_runtime_item  
{
	DECLARE_REFERENCE_PTR(struct_runtime);

  public:
      struct_runtime (object_runtime_ptr my_object);

      virtual ~struct_runtime();


  protected:

  private:


};






class port_runtime : public object_runtime  
{
	DECLARE_CODE_INFO("rx", "0.5.0", "\
system port class. basic implementation of a port");

	DECLARE_VIRTUAL_REFERENCE_PTR(port_runtime);

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
	DECLARE_CODE_INFO("rx", "0.5.0", "\
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



      domains_type _domains;

      ports_type _ports;


};






class user_object : public object_runtime, 
                    	public rx::pointer
{
	DECLARE_CODE_INFO("rx", "0.1.0", "\
user object class. basic implementation of a user object");

	DECLARE_REFERENCE_PTR(user_object);
	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;

  public:
      user_object();

      user_object (const string_type& name, const rx_node_id& id);

      virtual ~user_object();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};


// Parameterized Class server::objects::server_const_value_item 

template <typename valT>
server_const_value_item<valT>::server_const_value_item (const valT& value)
      : _ref_count(1)
	, _storage(value)
{
}


template <typename valT>
server_const_value_item<valT>::~server_const_value_item()
{
}



template <typename valT>
simple_const_value<valT>& server_const_value_item<valT>::value ()
{
	return (_storage);
}

template <typename valT>
const simple_const_value<valT>& server_const_value_item<valT>::value () const
{
	return (_storage);
}

template <typename valT>
void server_const_value_item<valT>::bind ()
{
	_ref_count++;
}

template <typename valT>
void server_const_value_item<valT>::release ()
{
	if (0 == --_ref_count)
		delete this;
}

template <typename valT>
bool server_const_value_item<valT>::has_own_time () const
{
  return false;

}

template <typename valT>
namespace_item_attributes server_const_value_item<valT>::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access);
}

template <typename valT>
void server_const_value_item<valT>::item_lock ()
{
}

template <typename valT>
void server_const_value_item<valT>::item_unlock ()
{
}

template <typename valT>
void server_const_value_item<valT>::item_lock () const
{
}

template <typename valT>
void server_const_value_item<valT>::item_unlock () const
{
}

template <typename valT>
void server_const_value_item<valT>::get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const
{
	value().get_value(val);
	val.set_time(ts);
	if (mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	else if (mode.is_test())
		val.set_test();
	else
	val.set_quality(RX_GOOD_QUALITY);
}

template <typename valT>
void server_const_value_item<valT>::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	has_own_code_info = true;
}

template <typename valT>
string_type server_const_value_item<valT>::get_type_name () const
{
	return "SIMPLE VAL";
}


} // namespace objects
} // namespace server



#endif
