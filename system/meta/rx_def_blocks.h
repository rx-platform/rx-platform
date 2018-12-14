

/****************************************************************************
*
*  system\meta\rx_def_blocks.h
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


#ifndef rx_def_blocks_h
#define rx_def_blocks_h 1




namespace rx_platform {
namespace meta {
namespace def_blocks {
class variable_attribute;
class struct_attribute;
class mapper_attribute;

} // namespace def_blocks
} // namespace meta
} // namespace rx_platform


#include "system/server/rx_ns.h"
#include "system/serialization/rx_ser.h"
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
namespace object_defs
{
	class object_class;
	class application_class;
	class domain_class;
	class port_class;

}
namespace basic_defs
{
	class variable_class;
	class event_class;
	class filter_class;
	class source_class;
	class reference_type;
	class mapper_class;
	class struct_class;
}

}

namespace objects
{

namespace object_types
{
class object_runtime;
class domain_runtime;
class port_runtime;
class application_runtime;


}

namespace blocks
{
class struct_runtime;
class variable_runtime;

class const_value_item;
class value_item;
class complex_runtime_item;
}

}

typedef objects::blocks::complex_runtime_item* complex_runtime_ptr;
typedef objects::blocks::struct_runtime* struct_runtime_ptr;
typedef objects::blocks::variable_runtime* variable_runtime_ptr;
typedef rx_reference<pointers::reference_object> runtime_ptr_t;

}//namespace server

using namespace rx;
using namespace rx::pointers;


namespace rx_platform {

namespace meta {

namespace def_blocks {





class complex_class_attribute 
{

  public:
      complex_class_attribute (const string_type& name, const rx_node_id& id);

      virtual ~complex_class_attribute();


      virtual bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

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






class const_value_def 
{
public:
	const_value_def(const const_value_def& right) = default;
	const_value_def(const_value_def&& right) = default;
	const_value_def() = default;
	~const_value_def() = default;

  public:
      const_value_def (const string_type& name, rx_simple_value&& value);

      const_value_def (const string_type& name, const rx_simple_value& value);


      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_simple_value get_value () const;


      const string_type& get_name () const
      {
        return name_;
      }


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:


      string_type name_;

      values::rx_simple_value storage_;


};






class event_attribute : public complex_class_attribute  
{

  public:
      event_attribute (const string_type& name, const rx_node_id& id);

      virtual ~event_attribute();


      complex_runtime_ptr construct ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

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

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

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

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};






class simple_value_def 
{
public:
	simple_value_def(const simple_value_def& right) = default;
	simple_value_def(simple_value_def&& right) = default;
	simple_value_def() = default;
	~simple_value_def() = default;

  public:
      simple_value_def (const string_type& name, bool read_only, rx_simple_value&& value);

      simple_value_def (const string_type& name, bool read_only, const rx_simple_value& value);


      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_timed_value get_value (rx_time now) const;


      const bool get_read_only () const
      {
        return read_only_;
      }


      const string_type& get_name () const
      {
        return name_;
      }


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:


      bool read_only_;

      string_type name_;

      values::rx_simple_value storage_;


};


class struct_attribute;




class complex_data_type 
{

	typedef std::vector<const_value_def> const_values_type;
	typedef std::vector<simple_value_def> simple_values_type;
	typedef std::vector<std::unique_ptr<struct_attribute> > structs_type;
	typedef std::vector<std::unique_ptr<variable_attribute> > variables_type;


	typedef std::map<string_type, int> names_cahce_type;

  public:
      complex_data_type();

      complex_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);


      bool serialize_complex_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_complex_definition (base_meta_reader& stream, uint8_t type);

      bool register_struct (const string_type& name, const rx_node_id& id);

      bool register_variable (const string_type& name, const rx_node_id& id);

      bool check_name (const string_type& name, int rt_index);

      void construct (complex_runtime_ptr what);

      bool register_simple_value (const string_type& name, bool read_only, rx_simple_value&& val);

      bool register_const_value (const string_type& name, rx_simple_value&& val);

      bool register_simple_value (const string_type& name, bool read_only, const rx_simple_value& val);

      bool register_const_value (const string_type& name, const rx_simple_value& val);


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
	  bool register_const_value_static(const string_type& name, constT&& value);
	  template <typename valT>
	  bool register_simple_value_static(const string_type& name, bool read_only, valT&& value);

	  
	  static constexpr const int structs_mask =			0x01000000;
	  static constexpr const int simple_values_mask =	0x02000000;
	  static constexpr const int const_values_mask =	0x03000000;
	  static constexpr const int variables_mask =		0x04000000;
	  static constexpr const int mappings_mask =		0x05000000;
	  static constexpr const int sources_mask =			0x06000000;
	  static constexpr const int events_mask =			0x07000000;
	  static constexpr const int filters_mask =			0x08000000;

	  static constexpr const int type_mask =			0xff000000;
	  static constexpr const int index_mask =			0x00ffffff;

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


      bool serialize_mapped_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_mapped_definition (base_meta_reader& stream, uint8_t type);

      bool register_mapper (const mapper_attribute& item, complex_data_type& complex_data);


  protected:

  private:


      mappers_type mappers_;


};






class source_attribute : public complex_class_attribute  
{

  public:
      source_attribute (const string_type& name, const rx_node_id& id);

      virtual ~source_attribute();


      complex_runtime_ptr construct ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};






class struct_attribute : public complex_class_attribute  
{

  public:
      struct_attribute (const string_type& name, const rx_node_id& id);

      virtual ~struct_attribute();


      struct_runtime_ptr construct ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

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

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


};







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

      bool serialize_variable_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_variable_definition (base_meta_reader& stream, uint8_t type);


  protected:

  private:


      sources_type sources_;

      filters_type filters_;

      events_type events_;


};


} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {
namespace def_blocks {

template <typename valT>
bool complex_data_type::register_const_value_static(const string_type& name, valT&& value)
{
	rx_simple_value temp;
	temp.assign_static(std::forward<valT>(value));
	return register_const_value(name, temp);
}


template <typename valT>
bool complex_data_type::register_simple_value_static(const string_type& name, bool read_only, valT&& value)
{
	rx_simple_value temp;
	temp.assign_static<valT>(std::forward<valT>(value));
	return register_simple_value(name, read_only, temp);
}

} // namespace def_blocks
} // namespace meta
} // namespace server rx_platform


#endif
