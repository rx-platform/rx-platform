

/****************************************************************************
*
*  lib\rx_ser_lib.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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


#include "stdafx.h"


// rx_ser_lib
#include "lib/rx_ser_lib.h"



namespace rx {

// Class rx::base_meta_writter 

base_meta_writter::base_meta_writter(const base_meta_writter &right)
{
	RX_ASSERT(false);
}

base_meta_writter::base_meta_writter (int version)
	: version_(version)
{
}


base_meta_writter::~base_meta_writter()
{
}


base_meta_writter & base_meta_writter::operator=(const base_meta_writter &right)
{
	RX_ASSERT(false);
	return *this;
}



// Class rx::base_meta_reader 

base_meta_reader::base_meta_reader()
{
}

base_meta_reader::base_meta_reader(const base_meta_reader &right)
{
	RX_ASSERT(false);
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

