

/****************************************************************************
*
*  system\libraries\cpp_lib.cpp
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


#include "pch.h"


// cpp_lib
#include "system/libraries/cpp_lib.h"



namespace rx_platform {

namespace library {

// Class rx_platform::library::cpp_classes_manager 

cpp_classes_manager::cpp_classes_manager()
{
}

cpp_classes_manager::cpp_classes_manager(const cpp_classes_manager &right)
{
	RX_ASSERT(false);
}


cpp_classes_manager::~cpp_classes_manager()
{
}


cpp_classes_manager & cpp_classes_manager::operator=(const cpp_classes_manager &right)
{
	RX_ASSERT(false);
	return *this;
}



cpp_classes_manager& cpp_classes_manager::instance ()
{
	static cpp_classes_manager g_obj;
	return g_obj;
}

bool cpp_classes_manager::check_class (rx::pointers::code_behind_definition_t* cd)
{
	return this->definitions_.find(cd) != definitions_.end();
}


} // namespace library
} // namespace rx_platform

