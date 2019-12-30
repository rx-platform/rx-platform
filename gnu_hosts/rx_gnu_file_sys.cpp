

/****************************************************************************
*
*  gnu_hosts\rx_gnu_file_sys.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_gnu_file_sys
#include "gnu_hosts/rx_gnu_file_sys.h"



namespace gnu {

// Parameterized Class gnu::gnu_file_system_storage 

template <class policyT>
gnu_file_system_storage<policyT>::gnu_file_system_storage()
{
}


template <class policyT>
gnu_file_system_storage<policyT>::~gnu_file_system_storage()
{
}


template class gnu_file_system_storage<storage::storage_policy::file_path_addresing_policy>;
} // namespace gnu

