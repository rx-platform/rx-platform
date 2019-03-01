

/****************************************************************************
*
*  api\rx_meta_api.cpp
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


#include "pch.h"


// rx_meta_api
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"

using namespace model;

namespace rx_platform
{
namespace api
{
namespace meta
{

void rx_delete_object(const string_type& name, ns::rx_server_directory::smart_ptr dir, std::function<void(rx_result&&)> callback, pointers::reference_object::smart_ptr ref)
{
	model::platform_types_manager::instance().delete_runtime<object_type, pointers::reference_object::smart_ptr>(name, dir, callback, ref);
}

}
}
}


