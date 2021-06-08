

/****************************************************************************
*
*  lib\rx_lib.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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
#include "lib/rx_ser_lib.h"
#include "lib/rx_values.h"
#include "lib/rx_job.h"


#define MS_IN_DAY (1000*60*60*24)
#define MS_START_1984 0x00000afe0bd9e400L

#define server_time_struct_DIFF_TIMEVAL  116444736000000000ull


#define DEFAULT_TIME_QUALITY (TIME_QUALITY_NOT_SYNCHRONIZED|TIME_QUALITY_CLASS_T1)
#define SYNCHRONIZED_TIME_QUALITY (TIME_QUALITY_CLASS_T1)
string_type _not_implemented_func(const char* fname)
{
    return string_type(fname) + " not implemented";
}

namespace rx
{


rx_source_file::rx_source_file()
	: m_handle(0)
{
}
rx_result rx_source_file::open(const char* file_name)
{
	m_handle = rx_file(file_name, RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	if (m_handle != 0)
		return true;
	else
		return rx_result::create_from_last_os_error("Error opening file "s + file_name + " for read");
}
rx_result rx_source_file::open_write(const char* file_name)
{
	m_handle = rx_file(file_name, RX_FILE_OPEN_WRITE, RX_FILE_CREATE_ALWAYS);
	if (m_handle != 0)
		return true;
	else
		return rx_result::create_from_last_os_error("Error opening file "s + file_name + " for write");
}
rx_result rx_source_file::read_string(std::string& buff)
{
	if (m_handle == 0)
	{
		RX_ASSERT(false);
		return "File not opened!";
	}
	uint64_t size64;
	if (rx_file_get_size(m_handle, &size64) != RX_OK)
		return rx_result::create_from_last_os_error("Unable to get file size!");

	size_t size = (size_t)size64;
	char* temp = new char[size];

	uint32_t readed = 0;
	if (rx_file_read(m_handle, temp, (uint32_t)size, &readed) == RX_OK)
	{
		buff.assign(temp, size);
		delete[] temp;
		return true;
	}
	else
	{
		delete[] temp;
		return rx_result::create_from_last_os_error("Error reading file!");
	}
}
rx_result rx_source_file::write_string(const std::string& buff)
{
	if (m_handle == 0)
	{
		RX_ASSERT(false);
		return "File not opened!";
	}

	uint32_t size = (uint32_t)buff.size();
	uint32_t written = 0;
	if (rx_file_write(m_handle, buff.c_str(), size, &written) == RX_OK)
	{
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error writing to file!");
	}
}
rx_source_file::~rx_source_file()
{
	if (m_handle != 0)
		rx_file_close(m_handle);
}

void rx_dump_large_row(rx_row_type row, std::ostream& out, size_t console_width)
{
	if (row.empty())
		return;

	const size_t col_diff = 2;

	size_t count = row.size();

	std::vector<size_t> widths(count);
	for (size_t i = 0; i < count; i++)
	{
		widths[i] = row[i].value.size();
	}
	// first try to get how many columns do we need
	size_t columns = count + 1;
	std::vector<size_t> column_widths;
	size_t total_width = 1000000000ull;// i guess will be enough

	while (total_width>console_width)
	{
		columns--;
		column_widths.assign(columns, 0);
		size_t idx = 0;
		while (idx < count)
		{
			for (size_t i = 0; i < columns; i++)
			{
				size_t one_idx = idx + i;

				if (one_idx >= count)
					break;
				if (column_widths[i] < widths[one_idx])
				{
					column_widths[i] = widths[one_idx];
				}
			}
			idx += columns;
		}
		total_width = 0;
		for (size_t i = 0; i < columns; i++)
			total_width += (column_widths[i] + col_diff);
	}

	bool first = true;
	size_t idx = 0;
	while (idx < count)
	{
		if (first)
			first = false;
		else
			out << "\r\n";

		for (size_t i = 0; i < columns; i++)
		{
			size_t one_idx = idx + i;
			if (one_idx >= count)
				break;

			string_type rest(column_widths[i] + col_diff - row[one_idx].value.size(), ' ');

			if (!row[one_idx].prefix.empty())
				out << row[one_idx].prefix;
			out << row[one_idx].value;
			if (!row[one_idx].postfix.empty())
				out << row[one_idx].postfix;
			out << rest;
		}
		idx += columns;
	}

	out << "\r\n";
}

void rx_dump_table(const rx_table_type& table, std::ostream& out, bool column_names, bool dot_lines)
{
	if (table.empty())
		return;

	const size_t col_diff = 2;

	size_t columns_number = 0;
	for (const auto& row : table)
	{
		if (columns_number == 0)
			columns_number = row.size();
		else
		{
			if (!row.empty())
			{// we allow empty rows
				if (columns_number != row.size())
				{
					out << "Error in table format\r\n";
					RX_ASSERT(false);
					return;
				}
			}
		}
	}

	// o.k. we checked now so let's calculate columns width
	std::vector<size_t> widths(columns_number);
	for (const auto& row : table)
	{
		if (!row.empty())
		{
			for (size_t i = 0; i < columns_number; i++)
			{
				if (row[i].value.size() > widths[i])
					widths[i] = row[i].value.size();
			}
		}
	}
	char empty_char = dot_lines ? '.' : ' ';
	// now we have all widths
	bool first = true;
	for (const auto& row : table)
	{
		if(!first)
			out << "\r\n";
		if (!row.empty())
		{
			for (size_t i = 0; i < columns_number; i++)
			{
				string_type rest(widths[i] + col_diff - row[i].value.size(),
					i==columns_number-1 || first || row[i].value.empty()
					? ' ' : empty_char);
				if (!row[i].prefix.empty())
					out << row[i].prefix;
				out << row[i].value;
				if (!row[i].postfix.empty())
					out << row[i].postfix;
				out << rest;
			}
		}
		if (first)
			first = false;
		if (column_names)
		{
			out << "\r\n";
			size_t total_width = 0;
			for (size_t i = 0; i < columns_number; i++)
				total_width += (widths[i] + col_diff);
			string_type rest(total_width, '=');
			out << rest;
			column_names = false;
		}
	}

	out << "\r\n";
}

rx_result create_directory(const std::string& dir, bool fail_on_exsists)
{
	return rx_create_directory(dir.c_str(), fail_on_exsists ? 1 : 0) != 0;
}
rx_result rx_delete_all_files(const std::string& dir, const std::string& pattern)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	auto result = rx_list_files(dir, pattern, files,dirs);
	if (result)
	{
		for (auto& one : files)
		{
			string_type temp_path = rx_combine_paths(dir, one);
			if (!rx_file_delete(temp_path.c_str()))
				return rx_result::create_from_last_os_error("Error deleting file.");
		}
	}
	return result;
}

rx_result rx_list_files(const std::string& dir, const std::string& pattern, std::vector<std::string>& files, std::vector<std::string>& directories)
{
	std::string search = rx_combine_paths(dir, pattern);
	rx_file_directory_entry_t one;

	find_file_handle_t hndl = rx_open_find_file_list(search.c_str(), &one);
	if (hndl)
	{
		do
		{
			if (strcmp(one.file_name, ".") != 0 && strcmp(one.file_name, "..") != 0)
			{
				if (one.is_directory)
					directories.emplace_back(one.file_name);
				else
					files.emplace_back(one.file_name);
			}

		} while (rx_get_next_file(hndl, &one));
		rx_find_file_close(hndl);
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error listing directory "s + search);
	}
}
std::string rx_get_extension(const std::string& path)
{
	auto idx = path.find_last_of(".\\/");
	if (idx == string_type::npos || path[idx] != '.')
		return string_type();
	else
		return path.substr(idx + 1);
}
std::string rx_remove_extension(const std::string& path)
{
	auto idx = path.find_last_of(".\\/");
	if (idx == string_type::npos || path[idx] != '.')
		return string_type(path);
	else
		return path.substr(0, idx);
}
std::string rx_combine_paths(const std::string& path1, const std::string& path2)
{
	std::string path;
    path = path1;
	if (!path1.empty())
	{
		if (path1.at(path1.size() - 1) != '\\' && path1.at(path1.size() - 1) != '/')
		{
			path += "/";
		}
	}
	if (!path2.empty())
	{
		if (path2.at(0) == '\\' || path2.at(0) == '/')
			path += path2.substr(1);
		else
			path += path2;
	}
	return path;
}
rx_result file_exist(const std::string& file)
{
	return rx_file_exsist(file.c_str())!=0;
}

rx_result::rx_result(bool value)
{
	if (!value)
		result_value_ = std::make_unique<rx_result_erros_t>(string_vector{ "Undefined error!"s });
}
rx_result::rx_result(const string_vector& errors)
	: result_value_(std::make_unique<rx_result_erros_t>(errors))
{
}
rx_result::rx_result(string_vector&& errors)
	: result_value_ (std::make_unique<rx_result_erros_t>(std::move(errors)))
{
}
rx_result::rx_result(const char* error)
	: result_value_(std::make_unique<rx_result_erros_t>(string_vector{ error }))
{
}
rx_result::rx_result(const string_type& error)
	: result_value_(std::make_unique<rx_result_erros_t>(string_vector{ error }))
{
}
rx_result::rx_result(string_type&& error)
	: result_value_(std::make_unique<rx_result_erros_t>(string_vector{ std::move(error) }))
{
}
void rx_result::register_error(string_type&& error)
{
	if (!result_value_)
		result_value_ = std::make_unique<rx_result_erros_t>(string_vector{ std::move(error) });
	else
		result_value_->emplace_back(std::move(error));
}

void rx_result::register_errors(const rx_result_erros_t& errors)
{
	if (!result_value_)
		result_value_ = std::make_unique<rx_result_erros_t>(errors);
	else
	{
		for (const auto& one : errors)
			result_value_->emplace_back(one);
	}
}
void rx_result::register_errors(rx_result_erros_t&& errors)
{
	if (!result_value_)
		result_value_ = std::make_unique<rx_result_erros_t>(std::move(errors));
	else
	{
		for (auto& one : errors)
			result_value_->emplace_back(std::move(one));
	}
}
const rx_result_erros_t& rx_result::errors() const
{
	return *result_value_;
}
rx_result::operator bool() const
{
	return !result_value_;
}

string_type rx_result::errors_line(char delim) const
{
	if (*this)
		return "";
	else if (!result_value_ || result_value_->empty())
		return "No specific errors!";
	std::ostringstream ss;
	for (const auto& one : *result_value_)
	{
		if (!ss.eof())// using this as a bool!
			ss << delim;
		ss << one;
	}
	return ss.str();
}
rx_result rx_result::create_from_last_os_error(const string_type& text)
{
	char buffer[0x100];

	rx_last_os_error(text.empty() ? nullptr : text.c_str(), buffer, sizeof(buffer));

	return rx_result(buffer);
}
rx_result rx_result::create_from_c_error(const string_type& text)
{
	return rx_result(text);
}


const rx_item_reference rx_item_reference::null_ref;


// placement new and MSVC debug heap problem
#ifdef _MSC_VER 
#ifdef _DEBUG
#undef new
#endif
#endif

rx_item_reference::rx_item_reference()
{
	is_id_ = true;
	new(&id_) rx_node_id();
}

rx_item_reference::rx_item_reference(const rx_item_reference& right)
{
	if (right.is_id_)
	{
		new(&id_) rx_node_id(right.id_);
	}
	else
	{
		new(&path_) string_type(right.path_);
	}
	is_id_ = right.is_id_;
}

rx_item_reference::rx_item_reference(const rx_node_id& right)
{
	is_id_ = true;
	new(&id_) rx_node_id(right);
}

rx_item_reference::rx_item_reference(const string_type& right)
{
	is_id_ = false;
	new(&path_) string_type(right);
}

rx_item_reference::rx_item_reference(const char* right)
{
	is_id_ = false;
	new(&path_) string_type(right);
}

rx_item_reference::rx_item_reference(const rx_simple_value& right)
{
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(right.get_storage().get_id_value());
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(right.get_storage().get_string_value());
	}
}



rx_item_reference::~rx_item_reference()
{
	clear_content();
}


rx_item_reference& rx_item_reference::operator=(const rx_item_reference& right)
{
	clear_content();
	if (right.is_id_)
	{
		new(&id_) rx_node_id(right.id_);
	}
	else
	{
		new(&path_) string_type(right.path_);
	}
	is_id_ = right.is_id_;
	return *this;
}


bool rx_item_reference::is_null() const
{
	if (is_id_)
	{
		return id_.is_null();
	}
	else
	{
		return path_.empty();
	}
}

void rx_item_reference::clear_content()
{
	if (is_id_)
	{
		id_.~rx_node_id();
	}
	else
	{
		path_.~string_type();
	}
}

rx_item_reference& rx_item_reference::operator = (const rx_node_id& right)
{
	clear_content();
	is_id_ = true;
	new(&id_) rx_node_id(right);
	return *this;
}

rx_item_reference& rx_item_reference::operator = (const string_type& right)
{
	clear_content();
	is_id_ = false;
	new(&path_) string_type(right);
	return *this;
}

bool rx_item_reference::is_node_id() const
{
	return is_id_;
}

string_type rx_item_reference::to_string() const
{
	return is_id_ ? id_.to_string() : path_;
}

const string_type& rx_item_reference::get_path() const
{
	if (is_id_)
		throw std::invalid_argument("Target is referenced by id!");
	else
		return path_;
}

const rx_node_id& rx_item_reference::get_node_id() const
{
	if (!is_id_)
		throw std::invalid_argument("Target is referenced by path!");
	else
		return id_;
}

rx_simple_value rx_item_reference::to_value() const
{
	values::rx_simple_value temp;
	if (is_id_)
	{
		temp.assign_static<rx_node_id>(rx_node_id(id_));
	}
	else
	{
		temp.assign_static<string_type>(string_type(path_));
	}
	return temp;
}

rx_item_reference& rx_item_reference::operator = (const rx_simple_value& right)
{
	clear_content();
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(right.get_storage().get_id_value());
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(right.get_storage().get_string_value());
	}
	return *this;
}


rx_item_reference::rx_item_reference(rx_item_reference&& right) noexcept
{
	if (right.is_id_)
	{
		new(&id_) rx_node_id(std::move(right.id_));
	}
	else
	{
		new(&path_) string_type(std::move(right.path_));
	}
	is_id_ = right.is_id_;

}
rx_item_reference& rx_item_reference::operator=(rx_item_reference&& right) noexcept
{
	clear_content();
	if (right.is_id_)
	{
		new(&id_) rx_node_id(std::move(right.id_));
	}
	else
	{
		new(&path_) string_type(std::move(right.path_));
	}
	is_id_ = right.is_id_;
	return *this;
}

rx_item_reference::rx_item_reference(rx_node_id&& right) noexcept
{
	is_id_ = true;
	new(&id_) rx_node_id(std::move(right));
}
rx_item_reference::rx_item_reference(string_type&& right) noexcept
{
	is_id_ = false;
	new(&path_) string_type(std::move(right));
}

rx_item_reference::rx_item_reference(rx_simple_value&& right) noexcept
{
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(std::move(right.get_storage().get_id_value()));
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(std::move(right.get_storage().get_string_value()));
	}
}

rx_item_reference& rx_item_reference::operator= (rx_node_id&& right) noexcept
{
	clear_content();
	is_id_ = true;
	new(&id_) rx_node_id(std::move(right));
	return *this;

}
rx_item_reference& rx_item_reference::operator= (string_type&& right) noexcept
{
	clear_content();
	is_id_ = false;
	new(&path_) string_type(std::move(right));
	return *this;
}
rx_item_reference& rx_item_reference::operator= (rx_simple_value&& right) noexcept
{
	clear_content();
	if (right.get_type() == RX_NODE_ID_TYPE)
	{
		is_id_ = true;
		new(&id_) rx_node_id(std::move(right.get_storage().get_id_value()));
	}
	else
	{
		is_id_ = false;
		new(&path_) string_type(std::move(right.get_storage().get_string_value()));
	}
	return *this;
}

rx_uuid::rx_uuid()
{
	memzero(&m_uuid, sizeof(rx_uuid_t));
}

rx_uuid::rx_uuid(const rx_uuid& rigth)
{
	memcpy(&m_uuid, &rigth.m_uuid, sizeof(rx_uuid_t));
}

rx_uuid::rx_uuid(const rx_uuid_t& rigth)
{
	memcpy(&m_uuid, &rigth, sizeof(rx_uuid_t));
}
rx_uuid& rx_uuid::operator=(const rx_uuid& right)
{
	memcpy(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t));
	return *this;
}
bool rx_uuid::operator==(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t)) == 0);
}
bool rx_uuid::operator!=(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right, sizeof(rx_uuid_t)) != 0);
}
bool rx_uuid::operator>(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t))>0);
}
bool rx_uuid::operator>=(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t)) >= 0);

}
bool rx_uuid::operator<(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t))<0);
}

bool rx_uuid::operator<=(const rx_uuid& right) const
{
	return (memcmp(&m_uuid, &right.m_uuid, sizeof(rx_uuid_t)) <= 0);
}
// {98862E03-F664-48C5-A6AC-24761B5D479F}
rx_uuid g_null_uuid;


rx_uuid rx_uuid::create_new()
{
	rx_uuid ret;
	rx_uuid_t temp;
	rx_generate_new_uuid(&temp);
	memcpy(&ret.m_uuid, &temp, sizeof(rx_uuid_t));
	return ret;
}
const rx_uuid& rx_uuid::null_uuid()
{
	return g_null_uuid;
}
rx_uuid rx_uuid::create_from_string(const string_type& str)
{

	rx_uuid_t temp;
	if (RX_OK == rx_string_to_uuid(str.c_str(), &temp))
		return temp;
	else
		return g_null_uuid;
}
bool rx_uuid::is_null() const
{
	return memcmp(this, &g_null_uuid, sizeof(rx_uuid_t)) == 0;
}

void rx_uuid::to_string(string_type& str) const
{
	char buff[0x100];
	if (RX_OK == rx_uuid_to_string(&m_uuid, buff))
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


const rx_node_id rx_node_id::null_id;


std::ostream & operator << (std::ostream &out, const rx_node_id &val)
{
	out << val.to_string();
	return out;
}

rx_node_id::rx_node_id() noexcept
{
	namespace_ = 0;
	value_.int_value = 0;
	node_type_ = rx_node_id_type::numeric;
}

rx_node_id::rx_node_id(const rx_node_id &right)
{
	memcpy(this, &right, sizeof(right));
	if (!right.is_simple())
	{
		switch (node_type_)
		{
		case rx_node_id_type::string:
			value_.string_value = new string_type(*right.value_.string_value);
			break;
		case rx_node_id_type::bytes:
			value_.bstring_value = new byte_string(*right.value_.bstring_value);
			break;
		default:;
		}
	}
}

rx_node_id::rx_node_id(uint32_t id, uint16_t namesp)
	: namespace_(0),
	node_type_(rx_node_id_type::numeric)
{
	value_.int_value = id;
	if (id)
		namespace_ = namesp;
	else
		namespace_ = 0;
}

rx_node_id::rx_node_id(const char* id, uint16_t namesp)
{
	value_.string_value = new string_type(id);
	node_type_ = rx_node_id_type::string;
	namespace_ = namesp;
}
/*
rx_node_id::rx_node_id(const rx_uuid_t& id, uint16_t namespace)
{
	value_.uuid_value = id;
	node_type_ = rx_node_id_type::uuid;
	namespace_ = namespace;
}
*/
rx_node_id::rx_node_id(rx_uuid_t id, uint16_t namesp)
{
	value_.uuid_value = id;
	node_type_ = rx_node_id_type::uuid;
	namespace_ = namesp;
}


rx_node_id::rx_node_id(const byte_string& id, uint16_t namesp)
{
	value_.bstring_value = new byte_string(id);
	node_type_ = rx_node_id_type::bytes;
	namespace_ = namesp;
}


rx_node_id::rx_node_id(rx_node_id&& right) noexcept
{
	memcpy(this, &right, sizeof(right));
	if (!right.is_simple())
	{
		switch (node_type_)
		{
		case rx_node_id_type::string:
			*value_.string_value = std::move(*right.value_.string_value);
			break;
		case rx_node_id_type::bytes:
			*value_.bstring_value = std::move(*right.value_.bstring_value);
			break;
		default:;
		}
		// just dummy because
		right.node_type_ = rx_node_id_type::numeric;
	}
}


rx_node_id::~rx_node_id()
{
	clear_content();
}


rx_node_id & rx_node_id::operator=(const rx_node_id &right)
{
	clear_content();
	memcpy(this, &right, sizeof(right));
	if (!right.is_simple())
	{
		switch (node_type_)
		{
		case rx_node_id_type::string:
			value_.string_value = new string_type(*right.value_.string_value);
			break;
		case rx_node_id_type::bytes:
			value_.bstring_value = new byte_string(*right.value_.bstring_value);
			break;
		default:;
		}
	}
	return *this;
}

rx_node_id & rx_node_id::operator=(rx_node_id &&right) noexcept
{
	clear_content();
	memcpy(this, &right, sizeof(right));
	if (!right.is_simple())
	{
		switch (node_type_)
		{
		case rx_node_id_type::string:
			*value_.string_value = std::move(*right.value_.string_value);
			break;
		case rx_node_id_type::bytes:
			*value_.bstring_value = std::move(*right.value_.bstring_value);
			break;
		default:;
		}
		right.node_type_ = rx_node_id_type::numeric;
	}
	return *this;
}

bool rx_node_id::operator==(const rx_node_id &right) const
{
	if (namespace_ != right.namespace_)
		return false;
	if (node_type_ != right.node_type_)
		return false;
	switch (node_type_)
	{
	case rx_node_id_type::numeric:
		return value_.int_value == right.value_.int_value;
	case rx_node_id_type::uuid:
		return memcmp(&value_.uuid_value, &right.value_.uuid_value, sizeof(value_.uuid_value)) == 0;
	case rx_node_id_type::string:
		return (*(value_.string_value)) == (*(right.value_.string_value));
	case rx_node_id_type::bytes:
		return (*(value_.bstring_value)) == (*(right.value_.bstring_value));
	default:
		RX_ASSERT(false);
		return false;
	}
}

bool rx_node_id::operator!=(const rx_node_id &right) const
{
	return !operator==(right);
}



bool rx_node_id::operator < (const rx_node_id& right) const
{


	if (namespace_<right.namespace_)
		return true;

	if (namespace_>right.namespace_)
		return false;

	if (node_type_<right.node_type_)
		return true;

	if (node_type_>right.node_type_)
		return false;

	switch (node_type_)
	{
	case rx_node_id_type::numeric:
		return value_.int_value<right.value_.int_value;
	case rx_node_id_type::uuid:
		return memcmp(&value_.uuid_value, &right.value_.uuid_value, sizeof(value_.uuid_value))<0;
	case rx_node_id_type::string:
		return (*(value_.string_value))<(*(right.value_.string_value));
	case rx_node_id_type::bytes:
		return (*(value_.bstring_value))<(*(right.value_.bstring_value));
	default:
		RX_ASSERT(false);
		return false;
	}
}

void rx_node_id::to_string(string_type& val) const
{
	if (is_null())
	{
		val.clear();
		return;
	}

	char buff[0x40];
	const char* type = "err";
	switch (node_type_)
	{
	case rx_node_id_type::numeric:
		type = "i";
		break;
	case rx_node_id_type::string:
		type = "s";
		break;
	case rx_node_id_type::uuid:
		type = "g";
		break;
	case rx_node_id_type::bytes:
		type = "b";
		break;
	}

	string_type value;

	switch (node_type_)
	{
	case rx_node_id_type::numeric:
		snprintf(buff, 0x40, "%d", value_.int_value);
		value = buff;
		break;
	case rx_node_id_type::string:
		value = *value_.string_value;
		break;
	case rx_node_id_type::uuid:
		rx_uuid(value_.uuid_value).to_string(value);
		break;
	case rx_node_id_type::bytes:
		{
			for (size_t i = 0; i<value_.bstring_value->size(); i++)
			{
				snprintf(buff, 0x40, "%02X", (int)value_.bstring_value->at(i));
				value += buff;
			}
		}
		break;
	}
	val.clear();
	if (namespace_ != DEFAULT_NAMESPACE)
	{
		char buffer[0x10];
		snprintf(buffer, 0x10, "%d:", (int)namespace_);
		val = buffer;
	}
	val += type;
	val += ':';
	val += value;
}
string_type rx_node_id::to_string() const
{
	string_type temp;
	to_string(temp);
	return temp;
}
rx_node_id rx_node_id::generate_new(uint16_t namesp)
{
	return rx_node_id(rx_uuid::create_new().uuid(), namesp);
}
rx_node_id rx_node_id::from_string(const char* value)
{
	rx_node_id ret;
	if (value == nullptr || *value == '\0')
	{
		return rx_node_id();
	}
	string_type strid(value);
	size_t idx = strid.find(':');
	if (idx != string_type::npos)
	{
		string_type type;
		size_t idx2 = strid.find(':', idx + 1);
		if (idx2 != string_type::npos)
		{
			ret.namespace_ = atoi(strid.substr(0, idx).c_str());
			type = strid.substr(idx + 1, idx2 - idx - 1);
		}
		else
		{
			ret.namespace_ = DEFAULT_NAMESPACE;
			type = strid.substr(0, idx);
			idx2 = idx;
		}
		if (type == "i")
		{
			ret.node_type_ = rx_node_id_type::numeric;
			ret.value_.int_value = atoi(strid.substr(idx2 + 1).c_str());
			if (ret.value_.int_value == 0)
				ret.namespace_ = 0;
		}
		else if (type == "s")
		{
			ret.node_type_ = rx_node_id_type::string;
			ret.value_.string_value = new string_type(strid.substr(idx2 + 1).c_str());
		}
		else if (type == "g")
		{
			ret.node_type_ = rx_node_id_type::uuid;
			rx_string_to_uuid(strid.substr(idx2 + 1).c_str(), &ret.value_.uuid_value);
		}
		else if (type == "b")
		{
		}
	}
	return ret;
}

bool rx_node_id::is_null() const
{
	return namespace_ == 0 && node_type_ == rx_node_id_type::numeric && value_.int_value == 0;
}
rx_node_id::operator bool() const
{
	return !is_null();
}
bool rx_node_id::is_standard() const
{
	return namespace_ == 1 && node_type_ == rx_node_id_type::numeric;
}
bool rx_node_id::is_opc() const
{
	return namespace_ == 0 && node_type_ == rx_node_id_type::numeric;
}

bool rx_node_id::is_simple() const
{
	return node_type_ == rx_node_id_type::numeric || node_type_ == rx_node_id_type::uuid;
}

void rx_node_id::clear_content()
{
	if (!is_simple())
	{
		switch (node_type_)
		{
		case rx_node_id_type::string:
			delete value_.string_value;
			break;
		case rx_node_id_type::bytes:
			delete value_.bstring_value;
			break;
		default:;
		}
	}
}

void rx_node_id::set_string_id(const char* strid)
{
	clear_content();
	node_type_ = rx_node_id_type::string;
	value_.string_value = new string_type(strid);
}

bool rx_node_id::is_guid() const
{
	return node_type_ == rx_node_id_type::uuid;
}

bool rx_node_id::get_uuid(rx_uuid_t& id) const
{
	if (node_type_ == rx_node_id_type::uuid)
	{
		id = value_.uuid_value;
		return true;
	}
	else
		return false;
}

bool rx_node_id::get_numeric(uint32_t& id) const
{
	if (node_type_ == rx_node_id_type::numeric)
	{
		id = value_.int_value;
		return true;
	}
	else
		return false;
}

bool rx_node_id::get_string(string_type& id) const
{
	if (node_type_ == rx_node_id_type::string)
	{
		if (value_.string_value)
			id = *value_.string_value;
		return true;
	}
	else
		return false;
}
bool rx_node_id::get_bytes(byte_string& id) const
{
	if (node_type_ == rx_node_id_type::bytes)
	{
		if (value_.bstring_value)
			id = *value_.bstring_value;
		return true;
	}
	else
		return false;
}

const rx_uuid_t& rx_node_id::get_uuid() const
{
	if (node_type_ == rx_node_id_type::uuid)
		return value_.uuid_value;
	else
		throw std::invalid_argument("Wrong node id type");
}
uint32_t rx_node_id::get_numeric() const
{
	if (node_type_ == rx_node_id_type::numeric)
		return value_.int_value;
	else
		throw std::invalid_argument("Wrong node id type");
}
const string_type& rx_node_id::get_string() const
{
	if (node_type_ == rx_node_id_type::string)
		return *value_.string_value;
	else
		throw std::invalid_argument("Wrong node id type");
}
const byte_string& rx_node_id::get_bytes() const
{
	if (node_type_ == rx_node_id_type::bytes)
		return *value_.bstring_value;
	else
		throw std::invalid_argument("Wrong node id type");
}

const uint16_t rx_node_id::get_namespace() const
{
	return namespace_;
}

void rx_node_id::set_namespace(uint16_t value)
{
	namespace_ = value;
}

const rx_node_id_type rx_node_id::get_node_type() const
{
	return node_type_;
}

void rx_node_id::set_node_type(rx_node_id_type value)
{
	clear_content();
	node_type_ = value;
}

#ifdef _MSC_VER 
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif

const char* g_complie_time;
const char* g_lib_version;

namespace
{
	class dummy_starter
	{
	public:

		char compile_buffer[0x100];
		char version_buffer[0x100];
		dummy_starter()
		{
			static_assert(sizeof(rx_uuid_t) == 16);
			create_module_compile_time_string(__DATE__, __TIME__, compile_buffer);
			g_complie_time = compile_buffer;
			create_module_version_string("", RX_LIB_MAJOR_VERSION, RX_LIB_MINOR_VERSION, RX_LIB_BUILD_NUMBER,__DATE__, __TIME__, version_buffer);
			g_lib_version = version_buffer;
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
rx_time g_null_time = { 0 };
std::atomic<int64_t> g_current_offset(0);
std::atomic<uint32_t> g_current_time_quality(DEFAULT_TIME_QUALITY);
}
rx_time::rx_time() noexcept
{
	t_value = 0;
}

rx_time::rx_time(const timeval& tv)
{
	if (tv.tv_sec == 0 && tv.tv_usec == 0)
		t_value = 0;
	else
	{
		uint64_t temp = ((uint64_t)tv.tv_usec) * 10 + ((uint64_t)tv.tv_sec) * 10000000;
		temp += server_time_struct_DIFF_TIMEVAL;
		t_value = temp;
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
		uint64_t temp = t_value;
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

	rx_os_collect_time(&full, this);
}
void rx_time::to_asn_generalized_time(asn_generalized_time& bt) const
{
	rx_full_time full;
	rx_os_split_time(this, &full);

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
rx_time::rx_time(const rx_time_struct& ft)
{
	memcpy(this, &ft, sizeof(rx_time_struct));
}
rx_time::rx_time(const uint64_t interval)
{
	t_value = interval*(uint64_t)10000;
}
rx_time& rx_time::operator=(const rx_time_struct& right) noexcept
{
	memcpy(this, &right, sizeof(rx_time_struct));
	return *this;
}
rx_time& rx_time::operator=(const uint64_t interval) noexcept
{
	t_value = interval*(uint64_t)10000;
	return *this;
}
bool  rx_time::is_valid_time(const rx_time_struct& arg)
{
	return arg.t_value>0x014f373b00000000ul;
}
bool  rx_time::is_valid_time() const
{
	return t_value>0x014f373b00000000ul;
}
rx_time rx_time::null_time()
{
	return g_null_time;
}
rx_time rx_time::now()
{
	rx_time_struct ret;
	rx_os_get_system_time(&ret);
	int64_t offset = g_current_offset;
	if (offset)
	{
		ret.t_value = ret.t_value + offset;
	}
	rx_time rxret(ret);
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
rx_time rx_time::operator+(const rx_time_struct& right) const
{
	rx_time res;
	res.t_value = t_value + right.t_value;

	return res;
}
rx_time rx_time::operator+(const uint64_t right) const
{

	rx_time res;
	res.t_value = t_value + right * 10000;

	return res;


}
rx_time rx_time::operator-(const rx_time_struct& right) const
{
	rx_time res;
	res.t_value = t_value - right.t_value;

	return res;


}
rx_time rx_time::operator-(const uint64_t right) const
{
	rx_time res;
	res.t_value = t_value - right * 10000;

	return res;

}

bool rx_time::operator==(const rx_time_struct& right) const
{
	return t_value == right.t_value;
}
bool rx_time::operator!=(const rx_time_struct& right) const
{
	return t_value != right.t_value;
}
bool rx_time::operator>(const rx_time_struct& right) const
{
	return t_value > right.t_value;
}
bool rx_time::operator>=(const rx_time_struct& right) const
{
	return t_value >= right.t_value;
}
bool rx_time::operator<(const rx_time_struct& right) const
{
	return t_value < right.t_value;
}


bool rx_time::operator<=(const rx_time_struct& right) const
{
	return t_value <= right.t_value;
}

rx_time& rx_time::to_local()
{
	rx_os_to_local_time(this);
	return *this;
}
rx_time& rx_time::to_UTC()
{
	rx_os_to_utc_time(this);
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
	return (uint32_t)(t_value / 10000);
}

bool rx_time::is_null() const
{
	return t_value == 0;
}
int64_t rx_time::get_longlong_miliseconds() const
{
	return (t_value / 10000);
}
int64_t rx_time::get_useconds() const
{
	return (t_value / 10);
}

std::string rx_time::get_string(bool with_date) const
{
	if (is_null())
		return "<null>";
	rx_full_time full;
	char buff[0x200];

	rx_os_split_time(this, &full);

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
	uint64_t abs = t_value / 10;

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
rx_time_struct rx_time::from_SNTP_time(uint32_t seconds, uint32_t fraction)
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
	int64_t temp = t_value;
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
rx_time_struct rx_time::from_IEC_string(const char* str)
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
		return g_null_time;

	if (*ptr != L'\0')
	{
		if (*ptr != L'T')
			return g_null_time;
		ptr++;

		if (sscanf(ptr, "%2u:%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 5;
		else if (sscanf(ptr, "%2u%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 4;
		else
			return g_null_time;

		if (*ptr == L':')
			ptr++;

		if (*ptr != L'\0')
		{// seconds too
			if (sscanf(ptr, "%2u", &os_time.second) == 1)
				ptr += 2;
			else
				return g_null_time;
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
		return g_null_time;
}
string_type rx_time::get_IEC_string() const
{
	char buff[0x200];

	rx_time_struct temp = *this;
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

	t_value = temp;

}


void rx_time::swap_bytes() const
{

	memory::rx_byte_swap<uint64_t>(t_value);

}
uint32_t rx_time::get_as_span() const
{
	uint64_t temp = t_value;

	return ((uint32_t)(temp / ((uint64_t)10000 * (uint64_t)1000 * (uint64_t)3600 * (uint64_t)24)));

}
time_stamp time_stamp::now()
{
	time_stamp ret;
	ret.rx_time = rx_time::now();
	ret.quality = rx_time::current_time_quality();
	return ret;
}




void rx_dump_ticks_to_stream(std::ostream& out,rx_timer_ticks_t ticks)
{
	out << std::fixed << std::setprecision(3) << ((double)ticks / 1000.0) << "ms";
}


}//namespace rx


