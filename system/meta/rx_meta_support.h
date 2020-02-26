

/****************************************************************************
*
*  system\meta\rx_meta_support.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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



// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"

namespace rx_platform {
namespace runtime {
namespace relations {
class relation_runtime;

} // namespace relations
} // namespace runtime
} // namespace rx_platform


#include "system/server/rx_ns.h"


namespace rx_platform {

namespace meta {





class type_check_context 
{
  public:
	  type_check_context() = default;
	  ~type_check_context() = default;
  	  type_check_context(const type_check_context&) = default;
	  type_check_context(type_check_context&&) = default;
	  type_check_context& operator=(const type_check_context&) = default;
   	  type_check_context& operator=(type_check_context&&) = default;

  public:

      bool is_check_ok () const;

      void add_error (const string_type& error);

      void reinit ();

      void add_error (const string_type& msg, const rx_result& error);


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
    typedef std::vector<pointers::reference<runtime::relations::relation_runtime> > additional_relations_type;

	typedef std::vector<runtime::structure::index_data> items_type;

  public:

      void add_const_value (const string_type& name, rx_simple_value value);

      void add_value (const string_type& name, rx_timed_value value, bool read_only);

      void add (const string_type& name, runtime::structure::mapper_data&& value);

      void add (const string_type& name, runtime::structure::struct_data&& value);

      void add_variable (const string_type& name, runtime::structure::variable_data&& value, rx_value val);

      void add (const string_type& name, runtime::structure::source_data&& value);

      void add (const string_type& name, runtime::structure::filter_data&& value);

      void add (const string_type& name, runtime::structure::event_data&& value);


      additional_relations_type additional_relations;


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

  public:
      construct_context();


      void reinit ();


      ns::rx_directory_resolver& get_directories ()
      {
        return directories_;
      }



      runtime_data_prototype runtime_data;


      rx_time now;


  protected:

  private:


      ns::rx_directory_resolver directories_;


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
	type_create_context() = default;
	~type_create_context() = default;
	type_create_context(const type_create_context&) = default;
	type_create_context(type_create_context&&) = default;
	type_create_context& operator=(const type_create_context&) = default;
	type_create_context& operator=(type_create_context&&) = default;

  public:

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


} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {
runtime::structure::runtime_item::smart_ptr create_runtime_data(runtime_data_prototype& prototype);
}
}


#endif
