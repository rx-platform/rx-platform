

/****************************************************************************
*
*  api\rx_runtime_api.cpp
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


// rx_runtime_api
#include "api/rx_runtime_api.h"

#include "model/rx_model_algorithms.h"

namespace rx_platform
{
namespace api
{
namespace runtime
{

rx_result get_working_item(const rx_node_id& what, std::function<void(rx_result_with<platform_item_ptr>&&)> callback)
{
	return RX_NOT_IMPLEMENTED;// model::algorithms::get_working_runtime_sync;
}

}// runtime
}// api
}// rx-platform


