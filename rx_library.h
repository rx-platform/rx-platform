

/****************************************************************************
*
*  rx_library.h
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


#ifndef rx_library_h
#define rx_library_h 1





// platform specific definitions
#include "os_itf/rx_ositf.h"

// C++ standard library and some mcro definitions
#include "lib/rx_std.h"

// enum bitwise helpers
#include "lib/rx_enum_operators.h"

//library it self
#include "lib/rx_lib.h"

// smart pointers
#include "lib/rx_ptr.h"

// memory buffers
#include "lib/rx_mem.h"

#include "lib/rx_meta_data.h"





using namespace rx;


#endif
