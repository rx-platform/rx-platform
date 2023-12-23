

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_data.h
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


#ifndef rx_data_h
#define rx_data_h 1




#include "lib/rx_values.h"
namespace data
{
//template <typename T, std::size_t... I>
//constexpr std::array<T, 8>
//generate(std::index_sequence<I...>)
//{
//	auto create =
//		[](std::size_t i) {
//		const auto& [arg0, arg1, arg2, arg3]
//			= generate_args((i >> 2) & 1, (i >> 1) & 1, (i >> 0) & 1);
//		return T{ arg0, arg1, arg2, arg3 };
//	};
//	return { create(I)... };
//}

template<typename T>
T create_initializer_list(const rx::values::rx_simple_value& val)
{
	T ret_value{};
	const auto& [arg0, arg1, arg2, arg3]
		= ret_value;
	return T{ arg0, arg1, arg2, arg3 };

	if (val.is_struct())
	{
		size_t size = val.struct_size();
		for (size_t i = 0; i < size; i++)
		{
			ret_value = val.extract_static(ret_value);
		}
	}
	return ret_value;
}
}




#endif
