

/****************************************************************************
*
*  model\rx_construction_list.cpp
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


// rx_construction_list
#include "model/rx_construction_list.h"

#include "model/rx_meta_internals.h"
#include "interfaces/rx_ip_endpoints.h"
#include "system/runtime/rx_port_types.h"


namespace rx_internal {

namespace model {

namespace construction {

// Parameterized Class rx_internal::model::construction::construction_list_dummy 





typedef std::tuple< construction_list_adapter<meta::object_types::port_type::RType>
	, construction_list_adapter<interfaces::ip_endpoints::udp_port> > my_tuple;


int do_types_testing()
{
	construction_list_types_collector<void> start;
	auto next = construction_list_types_collector<decltype(start), my_type2, my_type1>::resulting_type();
	auto impl = runtimes_constructor_implementation<decltype(next)>();
	auto val = impl.construct_object<my_type2>(55);
	std::tuple<my_type1, my_type2> resulting_tuple;
	auto sz = std::tuple_size<my_tuple>();
	constexpr size_t idx = get_index_of_helper<meta::object_types::domain_type, my_tuple>();
	//std::cout << "Returned idx = " << idx << "\r\n";
	//std::cout << "Duzina = " << sz.value << "\r\n";
	return 0;
}


auto dummy = { 0, do_types_testing(), 0 };

} // namespace construction
} // namespace model
} // namespace rx_internal

