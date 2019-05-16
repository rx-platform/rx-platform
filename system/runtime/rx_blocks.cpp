

/****************************************************************************
*
*  system\runtime\rx_blocks.cpp
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


// rx_blocks
#include "system/runtime/rx_blocks.h"

#include "sys_internal/rx_internal_ns.h"
#include "lib/rx_ser_lib.h"
#include "system/serialization/rx_ser.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace runtime {

namespace blocks {

// Class rx_platform::runtime::blocks::filter_runtime 

string_type filter_runtime::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_runtime::filter_runtime()
{
}



string_type filter_runtime::get_type_name () const
{
  return type_name;

}


// Class rx_platform::runtime::blocks::mapper_runtime 

string_type mapper_runtime::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_runtime::mapper_runtime()
{
}


mapper_runtime::~mapper_runtime()
{
}



string_type mapper_runtime::get_type_name () const
{
  return type_name;

}


// Class rx_platform::runtime::blocks::source_runtime 

string_type source_runtime::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_runtime::source_runtime()
{
}


source_runtime::~source_runtime()
{
}



string_type source_runtime::get_type_name () const
{
  return type_name;

}


// Class rx_platform::runtime::blocks::struct_runtime 

string_type struct_runtime::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_runtime::struct_runtime()
{
}

struct_runtime::struct_runtime (const string_type& name, const rx_node_id& id, bool system)
{
}



bool struct_runtime::serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_object(struct_runtime::type_name.c_str()))
		return false;/*
	if (!complex_runtime_item::serialize_definition(stream, type, ts, mode))
		return false;*/
	if (!stream.end_object())
		return false;
	return true;
}

bool struct_runtime::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	RX_ASSERT(false);//
	return true;
}

string_type struct_runtime::get_type_name () const
{
  return type_name;

}


// Class rx_platform::runtime::blocks::variable_runtime 

string_type variable_runtime::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_runtime::variable_runtime()
{
}

variable_runtime::variable_runtime (const string_type& name, const rx_node_id& id, bool system)
{
}


variable_runtime::~variable_runtime()
{
}



string_type variable_runtime::get_type_name () const
{
  return type_name;

}


// Class rx_platform::runtime::blocks::event_runtime 

string_type event_runtime::type_name = RX_CPP_EVENT_TYPE_NAME;

event_runtime::event_runtime()
{
}



string_type event_runtime::get_type_name () const
{
  return type_name;

}


// Class rx_platform::runtime::blocks::runtime_object 


void runtime_object::turn_on ()
{
	if (mode_.turn_on())
	{
		change_time_ = rx_time::now();
		item_->object_state_changed({ mode_, change_time_, this });
	}
}

void runtime_object::turn_off ()
{
	if (mode_.turn_off())
	{
		change_time_ = rx_time::now();
		item_->object_state_changed({ mode_, change_time_, this });
	}
}

void runtime_object::set_blocked ()
{
	if (mode_.set_blocked())
	{
		change_time_ = rx_time::now();
		item_->object_state_changed({ mode_, change_time_, this });
	}
}

void runtime_object::set_test ()
{
	if (mode_.set_test())
	{
		change_time_ = rx_time::now();
		item_->object_state_changed({ mode_, change_time_, this });
	}
}

void runtime_object::collect_data (data::runtime_values_data& data) const
{
	item_->collect_data(data);
}

void runtime_object::fill_data (const data::runtime_values_data& data)
{
	auto ctx = structure::init_context::create_initialization_context(this);
	item_->fill_data(data, ctx);
}

rx_result runtime_object::read_value (const string_type& path, rx_value& val) const
{
	auto state = get_object_state();
	return item_->get_value(state, path, val);
}

structure::hosting_object_data runtime_object::get_object_state () const
{
	return structure::hosting_object_data{ mode_, change_time_, this };
}

rx_result runtime_object::write_value (const string_type& path, rx_simple_value&& val, api::rx_context ctx)
{
	structure::write_context my_ctx = structure::write_context::create_write_context(this);
	return item_->write_value(path, std::move(val), my_ctx);
}

rx_result runtime_object::initialize_object ()
{
	return "Not implemented";
}

bool runtime_object::serialize (base_meta_writer& stream, uint8_t type) const
{
	data::runtime_values_data temp_data;
	item_->collect_data(temp_data);
	if (!stream.write_init_values("values", temp_data))
		return false;

	if (!stream.start_array("programs", programs_.size()))
		return false;
	for (const auto& one : programs_)
	{
		if (!one->save_program(stream, type))
			return false;
	}
	if (!stream.end_array())
		return false;
	return true;
}

bool runtime_object::deserialize (base_meta_reader& stream, uint8_t type)
{
	data::runtime_values_data temp_data;
	if (!stream.read_init_values("values", temp_data))
		return false;
	structure::init_context ctx;
	item_->fill_data(temp_data, ctx);

	if (!stream.start_array("programs"))
		return false;
	while (!stream.array_end())
	{
		logic::ladder_program::smart_ptr one(pointers::_create_new);
		if (!one->load_program(stream, type))
			return false;
		programs_.push_back(one);
	}
	return true;
}

void runtime_object::set_runtime_data (meta::runtime_data_prototype& prototype)
{
	item_ = std::move(create_runtime_data(prototype));
}


// Parameterized Class rx_platform::runtime::blocks::runtime_holder 


template <class T>
rx_result runtime_holder<T>::read_value (const string_type& path, rx_value& val, const T& whose) const
{
	if (path.empty())
	{// our value
		val = whose.get_value();
		return true;
	}
	return runtime.read_value(path, val);
}

template <class T>
rx_result runtime_holder<T>::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx, T& whose)
{
	if (path.empty())
	{// our value
		return RX_ACCESS_DENIED;
	}
	std::function<rx_result(const string_type&, rx_simple_value)> func = [this, ctx](const string_type& path, rx_simple_value val)
	{
		return runtime.write_value(path, std::move(val), ctx);
	};
	if (ctx.object && ctx.object.unsafe_ptr() == &whose)
		return func(path, std::move(val));
	else
	{
		rx_do_with_callback<rx_result, decltype(ctx.object), const string_type&, rx_simple_value>(func, whose.get_executer(), callback, ctx.object, path, std::move(val));
		return true;
	}
}

template <class T>
bool runtime_holder<T>::serialize (base_meta_writer& stream, uint8_t type) const
{

	if (!runtime.serialize(stream, type))
		return false;

	return true;
}

template <class T>
bool runtime_holder<T>::deserialize (base_meta_reader& stream, uint8_t type)
{

	if (!runtime.deserialize(stream, type))
		return false;

	return true;
}

template class runtime_holder<objects::object_runtime>;
template class runtime_holder<objects::application_runtime>;
template class runtime_holder<objects::domain_runtime>;
template class runtime_holder<objects::port_runtime>;
} // namespace blocks
} // namespace runtime
} // namespace rx_platform

