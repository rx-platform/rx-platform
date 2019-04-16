

/****************************************************************************
*
*  system\meta\rx_queries.h
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

      static rx_result_with<query_ptr> create_query (base_meta_reader& stream);

      virtual rx_result do_query (api::query_result& result, rx_directory_ptr dir) = 0;


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

      rx_result do_query (api::query_result& result, rx_directory_ptr dir);


      string_type type_name;

      string_type base_type;

      bool include_subtypes;

      static string_type query_name;


  protected:

  private:
	  template<typename T>
	  rx_result do_query(api::query_result& result, rx_directory_ptr dir, tl::type2type<T>);
	  template<typename T>
	  rx_result do_simple_query(api::query_result& result, rx_directory_ptr dir, tl::type2type<T>);

};






class runtime_objects_query : public rx_query  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, rx_directory_ptr dir);


      string_type type_name;

      string_type base_type;

      string_type subfolder;

      static string_type query_name;


  protected:

  private:


};


} // namespace queries
} // namespace meta
} // namespace rx_platform



#endif
