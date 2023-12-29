

/****************************************************************************
*
*  system\meta\rx_queries.h
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


#ifndef rx_queries_h
#define rx_queries_h 1




#include "api/rx_platform_api.h"
#include "lib/rx_ser_lib.h"
using namespace rx;


namespace rx_platform {

namespace meta {


namespace queries
{
class rx_query;
}
typedef std::shared_ptr<queries::rx_query> query_ptr;

namespace queries {





class rx_query 
{
	typedef std::map<string_type, std::function<query_ptr()> > registered_queries_type;

  public:
      virtual ~rx_query();


      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream) = 0;

      virtual const string_type& get_query_type () = 0;

      static rx_result init_query_types ();

      static void deinit_query_types ();

      static rx_result_with<query_ptr> create_query (base_meta_reader& stream);

      virtual rx_result do_query (api::query_result& result, const string_type& dir) = 0;


  protected:

  private:

      static rx_result_with<query_ptr> create_query_from_name (const string_type& type);



      static rx_query::registered_queries_type registered_queries_;


};






class derived_types_query : public rx_query  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, const string_type& dir);


      string_type type_name;

      string_type base_type;

      bool include_subtypes;

      static string_type query_name;

      string_type subfolder;


  protected:

  private:
	  template<typename T>
	  rx_result do_query(api::query_result& result, const string_type& dir, tl::type2type<T>);
	  template<typename T>
	  rx_result do_simple_query(api::query_result& result, const string_type& dir, tl::type2type<T>);
	  rx_result do_relation_query(api::query_result& result, const string_type& dir);
      rx_result do_data_query(api::query_result& result, const string_type& dir);

};






class runtime_objects_query : public rx_query  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, const string_type& dir);


      string_type type_name;

      string_type instance_name;

      string_type subfolder;

      static string_type query_name;

      rx_node_id instance;


  protected:

  private:


};






class translate_query : public rx_query  
{
	typedef std::vector<rx_item_reference> items_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, const string_type& dir);


      static string_type query_name;

      items_type items;


  protected:

  private:


};






class port_stack_query : public rx_query  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, const string_type& dir);


      string_type instance_name;

      string_type subfolder;

      rx_node_id instance;

      static string_type query_name;

      string_type type_name;


  protected:

  private:


};






class dependents_query : public rx_query  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, const string_type& dir);


      rx_item_reference item;

      static string_type query_name;

      string_type subfolder;


  protected:

  private:


};






class instaced_runtimes_query : public rx_query  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, const string_type& dir);


      rx_item_reference type_reference;

      static string_type query_name;

      string_type subfolder;

      string_type sub_query;


  protected:

  private:
      template<typename T>
      rx_result do_query(const rx_node_id& id, api::query_result& result, const string_type& dir, tl::type2type<T>);

};


} // namespace queries
} // namespace meta
} // namespace rx_platform



#endif
