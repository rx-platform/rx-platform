

/****************************************************************************
*
*  lib\rx_std.h
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


#ifndef rx_std_h
#define rx_std_h 1




// general stuff
#include <stdint.h>
#include <stdio.h>
#include <atomic>
#include <assert.h>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <limits>
#include <memory>
#include <complex>
#include <utility>
#include <type_traits>
#include <variant>
#include <cstddef>
// containers
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <list>
#include <stack>
#include <array>
#include <bitset>
//streams
#include <ios>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <future>
#include <charconv>


using namespace std::placeholders;
using namespace std::string_literals;



#include "version/rx_version.h"
#include "lib/rx_lib_version.h"

#include "type_lists.h"



template<int> struct compile_time_error;
template<> struct compile_time_error<true> {};
#define STATIC_CHECK_MSG(expr,msg)  {compile_time_error< ((expr)!=0) > Error_##msg; (void)Error_##msg; }
#define STATIC_CHECK(expr)  {compile_time_error< ((expr)!=0) > Error_StaticCheck; (void)Error_StaticCheck; }


typedef std::string string_type;
typedef std::string_view string_view_type;
typedef std::vector<std::byte> byte_string;
typedef std::vector<bool> bit_string;
typedef std::vector<string_type> string_vector;
typedef std::vector<string_type> string_array;







#endif
