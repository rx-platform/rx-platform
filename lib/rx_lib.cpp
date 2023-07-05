

/****************************************************************************
*
*  lib\rx_lib.cpp
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


// rx_lib
#include "lib/rx_lib.h"

#include "lib/rx_mem.h"
#include "version/rx_version.h"
#include "lib/rx_values.h"

using namespace rx::values;

string_type _implemented_func(const char* fname)
{
    return string_type(fname);
}

namespace rx
{
rx_string_wrapper::rx_string_wrapper()
{
	rx_init_string_value_struct(this, NULL, -1);
}
rx_string_wrapper::rx_string_wrapper(const string_value_struct& val)
{
	int ret_val = rx_copy_string_value(this, &val);
	if (!ret_val)
		rx_init_string_value_struct(this, NULL, -1);
}
rx_string_wrapper::rx_string_wrapper(const string_type& val)
{
	int ret_val = RX_ERROR;
	if (!val.empty())
	{
		ret_val = rx_init_string_value_struct(this, &val[0], (int)val.size());
	}
	if (!ret_val)
		rx_init_string_value_struct(this, NULL, -1);
}
rx_string_wrapper::rx_string_wrapper(const char* val)
{
	int ret_val = rx_init_string_value_struct(this, val, -1);
	if (!ret_val)
		rx_init_string_value_struct(this, NULL, -1);
}
rx_string_wrapper::rx_string_wrapper(string_view_type val)
{
	int ret_val = RX_ERROR;
	if (!val.empty())
	{
		ret_val = rx_init_string_value_struct(this, &val[0], (int)val.size());
	}
	if (!ret_val)
		rx_init_string_value_struct(this, NULL, -1);
}

bool rx_string_wrapper::empty() const
{
	return string_value_struct::size == 0;
}
size_t rx_string_wrapper::size() const
{
	return string_value_struct::size;
}
const char* rx_string_wrapper::c_str() const
{
	return rx_c_str(this);
}

string_type rx_string_wrapper::to_string() const
{
	return rx_to_std_string(*this);
}
string_view_type rx_string_wrapper::to_string_view() const
{
	if (string_value_struct::size == 0)
		return string_view_type();
	const char* ptr = rx_c_str(this);
	if (ptr)
		return string_view_type(ptr, string_value_struct::size);
	else
		return string_view_type();
}

rx_string_wrapper::~rx_string_wrapper()
{
	rx_destory_string_value_struct(this);
	static_assert(sizeof(rx_string_wrapper) == sizeof(string_value_struct), "Memory size has to be the same, no virtual functions or members");
}

string_type rx_to_std_string(const string_value_struct& str)
{
	const char* ptr = rx_c_str(&str);
	if (ptr)
		return string_type(ptr);
	else
		return string_type();
}

rx_bytes_wrapper::rx_bytes_wrapper()
{
	rx_init_bytes_value_struct(this, NULL, -1);
}
rx_bytes_wrapper::rx_bytes_wrapper(const bytes_value_struct& val)
{
	int ret_val = rx_copy_bytes_value(this, &val);
	if (!ret_val)
		rx_init_bytes_value_struct(this, NULL, -1);
}
rx_bytes_wrapper::rx_bytes_wrapper(const byte_string& val)
{
	int ret_val = RX_ERROR;
	if (!val.empty())
	{
		// this code bellow i personally hate
		// it is required as this is extern "C" structure
		ret_val = rx_init_bytes_value_struct(this, (uint8_t*)&val[0], val.size());
	}
	if (!ret_val)
		rx_init_bytes_value_struct(this, NULL, 0);
}
rx_bytes_wrapper::rx_bytes_wrapper(const uint8_t* data, size_t count)
{
	int ret_val = RX_ERROR;
	if (data && count)
	{
		ret_val = rx_init_bytes_value_struct(this, data, count);
	}
	if (!ret_val)
		rx_init_bytes_value_struct(this, NULL, 0);
}

bool rx_bytes_wrapper::empty() const
{
	return bytes_value_struct::size == 0;
}
size_t rx_bytes_wrapper::size() const
{
	return bytes_value_struct::size;
}
const uint8_t* rx_bytes_wrapper::data() const
{
	size_t temp = 0;
	return rx_c_ptr(this, &temp);
}

byte_string rx_bytes_wrapper::to_bytes() const
{
	return rx_to_std_bytes(*this);
}

rx_bytes_wrapper::~rx_bytes_wrapper()
{
	rx_destory_bytes_value_struct(this);
	static_assert(sizeof(rx_bytes_wrapper) == sizeof(bytes_value_struct), "Memory size has to be the same, no virtual functions or members");
}

byte_string rx_to_std_bytes(const bytes_value_struct& str)
{
	size_t count = 0;
	const std::byte* ptr = (const std::byte*)rx_c_ptr(&str, &count);
	if (ptr && count)
		return byte_string(ptr, ptr + count);
	else
		return byte_string();
}

byte_string rx_hexstr_to_bytes(const string_type& str)
{
	byte_string ret;
	uint32_t size = 0;
	uint32_t char_idx = 0;

	while (str[char_idx] != '\0')
	{
		uint8_t one_byte = 0;
		char temp = str[char_idx];
		if (temp >= '0' && temp <= '9')
		{
			one_byte += temp - '0';
		}
		else if (temp >= 'A' && temp <= 'F')
		{
			one_byte += temp - 'A' + 0xa;
		}
		else if (temp >= 'a' && temp <= 'f')
		{
			one_byte += temp - 'a' + 0xa;
		}
		else
		{
			return byte_string();
		}
		one_byte <<= 4;
		char_idx++;
		temp = str[char_idx];
		if (temp >= '0' && temp <= '9')
		{
			one_byte += temp - '0';
		}
		else if (temp >= 'A' && temp <= 'F')
		{
			one_byte += temp - 'A' + 0xa;
		}
		else if (temp >= 'a' && temp <= 'f')
		{
			one_byte += temp - 'a' + 0xa;
		}
		else
		{
			return byte_string();
		}
		char_idx++;
		ret.push_back((std::byte)one_byte);
	}
	return ret;
}


rx_result::rx_result()
{
	rx_init_result_struct(&data_);
}
rx_result::rx_result(rx_result&& right) noexcept
{
	rx_move_result_struct(&data_, &right.data_);
}
rx_result& rx_result::operator=(rx_result&& right) noexcept
{
	rx_destroy_result_struct(&data_);
	rx_move_result_struct(&data_, &right.data_);
	return *this;
}

rx_result::~rx_result()
{
	rx_destroy_result_struct(&data_);
}

rx_result::rx_result(bool value)
{
	if (value)
	{
		rx_init_result_struct(&data_);
	}
	else
	{
		auto res = rx_init_result_struct_with_error(&data_, 1, UNDEFINED_RESULT_TEXT, -1);
		RX_ASSERT(res == RX_OK);
		if (res != RX_OK)
		{
			rx_init_result_struct(&data_);
		}
	}
}
rx_result::rx_result(rx_result_struct errors) noexcept
{
	rx_move_result_struct(&data_, &errors);
}
rx_result::rx_result(const rx_result_struct* errors)
{
	rx_copy_result_struct(&data_, errors);
}
rx_result::rx_result(const rx_result_erros_t& errors)
{
	if (errors.empty())
	{
		rx_init_result_struct(&data_);
	}
	else
	{
		const char* static_errors[RESULT_STATIC_SIZE * 2];
		const char** texts = static_errors;
		size_t errors_size = errors.size();
		if (errors_size > sizeof(static_errors) / sizeof(static_errors[0]))
			texts = new const char* [errors_size];
		for (size_t i = 0; i < errors_size; i++)
		{
			texts[i] = errors[i].c_str();
		}
		int ret = rx_init_result_struct_with_errors(&data_, nullptr, texts, errors_size);
		RX_ASSERT(ret == RX_OK);
		if (ret != RX_OK)
			rx_init_result_struct(&data_);
		if (errors_size > sizeof(static_errors) / sizeof(static_errors[0]))
			delete[] texts;
	}
}
rx_result::rx_result(string_view_type error)
{
	int res = RX_OK;
	if (error.empty())
	{
		res = rx_init_result_struct_with_error(&data_, 1, UNDEFINED_RESULT_TEXT, -1);
	}
	else
	{
		res = rx_init_result_struct_with_error(&data_, 1, &error[0], (int)error.size());
	}
	RX_ASSERT(res == RX_OK);
	if (res != RX_OK)
	{
		rx_init_result_struct(&data_);
	}
}

rx_result::rx_result(const string_type& error)
{
	int res = RX_OK;
	if (error.empty())
	{
		res = rx_init_result_struct_with_error(&data_, 1, UNDEFINED_RESULT_TEXT, -1);
	}
	else
	{
		res = rx_init_result_struct_with_error(&data_, 1, error.c_str(), (int)error.size());
	}
	RX_ASSERT(res == RX_OK);
	if (res != RX_OK)
	{
		rx_init_result_struct(&data_);
	}
}
rx_result::rx_result(const char* error)
{
	int res = RX_OK;
	if (error == NULL)
	{
		res = rx_init_result_struct_with_error(&data_, 1, UNDEFINED_RESULT_TEXT, -1);
	}
	else
	{
		res = rx_init_result_struct_with_error(&data_, 1, error, -1);
	}
	RX_ASSERT(res == RX_OK);
	if (res != RX_OK)
	{
		rx_init_result_struct(&data_);
	}
}

rx_result_struct rx_result::move()
{
	rx_result_struct ret;
	rx_move_result_struct(&ret, &data_);
	return ret;
}

const rx_result_struct* rx_result::c_ptr() const
{
	return &data_;
}
rx_result_erros_t rx_result::errors() const
{
	uint32_t code;
	const char* err;
	rx_result_erros_t ret;
	size_t count = rx_result_errors_count(&data_);
	if (count)
	{
		ret.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			err = rx_result_get_error(&data_, i, &code);
			if (err)
				ret.emplace_back(err);
			else
				ret.emplace_back("Undefined");
		}
	}
	return ret;
}
rx_result::operator bool() const
{
	return rx_result_ok(&data_);
}

string_type rx_result::errors_line(char delim) const
{
	if (*this)
		return "";
	rx_result_erros_t errs = errors();
	if (errs.empty())
		return "No specific errors!";
	std::ostringstream ss;
	bool first = true;
	for (const auto& one : errs)
	{
		if (first)
			first = false;
		else
			ss << delim;
		ss << one;
	}
	return ss.str();
}

void rx_result::register_error(string_view_type error, uint32_t code)
{
	if (error.empty())
		rx_result_add_error(&data_, code, UNDEFINED_RESULT_TEXT, -1);
	else
		rx_result_add_error(&data_, code, &error[0], (int)error.size());

}
void rx_result::register_errors(const rx_result_erros_t& errors)
{
	if (errors.empty())
		return; // nothing to do

	for (const auto& one : errors)
		register_error(one);
}

rx_result rx_result::create_from_last_os_error(string_view_type text)
{
	char buffer[0x100];

	rx_last_os_error(text.empty() ? nullptr : &text[0], buffer, sizeof(buffer));

	return rx_result(buffer);
}
rx_result rx_result::create_from_c_error(string_view_type text)
{
	return rx_result(text);
}



// placement new and MSVC debug heap problem
#ifdef _MSC_VER
#ifdef _DEBUG
#undef new
#endif
#endif

rx_item_reference::rx_item_reference()
{
	rx_init_null_reference(&data_);
}
rx_item_reference::rx_item_reference(const rx_reference_struct* data)
{
	auto ret = rx_copy_reference(&data_, data);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
}

rx_item_reference::rx_item_reference(rx_reference_struct data) noexcept
{
	rx_move_reference(&data_, &data);
}
rx_item_reference::rx_item_reference(const rx_item_reference& right)
{
	auto ret = rx_copy_reference(&data_, &right.data_);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
}

rx_item_reference::rx_item_reference(rx_item_reference&& right) noexcept
{
	rx_move_reference(&data_, &right.data_);
}
rx_item_reference::rx_item_reference(const rx_node_id& right)
{
	auto ret = rx_init_id_reference(&data_, right.c_ptr());
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
}

rx_item_reference::rx_item_reference(string_view_type right)
{
	int ret;
	if (right.empty())
	{
		rx_init_path_reference(&data_, NULL, -1);
	}
	else
	{
		ret = rx_init_path_reference(&data_, &right[0], (int)right.size());
		RX_ASSERT(ret==RX_OK);
        if(ret!=RX_OK)
            rx_init_null_reference(&data_);
	}
}

rx_item_reference::rx_item_reference(const char* right)
{
	auto ret = rx_init_path_reference(&data_, right, -1);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
}

rx_item_reference::rx_item_reference(const string_type& right)
{
	int ret = RX_OK;
	if (right.empty())
	{
		rx_init_path_reference(&data_, NULL, -1);
	}
	else
	{
		ret = rx_init_path_reference(&data_, &right[0], (int)right.size());
		if(ret != RX_OK)
            rx_init_path_reference(&data_, NULL, -1);
	}
	RX_ASSERT(ret);
}


rx_item_reference::~rx_item_reference()
{
	rx_deinit_reference(&data_);
	static_assert(sizeof(rx_item_reference) == sizeof(rx_reference_struct), "Memory size has to be the same, no virtual functions or members");
}


rx_item_reference& rx_item_reference::operator=(const rx_item_reference& right)
{
	rx_deinit_reference(&data_);
	auto ret = rx_copy_reference(&data_, &right.data_);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
	return *this;
}
rx_item_reference& rx_item_reference::operator=(rx_item_reference&& right) noexcept
{

	rx_deinit_reference(&data_);
	auto ret = rx_move_reference(&data_, &right.data_);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
	return *this;
}



bool rx_item_reference::is_null() const
{
	return rx_is_null_reference(&data_) != 0;
}

const rx_reference_struct* rx_item_reference::c_ptr() const
{
	return &data_;
}
rx_reference_struct rx_item_reference::move() noexcept
{
	rx_reference_struct ret;
	rx_move_reference(&ret, &data_);
	return ret;
}

rx_item_reference& rx_item_reference::operator = (const rx_node_id& right)
{
	rx_deinit_reference(&data_);
	auto ret = rx_init_id_reference(&data_, right.c_ptr());
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
	return *this;
}

rx_item_reference& rx_item_reference::operator = (const string_type& right)
{
	rx_deinit_reference(&data_);
	auto ret = rx_init_path_reference(&data_, right.c_str(), -1);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
	return *this;
}
rx_item_reference& rx_item_reference::operator = (string_view_type right)
{
	rx_deinit_reference(&data_);
	auto ret = rx_init_path_reference(&data_, &right[0], (int)right.size());
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_reference(&data_);
	return *this;
}
bool rx_item_reference::is_node_id() const
{
	return data_.is_path == 0;
}

string_type rx_item_reference::to_string() const
{
	string_value_struct str;
	int ret = rx_reference_to_string(&data_, &str);
	if (ret)
	{
		const char* ptr = rx_c_str(&str);
		if (ptr)
			return string_type(ptr);
		else
			return string_type();
	}
	return string_type();
}

string_type rx_item_reference::get_path() const
{
	if (data_.is_path == 0)
	{
		throw std::invalid_argument("Target is referenced by id!");
	}
	else
	{
		const char* ptr = rx_c_str(&data_.data.path);
		if (ptr)
			return string_type(ptr);
		else
			return string_type();
	}
}

rx_node_id rx_item_reference::get_node_id() const
{
	if (data_.is_path)
		throw std::invalid_argument("Target is referenced by path!");
	else
		return rx_node_id(&data_.data.id);
}

rx_uuid::~rx_uuid()
{
	static_assert(sizeof(rx_uuid) == sizeof(rx_uuid_t), "Memory size has to be the same, no virtual functions or members");
}
rx_uuid::rx_uuid()
{
	memzero(this, sizeof(rx_uuid_t));
}

rx_uuid::rx_uuid(const rx_uuid& rigth)
{
	memcpy(this, &rigth, sizeof(rx_uuid_t));
}

rx_uuid::rx_uuid(const rx_uuid_t& rigth)
{
	memcpy(this, &rigth, sizeof(rx_uuid_t));
}
rx_uuid& rx_uuid::operator=(const rx_uuid& right)
{
	memcpy(this, &right, sizeof(rx_uuid_t));
	return *this;
}
rx_uuid::rx_uuid(rx_uuid&& right) noexcept
{
	memcpy(this, &right, sizeof(rx_uuid_t));
}
rx_uuid& rx_uuid::operator=(rx_uuid&& right) noexcept
{
	memcpy(this, &right, sizeof(rx_uuid_t));
	return *this;
}
bool rx_uuid::operator==(const rx_uuid& right) const
{
	return (memcmp(this, &right, sizeof(rx_uuid_t)) == 0);
}
bool rx_uuid::operator!=(const rx_uuid& right) const
{
	return (memcmp(this, &right, sizeof(rx_uuid_t)) != 0);
}
bool rx_uuid::operator>(const rx_uuid& right) const
{
	return (memcmp(this, &right, sizeof(rx_uuid_t))>0);
}
bool rx_uuid::operator>=(const rx_uuid& right) const
{
	return (memcmp(this, &right, sizeof(rx_uuid_t)) >= 0);

}
bool rx_uuid::operator<(const rx_uuid& right) const
{
	return (memcmp(this, &right, sizeof(rx_uuid_t)) <0);
}

bool rx_uuid::operator<=(const rx_uuid& right) const
{
	return (memcmp(this, &right, sizeof(rx_uuid_t)) <= 0);
}
// {98862E03-F664-48C5-A6AC-24761B5D479F}
//rx_uuid g_null_uuid;


rx_uuid rx_uuid::create_new()
{
	rx_uuid ret;
	rx_generate_new_uuid(&ret);
	return ret;
}
const rx_uuid& rx_uuid::null_uuid()
{
	static rx_uuid s_null = rx_uuid{ {0} };
	return s_null;
}
rx_uuid rx_uuid::create_from_string(const string_type& str)
{

	rx_uuid temp;
	if (RX_OK == rx_string_to_uuid(str.c_str(), &temp))
		return temp;
	else
		return rx_uuid{ {0} };
}
bool rx_uuid::is_null() const
{
	static rx_uuid s_null = rx_uuid{ {0} };
	return memcmp(this, &s_null, sizeof(rx_uuid_t)) == 0;
}

void rx_uuid::to_string(string_type& str) const
{
	char buff[0x40];
	if (RX_OK == rx_uuid_to_string(this, buff))
	{
		str = buff;
	}
}

string_type rx_uuid::to_string() const
{
	string_type temp;
	to_string(temp);
	return temp;
}


std::ostream & operator << (std::ostream &out, const rx_node_id &val)
{
	out << val.to_string();
	return out;
}

rx_node_id::rx_node_id() noexcept
{
	rx_init_null_node_id(&data_);

}
rx_node_id::rx_node_id(const rx_node_id_struct* right)
{
	auto ret = rx_copy_node_id(&data_, right);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
}
rx_node_id::rx_node_id(rx_node_id_struct right) noexcept
{
	rx_move_node_id(&data_, &right);
}

rx_node_id::rx_node_id(const rx_node_id &right) 
{
	auto ret = rx_copy_node_id(&data_, &right.data_);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
}

rx_node_id::rx_node_id(uint32_t id, uint16_t namesp) noexcept
{
	auto ret = rx_init_int_node_id(&data_, id, namesp);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
}

rx_node_id::rx_node_id(const char* id, uint16_t namesp)
{
	auto ret = rx_init_string_node_id(&data_, id, -1, namesp);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
}
rx_node_id::rx_node_id(rx_uuid_t& id, uint16_t namesp)
{
	auto ret = rx_init_uuid_node_id(&data_, &id, namesp);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
}


rx_node_id::rx_node_id(const byte_string& id, uint16_t namesp)
{
	auto ret = rx_init_bytes_node_id(&data_, (uint8_t*)&id[0], id.size(), namesp);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
}


rx_node_id::rx_node_id(rx_node_id&& right) noexcept
{
	auto ret = rx_copy_node_id(&data_, &right.data_);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
}


rx_node_id::~rx_node_id()
{
	rx_destory_node_id(&data_);
	static_assert(sizeof(rx_node_id) == sizeof(rx_node_id_struct), "Memory size has to be the same, no virtual functions or members");
}


rx_node_id & rx_node_id::operator=(const rx_node_id &right)
{
	rx_destory_node_id(&data_);
	auto ret = rx_copy_node_id(&data_, &right.data_);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
	return *this;
}

rx_node_id& rx_node_id::operator=(rx_node_id &&right) noexcept
{
	rx_destory_node_id(&data_);
	auto ret = rx_move_node_id(&data_, &right.data_);
	RX_ASSERT(ret==RX_OK);
	if(ret!=RX_OK)
        rx_init_null_node_id(&data_);
	return *this;
}

const rx_node_id_struct* rx_node_id::c_ptr() const
{
	return &data_;
}
rx_node_id_struct rx_node_id::move() noexcept
{
	rx_node_id_struct ret;
	rx_move_node_id(&ret, &data_);
	return ret;
}

bool rx_node_id::operator==(const rx_node_id &right) const
{
	return rx_compare_node_ids(&data_, &right.data_) == 0;
}

bool rx_node_id::operator!=(const rx_node_id &right) const
{
	return !operator==(right);
}



bool rx_node_id::operator < (const rx_node_id& right) const
{
	return rx_compare_node_ids(&data_, &right.data_) < 0;
}

void rx_node_id::to_string(string_type& val) const
{
	string_value_struct str;
	int ret = rx_node_id_to_string(&data_, &str);
	if (ret)
	{
		const char* ptr = rx_c_str(&str);
		if (ptr)
			val = ptr;
		else
			val.clear();
	}
	else
	{
		val.clear();
	}
}
string_type rx_node_id::to_string() const
{
	string_type temp;
	to_string(temp);
	return temp;
}
rx_node_id rx_node_id::generate_new(uint16_t namesp)
{
	rx_uuid_t temp;
	rx_generate_new_uuid(&temp);
	return rx_node_id(temp, namesp);
}
rx_node_id rx_node_id::opcua_standard_id(uint32_t id)
{
	return rx_node_id(id, 0);
}
rx_node_id rx_node_id::from_string(const char* value)
{
	rx_node_id ret;
	int ret_val = rx_node_id_from_string(&ret.data_, value);
	if (ret_val)
		return ret;
	else
		return rx_node_id();
}

bool rx_node_id::is_null() const
{
	return rx_is_null_node_id(&data_);
}
rx_node_id::operator bool() const
{
	return !is_null();
}
bool rx_node_id::is_standard() const
{
	return data_.namespace_index == 1 && data_.node_type == rx_node_id_numeric;
}
bool rx_node_id::is_opc() const
{
	return data_.namespace_index == 0 && data_.node_type == rx_node_id_numeric;
}


void rx_node_id::set_string_id(const char* strid)
{
	uint16_t namesp = data_.namespace_index;
	rx_destory_node_id(&data_);
	rx_init_string_node_id(&data_, strid, -1, namesp);
}

bool rx_node_id::is_guid() const
{
	return data_.node_type == rx_node_id_uuid;
}

bool rx_node_id::get_uuid(rx_uuid_t& id) const
{
	if (data_.node_type == rx_node_id_uuid)
	{
		id = data_.value.uuid_value;
		return true;
	}
	else
		return false;
}

bool rx_node_id::get_numeric(uint32_t& id) const
{
	if (data_.node_type == rx_node_id_numeric)
	{
		id = data_.value.int_value;
		return true;
	}
	else
		return false;
}

bool rx_node_id::get_string(string_type& id) const
{
	if (data_.node_type == rx_node_id_string)
	{
		const char* ptr = rx_c_str(&data_.value.string_value);
		if (ptr)
			id = ptr;
		else
			id.clear();
		return true;
	}
	else
		return false;
}
bool rx_node_id::get_bytes(byte_string& id) const
{
	if (data_.node_type == rx_node_id_bytes)
	{
		size_t len = 0;
		const std::byte* ptr = (const std::byte*)rx_c_ptr(&data_.value.bstring_value, &len);
		if (ptr && len)
		{
			id.assign(ptr, ptr + len);
		}
		else
		{
			id.clear();
		}
		return true;
	}
	else
		return false;
}

const rx_uuid_t& rx_node_id::get_uuid() const
{
	if (data_.node_type == rx_node_id_uuid)
		return data_.value.uuid_value;
	else
		throw std::invalid_argument("Wrong node id type");
}
uint32_t rx_node_id::get_numeric() const
{
	if (data_.node_type == rx_node_id_numeric)
		return data_.value.int_value;
	else
		throw std::invalid_argument("Wrong node id type");
}
const uint16_t rx_node_id::get_namespace() const
{
	return data_.namespace_index;
}

void rx_node_id::set_namespace(uint16_t value)
{
	data_.namespace_index = value;
}

const rx_node_id_type rx_node_id::get_node_type() const
{
	return data_.node_type;
}


#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif

const char* g_complie_time;
const char* g_lib_version;
const char* g_abi_version;
const char* g_common_version;

namespace
{
	class dummy_starter
	{
	public:

		char compile_buffer[0x100];
		char version_buffer[0x100];
		char abi_version_buffer[0x100];
		char common_version_buffer[0x100];
		dummy_starter()
		{
			static_assert(sizeof(rx_uuid_t) == 16);
			create_module_compile_time_string(__DATE__, __TIME__, compile_buffer);
			g_complie_time = compile_buffer;
			create_module_version_string("", RX_LIB_MAJOR_VERSION, RX_LIB_MINOR_VERSION, RX_LIB_BUILD_NUMBER,__DATE__, __TIME__, version_buffer);
			g_lib_version = version_buffer;
			create_module_version_string("", RX_ABI_MAJOR_VERSION, RX_ABI_MINOR_VERSION, RX_ABI_BUILD_NUMBER, __DATE__, __TIME__, abi_version_buffer);
			g_abi_version = abi_version_buffer;
			create_module_version_string("", RX_COMMON_MAJOR_VERSION, RX_COMMON_MINOR_VERSION, RX_COMMON_BUILD_NUMBER, __DATE__, __TIME__, common_version_buffer);
			g_common_version = common_version_buffer;
		}
	};

	dummy_starter g_obj;
}


string_type& to_upper(string_type& str)
{
	for (auto & c : str) c = toupper(c);
	return str;
}

void rx_split_string(const string_type& what, string_vector& result, char delimeter)
{
	size_t idx1 = 0;
	size_t idx2 = what.find(delimeter);
	while (idx2 != string_type::npos)
	{
		result.push_back(what.substr(idx1, idx2 - idx1));
		idx1 = idx2 + 1;
		idx2 = what.find(delimeter, idx1);
	}
	result.push_back(what.substr(idx1));
}
void extract_next(const string_type& path, string_type& name, string_type& rest, char delimeter)
{
	size_t idx = path.find(delimeter);
	if (idx != string_type::npos)
	{
		name = path.substr(0, idx);
		rest = path.substr(idx + 1);
	}
	else
	{
		name = path;
		rest = "";
	}
}
string_type replace_in_string(const string_type& str, const string_type find, const string_type replace)
{
	string_type result;
	size_t find_len = find.size();
	size_t str_size = str.size();
	result.reserve(str.size());
	size_t idx = 0;
	size_t old_idx;
	do
	{
		old_idx = idx;
		idx = str.find(find, idx);
		if (idx == string_type::npos)
		{
			result += str.substr(old_idx);
		}
		else
		{
			if(idx>old_idx)
				result += str.substr(old_idx, idx - old_idx);
			result += replace;
			idx += find_len;
		}
	} while (idx != string_type::npos && idx < str_size);

	return result;
}

string_type& trim_left_in_place(string_type& str) {
	size_t i = 0;
	while (i < str.size() && isspace(str[i])) { ++i; };
	return str.erase(0, i);
}

string_type& trim_right_in_place(string_type& str) {
	size_t i = str.size();
	while (i > 0 && isspace(str[i - 1])) { --i; };
	return str.erase(i, str.size());
}

string_type& rx_trim_in_place(string_type& str) {
	return trim_left_in_place(trim_right_in_place(str));
}

string_type rx_trim(const string_type str) {
	string_type temp(str);
	return trim_left_in_place(trim_right_in_place(temp));
}


string_type get_code_module(const string_type& full)
{
	static size_t len = strlen(SOURCE_CODE_PATH);

	if (full.size() > len)
	{
		size_t idx = full.find(SOURCE_CODE_PATH);
		if (idx != string_type::npos)
		{
			return full.substr(len + idx);
		}
	}
	size_t idx = full.find_last_of("/\\");
	if (idx == string_type::npos)
		return full;
	else
		return full.substr(idx + 1);
}

rx_mode_type::rx_mode_type()
{
	raw_format = 0;
}

bool rx_mode_type::can_callculate(uint32_t quality) const
{
	return (raw_format & RX_MODE_MASK_OFF) == 0;
}
bool rx_mode_type::can_callculate(const values::rx_value& value) const
{
	return (raw_format & RX_MODE_MASK_OFF) == 0;
}
bool rx_mode_type::is_on() const
{
	return (raw_format&RX_MODE_MASK_OFF) == 0;
}
bool rx_mode_type::is_test() const
{
	return is_on() && (raw_format&RX_MODE_MASK_TEST) != 0;
}
bool rx_mode_type::is_blocked() const
{
	return is_on() && (raw_format&RX_MODE_MASK_BLOCKED) != 0;
}
bool rx_mode_type::is_simulate() const
{
	return is_on() && (raw_format&RX_MODE_MASK_SIMULATE) != 0;
}
bool rx_mode_type::is_unassigned() const
{
	return (raw_format&RX_MODE_MASK_UNASSIGNED) != 0;
}
bool rx_mode_type::is_off() const
{
	return (raw_format&RX_MODE_MASK_OFF) != 0;
}
bool rx_mode_type::set_test()
{
	if (!is_off())
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format | (RX_MODE_MASK_TEST);
		return (old_stuff != raw_format);
	}
	return false;
}

bool rx_mode_type::reset_test()
{
	if (!is_off())
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format & (!RX_MODE_MASK_TEST);
		return (old_stuff != raw_format);
	}
	return false;
}
bool rx_mode_type::set_simulate()
{
	if (!is_off())
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format | (RX_MODE_MASK_SIMULATE);
		return (old_stuff != raw_format);
	}
	return false;
}
bool rx_mode_type::ret_simulate()
{
	if (!is_off())
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format & (~RX_MODE_MASK_SIMULATE);
		return (old_stuff != raw_format);
	}
	return false;
}
bool rx_mode_type::set_unassigned()
{
	uint32_t old_stuff = raw_format;
	raw_format = raw_format | (RX_MODE_MASK_UNASSIGNED);
	return (old_stuff != raw_format);
}

bool rx_mode_type::reset_unassigned()
{
	uint32_t old_stuff = raw_format;
	raw_format = raw_format & (~RX_MODE_MASK_UNASSIGNED);
	return (old_stuff != raw_format);
}

bool rx_mode_type::is_good() const
{
	return (raw_format&(RX_MODE_MASK_OFF | RX_MODE_MASK_UNASSIGNED)) != 0;
}

bool rx_mode_type::set_blocked()
{
	if (!is_off())
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format | (RX_MODE_MASK_BLOCKED);
		return (old_stuff != raw_format);
	}
	return false;
}
bool rx_mode_type::reset_blocked()
{
	if (!is_off())
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format & (~RX_MODE_MASK_BLOCKED);
		return (old_stuff != raw_format);
	}
	return false;
}
bool rx_mode_type::turn_on()
{
	uint32_t old_stuff = raw_format;
	raw_format = 0;
	return (old_stuff != raw_format);
}
bool rx_mode_type::turn_off()
{
	uint32_t old_stuff = raw_format;
	raw_format = RX_MODE_MASK_OFF;
	return (old_stuff != raw_format);
}

namespace
{
std::atomic<int64_t> g_current_offset(0);
std::atomic<uint32_t> g_current_time_quality(DEFAULT_TIME_QUALITY);
}
rx_time::~rx_time()
{
	static_assert(sizeof(rx_time) == sizeof(rx_time_struct), "Memory size has to be the same, no virtual functions or members");
}
rx_time::rx_time() noexcept
{
	data_.t_value = 0;
}

rx_time::rx_time(const rx_time& right)
{
	data_.t_value = right.data_.t_value;
}
rx_time::rx_time(rx_time&& right) noexcept
{
	data_.t_value = right.data_.t_value;
}
rx_time& rx_time::operator=(const rx_time& right)
{
	data_.t_value = right.data_.t_value;
	return *this;
}
rx_time& rx_time::operator=(rx_time&& right) noexcept
{
	data_.t_value = right.data_.t_value;
	return *this;
}
rx_time::rx_time(rx_time_struct tm) noexcept
{
	data_.t_value = tm.t_value;
}

rx_time::rx_time(const timeval& tv)
{
	if (tv.tv_sec == 0 && tv.tv_usec == 0)
		data_.t_value = 0;
	else
	{
		uint64_t temp = ((uint64_t)tv.tv_usec) * 10 + ((uint64_t)tv.tv_sec) * 10000000;
		temp += server_time_struct_DIFF_TIMEVAL;
		data_.t_value = temp;
	}
}
void rx_time::to_timeval(timeval& tv) const
{
	if (is_null())
	{
		tv.tv_sec = 0;
		tv.tv_usec = 0;
	}
	else
	{
		uint64_t temp = data_.t_value;
		temp -= server_time_struct_DIFF_TIMEVAL;
		temp /= 10;
		tv.tv_sec = (long)(temp / 1000000UL);
		tv.tv_usec = (long)(temp % 1000000UL);
	}
}
rx_time::rx_time(const asn_generalized_time& bt)
{
	rx_full_time full;
	memzero(&full, sizeof(full));
	full.year = bt.year;
	full.month = bt.month;
	full.day = bt.day;
	full.hour = bt.hour;
	full.minute = bt.minute;
	full.second = bt.second;
	full.milliseconds = (bt.fraction / 1000);

	rx_os_collect_time(&full, &data_);
}
void rx_time::to_asn_generalized_time(asn_generalized_time& bt) const
{
	rx_full_time full;
	rx_os_split_time(&data_, &full);

	bt.year = full.year;
	bt.month = full.month;
	bt.day = full.day;
	bt.hour = full.hour;
	bt.minute = full.minute;
	bt.second = full.second;
	bt.fraction = full.milliseconds * 1000;// to us
}
rx_time::rx_time(const asn_binary_time& bt)
{
	rx_time now;
	if (bt.days != 0 && bt.mstime != 0)
	{
		if (bt.full)
		{// 6 bytes
			now = rx_time(MS_START_1984);
			now = now + bt.mstime;
			int64_t tempdays = ((int64_t)bt.days) * 1000 * 60 * 60 * 24;
			now = now + tempdays;
		}
		else
		{// 4 bytes
			now = rx_time::now();
			int64_t temp = now.get_longlong_miliseconds() % MS_IN_DAY;
			now = now - temp;
			now = now + bt.mstime;
		}
	}
	memcpy(this, &now, sizeof(rx_time_struct));
}
void rx_time::to_asn_binary_time(asn_binary_time& bt) const
{
	if (is_null())
	{
		bt.days = 0;
		bt.full = true;
		bt.mstime = 0;
	}
	else
	{
		int64_t mine = get_longlong_miliseconds();
		mine = mine - MS_START_1984;
		int64_t mine_ms = mine%MS_IN_DAY;
		int64_t mine_days = mine / MS_IN_DAY;
		bt.full = true;
		bt.days = (uint16_t)mine_days;
		bt.mstime = (uint32_t)mine_ms;
	}
}
rx_time::rx_time(const uint64_t interval)
{
	data_.t_value = interval*(uint64_t)10000;
}
rx_time& rx_time::operator=(const rx_time_struct& right) noexcept
{
	memcpy(&data_, &right, sizeof(rx_time_struct));
	return *this;
}
rx_time& rx_time::operator=(const uint64_t interval) noexcept
{
	data_.t_value = interval*(uint64_t)10000;
	return *this;
}
bool  rx_time::is_valid_time(const rx_time_struct& arg)
{
	return arg.t_value>0x014f373b00000000ul;
}
bool  rx_time::is_valid_time() const
{
	return data_.t_value>0x014f373b00000000ul;
}
rx_time rx_time::null_time()
{
	return rx_time(0);
}
rx_time rx_time::now()
{
	rx_time ret;
	rx_os_get_system_time(&ret.data_);
	int64_t offset = g_current_offset;
	if (offset)
	{
		ret.data_.t_value = ret.data_.t_value + offset;
	}
	return ret;
}
uint32_t rx_time::current_time_quality()
{
	return g_current_time_quality;
}
void rx_time::set_current_time_offset(int64_t offset)
{
	int64_t temp = offset * 10ll;
	g_current_offset.fetch_add(temp);
}
void rx_time::set_synchronized(bool value)
{
	uint32_t temp = value ? SYNCHRONIZED_TIME_QUALITY : DEFAULT_TIME_QUALITY;
	g_current_time_quality.store(temp);
}
rx_time rx_time::operator+(const rx_time& right) const
{
	rx_time res;
	res.data_.t_value = data_.t_value + right.data_.t_value;

	return res;
}
rx_time rx_time::operator+(const uint64_t right) const
{

	rx_time res;
	res.data_.t_value = data_.t_value + right * 10000;

	return res;


}
rx_time rx_time::operator-(const rx_time& right) const
{
	rx_time res;
	res.data_.t_value = data_.t_value - right.data_.t_value;

	return res;


}
rx_time rx_time::operator-(const uint64_t right) const
{
	rx_time res;
	res.data_.t_value = data_.t_value - right * 10000;

	return res;

}

bool rx_time::operator==(const rx_time& right) const
{
	return data_.t_value == right.data_.t_value;
}
bool rx_time::operator!=(const rx_time& right) const
{
	return data_.t_value != right.data_.t_value;
}
bool rx_time::operator>(const rx_time& right) const
{
	return data_.t_value > right.data_.t_value;
}
bool rx_time::operator>=(const rx_time& right) const
{
	return data_.t_value >= right.data_.t_value;
}
bool rx_time::operator<(const rx_time& right) const
{
	return data_.t_value < right.data_.t_value;
}


bool rx_time::operator<=(const rx_time& right) const
{
	return data_.t_value <= right.data_.t_value;
}

rx_time& rx_time::to_local()
{
	rx_os_to_local_time(&data_);
	return *this;
}
rx_time& rx_time::to_UTC()
{
	rx_os_to_utc_time(&data_);
	return *this;
}

/*
rx_time& rx_time::to_local_full()
{
FILETIME mine, local;
SYSTEMTIME sys,loc;

mine.dwLowDateTime = (DWORD)(t_value & 0xFFFFFFFF);
mine.dwHighDateTime = (DWORD)(t_value >> 32);
if(FileTimeToSystemTime(&mine,&sys))
{
SystemTimeToTzSpecificLocalTime(NULL,&sys,&loc);
SystemTimeToFileTime(&loc, &local);
t_value = (((uint64_t)local.dwHighDateTime) << 32) + local.dwLowDateTime;
}
return *this;
}
rx_time& rx_time::to_UTC_full()
{
FILETIME mine, utc;
SYSTEMTIME sys, loc;

mine.dwLowDateTime = (DWORD)(t_value & 0xFFFFFFFF);
mine.dwHighDateTime = (DWORD)(t_value >> 32);
if(FileTimeToSystemTime(&mine,&loc))
{
TzSpecificLocalTimeToSystemTime(NULL,&loc,&sys);
SystemTimeToFileTime(&sys, &utc);
t_value = (((uint64_t)utc.dwHighDateTime) << 32) + utc.dwLowDateTime;
}
return *this;
}
*/

uint32_t rx_time::get_miliseconds() const
{
	return (uint32_t)(data_.t_value / 10000);
}

bool rx_time::is_null() const
{
	return data_.t_value == 0;
}
rx_time_struct rx_time::c_data() const
{
	return data_;
}
int64_t rx_time::get_longlong_miliseconds() const
{
	return (data_.t_value / 10000);
}
int64_t rx_time::get_useconds() const
{
	return (data_.t_value / 10);
}

std::string rx_time::get_string(bool with_date) const
{
	if (is_null())
		return "<null>";
	rx_full_time full;
	char buff[0x200];

	rx_os_split_time(&data_, &full);

	std::string ret;
	if (with_date)
	{
		snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
			full.year, full.month, full.day, full.hour, full.minute, full.second, full.milliseconds);
	}
	else
	{
		snprintf(buff, sizeof(buff), "%02d:%02d:%02d.%03d",
			full.hour, full.minute, full.second, full.milliseconds);
	}

	ret = buff;

	return ret;
}

void rx_time::get_time_string(char* buff, size_t len) const
{
	uint64_t abs = data_.t_value / 10;

	uint64_t usec = abs % 1000;
	abs = abs / 1000;

	uint64_t msec = abs % 1000;
	abs = abs / 1000;

	uint64_t sec = abs % 60;
	abs = abs / 60;

	uint64_t min = abs % 60;
	abs = abs / 60;

	uint64_t hour = abs % 24;

	_snprintf_s(buff, len, len, "%02d:%02d:%02d.%03d %03d",
		(int)hour, (int)min, (int)sec, (int)msec, (int)usec);

}
rx_time rx_time::from_SNTP_time(uint32_t seconds, uint32_t fraction)
{
	uint64_t temp = ((uint64_t)seconds) * 10000000ull;
	temp = temp + (((uint64_t)fraction) * 10000000ull / 0x100000000ull);


	temp = temp + 0x014f373bfde04000ull;

	rx_time_struct ret;
	ret.t_value = temp;
	return ret;
}
void rx_time::to_SNTP_time(uint32_t& seconds, uint32_t& fraction)
{
	int64_t temp = data_.t_value;
	temp = temp - 0x014f373bfde04000ull;

	if (temp > 0)
	{
		seconds = (uint32_t)(temp / 10000000);
		temp = temp % 10000000;
		temp = temp * 0x100000000ull;
		temp = temp / 10000000;

		//fraction = (uint32_t)(temp%1000000);
		fraction = (uint32_t)temp;
	}
	else
	{
		seconds = 0;
		fraction = 0;
	}

}
rx_time rx_time::from_IEC_string(const char* str)
{
	const char* ptr = str;

	rx_full_time os_time;

	os_time.hour = 0;
	os_time.minute = 0;
	os_time.second = 0;
	os_time.milliseconds = 0;

	if (sscanf(ptr, "%4u-%2u-%2u", &os_time.year, &os_time.month, &os_time.day) == 3)
		ptr += 10;
	else if (sscanf(ptr, "%4u%2u%2u", &os_time.year, &os_time.month, &os_time.day) == 3)
		ptr += 8;
	else
		return rx_time();

	if (*ptr != L'\0')
	{
		if (*ptr != L'T')
			return rx_time();
		ptr++;

		if (sscanf(ptr, "%2u:%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 5;
		else if (sscanf(ptr, "%2u%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 4;
		else
			return rx_time();

		if (*ptr == L':')
			ptr++;

		if (*ptr != L'\0')
		{// seconds too
			if (sscanf(ptr, "%2u", &os_time.second) == 1)
				ptr += 2;
			else
				return rx_time();
			if (*ptr == L'.')
			{
				ptr++;
				if (sscanf(ptr, "%3u", &os_time.milliseconds) == 1)
					ptr += 3;
			}
		}
	}

	rx_time_struct ts;
	if (rx_os_collect_time(&os_time, &ts))
	{
		return ts;
	}
	else
		return rx_time();
}
string_type rx_time::get_IEC_string() const
{
	char buff[0x200];

	rx_time_struct temp = data_;
	string_type ret;

	rx_full_time os_time;
	if (rx_os_split_time(&temp, &os_time))
	{
		snprintf(buff, sizeof(buff) / sizeof(buff[0]), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
			os_time.year, os_time.month, os_time.day,
			os_time.hour, os_time.minute, os_time.second, os_time.milliseconds);
		ret += buff;
	}

	return ret;

}

void rx_time::set_as_span(uint32_t days)
{

	uint64_t temp = ((uint64_t)days) * 10000 * 1000 * 3600 * 24;

	data_.t_value = temp;

}

uint32_t rx_time::get_as_span() const
{
	uint64_t temp = data_.t_value;

	return ((uint32_t)(temp / ((uint64_t)10000 * (uint64_t)1000 * (uint64_t)3600 * (uint64_t)24)));

}
rx_time_stamp rx_time_stamp::now()
{
	rx_time_stamp ret;
	ret.time = rx_time::now().c_data();
	ret.quality = rx_time::current_time_quality();
	return ret;
}




void rx_dump_ticks_to_stream(std::ostream& out,rx_timer_ticks_t ticks)
{
	out << std::fixed << std::setprecision(3) << ((double)ticks / 1000.0) << "ms";
}


}//namespace rx


