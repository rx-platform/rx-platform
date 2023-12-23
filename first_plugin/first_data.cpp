

/****************************************************************************
*
*  first_plugin\first_data.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#include "pch.h"


// first_data
#include "first_plugin/first_data.h"

#include "lib/rx_values.h"
namespace data
{

void the_create_test_function()
{
	std::vector<rx::values::rx_simple_value> vals(3);
	vals[0].assign_static<int32_t>(55);
	values::rx_simple_value val;
	val.assign_static(std::move(vals));
//	test_struct struct_val = create_initializer_list<test_struct>(val);
}
}


