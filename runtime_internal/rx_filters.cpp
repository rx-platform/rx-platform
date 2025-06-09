

/****************************************************************************
*
*  runtime_internal\rx_filters.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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

#include "system/runtime/rx_runtime_helpers.h"
#include "model/rx_meta_internals.h"
#include "system/runtime/rx_value_templates.h"
#include "lib/rx_string_encoding.h"

// rx_filters
#include "runtime_internal/rx_filters.h"

#include "sys_internal/rx_async_functions.h"


namespace rx_internal {

namespace sys_runtime {

namespace filters {
rx_result register_filter_constructors()
{
	auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_LINEAR_SCALING_FILTER_TYPE_ID, [] {
			return rx_create_reference<linear_scaling_filter>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_LIMIT_FILTER_TYPE_ID, [] {
			return rx_create_reference<limit_filter>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_LO_CUTOFF_FILTER_TYPE_ID, [] {
			return rx_create_reference<cutoff_scaling>(true);
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_HI_CUTOFF_FILTER_TYPE_ID, [] {
			return rx_create_reference<cutoff_scaling>(false);
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_QUALITY_FILTER_TYPE_ID, [] {
			return rx_create_reference<quality_filter>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_ASCII_FILTER_TYPE_ID, [] {
			return rx_create_reference<ascii_filter>();
		});//
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_CUMULATIVE_SPEED_FILTER_TYPE_ID, [] {
			return rx_create_reference<cumulative_speed_filter>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_LATCH_FILTER_TYPE_ID, [] {
			return rx_create_reference<latch_filter>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_HEX2DEC_FILTER_TYPE_ID, [] {
			return rx_create_reference<hex2decimal>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_CALC_FILTER_TYPE_ID, [] {
			return rx_create_reference<calculation_filter>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_ROUND_FILTER_TYPE_ID, [] {
			return rx_create_reference<round_filter>();
		});
	return true;
}

// Class rx_internal::sys_runtime::filters::linear_scaling_filter 


rx_result linear_scaling_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	auto result = hi_eu_.bind(".HiEU", ctx);
	if (!result)
		return result.errors();
	result = low_eu_.bind(".LowEU", ctx);
	if (!result)
		return result.errors();
	result = hi_raw_.bind(".HiRaw", ctx);
	if (!result)
		return result.errors();
	result = low_raw_.bind(".LowRaw", ctx);
	if (!result)
		return result.errors();
	return true;
}

rx_result linear_scaling_filter::filter_input (rx_value& val)
{
	double hi_eu = hi_eu_;
	double low_eu = low_eu_;
	double hi_raw = hi_raw_;
	double low_raw = low_raw_;
	double in = val.get_float();
	if (abs(hi_raw - low_raw) == 0)
	{
		val.set_quality(RX_BAD_QUALITY_DIVISION_BY_ZERO);
		return true;
	}
	double result = ((in - low_raw) * (hi_eu - low_eu)) / (hi_raw - low_raw) + low_eu;
	val.set_float(result, val.get_type());
	return true;
}

rx_result linear_scaling_filter::filter_output (rx_simple_value& val)
{
	double hi_eu = hi_eu_;
	double low_eu = low_eu_;
	double hi_raw = hi_raw_;
	double low_raw = low_raw_;
	double in = val.get_float();
	if (abs(hi_raw - low_raw) == 0)
	{
		return "Division by zero";
	}
	double result = ((in - low_eu) * (hi_raw - low_raw)) / (hi_eu - low_eu) + low_raw;
	val.set_float(result, val.get_type());
	return true;
}


// Class rx_internal::sys_runtime::filters::cutoff_scaling 

cutoff_scaling::cutoff_scaling (bool lo_cutoff)
      : input_cutoff_(0.0),
        output_cutoff_(0.0),
        lo_cutoff_(lo_cutoff)
{
}



rx_result cutoff_scaling::initialize_filter (runtime::runtime_init_context& ctx)
{
	auto result = input_cutoff_.bind(".InCutoffValue", ctx);
	if (!result)
		return result.errors();
	result = output_cutoff_.bind(".OutCutoffValue", ctx);
	if (!result)
		return result.errors();
	return true;
}

rx_result cutoff_scaling::filter_input (rx_value& val)
{
	double cutoff_value = input_cutoff_;
	double numeric_val = val.get_float();

	if (lo_cutoff_)
		numeric_val = numeric_val < cutoff_value ? cutoff_value : numeric_val;
	else
		numeric_val = numeric_val > cutoff_value ? cutoff_value : numeric_val;

	val.set_float(numeric_val, val.get_type());

	return true;
}

rx_result cutoff_scaling::filter_output (rx_simple_value& val)
{
	double cutoff_value = output_cutoff_;
	double numeric_val = val.get_float();

	if (lo_cutoff_)
		numeric_val = numeric_val < cutoff_value ? cutoff_value : numeric_val;
	else
		numeric_val = numeric_val > cutoff_value ? cutoff_value : numeric_val;

	val.set_float(numeric_val, val.get_type());

	return true;
}


// Class rx_internal::sys_runtime::filters::limit_filter 


rx_result limit_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	auto result = hi_input_.bind(".HiInput", ctx);
	if (!result)
		return result.errors();
	result = low_input_.bind(".LowInput", ctx);
	if (!result)
		return result.errors();
	result = hi_output_.bind(".HiOutput", ctx);
	if (!result)
		return result.errors();
	result = low_output_.bind(".LowOutput", ctx);
	if (!result)
		return result.errors();
	return true;
}

rx_result limit_filter::filter_input (rx_value& val)
{
	double hi_val = hi_input_;
	double low_val = low_input_;
	double numeric_val = val.get_float();
	if(numeric_val<low_val || numeric_val>hi_val)
		val.set_quality(RX_UNCERTAIN_QUALITY_OUT_OF_RANGE);
	return true;
}

rx_result limit_filter::filter_output (rx_simple_value& val)
{
	double hi_val = hi_output_;
	double low_val = low_output_;
	double numeric_val = val.get_float();
	if (numeric_val<low_val || numeric_val>hi_val)
		return "Value out of range.";
	else
		return true;
}


// Class rx_internal::sys_runtime::filters::quality_filter 


rx_result quality_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	good_value_ = ctx.get_item_static(".GoodValue", true);
	return true;
}

rx_result quality_filter::filter_input (rx_value& val)
{
	rx_time ts = val.get_time();
	if (val.is_good())
	{
		val.assign_static(good_value_, ts);
	}
	else
	{
		val.assign_static(!good_value_, ts);
	}
	val.set_good_locally();
	return true;
}

bool quality_filter::supports_output () const
{
  return false;

}


// Class rx_internal::sys_runtime::filters::ascii_filter 


rx_result ascii_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	invalid_char_ = ctx.get_item_static(".GoodValue", '?');
	columns_ = ctx.get_item_static(".Columns", 0);
	max_len_ = ctx.get_item_static(".MaxLen", 0);
	return true;
}

rx_result ascii_filter::filter_input (rx_value& val)
{
	if (val.is_string())
	{// do the conversion
		string_type str_val = val.extract_static<string_type>("");
		if (!str_val.empty())
		{
			str_val = utf8_to_ascii(str_val, invalid_char_);
			if (columns_ > 0)
			{
				str_val = wrap(str_val);
			}
			if (max_len_ > 0 && str_val.size()>max_len_)
			{
				str_val.resize(max_len_);
			}
			val.assign_static(str_val);
		}
	}
	else
	{
		val.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
	}
	return true;
}

rx_result ascii_filter::filter_output (rx_simple_value& val)
{
	if (val.is_string())
	{// do the conversion
		string_type str_val = val.extract_static<string_type>("");
		str_val = utf8_to_ascii(str_val, invalid_char_);
		if (columns_ > 0)
		{
			str_val = wrap(str_val);
		}
		if (max_len_ > 0 && str_val.size() > max_len_)
		{
			str_val.resize(max_len_);
		}
		val.assign_static(str_val);
		return true;
	}
	else
	{
		return RX_INVALID_CONVERSION;
	}
}

string_type ascii_filter::wrap (const string_type& what)
{
	std::istringstream words(what);
	std::ostringstream wrapped;
	std::string word;

	if (words >> word)
	{
		wrapped << word;
		size_t space_left = columns_ - word.length();
		while (words >> word)
		{
			if (space_left < word.length() + 1)
			{
				for (size_t i = 0; i < space_left; i++)
					wrapped << ' ';
				wrapped << word;
				space_left = columns_ - word.length();
			}
			else
			{
				wrapped << ' ' << word;
				space_left -= word.length() + 1;
			}
		}
	}
	return wrapped.str();
}


// Class rx_internal::sys_runtime::filters::hex2decimal 


rx_result hex2decimal::initialize_filter (runtime::runtime_init_context& ctx)
{
	empty_is_zero_ = ctx.get_item_static(".EmptyIsZero", false);

	return true;
}

rx_result hex2decimal::filter_input (rx_value& val)
{
	if (val.is_string() && val.is_good())
	{
		auto quality = val.get_quality();
		auto time = val.get_time();
		int64_t temp_val;
		auto str = val.get_string();
		if (str.empty() && empty_is_zero_)
		{
			val.assign_static("0", time);
			val.set_quality(quality);
		}
		else if (std::from_chars(str.c_str(), str.c_str() + str.size(), temp_val, 16).ec == std::errc{})
		{
			char buff[0x40];
			auto [ptr, ec] = std::to_chars(buff, buff + (sizeof(buff) / sizeof(buff[0])), temp_val);
			if (ec == std::errc{})
			{
				*ptr = '\0';
				val.assign_static(buff, time);
				val.set_quality(quality);
			}
			else
			{
				val.set_quality(RX_BAD_QUALITY);
			}
		}
		else
		{
			val.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
		}
	}
	else
	{
		val.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
	}
	return true;
}

rx_result hex2decimal::filter_output (rx_simple_value& val)
{
	return RX_NOT_IMPLEMENTED;
}


// Class rx_internal::sys_runtime::filters::latch_filter 


rx_result latch_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	unlatch_.bind(".Unlatch", ctx, [this](const bool& val)
		{
			if (val)
			{
				if (!latched_.is_null())
				{
					auto type = latched_.get_type();
					latched_.set_float(0.0, type);
					filter_changed();
				}
				unlatch_ = false;
			}
		});
	timeout_.bind(".Timeout", ctx);
	return true;
}

rx_result latch_filter::start_filter (runtime::runtime_start_context& ctx)
{
	timer_ = ctx.create_timer_function(smart_this(), [this]()
		{
			timer_->suspend();
			if (!latched_.is_null())
			{
				auto type = latched_.get_type();
				latched_.set_float(0.0, type);
				filter_changed();
			}
		});
	return true;
}

rx_result latch_filter::stop_filter (runtime::runtime_stop_context& ctx)
{
	if (timer_)
	{
		timer_->cancel();
		timer_ = rx_timer_ptr::null_ptr;
	}
	return true;
}

rx_result latch_filter::filter_input (rx_value& val)
{
	if (val.is_numeric() && val.is_good())
	{
		if (val.get_float() == 0.0)
		{
			if (!latched_.is_null())
				val = latched_;
		}
		else
		{
			if (timeout_ > 0)
			{
				timer_->start(timeout_);
			}
			latched_ = val;
		}
	}
	else
	{
		val.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
	}
	return true;
}


// Class rx_internal::sys_runtime::filters::calculation_filter 

calculation_filter::calculation_filter()
      : input_path_(""),
        output_path_("")
	, input_point_(this)
	, output_point_(this)
{
}


calculation_filter::~calculation_filter()
{
}



rx_result calculation_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	input_point_.set_context(ctx.context);
	output_point_.set_context(ctx.context);
	auto result = input_path_.bind(".InPath", ctx, [this](const string_type& val)
		{
			connect_input(val);
		});
	result = output_path_.bind(".OutPath", ctx, [this](const string_type& val)
		{
			connect_output(val);
		});
	return true;
}

rx_result calculation_filter::start_filter (runtime::runtime_start_context& ctx)
{
	auto result = connect_input(input_path_);
	result = connect_output(output_path_);
	return true;
}

rx_result calculation_filter::stop_filter (runtime::runtime_stop_context& ctx)
{
	disconnect_input();
	disconnect_output();
	return true;
}

rx_result calculation_filter::filter_input (rx_value& val)
{
	val = input_point_.calculate_local_var(val, token_buffer_);
	return true;
}

rx_result calculation_filter::filter_output (rx_simple_value& val)
{
	rx_value temp(val);
	temp.set_good_locally();
	temp = output_point_.calculate_local_var(temp, token_buffer_);
	if (temp.is_good())
	{
		val = temp.to_simple();
		return true;
	}
	else
	{
		return "Expression error!";
	}
}

rx_result calculation_filter::connect_input (const string_type& path)
{
	try
	{
		input_point_.connect(path, 200);
		return true;
	}
	catch (std::exception& ex)
	{
		return ex.what();
	}
}

void calculation_filter::disconnect_input ()
{
	input_point_.disconnect();
}

rx_result calculation_filter::connect_output (const string_type& path)
{
	try
	{
		output_point_.connect(path, 200);
		return true;
	}
	catch (std::exception& ex)
	{
		return ex.what();
	}
}

void calculation_filter::disconnect_output ()
{
	output_point_.disconnect();
}

void calculation_filter::input_value_changed (const rx_value& val)
{
	filter_changed();
}

void calculation_filter::output_value_changed (const rx_value& val)
{
}

void calculation_filter::value_changed (const rx_value& val, calcualtion_point* whose)
{
	if(whose == &input_point_)
	{
		input_value_changed(val);
	}
	else if (whose != &output_point_)
	{
		RX_ASSERT(false);
	}
}


// Class rx_internal::sys_runtime::filters::calcualtion_point 

calcualtion_point::calcualtion_point (calculation_filter* my_filter)
      : my_filter_(my_filter)
{
}



void calcualtion_point::value_changed (const rx_value& val)
{
	my_filter_->value_changed(val, this);
}


// Class rx_internal::sys_runtime::filters::cumulative_speed_filter 


rx_result cumulative_speed_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	auto result = period_.bind(".Period", ctx, [this](const uint32_t& val)
		{
			if (val)
				timer_->start(val);
			else
				timer_->suspend();
		});
	return true;
}

rx_result cumulative_speed_filter::start_filter (runtime::runtime_start_context& ctx)
{
	timer_ = rx_create_io_periodic_function(smart_this(), [this]()
		{
			timer_tick();
		});
	if (period_ > 0)
		timer_->start(period_);
	my_value_.assign_static<double>(0);
	my_value_.set_time(ctx.context->now());
	my_value_.adapt_quality_to_mode(ctx.context->get_mode());

	return true;
}

rx_result cumulative_speed_filter::stop_filter (runtime::runtime_stop_context& ctx)
{
	timer_->cancel();
	timer_ = rx_timer_ptr::null_ptr;
	return true;
}

rx_result cumulative_speed_filter::deinitialize_filter (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result cumulative_speed_filter::filter_input (rx_value& val)
{
	if (val.is_good())
	{
		uint64_t num_val = val.extract_static<uint64_t>(0ull);
		current_value_ = num_val;
	}
	else
	{
		current_value_ = 0;
	}
	val = my_value_;
	return true;
}

bool cumulative_speed_filter::supports_output () const
{
  return false;

}

void cumulative_speed_filter::timer_tick ()
{
	double val = 0;
	auto ticks = rx_get_tick_count();
	if (last_tick_ > 0)
	{
		double diff =(double)(current_value_ - last_value_);
		double time = (double)(ticks - last_tick_) / 1000.0;
		if (time > 0)
		{
			val = diff / time;
		}
	}
	last_tick_ = ticks;
	last_value_ = current_value_;
	my_value_.assign_static<double>(val);
	my_value_.set_time(rx_time::now());
	my_value_.set_good_locally();
	filter_changed();
}


// Class rx_internal::sys_runtime::filters::round_filter 

round_filter::round_filter ()
      : decimal_places_(2),
        multiply_arg_(100)
{
}



rx_result round_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	auto result = decimal_places_.bind(".DecimalPlaces", ctx, [this](int val)
		{
			if (val >= 0)
			{
				multiply_arg_ = 1;
				for (int i = 0; i < val; i++)
					multiply_arg_ *= 10.0;
			}
			else
			{
				val = -val;
				multiply_arg_ = 1;
				for (int i = 0; i < val; i++)
					multiply_arg_ /= 10.0;
			}
			filter_changed();
		});
	if (!result)
		return result.errors();
	return true;
}

rx_result round_filter::filter_input (rx_value& val)
{
	if (val.is_numeric() && multiply_arg_ > 0.000000001)
	{
		auto type = val.get_type();
		double to_filter = val.get_float();
		to_filter = round(to_filter * multiply_arg_) / multiply_arg_;

		val.set_float(to_filter, type);
	}
	return true;
}

bool round_filter::supports_output () const
{
  return false;

}


} // namespace filters
} // namespace sys_runtime
} // namespace rx_internal

