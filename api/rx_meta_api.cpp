

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

rx_result rx_delete_object(const string_type& name
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	model::platform_types_manager::instance().delete_runtime<object_type, pointers::reference_object::smart_ptr>(
		name, ctx.directory, callback, ctx.object);
	return true;
}

rx_result rx_create_object(const string_type& name, const string_type& type_name, const data::runtime_values_data* init_data
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx)
{
	model::platform_types_manager::instance().create_runtime<object_type, pointers::reference_object::smart_ptr>(
		name, type_name, init_data, ctx.directory, callback, ctx.object);
	return true;
}


rx_result rx_create_object_type(const string_type& name
	, const string_type& base_name, rx_object_type_ptr prototype
	, std::function<void(rx_result_with<rx_object_type_ptr>&&)> callback, rx_context ctx)
{
	model::platform_types_manager::instance().create_type<object_type, pointers::reference_object::smart_ptr>(
		name, base_name, prototype, ctx.directory, callback, ctx.object);
	return true;
}


}
}
}


