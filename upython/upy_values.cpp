

/****************************************************************************
*
*  upython\upy_values.cpp
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

#ifdef UPYTHON_SUPPORT

// upy_values
#include "upython/upy_values.h"



namespace rx_platform {

namespace python {

// Class rx_platform::python::upy_convertor 


rx_result upy_convertor::upy_to_simple (mp_obj_t py, values::rx_simple_value& val)
{
	if (MP_OBJ_IS_STR(py))
	{
		val.assign_static(mp_obj_str_get_str(py));
		return true;
	}
	else if (mp_obj_is_bool(py))
	{
		val.assign_static(mp_const_true == py);
		return true;
	}
	else if (mp_obj_is_int(py))
	{
		val.assign_static(mp_obj_get_int(py));
		return true;
	}
	else if (mp_obj_is_float(py))
	{
		val.assign_static(mp_obj_get_float_to_d(py));
		return true;
	}
	else
	{
		return "Unsupported type";
	}
}

rx_result upy_convertor::upy_to_data (mp_obj_t py, data::runtime_values_data data)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result upy_convertor::value_to_upy (const values::rx_value& val, mp_obj_t* py)
{
	auto type = val.get_type();
	if (val.is_array() || val.is_struct())
		return "Array and struct are still unsupported";
	if (val.get_type() == RX_BOOL_TYPE)
	{
		*py = mp_obj_new_bool(val.get_unassigned());
	}
	else if (val.is_integer())
	{
		*py = mp_obj_new_int_from_ll(val.get_integer());
	}
	else if (val.is_unassigned())
	{
		*py = mp_obj_new_int_from_ull(val.get_unassigned());
	}
	else if (val.is_float())
	{
		*py = mp_obj_new_float_from_d(val.get_float());
	}
	else if (val.is_string())
	{
		auto temp = val.get_string();
		if(temp.empty())
			*py = mp_obj_new_str("", 0);
		else
			*py = mp_obj_new_str(temp.c_str(), temp.size());
	}
	return true;
}


} // namespace python
} // namespace rx_platform

#endif
