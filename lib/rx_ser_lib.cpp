

/****************************************************************************
*
*  lib\rx_ser_lib.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_ser_lib
#include "lib/rx_ser_lib.h"



namespace rx {

bool is_serialization_type_array(int type)
{
	return (type == STREAMING_TYPE_TYPES
		|| type == STREAMING_TYPE_OBJECTS
		|| type == STREAMING_TYPE_DETAILS
		|| type == STREAMING_TYPE_DIRECTORY
		|| type == STREAMING_TYPE_VALUES
		);
}


// Class rx::base_meta_writer 

base_meta_writer::base_meta_writer(const base_meta_writer &right)
{
	RX_ASSERT(false);
}

base_meta_writer::base_meta_writer (int version)
	: version_(version)
{
}


base_meta_writer::~base_meta_writer()
{
}


base_meta_writer & base_meta_writer::operator=(const base_meta_writer &right)
{
	RX_ASSERT(false);
	return *this;
}



// Class rx::base_meta_reader 

base_meta_reader::base_meta_reader(const base_meta_reader &right)
{
	RX_ASSERT(false);
}

base_meta_reader::base_meta_reader (int version)
	: version_(version)
{
}


base_meta_reader::~base_meta_reader()
{
}


base_meta_reader & base_meta_reader::operator=(const base_meta_reader &right)
{
	RX_ASSERT(false);
	return *this;
}



} // namespace rx

