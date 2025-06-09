

/****************************************************************************
*
*  system\meta\rx_def_blocks.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_def_blocks_h
#define rx_def_blocks_h 1



// rx_security
#include "security/rx_security.h"
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



using namespace rx;
using namespace rx::values;

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
	class relation_type;

}
namespace basic_types
{
	class variable_type;
	class event_type;
	class filter_type;
	class source_type;
	class mapper_type;
	class struct_type;
    class data_type;
    class method_type;
    class program_type;
    class display_type;
}

namespace meta_algorithm
{
class complex_data_algorithm;
class mapped_data_algorithm;
class variable_data_algorithm;
class filtered_data_algorithm;
class event_blocks_algorithm;
class data_blocks_algorithm; 
class method_blocks_algorithm;
class data_types_algorithm;
class mapsrc_data_algorithm;
}

}

namespace runtime
{



namespace display_blocks
{
class display_data;
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
    rx_event_subitem = 4,

	rx_first_invalid_subitem = 5,
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
      const_value_def (const string_type& name, rx_simple_value&& value, bool config_only);

      const_value_def (const string_type& name, const rx_simple_value& value, bool config_only);

      const_value_def (const string_type& name, const rx_item_reference& type_id, bool is_array, bool config_only);


      rx_result serialize_definition (base_meta_writer& stream) const;

      rx_result deserialize_definition (base_meta_reader& stream);

      rx_simple_value get_value () const;

      std::vector<values::rx_simple_value> get_values () const;


      const string_type& get_name () const
      {
        return name_;
      }


      const bool get_read_only () const
      {
        return read_only_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const bool get_persistent () const
      {
        return persistent_;
      }


      const bool get_config_only () const
      {
        return config_only_;
      }


      const int get_array_size () const
      {
        return array_size_;
      }


      rx_item_reference get_data_type_ref () const
      {
        return data_type_ref_;
      }


      const security::security_guard& get_security_guard () const;


  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      bool read_only_;

      string_type description_;

      bool persistent_;

      bool config_only_;

      int array_size_;

      std::vector<values::rx_simple_value> values_;

      values::rx_simple_value value_;

      rx_item_reference data_type_ref_;


};







class event_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::event_type TargetType;
      template<class typeT>
      friend class meta_algorithm::meta_blocks_algorithm;
	  friend class meta_algorithm::event_blocks_algorithm;
      typedef meta_algorithm::event_blocks_algorithm AlgorithmType;

	  event_attribute(const event_attribute& right) = default;
	  event_attribute(event_attribute&& right) = default;
	  event_attribute() = default;
	  ~event_attribute() = default;

  public:
      event_attribute (const string_type& name, const rx_node_id& id, const rx_node_id& data_id = rx_node_id());

      event_attribute (const string_type& name, const string_type& target_name);


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      rx_item_reference get_arguments () const
      {
        return arguments_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      rx_item_reference target_;

      rx_item_reference arguments_;

      string_type description_;


};






class simple_value_def 
{
public:
	simple_value_def(const simple_value_def& right) = default;
	simple_value_def(simple_value_def&& right) = default;
	simple_value_def() = default;
	~simple_value_def() = default;

  public:
      simple_value_def (const string_type& name, rx_simple_value&& value, bool read_only, bool persistent);

      simple_value_def (const string_type& name, const rx_simple_value& value, bool read_only, bool persistent);

      simple_value_def (const string_type& name, const rx_item_reference& type_id, bool is_array, bool read_only, bool persistent);


      rx_result serialize_definition (base_meta_writer& stream) const;

      rx_result deserialize_definition (base_meta_reader& stream);

      rx_timed_value get_value (rx_time now) const;

      std::vector<values::rx_timed_value> get_values (rx_time now) const;


      const string_type& get_name () const
      {
        return name_;
      }


      const bool get_read_only () const
      {
        return read_only_;
      }


      const bool get_persistent () const
      {
        return persistent_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const int get_array_size () const
      {
        return array_size_;
      }


      rx_item_reference get_data_type_ref () const
      {
        return data_type_ref_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      bool read_only_;

      bool persistent_;

      string_type description_;

      int array_size_;

      std::vector<values::rx_simple_value> values_;

      values::rx_simple_value value_;

      rx_item_reference data_type_ref_;


};


class struct_attribute;

typedef std::vector<std::pair<string_type, int> > names_cahce_type;




class complex_data_type 
{
	
	typedef std::vector<const_value_def> const_values_type;
	typedef std::vector<simple_value_def> simple_values_type;
	typedef std::vector<struct_attribute> structs_type;
	typedef std::vector<variable_attribute> variables_type;
    typedef std::vector<event_attribute> events_type;

    friend class rx_platform::meta::meta_algorithm::complex_data_algorithm;


  public:

      rx_result register_struct (const string_type& name, const rx_node_id& id);

      rx_result register_variable (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only, bool persistent);

      rx_result register_event (const def_blocks::event_attribute& what);

      rx_result register_event (const string_type& name, const rx_node_id& id, const rx_node_id& data_id);

      rx_result register_simple_value (const string_type& name, rx_simple_value&& val, bool read_only, bool persistent);

      rx_result register_const_value (const string_type& name, rx_simple_value&& val, bool config_only = false);

      rx_result register_simple_value (const string_type& name, const rx_simple_value& val, bool read_only, bool persistent);

      rx_result register_const_value (const string_type& name, const rx_simple_value& val, bool config_only = false);

      rx_result register_simple_value (const string_type& name, const rx_item_reference& ref, bool is_array, bool read_only, bool persistent);

      rx_result register_const_value (const string_type& name, const rx_item_reference& ref, bool is_array, bool config_only = false);

      rx_result check_name (const string_type& name, int rt_index);


      const const_values_type& get_const_values () const
      {
        return const_values_;
      }


      const events_type& events () const
      {
        return events_;
      }



      const names_cahce_type& get_names_cache () const
      {
        return names_cache_;
      }



      rx::data::runtime_values_data overrides;

      security::security_guard security_guard;


      bool is_sealed;

      bool is_abstract;

      string_type description;

	  template <typename constT>
	  bool register_const_value_static(const string_type& name, constT&& value, bool config_only = false);
	  template <typename valT>
	  bool register_simple_value_static(const string_type& name, valT&& value, bool read_only, bool persistent);
	  template <typename valT>
	  bool register_variable_static(const string_type& name, const rx_node_id& id, valT&& value, bool read_only, bool persistent);

	  
	  static constexpr const int structs_mask =			0x01000000;
	  static constexpr const int simple_values_mask =	0x02000000;
	  static constexpr const int const_values_mask =	0x03000000;
	  static constexpr const int variables_mask =		0x04000000;
	  static constexpr const int mappings_mask =		0x05000000;
	  static constexpr const int sources_mask =			0x06000000;
	  static constexpr const int events_mask =			0x07000000;
	  static constexpr const int filters_mask =			0x08000000;

      static constexpr const int datas_mask =           0x09000000;

      static constexpr const int methods_mask =         0x0a000000;
      static constexpr const int programs_mask =        0x0b000000;
      static constexpr const int displays_mask =        0x0c000000;

      static constexpr const int relations_mask =       0x0e000000;

	  static constexpr const int type_mask =			0xff000000;
	  static constexpr const int index_mask =			0x00ffffff;

  protected:

  private:


      const_values_type const_values_;

      simple_values_type simple_values_;

      structs_type structs_;

      variables_type variables_;

      events_type events_;


      names_cahce_type names_cache_;


};






class struct_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::struct_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;
      typedef meta_algorithm::meta_blocks_algorithm<struct_attribute> AlgorithmType;

	  struct_attribute(const struct_attribute& right) = default;
	  struct_attribute(struct_attribute&& right) = default;
	  struct_attribute() = default;
	  ~struct_attribute() = default;

  public:
      struct_attribute (const string_type& name, const rx_node_id& id);

      struct_attribute (const string_type& name, const string_type& target_name);


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const int get_array_size () const
      {
        return array_size_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      rx_item_reference target_;

      string_type description_;

      int array_size_;


};






class variable_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::variable_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;
      typedef meta_algorithm::meta_blocks_algorithm<variable_attribute> AlgorithmType;

	  variable_attribute(const variable_attribute& right) = default;
	  variable_attribute(variable_attribute&& right) = default;
	  variable_attribute() = default;
	  ~variable_attribute() = default;

  public:
      variable_attribute (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only);

      variable_attribute (const string_type& name, const string_type& target_name);


      rx_value get_value (rx_time now) const;

      std::vector<values::rx_value> get_values (rx_time now) const;


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const bool get_read_only () const
      {
        return read_only_;
      }


      const bool get_persistent () const
      {
        return persistent_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const int get_array_size () const
      {
        return array_size_;
      }


      rx_item_reference get_data_type_ref () const
      {
        return data_type_ref_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      rx_item_reference target_;

      bool read_only_;

      bool persistent_;

      string_type description_;

      int array_size_;

      std::vector<values::rx_simple_value> values_;

      values::rx_simple_value value_;

      rx_item_reference data_type_ref_;


};






struct io_attribute 
{


      bool input;

      bool output;

      bool simulation;

      bool process;

  public:

  protected:

  private:


};






class filter_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::filter_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;
      typedef meta_algorithm::meta_blocks_algorithm<filter_attribute> AlgorithmType;

	  filter_attribute(const filter_attribute& right) = default;
	  filter_attribute(filter_attribute&& right) = default;
	  filter_attribute() = default;
	  ~filter_attribute() = default;

  public:
      filter_attribute (const string_type& name, const rx_node_id& id);

      filter_attribute (const string_type& name, const string_type& target_name);


      const io_attribute& get_io () const
      {
        return io_;
      }



      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      io_attribute io_;


      string_type name_;

      rx_item_reference target_;

      string_type description_;


};






class mapper_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::mapper_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;
      typedef meta_algorithm::meta_blocks_algorithm<mapper_attribute> AlgorithmType;

	  mapper_attribute(const mapper_attribute& right) = default;
	  mapper_attribute(mapper_attribute&& right) = default;
	  mapper_attribute() = default;
	  ~mapper_attribute() = default;

  public:
      mapper_attribute (const string_type& name, const rx_node_id& id);

      mapper_attribute (const string_type& name, const string_type& target_name);


      const io_attribute& get_io () const
      {
        return io_;
      }



      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      io_attribute io_;


      string_type name_;

      rx_item_reference target_;

      string_type description_;


};






class source_attribute 
{
  public:
	  typedef rx_platform::meta::basic_types::source_type TargetType;
	  template<class typeT>
	  friend class meta_algorithm::meta_blocks_algorithm;
      typedef meta_algorithm::meta_blocks_algorithm<source_attribute> AlgorithmType;

	  source_attribute(const source_attribute& right) = default;
	  source_attribute(source_attribute&& right) = default;
	  source_attribute() = default;
	  ~source_attribute() = default;

  public:
      source_attribute (const string_type& name, const rx_node_id& id);

      source_attribute (const string_type& name, const string_type& target_name);


      const io_attribute& get_io () const
      {
        return io_;
      }



      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      io_attribute io_;


      string_type name_;

      rx_item_reference target_;

      string_type description_;


};







class mapsrc_data_type 
{
    typedef std::vector<source_attribute> sources_type;
	typedef std::vector<mapper_attribute> mappers_type;

    friend class rx_platform::meta::meta_algorithm::mapsrc_data_algorithm;

  public:
      mapsrc_data_type();

      ~mapsrc_data_type();


      rx_result register_mapper (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);

      rx_result register_source (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);


  protected:

  private:


      mappers_type mappers_;

      sources_type sources_;


};







class variable_data_type 
{
	typedef std::vector<filter_attribute> filters_type;


    friend class rx_platform::meta::meta_algorithm::variable_data_algorithm;

  public:

      rx_result register_filter (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);


  protected:

  private:


      filters_type filters_;


};






class filtered_data_type 
{
    typedef std::vector<filter_attribute> filters_type;

    friend class rx_platform::meta::meta_algorithm::filtered_data_algorithm;

  public:

      rx_result register_filter (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);


  protected:

  private:


      filters_type filters_;


};






class data_attribute 
{
public:
    typedef rx_platform::meta::basic_types::data_type TargetType;
    friend class meta_algorithm::data_blocks_algorithm;
    typedef meta_algorithm::data_blocks_algorithm AlgorithmType;

    data_attribute(const data_attribute& right) = default;
    data_attribute(data_attribute&& right) noexcept = default;
    data_attribute& operator=(const data_attribute & right) = default;
    data_attribute& operator=(data_attribute && right) noexcept = default;
    data_attribute() = default;
    ~data_attribute() = default;

  public:
      data_attribute (const string_type& name, const rx_node_id& id, bool is_array = false);

      data_attribute (const string_type& name, const string_type& target_name, bool is_array = false);


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const int get_array_size () const
      {
        return array_size_;
      }



  protected:

  private:


      string_type name_;

      rx_item_reference target_;

      string_type description_;

      int array_size_;


};






class method_attribute 
{
public:
    typedef rx_platform::meta::basic_types::method_type TargetType;
    template<class typeT>
    friend class meta_algorithm::meta_blocks_algorithm;
    friend class meta_algorithm::method_blocks_algorithm;
    typedef meta_algorithm::method_blocks_algorithm AlgorithmType;

    method_attribute(const method_attribute& right) = default;
    method_attribute(method_attribute && right) = default;
    method_attribute() = default;
    ~method_attribute() = default;

  public:
      method_attribute (const string_type& name, const rx_node_id& id);

      method_attribute (const string_type& name, const string_type& target_name);


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      rx_item_reference get_inputs () const
      {
        return inputs_;
      }


      rx_item_reference get_outputs () const
      {
        return outputs_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      rx_item_reference target_;

      rx_item_reference inputs_;

      rx_item_reference outputs_;

      string_type description_;


};






class program_attribute 
{
public:
    typedef rx_platform::meta::basic_types::program_type TargetType;
    template<class typeT>
    friend class meta_algorithm::meta_blocks_algorithm;
    typedef meta_algorithm::meta_blocks_algorithm<program_attribute> AlgorithmType;

    program_attribute(const program_attribute& right) = default;
    program_attribute(program_attribute && right) = default;
    program_attribute() = default;
    ~program_attribute() = default;

  public:
      program_attribute (const string_type& name, const rx_node_id& id);

      program_attribute (const string_type& name, const string_type& target_name);


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      rx_item_reference target_;

      string_type description_;


};






class display_attribute 
{
public:
    typedef rx_platform::meta::basic_types::display_type TargetType;
    template<class typeT>
    friend class meta_algorithm::meta_blocks_algorithm;
    typedef meta_algorithm::meta_blocks_algorithm<display_attribute> AlgorithmType;

    display_attribute(const display_attribute& right) = default;
    display_attribute(display_attribute && right) = default;
    display_attribute() = default;
    ~display_attribute() = default;

  public:
      display_attribute (const string_type& name, const rx_node_id& id);

      display_attribute (const string_type& name, const string_type& target_name);


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const security::security_guard& get_security_guard () const
      {
        return security_guard;
      }



  protected:

  private:

      security::security_guard security_guard;



      string_type name_;

      rx_item_reference target_;

      string_type description_;


};






class data_value_def 
{
public:
    data_value_def(const data_value_def& right) = default;
    data_value_def(data_value_def&& right) = default;
    data_value_def() = default;
    ~data_value_def() = default;

  public:
      data_value_def (const string_type& name, rx_simple_value&& value);

      data_value_def (const string_type& name, const rx_simple_value& value);


      rx_result serialize_definition (base_meta_writer& stream) const;

      rx_result deserialize_definition (base_meta_reader& stream);

      rx_simple_value get_value () const;

      std::vector<values::rx_simple_value> get_values () const;


      const string_type& get_name () const
      {
        return name_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      const int get_array_size () const
      {
        return array_size_;
      }



  protected:

  private:


      string_type name_;

      string_type description_;

      int array_size_;

      std::vector<values::rx_simple_value> values_;

      values::rx_simple_value value_;


};






class data_type_def 
{
    typedef std::vector<def_blocks::data_value_def> values_type;
    typedef std::vector<def_blocks::data_attribute> children_type;

    friend class meta_algorithm::data_types_algorithm;

  public:

      rx_result register_child (const string_type& name, const rx_item_reference& ref, bool is_array);

      rx_result register_value (const string_type& name, rx_simple_value&& val);

      rx_result register_value (const string_type& name, const rx_simple_value& val);

      rx_result check_name (const string_type& name, int rt_index);


      const children_type& get_children () const
      {
        return children_;
      }


      const values_type& get_values () const
      {
        return values_;
      }


      const rx::data::runtime_values_data& get_overrides () const
      {
        return overrides_;
      }



      const names_cahce_type& get_names_cache () const
      {
        return names_cache_;
      }


      const string_type& get_description () const
      {
        return description_;
      }


      template <typename valT>
      bool register_value_static(const string_type& name, valT&& value)
      {
          rx_simple_value temp;
          temp.assign_static(std::forward<valT>(value));
          return register_value(name, std::move(temp));
      }

      static constexpr const int simple_values_mask = 0x01000000;
      static constexpr const int child_values_mask = 0x02000000;

      static constexpr const int type_mask = 0xff000000;
      static constexpr const int index_mask = 0x00ffffff;
  protected:

  private:


      children_type children_;

      values_type values_;

      rx::data::runtime_values_data overrides_;


      names_cahce_type names_cache_;

      string_type description_;


};






class mapped_data_type 
{
    typedef std::vector<mapper_attribute> mappers_type;

    friend class rx_platform::meta::meta_algorithm::mapped_data_algorithm;

  public:
      mapped_data_type();

      ~mapped_data_type();


      rx_result register_mapper (const string_type& name, const rx_node_id& id, complex_data_type& complex_data);


  protected:

  private:


      mappers_type mappers_;


};


} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {
namespace def_blocks {

template <typename valT>
bool complex_data_type::register_const_value_static(const string_type& name, valT&& value, bool config_only)
{
	rx_simple_value temp;
	temp.assign_static(std::forward<valT>(value));
	return register_const_value(name, std::move(temp), config_only);
}
template <typename valT>
bool complex_data_type::register_simple_value_static(const string_type& name, valT&& value, bool read_only, bool persistent)
{
	rx_simple_value temp;
	temp.assign_static<valT>(std::forward<valT>(value));
	return register_simple_value(name, std::move(temp), read_only, persistent);
}
template <typename valT>
bool complex_data_type::register_variable_static(const string_type& name, const rx_node_id& id, valT&& value, bool read_only, bool persistent)
{
	rx_simple_value temp;
	temp.assign_static<valT>(std::forward<valT>(value));
	return register_variable(name, id, std::move(temp), read_only, persistent);
}


} // namespace def_blocks
} // namespace meta
} // namespace server rx_platform


#endif
