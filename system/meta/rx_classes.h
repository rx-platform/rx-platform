

/****************************************************************************
*
*  system\meta\rx_classes.h
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


#ifndef rx_classes_h
#define rx_classes_h 1



// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace objects {
class object_runtime;
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

#define RX_INITIAL_ITEM_VERSION 0x10000

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

class meta_helpers;



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

class const_value_item;
class value_item;

typedef rx_reference<object_runtime> object_runtime_ptr;
}



typedef rx_reference<objects::complex_runtime_item> complex_runtime_ptr;
typedef rx_reference<objects::struct_runtime> struct_runtime_ptr;
typedef rx_reference<objects::variable_runtime> variable_runtime_ptr;
typedef rx_reference<pointers::reference_object> runtime_ptr_t;

}//namespace server

using namespace rx;
using namespace rx::pointers;


namespace rx_platform {

namespace meta {
// had to do forward declaration because of back template pattern

struct meta_data_t
{
	meta_data_t()
	{
		memzero(this, sizeof(meta_data_t));
		wd_timer_period = 1000;
	}
	uint32_t wd_timer_period;
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

      virtual void get_value (values::rx_value& val) const;


      const string_type& get_name () const
      {
        return name_;
      }


      rx_time get_created_time () const
      {
        return created_time_;
      }


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }



  protected:

  private:


      string_type name_;

      rx_time created_time_;

      rx_time modified_time_;


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
        return read_only_;
      }


      const string_type& get_name () const
      {
        return name_;
      }



  protected:

  private:


      bool read_only_;

      string_type name_;


};






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


      allways_good_value<valT> storage_;


};






class complex_class_attribute
{

  public:
      complex_class_attribute (const string_type& name, const rx_node_id& id);

      virtual ~complex_class_attribute();


      virtual bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      virtual bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      const string_type& get_name () const
      {
        return name_;
      }


      rx_node_id get_target_id () const
      {
        return target_id_;
      }



  protected:

  private:


      string_type name_;

      rx_node_id target_id_;


};






class struct_attribute : public complex_class_attribute  
{

  public:
      struct_attribute (const string_type& name, const rx_node_id& id);

      virtual ~struct_attribute();


      struct_runtime_ptr construct ();

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};






class variable_attribute : public complex_class_attribute  
{

  public:
      variable_attribute (const string_type& name, const rx_node_id& id);

      virtual ~variable_attribute();


      variable_runtime_ptr construct ();

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};






class mapper_attribute : public complex_class_attribute  
{

  public:
      mapper_attribute (const string_type& name, const rx_node_id& id);

      virtual ~mapper_attribute();


      complex_runtime_ptr construct ();

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};






class source_attribute : public complex_class_attribute  
{

  public:
      source_attribute (const string_type& name, const rx_node_id& id);

      virtual ~source_attribute();


      complex_runtime_ptr construct ();

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};






class filter_attribute : public complex_class_attribute  
{

  public:
      filter_attribute (const string_type& name, const rx_node_id& id);

      virtual ~filter_attribute();


      complex_runtime_ptr construct ();

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};






class event_attribute : public complex_class_attribute  
{

  public:
      event_attribute (const string_type& name, const rx_node_id& id);

      virtual ~event_attribute();


      complex_runtime_ptr construct ();

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


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


      allways_good_value<valT> storage_;


};







class checkable_data
{
	template <class T1, bool T2>
	friend class checkable_type;

  public:
      checkable_data();

      checkable_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false);


      bool serialize_node (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const;

      bool deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value);

      bool check_in (base_meta_reader& stream);

      bool check_out (base_meta_writter& stream) const;

      bool serialize_checkable_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_checkable_definition (base_meta_reader& stream, uint8_t type);

      void construct (complex_runtime_ptr what);

      values::rx_value get_value () const;

      void construct (objects::object_runtime_ptr what);


      const rx_node_id& get_parent () const
      {
        return parent_;
      }


      uint32_t get_version () const
      {
        return version_;
      }


      rx_time get_created_time () const
      {
        return created_time_;
      }


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }


      string_type get_name () const
      {
        return name_;
      }


      const rx_node_id& get_id () const
      {
        return id_;
      }


      bool get_system () const
      {
        return system_;
      }


      namespace_item_attributes get_attributes () const
      {
        return attributes_;
      }


  protected:

  private:


      rx_node_id parent_;

      uint32_t version_;

      rx_time created_time_;

      rx_time modified_time_;

      string_type name_;

      rx_node_id id_;

      bool system_;

      namespace_item_attributes attributes_;


};


class struct_attribute;
typedef std::unique_ptr<struct_attribute> struct_type_unique_ptr;




class complex_data_type
{

	typedef std::vector<std::unique_ptr<const_value> > const_values_type;
	typedef std::vector<std::unique_ptr<simple_value_def> > simple_values_type;
	typedef std::vector<std::unique_ptr<struct_attribute> > structs_type;
	typedef std::vector<std::unique_ptr<variable_attribute> > variables_type;


	typedef std::set<string_type> names_cahce_type;

  public:
      complex_data_type();

      complex_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);


      bool serialize_complex_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_complex_definition (base_meta_reader& stream, uint8_t type);

      bool register_struct (const string_type& name, const rx_node_id& id);

      bool register_variable (const string_type& name, const rx_node_id& id);

      bool check_name (const string_type& name);

      void construct (complex_runtime_ptr what);

      void construct (objects::object_runtime_ptr what);


      const const_values_type& get_const_values () const
      {
        return const_values_;
      }



      const bool is_sealed () const
      {
        return sealed_;
      }


      const bool is_abstract () const
      {
        return abstract_;
      }


		template <typename constT>
		bool register_const_value(const string_type& name, const constT& value);
		template <typename valT>
		bool register_simple_value(const string_type& name, const valT& value);
  protected:

  private:


      const_values_type const_values_;

      simple_values_type simple_values_;

      structs_type structs_;

      variables_type variables_;


      bool sealed_;

      names_cahce_type names_cache_;

      bool abstract_;


};







class mapped_data_type
{
	typedef std::vector<mapper_attribute> mappers_type;

  public:
      mapped_data_type();

      mapped_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);


      bool serialize_mapped_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_mapped_definition (base_meta_reader& stream, uint8_t type);

      bool register_mapper (const mapper_attribute& item, complex_data_type& complex_data);

      void construct (complex_runtime_ptr what);

      void construct (objects::object_runtime_ptr what);


  protected:

  private:


      mappers_type mappers_;


};







class struct_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(struct_class);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
basic struct class.\r\n\
basic implementation inside object class");
public:
	typedef objects::struct_runtime RType;
	typedef RType CType;

  public:
      struct_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~struct_class();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      namespace_item_attributes get_attributes () const;

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      rx_platform::meta::complex_data_type& complex_data ()
      {
        return complex_data_;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }



      static string_type type_name;


  protected:

  private:
      struct_class();



      rx_platform::meta::complex_data_type complex_data_;

      checkable_data meta_data_;


};


typedef pointers::reference<struct_class> struct_class_ptr;






class variable_data_type
{
	typedef std::vector<source_attribute> sources_type;
	typedef std::vector<filter_attribute> filters_type;
	typedef std::vector<event_attribute> events_type;

  public:
      variable_data_type();

      variable_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      virtual ~variable_data_type();


      bool register_source (const source_attribute& item, complex_data_type& complex_data);

      bool register_filter (const filter_attribute& item, complex_data_type& complex_data);

      bool register_event (const event_attribute& item, complex_data_type& complex_data);

      void construct (complex_runtime_ptr what);

      bool serialize_variable_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_variable_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


      sources_type sources_;

      filters_type filters_;

      events_type events_;


};







class variable_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(variable_class);
public:
	typedef objects::variable_runtime RType;
	typedef RType CType;

  public:
      variable_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~variable_class();


      void construct (complex_runtime_ptr what);

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      platform_item_ptr get_item_ptr ();

      rx_value get_value () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }


      rx_platform::meta::complex_data_type& complex_data ()
      {
        return complex_data_;
      }


      variable_data_type& variable_data ()
      {
        return variable_data_;
      }



      static string_type type_name;


  protected:

  private:
      variable_class();



      checkable_data meta_data_;

      rx_platform::meta::complex_data_type complex_data_;

      variable_data_type variable_data_;


};

typedef pointers::reference<rx_platform::meta::variable_class> variable_class_ptr;






class mapper_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(mapper_class);

  public:
      mapper_class();


      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }


      rx_platform::meta::complex_data_type& complex_data ()
      {
        return complex_data_;
      }



      static string_type type_name;


  protected:

  private:


      checkable_data meta_data_;

      rx_platform::meta::complex_data_type complex_data_;


};

typedef pointers::reference<mapper_class> mapper_class_ptr;






class source_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(source_class);

  public:
      virtual ~source_class();


      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }



      static string_type type_name;


  protected:

  private:
      source_class();



      rx_platform::meta::complex_data_type complex_data_;

      checkable_data meta_data_;


};







class filter_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(filter_class);

  public:
      virtual ~filter_class();


      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }



      static string_type type_name;


  protected:

  private:
      filter_class();



      rx_platform::meta::complex_data_type complex_data_;

      checkable_data meta_data_;


};







class event_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(event_class);

  public:
      virtual ~event_class();


      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }



      static string_type type_name;


  protected:

  private:
      event_class();



      rx_platform::meta::complex_data_type complex_data_;

      checkable_data meta_data_;


};


// Parameterized Class rx_platform::meta::class_const_value

template <typename valT>
class_const_value<valT>::class_const_value (const valT& value)
	: storage_(value)
{
}

template <typename valT>
class_const_value<valT>::class_const_value (const valT& value, const string_type& name)
	: storage_(value)
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
	val = rx_value(storage_.value());
}


// Parameterized Class rx_platform::meta::simple_value_item

template <typename valT>
simple_value_item<valT>::simple_value_item (const valT& value)
	: storage_(value)
{
}

template <typename valT>
simple_value_item<valT>::simple_value_item (const valT& value, const string_type& name)
	: storage_(value)
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
	val = rx_value(storage_.value());
}


} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {

template <typename constT>
bool complex_data_type::register_const_value(const string_type& name, const constT& value)
{
	typedef class_const_value<constT> const_t;

	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
		//if (check_name(name))
	{
		const_values_.emplace_back(std::make_unique<const_t>(value, name));
		return true;
	}
	else
	{
		return false;
	}
}


template <typename valT>
bool complex_data_type::register_simple_value(const string_type& name, const valT& value)
{
	typedef simple_value_item<valT> val_t;

	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
		//if (check_name(name))
	{
		simple_values_.emplace_back(std::make_unique<val_t>(value, name));
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


// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	  const metaT& get_meta() const
	  {
		  return meta_;
	  }

	: checkable_type(name,id,rx_node_id::null_id, system)

	return sizeof(this);

	return variable_data_;

	return mapped_data_;

	return complex_data_;

	return object_data_;

	if (!serialize_checkable_definition(stream, type))
		return false;
	if (!complex_data_.serialize_complex_definition(stream, type))
		return false;
	if (!mapped_data_.serialize_mapped_definition(stream, type))
		return false;
	if (!variable_data_.serialize_variable_definition(stream, type))
		return false;
	if (!object_data_.serialize_object_definition(stream, type))
		return false;
	return true;


	if (!deserialize_checkable_definition(stream, type))
		return false;
	if (!complex_data_.deserialize_complex_definition(stream, type))
		return false;
	if (!mapped_data_.deserialize_mapped_definition(stream, type))
		return false;
	if (!variable_data_.deserialize_variable_definition(stream, type))
		return false;
	if (!object_data_.deserialize_object_definition(stream, type))
		return false;
	return true;

#endif
