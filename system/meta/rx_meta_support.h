

/****************************************************************************
*
*  system\meta\rx_meta_support.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_meta_support_h
#define rx_meta_support_h 1


// temporary error codes!!!
#define RX_ITEM_NOT_FOUND 0x801
#define RX_NO_INVERSE_NAME_FOUND 0x802


// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"

#include "system/server/rx_ns.h"


namespace rx_platform {

namespace meta {






class runtime_data_prototype 
{
	typedef std::vector<runtime::structure::const_value_data> const_values_type;
	typedef std::vector<runtime::structure::value_data> values_type;
	typedef std::vector<runtime::structure::variable_data> variables_type;
	typedef std::vector<runtime::structure::struct_data> structs_type;
	typedef std::vector<runtime::structure::source_data> sources_type;
	typedef std::vector<runtime::structure::mapper_data> mappers_type;
	typedef std::vector<runtime::structure::filter_data> filters_type;
	typedef std::vector<runtime::structure::event_data> events_type;

	typedef std::vector<runtime::structure::index_data> items_type;

  public:

      void add_const_value (const string_type& name, rx_simple_value value);

      void add_value (const string_type& name, rx_timed_value value, bool read_only);

      void add (const string_type& name, runtime::structure::mapper_data&& value);

      void add (const string_type& name, runtime::structure::struct_data&& value);

      void add_variable (const string_type& name, runtime::structure::variable_data&& value);

      void add (const string_type& name, runtime::structure::source_data&& value);

      void add (const string_type& name, runtime::structure::filter_data&& value);

      void add (const string_type& name, runtime::structure::event_data&& value);


      items_type items;

      const_values_type const_values;

      values_type values;

      variables_type variables;

      structs_type structs;

      sources_type sources;

      mappers_type mappers;

      filters_type filters;

      events_type events;


  protected:

  private:

      bool check_name (const string_type& name) const;



};







class construct_context 
{
    typedef std::stack<data::runtime_values_data*, std::vector<data::runtime_values_data*> > override_stack_type;
    typedef std::vector<runtime_data_prototype> runtime_data_type;
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


      ns::rx_directory_resolver& get_directories ()
      {
        return directories_;
      }



      rx_time now;


  protected:

  private:


      runtime_data_type runtime_data_;


      ns::rx_directory_resolver directories_;

      string_array rt_names_;

      override_stack_type overrides_stack_;

      data::runtime_values_data overrides_;


};






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


} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {
runtime::structure::runtime_item::smart_ptr create_runtime_data(runtime_data_prototype& prototype);
}
}


#endif
