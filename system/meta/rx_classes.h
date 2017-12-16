

/****************************************************************************
*
*  system\meta\rx_classes.h
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


#ifndef rx_classes_h
#define rx_classes_h 1



// rx_ns
#include "system/server/rx_ns.h"
// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_values
#include "lib/rx_values.h"

namespace rx_platform {
namespace objects {
class complex_runtime_item;
} // namespace objects

namespace meta {
class mapper_attribute;
class variable_attribute;
class struct_attribute;

} // namespace meta
} // namespace rx_platform


#include "system/server/rx_ns.h"
#include "system/json/rx_ser.h"
#include "system/callbacks/rx_callback.h"

using namespace rx;
using namespace rx::values;
using namespace rx_platform::ns;

#define RT_TYPE_ID_CONST_VALUE 1
#define RT_TYPE_ID_VALUE 2
#define RT_TYPE_ID_FULL_VALUE 4


#define RX_TYPE_UNKNOWN		0x0
#define RX_TYPE_OBJ			0x01
#define RX_TYPE_VARIABLE	0x02
#define RX_TYPE_REF			0x04
#define RX_TYPE_SOURCE		0x08
#define RX_TYPE_EVENT		0x10
#define RX_TYPE_FILTER		0x20
#define RX_TYPE_MAPPER		0x40
#define RX_TYPE_APPLICATION 0x80
#define RX_TYPE_DOMAIN		0x100
#define RX_TYPE_STRUCT		0x200
#define RX_TYPE_PROGRAM		0x400

#define RX_TYPE_INSTANCE	0x80000000

namespace rx_platform {

namespace meta
{

class object_class;
class variable_class;
class event_class;
class filter_class;
class source_class;
class reference_type;
class mapper_class;
class application_class;
class domain_class;
class struct_class;
class port_class;

class const_value_item;
class value_item;

typedef TYPELIST_2(const_value_item, value_item) runtime_types;


typedef rx_reference<pointers::reference_object> runtime_ptr_t;

}

namespace objects
{
template<typename T>
class server_const_value_item;

class struct_runtime;
class port_runtime;
class variable_runtime;
class domain_runtime;
class application_runtime;
}


}//namespace server

using namespace rx;
using namespace rx::pointers;


namespace rx_platform {

namespace meta {
// had to do forward declaration because of back template pattern
class command_class;

struct meta_data_t
{
	meta_data_t()
	{
		memzero(this, sizeof(meta_data_t));
		wd_timer_period = 1000;
	}
	uint32_t wd_timer_period;
};







template <class metaT, bool _browsable>
class base_meta_type : public ns::rx_platform_item  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(base_meta_type);

  public:
      base_meta_type (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~base_meta_type();


      bool serialize (base_meta_writter& stream) const;

      virtual bool deserialize (base_meta_reader& stream);

      string_type get_type_name () const;

      void construct (runtime_ptr_t what);


      const rx_node_id& get_id () const
      {
        return _id;
      }


      const bool get_system () const
      {
        return _system;
      }



  protected:
      base_meta_type();


  private:


      rx_node_id _id;

      bool _system;


};






typedef base_meta_type< command_class , false  > command_class_t;






class command_class : public command_class_t, 
                      	protected rx::point
{

  public:
      command_class();

      virtual ~command_class();


      static string_type type_name;


  protected:

  private:


};







template <class metaT, bool _browsable>
class checkable_type : public base_meta_type<metaT, _browsable>  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(checkable_type);

	template <class T1, bool T2>
	friend class checkable_type;

  public:
      checkable_type();

      checkable_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false);

      virtual ~checkable_type();


      bool serialize_node (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const;

      bool deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value);

      bool check_in (base_meta_reader& stream);

      bool check_out (base_meta_writter& stream) const;

      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool is_browsable () const;

      void construct (runtime_ptr_t what);


      const string_type& get_name () const
      {
        return _name;
      }


      const rx_node_id& get_parent () const
      {
        return _parent;
      }



  protected:

      virtual bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      virtual bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  private:


      string_type _name;

      rx_node_id _parent;


};






class const_value
{
	const_value(const const_value &right) = delete;
	const_value(const_value &&right) = delete;
	const_value & operator=(const const_value &right) = delete;
	const_value & operator=(const_value &&right) = delete;

  public:
      const_value();

      const_value (const string_type& name);

      virtual ~const_value();


      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      virtual void get_value (values::rx_value& val) const = 0;


      const string_type& get_name () const
      {
        return _name;
      }



  protected:

  private:


      string_type _name;


};






class simple_value_def
{
	simple_value_def(const simple_value_def &right) = delete;
	simple_value_def(simple_value_def &&right) = delete;
	simple_value_def & operator=(const simple_value_def &right) = delete;
	simple_value_def & operator=(simple_value_def &&right) = delete;

  public:
      simple_value_def();

      simple_value_def (const string_type& name);

      virtual ~simple_value_def();


      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      virtual void get_value (values::rx_value& val) const = 0;


      const bool get_read_only () const
      {
        return _read_only;
      }


      const string_type& get_name () const
      {
        return _name;
      }



  protected:

  private:


      bool _read_only;

      string_type _name;


};






template <class metaT, bool _browsable>
class base_complex_type : public checkable_type<metaT, _browsable>, 
                          	protected rx::p
{
	DECLARE_REFERENCE_PTR(base_complex_type);
	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;

	typedef std::vector<std::unique_ptr<const_value> > const_values_type;
	typedef std::vector<std::unique_ptr<simple_value_def> > simple_values_type;
	typedef std::vector<variable_attribute> variables_type;
	typedef std::vector<struct_attribute> structs_type;


	typedef std::set<string_type> names_cahce_type;


  public:
      base_complex_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      virtual ~base_complex_type();


      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool register_struct (const struct_attribute& item);

      bool register_variable (const variable_attribute& item);

      void construct (runtime_ptr_t what);


      const const_values_type& get_const_values () const
      {
        return _const_values;
      }



      const bool is_sealed () const
      {
        return _sealed;
      }


      const bool is_abstract () const
      {
        return _abstract;
      }


		template <typename constT>
		bool register_const_value(const string_type& name, const constT& value);
		template <typename valT>
		bool register_simple_value(const string_type& name, const valT& value);
  protected:
      base_complex_type();


      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      bool check_name (const string_type& name);


  private:


      const_values_type _const_values;

      simple_values_type _simple_values;

      structs_type _structs;

      variables_type _variables;


      bool _sealed;

      names_cahce_type _names_cache;

      bool _abstract;


};






template <class metaT, bool _browsable>
class base_mapped_class : public base_complex_type<metaT, _browsable>  
{
	DECLARE_REFERENCE_PTR(base_mapped_class);
	typedef std::vector<mapper_attribute> mappers_type;

  public:
      base_mapped_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      virtual ~base_mapped_class();


      bool register_mapper (const mapper_attribute& item);

      void construct (runtime_ptr_t what);


  protected:
      base_mapped_class();


      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  private:


      mappers_type _mappers;


};






typedef base_mapped_class< struct_class , false  > struct_class_t;






class struct_class : public struct_class_t  
{
	DECLARE_REFERENCE_PTR(struct_class);
	typedef objects::struct_runtime RType;

  public:
      struct_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~struct_class();


      static string_type type_name;


  protected:

  private:
      struct_class();



};






typedef base_complex_type< mapper_class , false  > mapper_class_t;






class mapper_class : public mapper_class_t  
{

  public:
      virtual ~mapper_class();


      static string_type type_name;


  protected:

  private:
      mapper_class();



};

typedef pointers::reference<mapper_class> mapper_class_ptr;






template <typename valT>
class class_const_value : public const_value  
{

  public:
      class_const_value (const valT& value);

      class_const_value (const valT& value, const string_type& name);

      virtual ~class_const_value();


      void get_value (values::rx_value& val) const;


  protected:

  private:


      simple_const_value<valT> _storage;


};






class complex_class_attribute
{

  public:
      complex_class_attribute();

      virtual ~complex_class_attribute();


      virtual bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      virtual bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      const string_type& get_name () const
      {
        return _name;
      }


      const rx_node_id& get_target_id () const
      {
        return _target_id;
      }



  protected:

  private:


      string_type _name;

      rx_node_id _target_id;


};






class struct_attribute : public complex_class_attribute  
{

  public:
      struct_attribute();

      virtual ~struct_attribute();


  protected:

  private:


};






class variable_attribute : public complex_class_attribute  
{

  public:
      variable_attribute();

      virtual ~variable_attribute();


  protected:

  private:


};






class mapper_attribute : public complex_class_attribute  
{

  public:
      mapper_attribute();

      virtual ~mapper_attribute();


  protected:

  private:


};






class source_attribute : public complex_class_attribute  
{

  public:
      source_attribute();

      virtual ~source_attribute();


  protected:

  private:


};






class filter_attribute : public complex_class_attribute  
{

  public:
      filter_attribute();

      virtual ~filter_attribute();


  protected:

  private:


};






class event_attribute : public complex_class_attribute  
{

  public:
      event_attribute();

      virtual ~event_attribute();


  protected:

  private:


};






template <class metaT, bool _browsable = false>
class base_variable_class : public base_mapped_class<metaT, _browsable>  
{
	DECLARE_REFERENCE_PTR(base_variable_class);

	typedef std::vector<source_attribute> sources_type;
	typedef std::vector<filter_attribute> filters_type;
	typedef std::vector<event_attribute> events_type;

  public:
      base_variable_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      virtual ~base_variable_class();


      bool register_source (const source_attribute& item);

      bool register_filter (const filter_attribute& item);

      bool register_event (const event_attribute& item);


  protected:
      base_variable_class();


  private:


      sources_type _sources;

      filters_type _filters;

      events_type _events;


};






typedef base_variable_class< variable_class  > variable_class_t;






class variable_class : public variable_class_t  
{
	DECLARE_REFERENCE_PTR(variable_class);
public:
	typedef objects::variable_runtime RType;

  public:
      variable_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~variable_class();


      static string_type type_name;


  protected:

  private:
      variable_class();



};

typedef pointers::reference<rx_platform::meta::variable_class> variable_class_ptr;





typedef base_complex_type< source_class , false  > source_class_t;






class source_class : public source_class_t  
{

  public:
      virtual ~source_class();


      static string_type type_name;


  protected:

  private:
      source_class();



};






typedef base_complex_type< event_class , false  > event_class_t;






typedef base_complex_type< filter_class , false  > filter_class_t;






class filter_class : public filter_class_t  
{

  public:
      virtual ~filter_class();


      static string_type type_name;


  protected:

  private:
      filter_class();



};






class event_class : public event_class_t  
{

  public:
      virtual ~event_class();


      static string_type type_name;


  protected:

  private:
      event_class();



};







template <typename valT>
class simple_value_item : public simple_value_def  
{

  public:
      simple_value_item (const valT& value);

      simple_value_item (const valT& value, const string_type& name);

      virtual ~simple_value_item();


      void get_value (values::rx_value& val) const;


  protected:

  private:


      simple_const_value<valT> _storage;


};


// Parameterized Class rx_platform::meta::class_const_value

template <typename valT>
class_const_value<valT>::class_const_value (const valT& value)
	: _storage(value)
{
}

template <typename valT>
class_const_value<valT>::class_const_value (const valT& value, const string_type& name)
	: _storage(value)
	, const_value(name)
{
}


template <typename valT>
class_const_value<valT>::~class_const_value()
{
}



template <typename valT>
void class_const_value<valT>::get_value (values::rx_value& val) const
{
	_storage.get_value(val);
}


// Parameterized Class rx_platform::meta::simple_value_item

template <typename valT>
simple_value_item<valT>::simple_value_item (const valT& value)
	: _storage(value)
{
}

template <typename valT>
simple_value_item<valT>::simple_value_item (const valT& value, const string_type& name)
	: _storage(value)
	, simple_value_def(name)
{
}


template <typename valT>
simple_value_item<valT>::~simple_value_item()
{
}



template <typename valT>
void simple_value_item<valT>::get_value (values::rx_value& val) const
{
	_storage.get_value(val);
}


} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {

template <class metaT, bool _browsable>
template <typename constT>
bool base_complex_type<metaT, _browsable>::register_const_value(const string_type& name, const constT& value)
{
	typedef class_const_value<constT> const_t;

	auto it = _names_cache.find(name);
	if (it == _names_cache.end())
		//if (check_name(name))
	{
		_const_values.emplace_back(std::make_unique<const_t>(value, name));
		return true;
	}
	else
	{
		return false;
	}
}


template <class metaT, bool _browsable>
template <typename valT>
bool base_complex_type<metaT, _browsable>::register_simple_value(const string_type& name, const valT& value)
{
	typedef simple_value_item<valT> val_t;

	auto it = _names_cache.find(name);
	if (it == _names_cache.end())
		//if (check_name(name))
	{
		_simple_values.emplace_back(std::make_unique<val_t>(value, name));
		return true;
	}
	else
	{
		return false;
	}
}

} // namespace meta
} // namespace server rx_platform


#endif
