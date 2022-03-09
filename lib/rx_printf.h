

/****************************************************************************
*
*  lib\rx_printf.h
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


#ifndef rx_printf_h
#define rx_printf_h 1




namespace rx
{

template<typename T1>
void rx_sprintf_internal(std::ostringstream& ss, T1 t1)
{
	ss << t1;
}

template<typename T1, typename... Args>
void rx_sprintf_internal(std::ostringstream& ss, T1 t1, Args... args)
{
	ss << t1;
	rx_sprintf_internal(ss, args...);
}

template<typename... Args>
std::string rx_sprintf(Args&&... args)
{
	string_type str;
	str.reserve(0x100);
	std::ostringstream ss(str);

	rx_sprintf_internal(ss, args...);

	return ss.str();
	
}




}// namespace rx


#endif
