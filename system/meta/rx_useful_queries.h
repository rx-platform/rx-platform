

/****************************************************************************
*
*  system\meta\rx_useful_queries.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_useful_queries_h
#define rx_useful_queries_h 1



// rx_queries
#include "system/meta/rx_queries.h"



namespace rx_platform {

namespace meta {

namespace queries {





class ns_suggetions_query : public rx_query  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_query_type ();

      rx_result do_query (api::query_result& result, const string_type& dir);


      string_type type_name;

      string_type instance_name;

      string_type suggested_path;

      static string_type query_name;

      rx_node_id instance;


  protected:

  private:


};


} // namespace queries
} // namespace meta
} // namespace rx_platform



#endif
