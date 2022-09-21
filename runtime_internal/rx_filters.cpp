

/****************************************************************************
*
*  runtime_internal\rx_filters.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_LATCH_FILTER_TYPE_ID, [] {
			return rx_create_reference<latch_filter>();
		});
	result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_HEX2DEC_FILTER_TYPE_ID, [] {
			return rx_create_reference<hex2decimal>();
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


} // namespace filters
} // namespace sys_runtime
} // namespace rx_internal

