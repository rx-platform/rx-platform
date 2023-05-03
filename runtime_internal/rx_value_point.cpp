

/****************************************************************************
*
*  runtime_internal\rx_value_point.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_data_source
#include "runtime_internal/rx_data_source.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_value_point
#include "runtime_internal/rx_value_point.h"

#include "rx_configuration.h"
#include "system/server/rx_ns.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_internal {

namespace sys_runtime {

namespace data_source {

#define TAG_ID_OFFSET 0x80
#define TAG_ID_MASK 0x7f

#define DELIMETER 1
#define VARIABLE 2
#define NUMBER 3
#define STRING 4

// unary operations
#define SIN_CODE	0xff
#define COS_CODE	SIN_CODE-1
#define TAN_CODE	COS_CODE-1
#define LOG_CODE	TAN_CODE-1
#define LN_CODE		LOG_CODE-1
#define EXP_CODE	LN_CODE-1
#define SQRT_CODE	EXP_CODE-1
#define ABS_CODE	SQRT_CODE-1
#define ASIN_CODE	ABS_CODE-1
#define ACOS_CODE	ASIN_CODE-1
#define ATAN_CODE	ACOS_CODE-1


#define MAX_BI_CODE ATAN_CODE-1

// binary operations
#define GE_CODE		MAX_BI_CODE-1
#define LE_CODE		GE_CODE-1
#define NE_CODE		LE_CODE-1
#define DIV_CODE	NE_CODE-1
#define BIT_CODE	DIV_CODE-1
#define AND_CODE	BIT_CODE-1
#define OR_CODE		AND_CODE-1

#define MAX_REG_CODE OR_CODE-1


struct oper_struct
{
	uint8_t code;
	const char* name;
	size_t len;
};

oper_struct operations[] = {
	{ ABS_CODE,"abs",3},
	{ GE_CODE,">=",2},
	{ LE_CODE,"<=",2},
	{ NE_CODE,"!=",2},
	{ DIV_CODE,"div",3},
	{ BIT_CODE,"bit",3},
	{ AND_CODE,"&&",2},
	{ OR_CODE,"||",2},
	{ SQRT_CODE,"sqrt",4},
	{ SIN_CODE,"sin",3},
	{ COS_CODE,"cos",3},
	{ TAN_CODE,"tan",3},
	{ LOG_CODE,"log",3},
	{ LN_CODE,"ln",2},
	{ EXP_CODE,"exp",3},
	{ ASIN_CODE,"asin",4},
	{ ACOS_CODE,"acos",4},
	{ ATAN_CODE,"atan",4}
};

#define POINT_LOCAL_VAR_NAME "x"


extern uint32_t g_count;
extern uint32_t g_tag_nums;
int isvariable(char c)
{
	if (((uint8_t)c) >= TAG_ID_OFFSET && ((uint8_t)c) < MAX_REG_CODE) return 1;
	return 0;
}
int iswhite(char c)
{
	if (c == ' ' || c == '\t' || c == '\r' || c == '\n') return 1;
	return 0;
}


int is_in(char ch, const char* s)
{
	while (*s)
		if (*s++ == ch) return 1;

	return 0;
}
int isunary(char c)
{
	if (((uint8_t)c) > MAX_BI_CODE || is_in(c, "+-!")) return 1;
	return 0;
}
int isoper(char c)
{
	if (((uint8_t)c) > MAX_REG_CODE || is_in(c, "+-*/%^=><&|()!")) return 1;
	return 0;
}
int isdelim(char c)
{
	if (isoper(c) || iswhite(c) || c == '\0') return 1;
	return 0;
}
rx_value_t get_arithmetic_result_type(rx_value_t left, rx_value_t right, bool add)
{
	if (left > RX_NULL_TYPE && left <= RX_UINT64_TYPE
		&& right > RX_NULL_TYPE && right <= RX_UINT64_TYPE)
	{// integer values
		return std::max(left, right);// just return max
	}
	else if (left > RX_NULL_TYPE && left <= RX_DOUBLE_TYPE
		&& right > RX_NULL_TYPE && right <= RX_DOUBLE_TYPE)
	{// has floating point values
		if (left == RX_DOUBLE_TYPE || right == RX_DOUBLE_TYPE)
			return RX_DOUBLE_TYPE;
		else
			return RX_FLOAT_TYPE;
	}
	else if (left == RX_COMPLEX_TYPE || right == RX_COMPLEX_TYPE)
	{
		return RX_COMPLEX_TYPE;
	}
	else if (left == RX_TIME_TYPE && right > RX_NULL_TYPE && right <= RX_DOUBLE_TYPE)
	{// do time and numeric
		return RX_TIME_TYPE;
	}
	else if (add)
	{
		if (left == RX_STRING_TYPE)
		{// add string and any other
			return RX_STRING_TYPE;
		}
		else if (left == RX_BYTES_TYPE && right == RX_BYTES_TYPE)
		{// add two byte arrays
			return RX_BYTES_TYPE;
		}
	}
	return RX_NULL_TYPE;
}
rx_value add_values(const rx_value& left, const rx_value& right)
{
	rx_value result;
	result.set_good_locally();
	rx_value_t ret_type = get_arithmetic_result_type(left.get_type(), right.get_type(), true);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type == RX_BOOL_TYPE)
		{
			result.assign_static(left.get_bool() | right.get_bool());
		}
		else if(ret_type == RX_INT8_TYPE || ret_type == RX_INT16_TYPE || ret_type == RX_INT32_TYPE || ret_type == RX_INT64_TYPE)
		{// integers stuff
			result.set_integer(left.get_integer() + right.get_integer(), ret_type);
		}
		else if (ret_type == RX_UINT8_TYPE || ret_type == RX_UINT16_TYPE || ret_type == RX_UINT32_TYPE || ret_type == RX_UINT64_TYPE)
		{// integers stuff
			result.set_unassigned(left.get_unassigned() + right.get_unassigned(), ret_type);
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			result.set_float(left.get_float() + right.get_float(), ret_type);
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = left.get_complex();
			complex_value_struct two = right.get_complex();
			one.imag = one.imag + two.imag;
			one.real = one.real + two.real;
			result.assign_static(one);
		}
		else if (ret_type == RX_TIME_TYPE)
		{// time
			result.assign_static(rx_time(left.c_ptr()->value.value.time_value) + right.get_unassigned());
		}
		else if (ret_type == RX_BYTES_TYPE && left.get_type() == RX_BYTES_TYPE && right.get_type() == RX_BYTES_TYPE)
		{
			size_t size1 = 0;
			size_t size2 = 0;
			const uint8_t* ptr1 = rx_c_ptr(&left.c_ptr()->value.value.bytes_value, &size1);
			const uint8_t* ptr2 = rx_c_ptr(&right.c_ptr()->value.value.bytes_value, &size2);
			size_t new_size = size1 + size2;
			byte_string temp_buff;
			if (new_size)
			{
				temp_buff.assign(new_size, std::byte{ 0 });
				if (size1 > 0)
					memcpy(&temp_buff[0], ptr1, size1);

				if (size2 > 0)
					memcpy(&temp_buff[size1], ptr2, size2);
			}
			result.assign_static(temp_buff);
		}
		else if (ret_type == RX_STRING_TYPE && left.get_type() == RX_STRING_TYPE)
		{
			size_t size1 = left.c_ptr()->value.value.string_value.size;
			const char* ptr1 = rx_c_str(&left.c_ptr()->value.value.string_value);
			if (right.c_ptr()->value.value_type == RX_STRING_TYPE)
			{// do dynamic stuff
				size_t size2 = right.c_ptr()->value.value.string_value.size;
				const char* ptr2 = rx_c_str(&right.c_ptr()->value.value.string_value);
				size_t new_size = size1 + size2;
				string_type temp_buff;
				if (new_size)
				{
					temp_buff.reserve(new_size);
					if (ptr1)
						temp_buff += ptr1;
					if (ptr2)
						temp_buff += ptr2;
				}
				result.assign_static(temp_buff);
			}
			else
			{
				std::string right_str = right.to_string();
				size_t size2 = right_str.size();
				size_t new_size = size1 + size2;
				string_type temp_buff;
				if (new_size)
				{
					temp_buff.reserve(new_size);
					if (ptr1)
						temp_buff += ptr1;
					temp_buff += right_str;
				}
				result.assign_static(temp_buff);
			}
		}
		else
		{
			throw std::invalid_argument("Syntax error in add");
		}
	}
	else
	{
		throw std::invalid_argument("Syntax error in add");
	}
	return result;
}


rx_value sub_values(const rx_value& left, const rx_value& right)
{
	rx_value result;
	result.set_good_locally();
	rx_value_t ret_type = get_arithmetic_result_type(left.get_type(), right.get_type(), false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type == RX_BOOL_TYPE)
		{
			throw std::invalid_argument("Can not subtract bool values");
		}
		else if (ret_type == RX_INT8_TYPE || ret_type == RX_INT16_TYPE || ret_type == RX_INT32_TYPE || ret_type == RX_INT64_TYPE)
		{// integers stuff
			result.set_integer(left.get_integer() - right.get_integer(), ret_type);
		}
		else if (ret_type == RX_UINT8_TYPE || ret_type == RX_UINT16_TYPE || ret_type == RX_UINT32_TYPE || ret_type == RX_UINT64_TYPE)
		{// integers stuff
			result.set_unassigned(left.get_unassigned() - right.get_unassigned(), ret_type);
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			result.set_float(left.get_float() - right.get_float(), ret_type);
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = left.get_complex();
			complex_value_struct two = right.get_complex();
			one.imag = one.imag - two.imag;
			one.real = one.real - two.real;
			result.assign_static(one);
		}
		else if (ret_type == RX_TIME_TYPE)
		{// time
			result.assign_static(rx_time(left.c_ptr()->value.value.time_value) - right.get_unassigned());
		}
		else
		{
			throw std::invalid_argument("Syntax error in subtract");
		}
	}
	else
	{
		throw std::invalid_argument("Syntax error in subtract");
	}
	return result;
}


rx_value mul_values(const rx_value& left, const rx_value& right)
{
	rx_value result;
	result.set_good_locally();
	rx_value_t ret_type = get_arithmetic_result_type(left.c_ptr()->value.value_type, right.c_ptr()->value.value_type, false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type == RX_BOOL_TYPE)
		{
			result.assign_static(left.get_bool() & right.get_bool());
		}
		else if (ret_type == RX_INT8_TYPE || ret_type == RX_INT16_TYPE || ret_type == RX_INT32_TYPE || ret_type == RX_INT64_TYPE)
		{// integers stuff
			result.set_integer(left.get_integer() * right.get_integer(), ret_type);
		}
		else if (ret_type == RX_UINT8_TYPE || ret_type == RX_UINT16_TYPE || ret_type == RX_UINT32_TYPE || ret_type == RX_UINT64_TYPE)
		{// integers stuff
			result.set_unassigned(left.get_unassigned() * right.get_unassigned(), ret_type);
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			result.set_float(left.get_float() * right.get_float(), ret_type);
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = left.get_complex();
			complex_value_struct two = right.get_complex();
			complex_type z1{ one.real, one.imag };
			complex_type z2{ two.real, two.imag };
			z1 = z1 * z2;
			result.assign_static(complex_value_struct{ z1.real(), z1.imag() });
		}
		else
		{
			throw std::invalid_argument("Syntax error in multiplying");
		}
	}
	else
	{
		throw std::invalid_argument("Syntax error in multiplying");
	}
	return result;
}

rx_value div_values(const rx_value& left, const rx_value& right)
{
	rx_value result;
	result.set_good_locally();
	rx_value_t ret_type = get_arithmetic_result_type(left.get_type(), right.get_type(), false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type == RX_BOOL_TYPE)
		{
			throw std::invalid_argument("Can not divide bool values");
		}
		else if (ret_type == RX_INT8_TYPE || ret_type == RX_INT16_TYPE || ret_type == RX_INT32_TYPE || ret_type == RX_INT64_TYPE)
		{// integers stuff
			auto temp = right.get_integer();
			if (temp != 0)
				result.set_integer(left.get_integer() / temp, ret_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else if (ret_type == RX_UINT8_TYPE || ret_type == RX_UINT16_TYPE || ret_type == RX_UINT32_TYPE || ret_type == RX_UINT64_TYPE)
		{// integers stuff
			auto temp = right.get_unassigned();
			if (temp != 0)
				result.set_unassigned(left.get_unassigned() / temp, ret_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			auto temp = right.get_float();
			if (temp != 0)
				result.set_float(left.get_float() / temp, ret_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = left.get_complex();
			complex_value_struct two = right.get_complex();
			complex_type z1{ one.real, one.imag };
			complex_type z2{ two.real, two.imag };
			if (std::abs(z2) != 0)
			{
				z1 = z1 / z2;
				result.assign_static(complex_value_struct{ z1.real(), z1.imag() });
			}
			else
			{
				throw std::runtime_error("Division by zero!");
			}
		}
		else
		{
			throw std::invalid_argument("Syntax error in dividing");
		}
	}
	else
	{
		throw std::invalid_argument("Syntax error in dividing");
	}
	return result;
}


rx_value mod_values(const rx_value& left, const rx_value& right)
{
	rx_value result;
	result.set_good_locally();
	rx_value_t ret_type = get_arithmetic_result_type(left.get_type(), right.get_type(), false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type == RX_BOOL_TYPE)
		{
			throw std::invalid_argument("Can not divide bool values");
		}
		else if (ret_type == RX_INT8_TYPE || ret_type == RX_INT16_TYPE || ret_type == RX_INT32_TYPE || ret_type == RX_INT64_TYPE)
		{// integers stuff
			auto temp = right.get_integer();
			if (temp != 0)
				result.set_integer(left.get_integer() % temp, ret_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else if (ret_type == RX_UINT8_TYPE || ret_type == RX_UINT16_TYPE || ret_type == RX_UINT32_TYPE || ret_type == RX_UINT64_TYPE)
		{// integers stuff
			auto temp = right.get_unassigned();
			if (temp != 0)
				result.set_unassigned(left.get_unassigned() % temp, ret_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE || ret_type==RX_COMPLEX_TYPE)
		{// floating point stuff
			rx_value_t res_type = 0;
			auto temp = right.get_integer(&res_type);
			if(res_type==0)
				throw std::runtime_error("Syntax error!");
			if (temp != 0)
				result.set_integer(left.get_integer() % temp, res_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else
		{
			throw std::invalid_argument("Syntax error in dividing");
		}
	}
	else
	{
		throw std::invalid_argument("Syntax error in dividing");
	}
	return result;
}


// Class rx_internal::sys_runtime::data_source::value_point_impl 

value_point_impl::value_point_impl()
      : context_(nullptr),
        state_(value_point_not_connected),
        rate_(0)
{
}


value_point_impl::~value_point_impl()
{
}



void value_point_impl::connect (const string_type& path, uint32_t rate, data_controler* controler, char* buffer)
{
	if (!controler)
		controler = data_controler::get_controler();
	if (state_ != value_point_not_connected)
		disconnect(controler);

	rate_ = rate;
	parse_and_connect(path.c_str(), buffer, rx_time::now(), controler);

}

void value_point_impl::disconnect (data_controler* controler)
{
	if (!controler)
		controler = data_controler::get_controler();

	if (state_ != value_point_not_connected)
	{
		rate_ = 0;
		for (tag_handles_type::iterator it = tag_handles_.begin(); it != tag_handles_.end(); it++)
		{
			if (it->first)
			{
				controler->unregister_value(it->first, this);
				controler->remove_item(it->first);
			}
		}
		tag_handles_.clear();
		tag_variables_.clear();
		expression_.clear();
		state_ = value_point_not_connected;
	}
}

void value_point_impl::write (rx_simple_value val, runtime_transaction_id_t id, data_controler* controler)
{
	switch (state_)
	{
	case value_point_not_connected:
		{
			if (id)
			{// we should callback the negative result
				result_received(RX_NOT_CONNECTED, id);
			}
		}
		break;
	case value_point_connected:
		{
			if (id)
			{// we should callback the negative result
				result_received(RX_NOT_SUPPORTED, id);
			}
		}
		break;
	case value_point_connected_simple:
		{
			RX_ASSERT(tag_handles_.size() == 1);
			if (tag_handles_.empty() && id)
			{
				result_received(RX_NOT_SUPPORTED, id);
			}
			else
			{
				runtime_transaction_id_t new_id = id ? platform_runtime_manager::get_new_transaction_id() : 0u;
				if (id)
				{
					auto res = pending_transactions_.emplace(new_id, id);
					RX_ASSERT(res.second);
				}
				if (!controler)
					controler = data_controler::get_controler();
				controler->write_item(tag_handles_.begin()->first, std::move(val), new_id);
			}
		}
		break;
	default:
		RX_ASSERT(false);
	}
}

void value_point_impl::execute (data::runtime_values_data data, runtime_transaction_id_t id, data_controler* controler)
{
	switch (state_)
	{
	case value_point_not_connected:
		{
			if (id)
			{// we should callback the negative result
				execute_result_received(RX_NOT_CONNECTED, data::runtime_values_data(), id);
			}
		}
		break;
	case value_point_connected:
		{
			if (id)
			{// we should callback the negative result
				execute_result_received(RX_NOT_SUPPORTED, data::runtime_values_data(), id);
			}
		}
		break;
	case value_point_connected_simple:
		{
			RX_ASSERT(tag_handles_.size() == 1);
			if (tag_handles_.empty() && id)
			{
				execute_result_received(RX_NOT_SUPPORTED, data::runtime_values_data(), id);
			}
			else
			{
				runtime_transaction_id_t new_id = id ? platform_runtime_manager::get_new_transaction_id() : 0u;
				if (id)
				{
					auto res = pending_transactions_.emplace(new_id, id);
					RX_ASSERT(res.second);
				}
				if (!controler)
					controler = data_controler::get_controler();
				controler->execute_item(tag_handles_.begin()->first, std::move(data), new_id);
			}
		}
		break;
	default:
		RX_ASSERT(false);
	}
}

rx_value value_point_impl::calculate_local_var (const rx_value& val, char* token_buff)
{
	value_changed(0, val);
	rx_value res = internal_calculate(token_buff);
	return res;
}

void value_point_impl::parse_and_connect (const char* path, char* tbuff, const rx_time& now, data_controler* controler)
{
	char* token_buff = tbuff;
	auto path_len = strlen(path);
	if (path_len == 0)
		path_len = 0x10;
	if (!tbuff)
		token_buff = new char[path_len * 2];

	std::map<string_type, char> vars;

	int local_val_index = -1;

	string_type expres;
	size_t tags_count = 0;
	tag_variables_type tag_variables;

	const char* prog = path;

	char* write_to = token_buff;

	char current_var = (char)TAG_ID_OFFSET;

	while (*prog != '\0')
	{
		// skip blanks first
		if (iswhite(*prog))
			prog++;
		if (*prog == '\0')
			break;
		//find operators now
		if (*prog == '!' || *prog == '<' || *prog == '>')
		{
			if (*(prog + 1) == '=')
			{
				switch (*prog)
				{
				case '>':
					*write_to = (char)GE_CODE;
					break;
				case '<':
					*write_to = (char)LE_CODE;
					break;
				case '!':
					*write_to = (char)NE_CODE;
					break;
				default:
					assert(false);
				}
				write_to++;
				prog += 2;
			}
			else
				*write_to++ = *prog++;
		}
		else if (*prog == '|' && *(prog + 1) == '|')
		{
			*write_to = (char)OR_CODE;
			write_to++;
			prog += 2;
		}
		else if (*prog == '&' && *(prog + 1) == '&')
		{
			*write_to = (char)AND_CODE;
			write_to++;
			prog += 2;
		}
		else if (isoper(*prog))
			*write_to++ = *prog++;
		else if (iswdigit(*prog))
		{// found number here
			while (!isdelim(*prog))
				*write_to++ = *prog++;
		}
		else if (*prog == '\"')
		{// found string here
			*write_to++ = *prog++;
			while (!(*prog == '\"') && !(*prog == '\0'))
				*write_to++ = *prog++;
			if (*prog != '\0')
				*write_to++ = *prog++;
		}
		else if (*prog == '{')
		{// explicit variable declaration
			prog++;
			char* temp_ptr = write_to;
			while (!(*prog == '}') && !(*prog == '\0'))
				*write_to++ = *prog++;
			if (*prog != '\0')
				prog++;
			size_t my_size = write_to - temp_ptr;
			string_type var_name(temp_ptr, my_size);
			auto itv = vars.find(var_name);
			if (itv == vars.end())
			{
				if (var_name == POINT_LOCAL_VAR_NAME)
					local_val_index = (int)tags_count;
				tags_count++;
				rx_value dummy;
				dummy.set_quality(RX_NOT_CONNECTED_QUALITY);
				dummy.set_time(now);
				tag_variables.push_back(dummy);
				*temp_ptr = current_var++;
				vars.emplace(var_name, *temp_ptr);
			}
			else
				*temp_ptr = itv->second;
			temp_ptr++;
			write_to = temp_ptr;

		}
		else
		{// custom stuff, check for operations
			char* temp_ptr = write_to;
			while (!isdelim(*prog))
				*write_to++ = *prog++;
			bool found = false;
			size_t count = sizeof(operations) / sizeof(operations[0]);
			size_t my_size = write_to - temp_ptr;
			for (size_t idx = 0; idx < count; idx++)
			{
				if (my_size == operations[idx].len && memcmp(temp_ptr, operations[idx].name, operations[idx].len) == 0)
				{
					found = true;
					*temp_ptr = operations[idx].code;
					temp_ptr++;
					write_to = temp_ptr;
					break;
				}
			}
			if (!found)
			{// this is variable do it now
				string_type var_name(temp_ptr, my_size);
				auto itv = vars.find(var_name);
				if (itv == vars.end())
				{
					if (var_name == POINT_LOCAL_VAR_NAME)
						local_val_index = (int)tags_count;
					tags_count++;
					rx_value dummy;
					dummy.set_quality(RX_NOT_CONNECTED_QUALITY);
					dummy.set_time(now);
					tag_variables.push_back(dummy);
					*temp_ptr = current_var++;
					vars.emplace(var_name, *temp_ptr);
				}
				else
					*temp_ptr = itv->second;
				temp_ptr++;
				write_to = temp_ptr;
			}
		}
	}
	*write_to = '\0';
	expres = token_buff;
	bool simple = false;
	if (expres.size() == 1 && isvariable(expres[0]))
		simple = true;

	tag_handles_.clear();
	tag_variables_ = tag_variables;

	expression_ = expres;

	state_ = simple ? value_point_connected_simple : value_point_connected;

	calculate(token_buff);

	if (!vars.empty())
	{
		string_type buffer;
		for (auto it = vars.begin(); it != vars.end(); it++)
		{
			value_handle_type temp = 0;
			if (it->first != POINT_LOCAL_VAR_NAME)
			{
				if (translate_path(it->first, buffer))
					temp = controler->add_item(buffer, rate_);
				else
					temp = controler->add_item(it->first, rate_);
				if (temp)
				{
					tag_handles_[temp] = (it->second & TAG_ID_MASK);
					controler->register_value(temp, this);
				}
			}
			else
			{
				tag_handles_[0] = (it->second & TAG_ID_MASK);
			}
		}
	}

	if (!tbuff)
		delete[] token_buff;
}

void value_point_impl::calculate (char* token_buff)
{
	rx_value res = internal_calculate(token_buff);
	value_changed(res);
}

rx_value value_point_impl::internal_calculate (char* token_buff)
{
	uint32_t quality = 0;
	uint32_t bad_quality = 0;
	rx_time last_time;
	rx_time last_bad_time;
	rx_value res;
	bool bad = false;
	auto count = tag_variables_.size();
	for (size_t i = 0; i < count; i++)
	{
		if (tag_variables_[i].is_good())
		{
			if (tag_variables_[i].get_time() > last_time)
			{
				last_time = tag_variables_[i].get_time();
				quality = tag_variables_[i].get_quality();
				res = tag_variables_[i];
			}
		}
		else // (!tag_variables_[i].is_good())
		{// we're bad
			bad = true;
			if (tag_variables_[i].get_time() > last_time)
			{
				last_time = tag_variables_[i].get_time();
				res = tag_variables_[i];
			}
			if (tag_variables_[i].get_time() > last_bad_time)
			{
				last_bad_time = tag_variables_[i].get_time();
				bad_quality = tag_variables_[i].get_quality();
			}
		}
	}
	if (last_time.is_null())
		last_time = rx_time::now();
	if (bad)
	{
		res.set_quality(bad_quality);
		res.set_time(last_time);
		return res;
	}
	else
	{
		res.set_quality(RX_GOOD_QUALITY);
		try
		{
			get_expression(res, token_buff);
		}
		catch (const std::exception&)
		{
			res = rx_value();
			res.set_quality(RX_BAD_QUALITY_SYNTAX_ERROR);
		}
		if (res.is_good())
			res.set_quality(quality);
		res.set_time(last_time);
		return res;
	}
}

void value_point_impl::value_changed (value_handle_type handle, const rx_value& val)
{
	auto it = tag_handles_.find(handle);
	if (it != tag_handles_.end())
	{
		tag_variables_[it->second] = val;
	}
}

bool value_point_impl::shared_result_received (const rx_result& result, runtime_transaction_id_t id)
{
	auto it = pending_transactions_.find(id);
	if (it != pending_transactions_.end())
	{
		if (result)
			result_received(true, it->second);
		else
			result_received(result.errors(), it->second);
		pending_transactions_.erase(it);
		return true;
	}
	return false;
}

bool value_point_impl::single_result_received (rx_result result, runtime_transaction_id_t id)
{
	auto it = pending_transactions_.find(id);
	if (it != pending_transactions_.end())
	{
		result_received(std::move(result), it->second);
		pending_transactions_.erase(it);
		return true;
	}
	return false;
}

bool value_point_impl::shared_execute_result_received (const rx_result& result, const data::runtime_values_data& data, runtime_transaction_id_t id)
{
	auto it = pending_transactions_.find(id);
	if (it != pending_transactions_.end())
	{
		if (result)
			execute_result_received(true, data, it->second);
		else
			execute_result_received(result.errors(), data, it->second);
		pending_transactions_.erase(it);
		return true;
	}
	return false;
}

bool value_point_impl::single_execute_result_received (rx_result result, const data::runtime_values_data& data, runtime_transaction_id_t id)
{
	auto it = pending_transactions_.find(id);
	if (it != pending_transactions_.end())
	{
		execute_result_received(std::move(result), data, it->second);
		pending_transactions_.erase(it);
		return true;
	}
	return false;
}

void value_point_impl::result_received (rx_result&& result, runtime_transaction_id_t id)
{
}

void value_point_impl::execute_result_received (rx_result&& result, const data::runtime_values_data& data, runtime_transaction_id_t id)
{
}

bool value_point_impl::translate_path (const string_type& path, string_type& translated)
{
	if (context_)
	{
		if (path.size() > 2 && path[0] == RX_OBJECT_DELIMETER && path[1] != RX_DIR_DELIMETER && path[1] != RX_OBJECT_DELIMETER)
		{
			translated = context_->meta_info.get_full_path() + path;
			return true;
		}
		else
		{
			size_t idx = path.find_first_not_of(RX_OBJECT_DELIMETER_STR RX_DIR_DELIMETER_STR);
			if (idx != string_type::npos)
			{
				idx = path.find(RX_OBJECT_DELIMETER, idx);
				if (idx != string_type::npos)
				{
					rx_namespace_item item = context_->get_directory_resolver()->resolve_item(path.substr(0, idx));
					if (item)
					{
						translated = item.get_meta().get_full_path() + path.substr(idx);
						return true;
					}
				}
			}
		}
	}
	return false;
}

void value_point_impl::level0 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	char op;
	rx_value hold;
	hold.set_quality(RX_GOOD_QUALITY);

	level05(result, prog, token, tok_type, expres);
	while ((op = *token) == AND_CODE || op == OR_CODE)
	{
		get_token(prog, token, tok_type, expres);
		level05(hold, prog, token, tok_type, expres);
		arith(op, result, hold, prog, token, tok_type, expres);
	}
}

void value_point_impl::level05 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	char op;
	rx_value hold;
	hold.set_quality(RX_GOOD_QUALITY);

	level07(result, prog, token, tok_type, expres);
	while ((op = *token) == BIT_CODE || op == '&')
	{
		get_token(prog, token, tok_type, expres);
		level07(hold, prog, token, tok_type, expres);
		arith(op, result, hold, prog, token, tok_type, expres);
	}
}

void value_point_impl::level07 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	char op;
	rx_value hold;
	hold.set_quality(RX_GOOD_QUALITY);

	level1(result, prog, token, tok_type, expres);
	while ((op = *token) == '|')
	{
		get_token(prog, token, tok_type, expres);
		level1(hold, prog, token, tok_type, expres);
		arith(op, result, hold, prog, token, tok_type, expres);
	}
}

void value_point_impl::level1 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	char op;
	rx_value hold;
	hold.set_quality(RX_GOOD_QUALITY);

	level2(result, prog, token, tok_type, expres);
	while ((op = *token) == '>' || op == '<' || op == '=' || op == GE_CODE || op == LE_CODE || op == NE_CODE)
	{
		get_token(prog, token, tok_type, expres);
		level2(hold, prog, token, tok_type, expres);
		arith(op, result, hold, prog, token, tok_type, expres);
	}
}

void value_point_impl::level2 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	char op;
	rx_value hold;
	hold.set_quality(RX_GOOD_QUALITY);

	level3(result, prog, token, tok_type, expres);
	while ((op = *token) == '+' || op == '-')
	{
		get_token(prog, token, tok_type, expres);
		level3(hold, prog, token, tok_type, expres);
		arith(op, result, hold, prog, token, tok_type, expres);
	}
}

void value_point_impl::level3 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	char op;
	rx_value hold;
	hold.set_quality(RX_GOOD_QUALITY);

	level4(result, prog, token, tok_type, expres);
	while ((op = *token) == '*' || op == '/' || op == DIV_CODE || op == '%')
	{
		get_token(prog, token, tok_type, expres);
		level4(hold, prog, token, tok_type, expres);
		arith(op, result, hold, prog, token, tok_type, expres);
	}
}

void value_point_impl::level4 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	rx_value hold;
	hold.set_quality(RX_GOOD_QUALITY);

	level5(result, prog, token, tok_type, expres);
	if ((*token) == '^')
	{
		get_token(prog, token, tok_type, expres);
		level4(hold, prog, token, tok_type, expres);
		arith('^', result, hold, prog, token, tok_type, expres);
	}
}

void value_point_impl::level5 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	char op;
	op = '\0';

	if ((tok_type == DELIMETER) && (isunary(*token)))
	{
		op = *token;
		get_token(prog, token, tok_type, expres);
	}
	if (tok_type != STRING)
	{
		if (isunary(*token))
			level5(result, prog, token, tok_type, expres);
		else
			level6(result, prog, token, tok_type, expres);
		if (op)
			unary(op, result, prog, token, tok_type, expres);
	}
	else
	{
		level6(result, prog, token, tok_type, expres);
		if (op)
			unary(op, result, prog, token, tok_type, expres);
	}
}

void value_point_impl::level6 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	if ((*token == '(') && (tok_type == DELIMETER))
	{
		get_token(prog, token, tok_type, expres);
		level0(result, prog, token, tok_type, expres);
		if (*token != ')')
		{
			char err[256];
			snprintf(err, sizeof(err), "Unbalanced parentheses at char %d", (int)(prog - expres));
			throw std::runtime_error("Parser error!");
		}
		get_token(prog, token, tok_type, expres);
	}
	else
		primitive(result, prog, token, tok_type, expres);
}

void value_point_impl::primitive (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres)
{
	if (tok_type == NUMBER)
	{
		char* endptr = NULL;
		size_t sz = strlen(token);
		if (sz > 0 && (token[sz - 1] == 'f' || token[sz - 1] == 'F'))
		{// float value

			if (endptr - token != (int)(sz - 1))
				throw std::runtime_error("Syntax error!!!");
			result.assign_static<double>(strtod(token, &endptr));
			get_token(prog, token, tok_type, expres);
			return;
		}
		else
		{
			long val = 0;
			if (sz > 0 && (token[sz - 1] == 'h' || token[sz - 1] == 'H'))
			{// hex value
				val = strtol(token, &endptr, 16);
				if (endptr - token != (int)sz - 1)
					throw std::runtime_error("Syntax error!!!");
			}
			else
			{
				val = strtol(token, &endptr, 10);
				if (endptr - token != (int)sz)
				{// try double here
					double dblval = strtod(token, &endptr);
					if (endptr - token != (int)sz)
					{
						throw std::runtime_error("Syntax error!!!");
					}
					// we found double so do the stuff
					result.assign_static<double>(std::move(dblval));
					get_token(prog, token, tok_type, expres);
					return;
				}
			}
			if (val == 0 || val == 1)
				result.assign_static<bool>(val != 0);
			else if (val >= -128 && val <= 127)
				result.assign_static<int8_t>(std::move((int8_t)val));
			else if (val >= -32768 && val <= 32767)
				result.assign_static<int16_t>(std::move((int16_t)val));
			else
				result.assign_static<int32_t>(std::move((int32_t)val));
			get_token(prog, token, tok_type, expres);
			return;
		}
	}
	else if (tok_type == STRING)
	{
		result.assign_static(token);
		get_token(prog, token, tok_type, expres);
		return;
	}
	else if (tok_type == VARIABLE)
	{
		int index = (int)(((*token) & TAG_ID_MASK));
		if (index < 0 || index >= (int)tag_variables_.size())
		{
			char err[256];
			snprintf(err, sizeof(err), "Undefined tag at char %d", (int)(prog - expres));
			throw std::runtime_error(err);
		}
		result = tag_variables_[index];

		get_token(prog, token, tok_type, expres);
		return;
	}
	else
	{
		char err[256];
		snprintf(err, sizeof(err), "Syntax error at char %d", (int)(prog - expres));
		throw std::runtime_error(err);
	}
}

void value_point_impl::get_token (char*& prog, char*& token, char& tok_type, char*& expres)
{
	char* temp;

	tok_type = 0;
	temp = token;

	while (iswhite(*prog)) ++prog;

	if (isoper(*prog))
	{
		tok_type = DELIMETER;
		*temp++ = *prog++;
	}
	else if (isvariable(*prog))
	{
		*temp++ = *prog++;
		tok_type = VARIABLE;
	}
	else if (*prog == '"')
	{
		prog++;
		while (!(*prog == '"') && !(*prog == '\0')) *temp++ = *prog++;
		if (*prog != '"')
		{
			char err[256];
			snprintf(err, sizeof(err), "Unbalanced tag commas at char %d", (int)(prog - expres));
			throw std::runtime_error("Parser error!");
		}
		tok_type = STRING;
		prog++;
	}
	else if (iswdigit(*prog))
	{
		while (!isdelim(*prog)) *temp++ = *prog++;
		tok_type = NUMBER;
	}

	*temp = '\0';
}

void value_point_impl::put_back (char*& prog, char*& token, char& tok_type, char*& expres)
{
	char* t;
	t = token;
	for (; *t; t++) prog--;
}

void value_point_impl::unary (uint8_t o, rx_value& r, char*& prog, char*& token, char& tok_type, char*& expres)
{
	if (o == '-')
	{
		rx_value_t type = r.get_type();
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			temp.real = -temp.real;
			temp.imag = -temp.imag;
			r.assign_static(temp);
		}
		else if (r.is_integer())
		{
			r.set_integer(-r.get_integer(), type);
		}
		else if (r.is_float())
		{
			r.set_float(-r.get_float(), type);
		}
		else if (!r.is_unassigned())
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == '!')
	{
		bool val = r.get_bool();
		r.assign_static(val);
	}
	else if (o == ABS_CODE)
	{
		uint8_t type = r.get_type();
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			temp.real = -temp.real;
			temp.imag = -temp.imag;
			r.set_float(sqrt(temp.real * temp.real + temp.imag * temp.imag), RX_DOUBLE_TYPE);
		}
		else if (r.is_integer())
		{
			r.set_integer(abs(r.get_integer()), type);
		}
		else if (r.is_float())
		{
			r.set_float(abs(r.get_float()), type);
		}
		else if (!r.is_unassigned())
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == LOG_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = log10(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(log10(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == LN_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = std::log(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(std::log(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == EXP_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = exp(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(exp(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == SQRT_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = sqrt(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(sqrt(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == SIN_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = sin(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(sin(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == COS_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = cos(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(cos(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == TAN_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = tan(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(tan(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == ASIN_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = asin(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(asin(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == ACOS_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = acos(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(acos(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else if (o == ATAN_CODE)
	{
		if (r.is_complex())
		{
			complex_value temp = r.get_complex();
			complex_type arg(temp.real, temp.imag);
			complex_type res = atan(arg);
			complex_value_struct temp3;
			temp3.real = res.real();
			temp3.imag = res.imag();
			r.assign_static(temp3);
		}
		else if (r.is_numeric())
			r.set_float(atan(r.get_float()), RX_DOUBLE_TYPE);
		else
			throw std::runtime_error("Parser error, invalid op!");
	}
	else
		RX_ASSERT(false);
}

void value_point_impl::arith (uint8_t o, rx_value& r, rx_value& h, char*& prog, char*& token, char& tok_type, char*& expres)
{
	switch ((uint8_t)o)
	{
	case '>':
		r.assign_static<bool>(rx_compare_values(&r.c_ptr()->value, &h.c_ptr()->value) > 0);
		break;
	case GE_CODE:
		r.assign_static<bool>(rx_compare_values(&r.c_ptr()->value, &h.c_ptr()->value) >= 0);
		break;
	case LE_CODE:
		r.assign_static<bool>(rx_compare_values(&r.c_ptr()->value, &h.c_ptr()->value) <= 0);
		break;
	case NE_CODE:
		r.assign_static<bool>(rx_compare_values(&r.c_ptr()->value, &h.c_ptr()->value) != 0);
		break;
	case '<':
		r.assign_static<bool>(rx_compare_values(&r.c_ptr()->value, &h.c_ptr()->value) < 0);
		break;
	case '=':
		r.assign_static<bool>(rx_compare_values(&r.c_ptr()->value, &h.c_ptr()->value) == 0);
		break;
	case '+':
		r = add_values(r, h);
		break;
	case '-':
		r = sub_values(r, h);
		break;
	case '*':
		r = mul_values(r, h);
		break;
	case '/':
		r = div_values(r, h);
		break;
	case '%':
		r = mod_values(r, h);
		break;
	case BIT_CODE:
		{
			rx_value_t t1;
			rx_value_t t2;
			uint64_t res = r.get_unassigned(&t1);
			uint8_t hl = (uint8_t)h.get_unassigned(&t2);
			if (t1 == 0 || t2 == 0)
			{
				throw std::runtime_error("Syntax error, operator (|).");
			}
			uint64_t rl = (uint64_t)res;
			if (hl > 63)
				res = 0;
			r.assign_static<bool>(((rl & (1ll << hl)) != 0));
		}
		break;

	case '&':
		{
			rx_value_t t1;
			rx_value_t t2;
			uint64_t h1 = r.get_unassigned(&t1);
			uint64_t h2 = h.get_unassigned(&t2);
			if (t1 == 0 || t2 == 0)
			{
				throw std::runtime_error("Syntax error, operator (|).");
			}
			r.set_unassigned((h1 & h2), std::max(t1, t2));
		}
		break;

	case '|':
		{
			rx_value_t t1;
			rx_value_t t2;
			uint64_t h1 = r.get_unassigned(&t1);
			uint64_t h2 = h.get_unassigned(&t2);
			if (t1 == 0 || t2 == 0)
			{
				throw std::runtime_error("Syntax error, operator (|).");
			}
			r.set_unassigned((h1 | h2), std::max(t1, t2));
		}
		break;
	case '^':
		{
			if (r.is_complex() || h.is_complex())
			{
				complex_value_struct temp1 = r.get_complex();
				complex_value_struct temp2 = r.get_complex();
				complex_type arg1(temp1.real, temp1.imag);
				complex_type arg2(temp2.real, temp2.imag);
				complex_type res = pow(arg1, arg2);
				complex_value_struct temp3;
				temp3.real = res.real();
				temp3.imag = res.imag();
				r.assign_static(temp3);
			}
			else
			{
				r.set_float(pow(r.get_float(), h.get_float()), RX_DOUBLE_TYPE);
			}
		}
		break;
	case AND_CODE:
		r.assign_static<bool>(r.get_bool() && h.get_bool());
		break;
	case OR_CODE:
		r.assign_static<bool>(r.get_bool() || h.get_bool());
		break;
	default:
		RX_ASSERT(false);
	}
}

void value_point_impl::get_expression (rx_value& result, char* tok)
{
	if (state_ == value_point_connected_simple)
	{// simple just tag here
		RX_ASSERT(tag_variables_.size() == 1);
		result = tag_variables_[0];
	}
	else
	{// complex, full expression
		try
		{
			char* token = tok;
			char* expres = &expression_[0];
			char* prog = expres;
			char tok_type = 0;

			get_token(prog, token, tok_type, expres);
			if (!(*token))
			{
				throw std::runtime_error("No expression present");
			}
			level0(result, prog, token, tok_type, expres);
		}
		catch (std::runtime_error&)
		{
			result.set_quality(RX_BAD_QUALITY_SYNTAX_ERROR);
		}
	}
}


void value_point_impl::set_context (rx_platform::runtime::runtime_process_context * value)
{
  context_ = value;
}


} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal

