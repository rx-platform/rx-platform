

/****************************************************************************
*
*  lib\rx_rt_data.h
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


#ifndef rx_rt_data_h
#define rx_rt_data_h 1



// rx_values
#include "lib/rx_values.h"

using namespace rx;
using namespace rx::values;


namespace rx {

namespace data {





class runtime_value 
{

  public:

      values::rx_simple_value value;


  protected:

  private:


};






class runtime_values_data 
{
	typedef std::map<string_type, runtime_value> values_type;
	typedef std::map<string_type, runtime_values_data> children_type;

  public:

      void add_value (const string_type& name, const rx_simple_value& value);

      runtime_values_data& add_child (const string_type& name);

      rx_simple_value get_value (const string_type& path) const;


      values_type values;

      children_type children;

	  template<typename T>
	  void add_value_static(const string_type& name, T&& value)
	  {
		  rx_simple_value temp;
		  temp.assign_static(std::forward<T>(value));
		  add_value(name, temp);
	  }
  protected:

  private:


};


} // namespace data
} // namespace rx



#endif
