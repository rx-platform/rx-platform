

/****************************************************************************
*
*  lib\rx_std.h
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
// containers
#include <vector>
#include <set>
#include <map>
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


using namespace std::placeholders;
using namespace std::string_literals;



#include "version/rx_version.h"
#include "lib/rx_lib_version.h"

#include "lib/type_lists.h"

#define DEFAULT_NAMESPACE 1
#define DEFAULT_OPC_NAMESPACE 0
#define RX_USER_NAMESPACE 999


#define TIME_QUALITY_LEAP_SECONDS_KNOWN 0x80
#define TIME_QUALITY_CLOCK_FALIURE 0x40
#define TIME_QUALITY_NOT_SYNCHRONIZED 0x20

#define TIME_QUALITY_CLASS_UNKNOWN 0x1f
#define TIME_QUALITY_CLASS_T0 7
#define TIME_QUALITY_CLASS_T1 10
#define TIME_QUALITY_CLASS_T2 14
#define TIME_QUALITY_CLASS_T3 16
#define TIME_QUALITY_CLASS_T4 18
#define TIME_QUALITY_CLASS_T5 20


template<int> struct compile_time_error;
template<> struct compile_time_error<true> {};
#define STATIC_CHECK_MSG(expr,msg)  {compile_time_error< ((expr)!=0) > Error_##msg; (void)Error_##msg; }
#define STATIC_CHECK(expr)  {compile_time_error< ((expr)!=0) > Error_StaticCheck; (void)Error_StaticCheck; }


typedef std::string string_type;
typedef std::vector<uint8_t> byte_string;
typedef std::vector<bool> bit_string;
typedef std::vector<string_type> string_vector;
typedef std::vector<string_type> string_array;







#endif
