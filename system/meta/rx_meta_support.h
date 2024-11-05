

/****************************************************************************
*
*  system\meta\rx_meta_support.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_meta_support_h
#define rx_meta_support_h 1



#include "system/rx_platform_typedefs.h"
#include "system/server/rx_ns_resolver.h"
#include "system/runtime/rx_runtime_logic.h"
#include "system/runtime/rx_display_blocks.h"


// temporary error codes!!!
#define RX_ITEM_NOT_FOUND 0x801
#define RX_NO_INVERSE_NAME_FOUND 0x802

namespace rx_platform
{

namespace api
{
struct rx_context;
struct query_result_detail;
struct query_result;
}

namespace meta
{
struct object_type_creation_data;
struct type_creation_data;

}


meta_data create_type_meta_data(const meta::object_type_creation_data& type_data);

meta_data create_type_meta_data(const meta::type_creation_data& type_data);

rx_result_with<rx_storage_ptr> resolve_storage(const meta_data& data);



}// namespace rx_platform

// rx_rt_item_types
#include "system/runtime/rx_rt_item_types.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"

namespace rx_platform {
namespace meta {
class construct_context;

} // namespace meta
} // namespace rx_platform




namespace rx_platform {

namespace meta {





struct object_type_creation_data 
{


      string_type name;

      rx_node_id id;

      rx_node_id base_id;

      namespace_item_attributes attributes;

      string_type path;

  public:

  protected:

  private:


};






struct type_creation_data 
{


      string_type name;

      rx_node_id id;

      rx_node_id base_id;

      namespace_item_attributes attributes;

      string_type path;

  public:

  protected:

  private:


};






class type_create_context 
{
public:
	~type_create_context() = default;
	type_create_context(const type_create_context&) = delete;
	type_create_context(type_create_context&&) = delete;
	type_create_context& operator=(const type_create_context&) = delete;
	type_create_context& operator=(type_create_context&&) = delete;

  public:
      type_create_context ();


      bool created () const;

      void add_error (const string_type& error);

      void reinit ();


      const string_array& get_errors () const
      {
        return errors_;
      }


      ns::rx_directory_resolver& get_directories ()
      {
        return directories_;
      }



  protected:

  private:


      string_array errors_;

      ns::rx_directory_resolver directories_;


};


enum class check_record_type
{
    info,
    warning,
    error
};

const rx_error_severity_t rx_tolerable_severity = 16;
const rx_error_severity_t rx_low_severity = 64;
const rx_error_severity_t rx_medium_severity = 128;
const rx_error_severity_t rx_high_severity = 192;
const rx_error_severity_t rx_critical_severity = 255;




struct check_record 
{


      check_record_type type;

      rx_error_code_t code;

      rx_error_severity_t severity;

      string_type source;

      string_type text;

  public:

  protected:

  private:


};


typedef std::vector<check_record> check_records_type;
struct check_type_result
{
    check_records_type records;
    operator bool() const { return records.empty(); }
};




class type_check_context 
{
    typedef std::stack<string_type, std::vector<string_type> >sources_stack_type;
  public:
	  ~type_check_context() = default;
  	  type_check_context(const type_check_context&) = delete;
	  type_check_context(type_check_context&&) = delete;
	  type_check_context& operator=(const type_check_context&) = delete;
   	  type_check_context& operator=(type_check_context&&) = delete;

  public:
      type_check_context ();


      bool is_check_ok () const;

      void add_error (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity);

      void reinit ();

      void add_error (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity, const rx_result& error);

      const check_records_type& get_records () const;

      check_records_type&& move_records ();

      void push_source (const string_type& source);

      void pop_source ();

      const string_type& current_source () const;

      void add_warning (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity);

      void add_warning (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity, const rx_result& error);

      void add_info (const string_type& msg);

      rx_result_erros_t get_errors () const;


      ns::rx_directory_resolver& get_directories ()
      {
        return directories_;
      }



  protected:

  private:


      check_records_type records_;


      ns::rx_directory_resolver directories_;

      sources_stack_type sources_stack_;


};






class type_check_source 
{

  public:
      type_check_source (const string_type& source, type_check_context* ctx);

      ~type_check_source();


  protected:

  private:


      type_check_context *ctx_;


};






class dependencies_context 
{

  public:

      ns::rx_directory_resolver directories;

      std::set<rx_node_id> cache;


  protected:

  private:


};






class config_part_container 
{
public:
    typedef std::vector<std::unique_ptr<runtime_data::object_runtime_data> > objects_type;
    typedef std::vector<std::unique_ptr<runtime_data::domain_runtime_data> > domains_type;
    typedef std::vector<std::unique_ptr<runtime_data::port_runtime_data> > ports_type;
    typedef std::vector<std::unique_ptr<runtime_data::application_runtime_data> > apps_type;


    typedef std::vector<rx_object_type_ptr> object_types_type;
    typedef std::vector<rx_port_type_ptr> port_types_type;
    typedef std::vector<rx_domain_type_ptr> domain_types_type;
    typedef std::vector<rx_application_type_ptr> app_types_type;

    typedef std::vector<struct_type_ptr> struct_types_type;
    typedef std::vector<variable_type_ptr> variable_types_type;

  public:

      rx_result serialize (const string_type& name, base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (const string_type& name, base_meta_reader& stream, uint8_t type);


      objects_type objects;

      domains_type domains;

      ports_type ports;

      apps_type apps;

      object_types_type object_types;

      port_types_type port_types;

      domain_types_type domain_types;

      app_types_type app_types;

      struct_types_type struct_types;

      variable_types_type variable_types;


  protected:

  private:


};






class block_data_prototype 
{
    typedef std::vector<runtime::structure::array_wrapper<runtime::structure::const_value_data> > values_type;
    typedef std::vector<std::pair<rx_node_id, runtime::structure::array_wrapper<runtime::structure::block_data> > > children_type;

    typedef std::vector<runtime::structure::index_data> items_type;

  public:

      rx_result add_value (const string_type& name, rx_simple_value value);

      rx_result add_empty_array_value (const string_type& name, rx_simple_value value);

      rx_result add_value (const string_type& name, std::vector<values::rx_simple_value> value);

      rx_result add (const string_type& name, runtime::structure::block_data&& value, rx_node_id id);

      rx_result add_empty_array (const string_type& name, runtime::structure::block_data&& value, rx_node_id id);

      rx_result add (const string_type& name, std::vector<runtime::structure::block_data>&& value, rx_node_id id);

      runtime::structure::block_data create_block ();


      items_type items;

      values_type values;

      children_type children;


  protected:

  private:

      int check_member_name (const string_type& name) const;



};







class runtime_data_prototype 
{
	typedef std::vector<runtime::structure::array_wrapper<runtime::structure::const_value_data> > const_values_type;
    typedef std::vector<std::bitset<32> > const_values_opts_type;
	typedef std::vector< runtime::structure::array_wrapper<runtime::structure::value_data> > values_type;
	typedef std::vector<std::pair<rx_node_id, runtime::structure::array_wrapper<runtime::structure::variable_data> > > variables_type;
    typedef std::vector<runtime::structure::block_data> struct_blocks_type;
	typedef std::vector< std::pair<rx_node_id, runtime::structure::array_wrapper<runtime::structure::struct_data> > > structs_type;
	typedef std::vector< std::pair<rx_node_id, runtime::structure::source_data> > sources_type;
	typedef std::vector< std::pair<rx_node_id, runtime::structure::mapper_data> > mappers_type;
	typedef std::vector< std::pair<rx_node_id, runtime::structure::filter_data> > filters_type;
	typedef std::vector< std::pair<rx_node_id, runtime::structure::event_data> > events_type;
    typedef std::vector< std::pair<rx_node_id, runtime::structure::array_wrapper<runtime::structure::variable_block_data> > > variable_blocks_type;
    typedef std::vector< std::pair<rx_node_id, runtime::structure::array_wrapper<runtime::structure::value_block_data> > > blocks_type;

	typedef std::vector<runtime::structure::index_data> items_type;

  public:

      rx_result add_const_value (const string_type& name, rx_simple_value value, const std::bitset<32>& value_opt);

      rx_result add_const_value (const string_type& name, std::vector<values::rx_simple_value> value, const std::bitset<32>& value_opt);

      rx_result add_value (const string_type& name, rx_timed_value value, const std::bitset<32>& value_opt);

      rx_result add_value (const string_type& name, std::vector<rx_timed_value> value, const std::bitset<32>& value_opt);

      rx_result add (const string_type& name, runtime::structure::mapper_data&& value, rx_node_id id);

      rx_result add_struct (const string_type& name, runtime::structure::struct_data&& value, rx_node_id id, runtime::structure::block_data block);

      rx_result add_struct (const string_type& name, std::vector<runtime::structure::struct_data> value, rx_node_id id, runtime::structure::block_data block);

      rx_result add_variable (const string_type& name, runtime::structure::variable_data&& value, rx_node_id id);

      rx_result add_variable (const string_type& name, std::vector<runtime::structure::variable_data> value, rx_node_id id);

      rx_result add (const string_type& name, runtime::structure::source_data&& value, rx_node_id id);

      rx_result add (const string_type& name, runtime::structure::filter_data&& value, rx_node_id id);

      rx_result add (const string_type& name, runtime::structure::event_data&& value, rx_node_id id);

      rx_result add_variable_block (const string_type& name, runtime::structure::variable_block_data&& value, rx_node_id id);

      rx_result add_variable_block (const string_type& name, std::vector<runtime::structure::variable_block_data> value, rx_node_id id);

      rx_result add_value_block (const string_type& name, runtime::structure::value_block_data&& value, rx_node_id id);

      rx_result add_value_block (const string_type& name, std::vector<runtime::structure::value_block_data> value, rx_node_id id);

      runtime_data_prototype strip_normalized_prototype ();

      runtime::structure::block_data create_block_data ();


      items_type items;

      const_values_type const_values;

      values_type values;

      variables_type variables;

      structs_type structs;

      struct_blocks_type struct_blocks;

      sources_type sources;

      mappers_type mappers;

      filters_type filters;

      events_type events;

      const_values_opts_type const_values_opts;

      blocks_type blocks;

      variable_blocks_type variable_blocks;


  protected:

  private:

      int check_member_name (const string_type& name) const;


      bool check_read_only(const std::bitset<32>& to, const std::bitset<32>& from);
};

typedef std::vector<runtime_data_prototype> runtime_data_type;





class method_data_prototype 
{

  public:

      const rx_node_id get_inputs_id () const;
      void set_inputs_id (rx_node_id value);

      const rx_node_id& get_outputs_id () const;
      void set_outputs_id (const rx_node_id& value);


      runtime::logic_blocks::method_data method;

      runtime_data_type runtime_data;


      string_type name;


  protected:

  private:


      rx_node_id inputs_id_;

      rx_node_id outputs_id_;


};






class program_data_prototype 
{

  public:

      runtime::logic_blocks::program_data program;

      runtime_data_type runtime_data;


      string_type name;


  protected:

  private:


};






class display_data_prototype 
{

  public:

      runtime::display_blocks::display_data display;

      runtime_data_type runtime_data;


      string_type name;


  protected:

  private:


};






class object_data_prototype 
{
  public:

      typedef typename std::vector<method_data_prototype> methods_type;
      typedef typename std::vector<program_data_prototype> programs_type;
      typedef typename std::vector<display_data_prototype> displays_type;

  public:

      runtime_data_type runtime_data;

      methods_type methods;

      programs_type programs;

      displays_type displays;


  protected:

  private:


};







class construct_context 
{
    enum class active_state_t
    {
        regular = 0,
        in_method = 1,
        in_program = 2,
        in_display = 3
    };
    typedef std::stack<runtime::structure::block_data*, std::vector<runtime::structure::block_data*> > block_stack_type;
    typedef std::stack<data::runtime_values_data*, std::vector<data::runtime_values_data*> > override_stack_type;
    typedef std::vector<runtime_status_data> warnings_type;
 public:
    ~construct_context() = default;
    construct_context(const construct_context&) = delete;
    construct_context(construct_context&&) = delete;
    construct_context& operator=(const construct_context&) = delete;
    construct_context& operator=(construct_context&&) = delete;

  public:
      construct_context (const string_type& name);


      void reinit ();

      void push_overrides (const string_type& name, const data::runtime_values_data* vals);

      void pop_overrides ();

      const data::runtime_values_data* get_overrides ();

      const string_type& rt_name () const;

      void push_rt_name (const string_type& name);

      runtime_data_prototype pop_rt_name ();

      runtime_data_prototype& runtime_data ();

      runtime_data_type& runtime_stack ();

      void start_program (const string_type& name);

      void start_method (const string_type& name, rx_node_id& inputs_id, rx_node_id& outputs_id);

      void end_program (runtime::logic_blocks::program_data data);

      void end_method (runtime::logic_blocks::method_data data, rx_node_id inputs_id, rx_node_id outputs_id);

      runtime::logic_blocks::method_data& method_data ();

      runtime::logic_blocks::program_data& program_data ();

      void start_display (const string_type& name);

      void end_display (runtime::display_blocks::display_data data);

      runtime::display_blocks::display_data& display_data ();

      void register_warining (runtime_status_record data);

      object_data_prototype& object_data ();

      bool out_of_model ();

      void end_of_model_out (bool prev);

      bool is_in_model () const;


      ns::rx_directory_resolver& get_directories ()
      {
        return directories_;
      }



      rx_time now;

      runtime::structure::block_data changed_data_block;

      block_stack_type block_stack;

      runtime::structure::event_data* changed_event;


  protected:

  private:


      object_data_prototype runtime_data_;


      ns::rx_directory_resolver directories_;

      string_array rt_names_;

      override_stack_type overrides_stack_;

      data::runtime_values_data overrides_;

      active_state_t state_;

      warnings_type warnings_;

      int current_display_;

      int current_program_;

      int current_method_;

      bool in_model_;


};


} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {
runtime::structure::runtime_item::smart_ptr create_runtime_data(runtime_data_prototype& prototype);
void create_changed_data_block(runtime_data_prototype& vprototype);
}
}


#endif
