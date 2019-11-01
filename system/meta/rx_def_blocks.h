

/****************************************************************************
*
*  system\meta\rx_def_blocks.h
*
*  Copyright (c) 2018-2019 Dusan Ciric
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_def_blocks_h
#define rx_def_blocks_h 1



// rx_meta_algorithm
#include "system/meta/rx_meta_algorithm.h"
// rx_meta_support
#include "system/meta/rx_meta_support.h"
// rx_rt_data
#include "lib/rx_rt_data.h"

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
#include "system/runtime/rx_blocks.h"

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
namespace object_types
{
	class object_type;
	class application_type;
	class domain_type;
	class port_type;

}
namespace basic_types
{
	class variable_type;
	class event_type;
	class filter_type;
	class source_type;
	class reference_type;
	class mapper_type;
	class struct_type;
}


}

namespace runtime
{

namespace blocks
{
class struct_runtime;
class variable_runtime;
class event_runtime;
class filter_runtime;
class mapper_runtime;
class source_runtime;

class const_value_item;
class value_item;
}

}

}//namespace server

using namespace rx;
using namespace rx::pointers;


namespace rx_platform {
bool rx_is_valid_item_name(const string_type& name);

namespace meta {

namespace def_blocks {
enum rx_subitem_type : uint8_t
{
	rx_const_value_subitem = 0,
	rx_value_subitem = 1,
	rx_struct_subitem = 2,
	rx_variable_subitem = 3,

	rx_first_invalid_subitem = 4,
	rx_invalid_subitem = 0xff
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


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_simple_value get_value () const;


      const string_type& get_name () const
      {
        return name_;
      }



  protected:

  private:


      string_type name_;

      values::rx_simple_value storage_;


};







class event_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::event_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;

	  event_attribute(const event_attribute& right) = default;
	  event_attribute(event_attribute&& right) = default;
	  event_attribute() = default;
	  ~event_attribute() = default;

  public:
      event_attribute (const string_type& name, const rx_node_id& id);

      event_attribute (const string_type& name, const string_type& target_name);


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_result check (type_check_context& ctx);

      rx_result construct (construct_context& ctx) const;


      const string_type& get_name () const
      {
        return name_;
      }


      item_reference get_target () const
      {
        return target_;
      }



  protected:

  private:


      string_type name_;

      item_reference target_;


};






class filter_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::filter_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;

	  filter_attribute(const filter_attribute& right) = default;
	  filter_attribute(filter_attribute&& right) = default;
	  filter_attribute() = default;
	  ~filter_attribute() = default;

  public:
      filter_attribute (const string_type& name, const rx_node_id& id);

      filter_attribute (const string_type& name, const string_type& target_name);


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_result check (type_check_context& ctx);

      rx_result construct (construct_context& ctx) const;


      const string_type& get_name () const
      {
        return name_;
      }


      item_reference get_target () const
      {
        return target_;
      }



  protected:

  private:


      string_type name_;

      item_reference target_;


};






class mapper_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::mapper_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;

	  mapper_attribute(const mapper_attribute& right) = default;
	  mapper_attribute(mapper_attribute&& right) = default;
	  mapper_attribute() = default;
	  ~mapper_attribute() = default;

  public:
      mapper_attribute (const string_type& name, const rx_node_id& id);

      mapper_attribute (const string_type& name, const string_type& target_name);


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_result check (type_check_context& ctx);

      rx_result construct (construct_context& ctx) const;


      const string_type& get_name () const
      {
        return name_;
      }


      item_reference get_target () const
      {
        return target_;
      }



  protected:

  private:


      string_type name_;

      item_reference target_;


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


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_timed_value get_value (rx_time now) const;


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

      values::rx_simple_value storage_;


};


class struct_attribute;

typedef std::unordered_map<string_type, int> names_cahce_type;





class complex_data_type 
{
	
	typedef std::vector<const_value_def> const_values_type;
	typedef std::vector<simple_value_def> simple_values_type;
	typedef std::vector<struct_attribute> structs_type;
	typedef std::vector<variable_attribute> variables_type;


  public:
      complex_data_type();

      complex_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);


      rx_result serialize_complex_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_complex_definition (base_meta_reader& stream, uint8_t type);

      rx_result register_struct (const string_type& name, const rx_node_id& id);

      rx_result register_variable (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only);

      rx_result check_name (const string_type& name, int rt_index);

      rx_result construct (construct_context& ctx) const;

      rx_result register_simple_value (const string_type& name, bool read_only, rx_simple_value&& val);

      rx_result register_const_value (const string_type& name, rx_simple_value&& val);

      rx_result register_simple_value (const string_type& name, bool read_only, const rx_simple_value& val);

      rx_result register_const_value (const string_type& name, const rx_simple_value& val);

      bool check_type (type_check_context& ctx);


      const const_values_type& get_const_values () const
      {
        return const_values_;
      }


      const rx::data::runtime_values_data& get_overrides () const
      {
        return overrides_;
      }



      const bool is_sealed () const
      {
        return sealed_;
      }

      void set_sealed (bool value)
      {
        sealed_ = value;
      }


      const names_cahce_type& get_names_cache () const
      {
        return names_cache_;
      }


      const bool is_abstract () const
      {
        return abstract_;
      }

      void set_abstract (bool value)
      {
        abstract_ = value;
      }


	  template <typename constT>
	  bool register_const_value_static(const string_type& name, constT&& value);
	  template <typename valT>
	  bool register_simple_value_static(const string_type& name, bool read_only, valT&& value);
	  template <typename valT>
	  bool register_variable_static(const string_type& name, const rx_node_id& id, valT&& value, bool read_only);

	  
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

      rx::data::runtime_values_data overrides_;


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


      rx_result serialize_mapped_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_mapped_definition (base_meta_reader& stream, uint8_t type, complex_data_type& complex_data);

      rx_result construct (const names_cahce_type& names, construct_context& ctx) const;

      rx_result register_mapper (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);

      bool check_type (type_check_context& ctx);


  protected:

  private:


      mappers_type mappers_;


};






class source_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::source_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;

	  source_attribute(const source_attribute& right) = default;
	  source_attribute(source_attribute&& right) = default;
	  source_attribute() = default;
	  ~source_attribute() = default;

  public:
      source_attribute (const string_type& name, const rx_node_id& id);

      source_attribute (const string_type& name, const string_type& target_name);


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_result check (type_check_context& ctx);

      rx_result construct (construct_context& ctx) const;


      const string_type& get_name () const
      {
        return name_;
      }


      item_reference get_target () const
      {
        return target_;
      }



  protected:

  private:


      string_type name_;

      item_reference target_;


};






class struct_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::struct_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;

	  struct_attribute(const struct_attribute& right) = default;
	  struct_attribute(struct_attribute&& right) = default;
	  struct_attribute() = default;
	  ~struct_attribute() = default;

  public:
      struct_attribute (const string_type& name, const rx_node_id& id);

      struct_attribute (const string_type& name, const string_type& target_name);


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_result check (type_check_context& ctx);

      rx_result construct (construct_context& ctx) const;


      const string_type& get_name () const
      {
        return name_;
      }


      item_reference get_target () const
      {
        return target_;
      }



  protected:

  private:


      string_type name_;

      item_reference target_;


};






class variable_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::variable_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;

	  variable_attribute(const variable_attribute& right) = default;
	  variable_attribute(variable_attribute&& right) = default;
	  variable_attribute() = default;
	  ~variable_attribute() = default;

  public:
      variable_attribute (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only);

      variable_attribute (const string_type& name, const string_type& target_name);


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_value get_value (rx_time now) const;

      rx_result check (type_check_context& ctx);

      rx_result construct (construct_context& ctx) const;


      const string_type& get_name () const
      {
        return name_;
      }


      const bool get_read_only () const
      {
        return read_only_;
      }


      item_reference get_target () const
      {
        return target_;
      }



  protected:

  private:


      string_type name_;

      bool read_only_;

      values::rx_simple_value storage_;

      item_reference target_;


};







class variable_data_type 
{
	typedef std::vector<source_attribute> sources_type;
	typedef std::vector<filter_attribute> filters_type;
	typedef std::vector<event_attribute> events_type;

  public:
      variable_data_type();

      variable_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);

      ~variable_data_type();


      rx_result construct (const names_cahce_type& names, construct_context& ctx) const;

      rx_result serialize_variable_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_variable_definition (base_meta_reader& stream, uint8_t type, complex_data_type& complex_data);

      rx_result register_source (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);

      rx_result register_filter (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);

      rx_result register_event (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);

      bool check_type (type_check_context& ctx);


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
	return register_const_value(name, std::move(temp));
}

template <typename valT>
bool complex_data_type::register_simple_value_static(const string_type& name, bool read_only, valT&& value)
{
	rx_simple_value temp;
	temp.assign_static<valT>(std::forward<valT>(value));
	return register_simple_value(name, read_only, std::move(temp));
}
template <typename valT>
bool complex_data_type::register_variable_static(const string_type& name, const rx_node_id& id, valT&& value, bool read_only)
{
	rx_simple_value temp;
	temp.assign_static<valT>(std::forward<valT>(value));
	return register_variable(name, id, std::move(temp), read_only);
}


} // namespace def_blocks
} // namespace meta
} // namespace server rx_platform


#endif
