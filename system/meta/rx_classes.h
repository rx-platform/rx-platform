

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



// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_values
#include "lib/rx_values.h"
// rx_ns
#include "system/server/rx_ns.h"

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



#define DECLARE_META_TYPE \
public:\
	static bool g_type_registred;\
	static void init_meta_type();\
	uint32_t get_type () const\
		{ return type_id; }\
	static uint32_t type_id;\
	static const char* type_name;\
	static const char* command_str;\
private:\




#define DECLARE_META_OBJECT \
public:\
	uint32_t get_type () const\
				{ return type_id; }\
	static uint32_t type_id;\
private:\


namespace rx_platform {

namespace meta
{
void init_compiled_meta_types();


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

typedef TYPELIST_9(object_class, variable_class, source_class, event_class, filter_class, mapper_class, application_class, domain_class, struct_class) regular_scada_types;
typedef TYPELIST_10(reference_type, object_class, variable_class, source_class, event_class, filter_class, mapper_class, application_class, domain_class, struct_class) full_scada_types;

class const_value_item;
class value_item;

typedef TYPELIST_2(const_value_item, value_item) runtime_types;

}

namespace objects
{
template<typename T>
class server_const_value_item;



}


}//namespace server

using namespace rx;
using namespace rx::pointers;


namespace rx_platform {

namespace meta {
// had to do forward declaration because of back template pattern
class command_class;

// typedefs








template <class metaT>
class base_meta_type : public ns::rx_platform_item  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(base_meta_type);

  public:
      base_meta_type (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~base_meta_type();


      bool serialize (base_meta_writter& stream) const;

      virtual bool deserialize (base_meta_reader& stream);

      string_type get_type_name () const;


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






typedef base_meta_type< command_class  > command_class_t;






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







template <class metaT>
class checkable_type : public base_meta_type<metaT>  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(checkable_type);

  public:
      checkable_type();

      checkable_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false);

      virtual ~checkable_type();


      bool serialize_node (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const;

      bool deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value);

      bool check_in (base_meta_reader& stream);

      bool check_out (base_meta_writter& stream) const;

      bool generate_json (std::ostream& def, std::ostream& err) const;


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

  public:
      const_value();

      const_value(const const_value &right);

      const_value (const string_type& name);

      virtual ~const_value();

      const_value & operator=(const const_value &right);


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






class internal_value 
{

  public:
      internal_value();

      internal_value(const internal_value &right);

      virtual ~internal_value();

      internal_value & operator=(const internal_value &right);


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






template <class metaT>
class base_complex_type : public checkable_type<metaT>, 
                          	protected rx::p
{
	DECLARE_REFERENCE_PTR(base_complex_type);
	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;

	typedef std::vector<std::unique_ptr<const_value> > const_values_type;
	typedef std::vector<internal_value*> internal_values_type;
	typedef std::vector<variable_attribute> variables_type;
	typedef std::vector<struct_attribute> structs_type;


	typedef std::set<string_type> names_cahce_type;


  public:
      base_complex_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      virtual ~base_complex_type();


      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool register_internal_value (internal_value* item);

      bool register_struct (const struct_attribute& item);

      bool register_variable (const variable_attribute& item);


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
  protected:
      base_complex_type();


      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      bool check_name (const string_type& name);


  private:


      const_values_type _const_values;

      internal_values_type _internal_values;

      structs_type _structs;

      variables_type _variables;


      bool _sealed;

      names_cahce_type _names_cache;

      bool _abstract;


};






template <class metaT>
class base_mapped_class : public base_complex_type<metaT>  
{
	DECLARE_REFERENCE_PTR(base_mapped_class);
	typedef std::vector<mapper_attribute> mappers_type;

  public:
      base_mapped_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      virtual ~base_mapped_class();


      bool register_mapper (const mapper_attribute& item);


  protected:
      base_mapped_class();


      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  private:


      mappers_type _mappers;


};






typedef base_mapped_class< struct_class  > struct_class_t;


class struct_class;
typedef base_mapped_class< struct_class  > struct_class_t;




class struct_class : public struct_class_t  
{

  public:
      struct_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~struct_class();


      static string_type type_name;


  protected:

  private:
      struct_class();



};






typedef base_complex_type< mapper_class  > mapper_class_t;






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






template <class metaT>
class base_variable_class : public base_mapped_class<metaT>  
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

  public:
      variable_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~variable_class();


      static string_type type_name;


  protected:

  private:
      variable_class();



};

typedef pointers::reference<rx_platform::meta::variable_class> variable_class_ptr;





typedef base_complex_type< rx_platform::meta::source_class  > source_class_t;






class source_class : public source_class_t  
{

  public:
      virtual ~source_class();


      static string_type type_name;


  protected:

  private:
      source_class();



};






typedef base_complex_type< event_class  > event_class_t;






typedef base_complex_type< filter_class  > filter_class_t;






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
class internal_value_item : public internal_value  
{

  public:
      internal_value_item (const valT& value);

      internal_value_item (const valT& value, const string_type& name);

      virtual ~internal_value_item();


      void get_value (values::rx_value& val);


  protected:

  private:


      simple_const_value<valT> _storage;


};


// Parameterized Class rx_platform::meta::base_meta_type 

template <class metaT>
base_meta_type<metaT>::base_meta_type()
{
}

template <class metaT>
base_meta_type<metaT>::base_meta_type (const string_type& name, const rx_node_id& id, bool system)
	: _id(id),
	_system(system)
{
}


template <class metaT>
base_meta_type<metaT>::~base_meta_type()
{
}



template <class metaT>
bool base_meta_type<metaT>::serialize (base_meta_writter& stream) const
{
	if (!stream.write_id("NodeId", _id))
		return false;
	if (!stream.write_bool("System", _system))
		return false;
	return true;
}

template <class metaT>
bool base_meta_type<metaT>::deserialize (base_meta_reader& stream)
{
	if (!stream.read_id("NodeId", _id))
		return false;
	if (!stream.read_bool("System", _system))
		return false;
	return true;
}

template <class metaT>
string_type base_meta_type<metaT>::get_type_name () const
{
	return metaT::type_name;
}


// Parameterized Class rx_platform::meta::checkable_type 

template <class metaT>
checkable_type<metaT>::checkable_type()
{
}

template <class metaT>
checkable_type<metaT>::checkable_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system)
	: _name(name),
	base_meta_type<metaT>(name, id, system)
{
}


template <class metaT>
checkable_type<metaT>::~checkable_type()
{
}



template <class metaT>
bool checkable_type<metaT>::serialize_node (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const
{
  if (!stream.write_header(type))
		return false;

	if (!this->serialize_definition(stream, type))
		return false;

	if (!stream.write_footer())
		return false;

	return true;
}

template <class metaT>
bool checkable_type<metaT>::deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value)
{
  return false;
}

template <class metaT>
bool checkable_type<metaT>::check_in (base_meta_reader& stream)
{
	return false;
}

template <class metaT>
bool checkable_type<metaT>::check_out (base_meta_writter& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_CHECKOUT))
		return false;

	if (!this->serialize_definition(stream, STREAMING_TYPE_CHECKOUT))
		return false;

	if (!stream.write_footer())
		return false;

	return true;
}

template <class metaT>
bool checkable_type<metaT>::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!base_meta_type<metaT>::serialize(stream))
		return false;

	if (!stream.write_id("SuperId", _parent))
		return false;
	if (!stream.write_string("Name", _name.c_str()))
		return false;

	return true;
}

template <class metaT>
bool checkable_type<metaT>::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!base_meta_type<metaT>::deserialize(stream))
		return false;

	if (!stream.read_id("Parent", _parent))
		return false;
	if (!stream.read_string("Name", _name))
		return false;

	return true;
}

template <class metaT>
bool checkable_type<metaT>::generate_json (std::ostream& def, std::ostream& err) const
{
	err << "Function not implemented for this type.";
	return false;
}


// Parameterized Class rx_platform::meta::base_complex_type 

template <class metaT>
base_complex_type<metaT>::base_complex_type()
{
}

template <class metaT>
base_complex_type<metaT>::base_complex_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: checkable_type<metaT>(name, id, parent, system)
	, _sealed(sealed)
	, _abstract(abstract)
{
}


template <class metaT>
base_complex_type<metaT>::~base_complex_type()
{
	for (auto one : _internal_values)
		delete one;
}



template <class metaT>
bool base_complex_type<metaT>::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!checkable_type<metaT>::serialize_definition(stream, type))
		return false;

	if (!stream.write_bool("Sealed", _sealed))
		return false;

	if (!stream.write_bool("Abstract", _abstract))
		return false;

	if (!stream.start_array("Const", _const_values.size()))
		return false;
	for (const auto& one : _const_values)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one->serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("Vals", _internal_values.size()))
		return false;
	for (const auto& one : _internal_values)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one->serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("Structs", _structs.size()))
		return false;
	for (const auto& one : _structs)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("Vars", _variables.size()))
		return false;
	for (const auto& one : _variables)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	return true;
}

template <class metaT>
bool base_complex_type<metaT>::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!checkable_type<metaT>::deserialize_definition(stream, type))
		return false;

	if (!stream.read_bool("Sealed", _sealed))
		return false;

	return true;
}

template <class metaT>
bool base_complex_type<metaT>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writter writter;

	writter.write_header(STREAMING_TYPE_CLASS);

	this->serialize_definition(writter, STREAMING_TYPE_CLASS);

	writter.write_footer();

	string_type result;
	bool out = writter.get_string(result, true);

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return true;
}

template <class metaT>
bool base_complex_type<metaT>::register_internal_value (internal_value* item)
{
	if (check_name(item->get_name()))
	{
		_internal_values.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_complex_type<metaT>::register_struct (const struct_attribute& item)
{
	if (check_name(item.get_name()))
	{
		_structs.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_complex_type<metaT>::register_variable (const variable_attribute& item)
{
	if (check_name(item.get_name()))
	{
		_variables.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_complex_type<metaT>::check_name (const string_type& name)
{
	auto it = _names_cache.find(name);
	if (it == _names_cache.end())
	{
		_names_cache.emplace(name);
		return true;
	}
	else
	{
		return false;
	}
}


// Parameterized Class rx_platform::meta::base_mapped_class 

template <class metaT>
base_mapped_class<metaT>::base_mapped_class()
{
}

template <class metaT>
base_mapped_class<metaT>::base_mapped_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: base_complex_type<metaT>(name,id,parent,system,sealed,abstract)
{
}


template <class metaT>
base_mapped_class<metaT>::~base_mapped_class()
{
}



template <class metaT>
bool base_mapped_class<metaT>::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!base_complex_type<metaT>::serialize_definition(stream, type))
		return false;

	if (!stream.start_array("Mappers", _mappers.size()))
		return false;
	for (const auto& one : _mappers)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	return true;
}

template <class metaT>
bool base_mapped_class<metaT>::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!base_complex_type<metaT>::deserialize_definition(stream, type))
		return false;

	return true;
}

template <class metaT>
bool base_mapped_class<metaT>::register_mapper (const mapper_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		_mappers.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}


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


// Parameterized Class rx_platform::meta::base_variable_class 

template <class metaT>
base_variable_class<metaT>::base_variable_class()
{
}

template <class metaT>
base_variable_class<metaT>::base_variable_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: base_mapped_class<metaT>(name,id,parent,system,sealed,abstract)
{
}


template <class metaT>
base_variable_class<metaT>::~base_variable_class()
{
}



template <class metaT>
bool base_variable_class<metaT>::register_source (const source_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		_sources.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_variable_class<metaT>::register_filter (const filter_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		_filters.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_variable_class<metaT>::register_event (const event_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		_events.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}


// Parameterized Class rx_platform::meta::internal_value_item 

template <typename valT>
internal_value_item<valT>::internal_value_item (const valT& value)
{
}

template <typename valT>
internal_value_item<valT>::internal_value_item (const valT& value, const string_type& name)
{
}


template <typename valT>
internal_value_item<valT>::~internal_value_item()
{
}



template <typename valT>
void internal_value_item<valT>::get_value (values::rx_value& val)
{
}


} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {

template <class metaT>
template <typename constT>
bool base_complex_type<metaT>::register_const_value(const string_type& name, const constT& value)
{
    typedef class_const_value<constT> const_t;

	auto it = _names_cache.find(name);
	if (it == _names_cache.end())
	//if (check_name(name))
	{
		_const_values.emplace_back(new const_t(value, name));
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
