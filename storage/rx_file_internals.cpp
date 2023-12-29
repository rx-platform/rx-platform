

/****************************************************************************
*
*  storage\rx_file_internals.cpp
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

#include "lib/rx_ser_json.h"
#include "lib/rx_ser_bin.h"

// rx_file_internals
#include "storage/rx_file_internals.h"



namespace storage {

namespace files {

// Class storage::files::rx_json_file 


base_meta_reader& rx_json_file::read_stream ()
{
	return *reader_;
}

base_meta_writer& rx_json_file::write_stream ()
{
	return *writer_;
}

rx_result rx_json_file::open_for_read (const string_type& data, const string_type& file_path)
{
	if (reader_)
		return "File storage "s + file_path + " already opened for reading";
	if (writer_)
		return "File storage "s + file_path + " already opened for writing";

	reader_ = std::make_unique<serialization::json_reader>();
	if (reader_->parse_data(data))
	{
		return true;
	}
	else
	{
		rx_result ret = reader_->get_errors();
		reader_.reset();
		ret.register_error("Error parsing Json file "s + file_path + "!");
		return ret;
	}
}

rx_result rx_json_file::open_for_write (const string_type& file_path)
{
	if (reader_)
		return "File storage "s + file_path + " already opened for reading";
	if (writer_)
		return "File storage "s + file_path + " already opened for writing";

	writer_ = std::make_unique<serialization::pretty_json_writer>();
	return true;
}

rx_result rx_json_file::close_read (const string_type& file_path)
{
	if (!reader_)
		return "File storage "s + file_path + " not opened for reading";
	else // (reader_)
		reader_.reset();
	return true;
}

rx_result rx_json_file::close_write (const string_type& file_path)
{
	if (!writer_)
		return "File storage "s + file_path + " not opened for writing";
	else // (writer_)
		writer_.reset();
	return true;
}

rx_result rx_json_file::get_data (string_type& data)
{
	data = writer_->get_string();
	if (!data.empty())
		return true;
	else
		return "Error creating Json!";
}


// Class storage::files::rx_binary_file 


base_meta_reader& rx_binary_file::read_stream ()
{
	return *reader_;
}

base_meta_writer& rx_binary_file::write_stream ()
{
	return *writer_;
}

rx_result rx_binary_file::open_for_read (const byte_string& data, const string_type& file_path)
{
	if (reader_)
		return "File storage "s + file_path + " already opened for reading";
	if (writer_)
		return "File storage "s + file_path + " already opened for writing";

	buffer_.reinit();
	if (data.empty())
		return "Empty data";
	size_t size = data.size();
	buffer_.reinit();
	buffer_.push_data(&data[1], size - 1);
	reader_ = std::make_unique<serialization::std_buffer_reader>(buffer_);

	return true;
}

rx_result rx_binary_file::open_for_write (const string_type& file_path)
{
	if (reader_)
		return "File storage "s + file_path + " already opened for reading";
	if (writer_)
		return "File storage "s + file_path + " already opened for writing";

	buffer_.reinit();
	uint8_t be = rx_big_endian != 0 ? 1 : 0;
	buffer_.push_data(be);
	writer_ = std::make_unique<serialization::std_buffer_writer>(buffer_);
	return true;
}

rx_result rx_binary_file::close_read (const string_type& file_path)
{
	if (!reader_)
		return "File storage "s + file_path + " not opened for reading";
	else // (reader_)
		reader_.reset();
	return true;
}

rx_result rx_binary_file::close_write (const string_type& file_path)
{
	if (!writer_)
		return "File storage "s + file_path + " not opened for writing";
	else // (writer_)
		writer_.reset();
	return true;
}

rx_result rx_binary_file::get_data (byte_string& data)
{
	data = writer_->get_data();
	return true;
}


// Class storage::files::rx_runtime_file 


rx_result rx_runtime_file::open (const string_type& file_name)
{
	return RX_NOT_IMPLEMENTED;
}


// Class storage::files::rx_simple_file 


rx_result rx_simple_file::open_for_read (const string_type& file_path)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result rx_simple_file::open_for_write (const string_type& file_path)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result rx_simple_file::read_string (string_type& buff)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result rx_simple_file::write_string (const string_type& buff)
{
	return RX_NOT_IMPLEMENTED;
}


} // namespace files
} // namespace storage

