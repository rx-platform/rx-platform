

/****************************************************************************
*
*  lib\rx_rt_data.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_rt_data_h
#define rx_rt_data_h 1



// rx_values
#include "lib/rx_values.h"

using namespace rx;
using namespace rx::values;


namespace rx {

namespace data {





class runtime_values_data 
{

    typedef rx_simple_value value_type;
    typedef std::vector<rx_simple_value> array_value_type;
	typedef std::map<string_type, std::variant<value_type, array_value_type> > values_type;

    typedef runtime_values_data child_type;
    typedef std::vector<runtime_values_data> array_child_type;
	typedef std::map<string_type, std::variant<child_type, array_child_type> > children_type;


  public:

      void add_value (const string_type& name, const rx_simple_value& value);

      void add_value (const string_type& name, rx_simple_value&& value);

      void add_value (const string_type& name, std::vector<rx_simple_value> value);

      runtime_values_data* add_child (const string_type& name);

      void add_child (const string_type& name, runtime_values_data&& data);

      std::vector<runtime_values_data>* add_array_child (const string_type& name, size_t size);

      void add_array_child (const string_type& name, std::vector<runtime_values_data> data);

      rx_simple_value get_value (const string_type& path) const;

      bool get_array_value (const string_type& path, std::vector<rx_simple_value>& val) const;

      bool empty () const;

      void clear ();


      values_type values;

      children_type children;

	  template<typename T>
	  void add_value_static(const string_type& name, T&& value)
	  {
		  rx_simple_value temp;
		  temp.assign_static(std::forward<T>(value));
		  add_value(name, temp);
	  }
      template<typename T>
      T get_value_static(const string_type& name, const T& def)
      {
          rx_simple_value temp = get_value(name);
          return temp.extract_static<T>(def);
      }
  protected:

  private:

      string_view_type extract_index (string_view_type name, int& idx) const;



};


} // namespace data
} // namespace rx



#endif
