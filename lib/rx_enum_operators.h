

/****************************************************************************
*
*  lib\rx_enum_operators.h
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_enum_operators_h
#define rx_enum_operators_h 1




namespace rx {

template<typename ET, typename std::enable_if_t<std::is_enum<ET>::value, int> = 0>
constexpr inline ET& operator~(ET& value)
{
	value = static_cast<ET>(~static_cast<std::underlying_type_t<ET> >(value));
	return value;
}

template<typename ET, typename std::enable_if_t<std::is_enum<ET>::value, int> = 0>
constexpr inline ET operator|(ET left, ET right)
{
	return static_cast<ET>(static_cast<std::underlying_type_t<ET>>(left) | static_cast<std::underlying_type_t<ET>>(right));
}

template<typename ET, typename std::enable_if_t<std::is_enum<ET>::value, int> = 0>
constexpr inline ET operator&(ET left, ET right)
{
	return static_cast<ET>(static_cast<std::underlying_type_t<ET>>(left) & static_cast<std::underlying_type_t<ET>>(right));
}

}




#endif
