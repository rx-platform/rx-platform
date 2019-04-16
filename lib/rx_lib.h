

/****************************************************************************
*
*  lib\rx_lib.h
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


#ifndef rx_lib_h
#define rx_lib_h 1




////////////////////////////////////////////////////////////////////////////////////////////
// main server library file
////////////////////////////////////////////////////////////////////////////////////////////
namespace rx
{


struct rx_table_cell_struct
{
	rx_table_cell_struct(const string_type& vvalue)
		: value(vvalue)
	{
	}
	rx_table_cell_struct(const string_type& vvalue, const string_type& vprefix, const string_type& vpostfix)
		: prefix(vprefix), value(vvalue), postfix(vpostfix)
	{
	}
	rx_table_cell_struct(const rx_table_cell_struct& right) = default;
	rx_table_cell_struct(rx_table_cell_struct&& right) noexcept = default;
	string_type prefix;
	string_type value;
	string_type postfix;
};

typedef std::vector<rx_table_cell_struct> rx_row_type;
typedef std::vector<rx_row_type> rx_table_type;

void rx_dump_large_row(rx_row_type row, std::ostream& out, size_t console_width);
void rx_dump_table(const rx_table_type& table, std::ostream& out, bool column_names,bool dot_lines);


class rx_result
{
	typedef std::vector<string_type> result_erros_t;
	std::unique_ptr<result_erros_t> result_value_;
public:
	rx_result(bool value);
	rx_result(const string_vector& errors);
	rx_result(string_vector&& errors);
	rx_result(const char* error);
	rx_result(const string_type& error);
	rx_result(string_type&& error);
	void register_error(string_type&& error);
	void register_errors(result_erros_t&& errors);
	void register_errors(const result_erros_t& errors);
	operator bool() const;
	const result_erros_t& errors()const;
	rx_result(const rx_result& right) = delete;// because of the unique_ptr!

	rx_result() = default;
	~rx_result() = default;
	rx_result(rx_result&&) noexcept = default;
	rx_result& operator=(const rx_result&) = delete;
	rx_result& operator=(rx_result&&) noexcept = default;
};

void rx_dump_error_result(std::ostream& err, const rx_result& result);

template<class T>
class rx_result_with
{
	T value_;
	typedef std::vector<string_type> result_erros_t;
	std::unique_ptr<result_erros_t> errors_;
public:
	rx_result_with(const T& value)
		: value_(value)
	{
		if (!value_)
			errors_ = std::make_unique<result_erros_t>(string_vector{ "Undefined error!"s });
	}
	rx_result_with(T&& value)
		: value_(std::move(value))
	{
		if (!value)
			errors_ = std::make_unique<result_erros_t>(string_vector{ "Undefined error!"s });
	}
	rx_result_with(const string_vector& errors)
		: errors_(std::make_unique<result_erros_t>(errors))
	{
	}
	rx_result_with(string_vector&& errors)
		: errors_(std::make_unique<result_erros_t>(std::move(errors)))
	{
	}
	rx_result_with(const char* error)
		: errors_(std::make_unique<result_erros_t>(string_vector{ error }))
	{
	}
	rx_result_with(const string_type& error)
		: errors_(std::make_unique<result_erros_t>(string_vector{ error }))
	{
	}
	rx_result_with(string_type&& error)
		: errors_(std::make_unique<result_erros_t>(string_vector{ std::move(error) }))
	{
	}
	void register_error(string_type&& error)
	{
		if (!errors_)
			errors_ = std::make_unique<result_erros_t>(string_vector{ std::move(error) });
		else
			errors_->emplace_back(std::move(error));
	}
	void register_errors(const result_erros_t& errors)
	{
		if (!errors_)
			errors_ = std::make_unique<result_erros_t>(errors);
		else
		{
			for (const auto& one : errors)
				errors_->emplace_back(one);
		}
	}
	void register_errors(result_erros_t&& errors)
	{
		if (!errors_)
			errors_ = std::make_unique<result_erros_t>(std::move(errors));
		else
		{
			for (auto& one : errors)
				errors_->emplace_back(std::move(one));
		}
	}
	operator T() const
	{
		return value_;
	}
	const T& value() const
	{
		return value_;
	}
	T& value()
	{
		return value_;
	}
	operator bool() const
	{
		return static_cast<bool>(value_);
	}
	const result_erros_t& errors()const
	{
		return *errors_;
	}
	rx_result_with(const rx_result_with& right) = delete;// because of the unique_ptr!

	rx_result_with() = default;
	~rx_result_with() = default;
	rx_result_with(rx_result_with&&) noexcept = default;
	rx_result_with& operator=(const rx_result_with&) = delete;
	rx_result_with& operator=(rx_result_with&&) noexcept = default;
};

rx_result rx_list_files(const std::string& dir, const std::string& pattern, std::vector<std::string>& files, std::vector<std::string>& directories);
std::string rx_combine_paths(const std::string& path1, const std::string& path2);
std::string rx_get_extension(const std::string& path);
std::string rx_remove_extension(const std::string& path);

rx_result create_directory(const std::string& dir, bool fail_on_exsists);
rx_result rx_delete_all_files(const std::string& dir, const std::string& pattern);

rx_result file_exist(const std::string& path, const std::string& file);
rx_result file_exist(const std::string& file);
rx_result rx_get_full_path(const std::string& base, std::string& path);


extern const char* g_complie_time;
extern const char* g_lib_version;


string_type& to_upper(string_type& str);

string_type get_code_module(const string_type& full);

///////////////////////////////////////////////////////////////
// PHYSICAL INTERFACES
///////////////////////////////////////////////////////////////
enum interface_status_type
{
	status_disconnected = 0,
	status_active
};
#define MAC_ADDR_SIZE 6
struct ETH_interface
{
	uint8_t mac_address[MAC_ADDR_SIZE];
	string_type name;
	size_t index;
	interface_status_type status;
	void init()
	{
		index = 0;
		status = status_disconnected;
		name = "<unnanmed>";
	}
};



struct IP_interface
{
	string_type ip_address;
	string_type name;
	size_t index;
	interface_status_type status;
	void init()
	{
		index = 0;
		status = status_disconnected;
		name = "<unnanmed>";
	}
};

///////////////////////////////////////////////////////////////
// TIMES FOR ASN1 MMS IEC TIME VALUES
///////////////////////////////////////////////////////////////

struct asn_generalized_time
{
	uint16_t year;
	uint16_t month;
	uint16_t day;

	uint16_t hour;
	uint16_t minute;
	uint16_t second;

	uint32_t fraction;
};


struct asn_binary_time
{
	bool full;
	uint32_t mstime;
	uint16_t days;
};

class rx_uuid
{
private:
	rx_uuid_t m_uuid;
public:
	rx_uuid();
	rx_uuid(const rx_uuid& rigth);
	rx_uuid(const rx_uuid_t& right);
	rx_uuid& operator=(const rx_uuid& right);

	static rx_uuid create_new();
	static rx_uuid create_from_string(const string_type& str);

	static const rx_uuid& null_uuid();

	const rx_uuid_t& uuid()
	{
		return m_uuid;
	}

	bool operator==(const rx_uuid& right) const;
	bool operator!=(const rx_uuid& right) const;
	bool operator>(const rx_uuid& right) const;
	bool operator>=(const rx_uuid& right) const;
	bool operator<(const rx_uuid& right) const;
	bool operator<=(const rx_uuid& right) const;

	bool is_null() const;

	void to_string(string_type& buff) const;
	string_type to_string() const;

};

typedef rx_uuid rx_id;


union rx_node_id_union
{
	uint32_t int_value;
	string_type* string_value;
	byte_string* bstring_value;
	rx_uuid_t uuid_value;
};



enum rx_node_id_type
{
	numeric_rx_node_id = 0,
	string_rx_node_id = 1,
	uuid_rx_node_id = 2,
	bytes_rx_node_id = 3
};


class rx_node_id
{
	friend struct std::hash<rx::rx_node_id>;
public:
	rx_node_id();
	rx_node_id(const rx_node_id &right);
	rx_node_id(uint32_t id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(const char* id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(rx_uuid_t id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(const byte_string& id, uint16_t namesp = DEFAULT_NAMESPACE);
	rx_node_id(rx_node_id&& right)  noexcept;
	~rx_node_id();

	rx_node_id & operator=(const rx_node_id &right);
	rx_node_id & operator=(rx_node_id &&right) noexcept;

	bool operator==(const rx_node_id &right) const;
	bool operator!=(const rx_node_id &right) const;
	bool operator < (const rx_node_id& right) const;

	void to_string(string_type& val) const;
	string_type to_string() const;
	static rx_node_id from_string(const char* value);

	static rx_node_id generate_new(uint16_t namesp = RX_USER_NAMESPACE);
	bool is_null() const;
	operator bool() const;
	bool is_standard() const;
	bool is_opc() const;
	void set_string_id(const char* strid);
	bool is_guid() const;

	bool get_uuid(rx_uuid_t& id) const;
	bool get_numeric(uint32_t& id) const;
	bool get_string(string_type& id) const;
	bool get_bytes(byte_string& id) const;
	
	const rx_uuid_t& get_uuid() const;
	uint32_t get_numeric() const;
	const string_type& get_string() const;
	const byte_string& get_bytes() const;


	const uint16_t get_namespace() const;
	void set_namespace(uint16_t value);

	const rx_node_id_type get_node_type() const;
	void set_node_type(rx_node_id_type value);

	static const rx_node_id null_id;
private:
	bool is_simple() const;
	void clear_content();
	uint16_t namespace_;
	rx_node_id_type node_type_;
	rx_node_id_union value_;

};

std::ostream & operator << (std::ostream &out, const rx_node_id &val);

typedef std::vector<rx_node_id> rx_node_ids;
///////////////////////////////////////////////////////////////
//

#define RX_MODE_MASK_TEST		0x01
#define RX_MODE_MASK_BLOCKED	0x02
#define RX_MODE_MASK_SIMULATE	0x04
#define RX_MODE_MASK_OFF		0x08
#define RX_MODE_MASK_UNASSIGNED	0x10


struct rx_mode_type
{
	rx_mode_type()
	{
		raw_format = 0;
	}
	uint32_t raw_format;
	bool is_on() const
	{
		return (raw_format&RX_MODE_MASK_OFF) == 0;
	}
	bool is_test() const
	{
		return is_on() && (raw_format&RX_MODE_MASK_TEST) != 0;
	}
	bool is_blocked() const
	{
		return is_on() && (raw_format&RX_MODE_MASK_BLOCKED) != 0;
	}
	bool is_simulate() const
	{
		return is_on() && (raw_format&RX_MODE_MASK_SIMULATE) != 0;
	}
	bool is_unassigned() const
	{
		return (raw_format&RX_MODE_MASK_UNASSIGNED) != 0;
	}
	bool is_off() const
	{
		return (raw_format&RX_MODE_MASK_OFF) != 0;
	}
	bool set_test()
	{
		if (!is_off())
		{
			uint32_t old_stuff = raw_format;
			raw_format = raw_format | (RX_MODE_MASK_TEST);
			return (old_stuff != raw_format);
		}
		return false;
	}

	bool reset_test()
	{
		if (!is_off())
		{
			uint32_t old_stuff = raw_format;
			raw_format = raw_format & (!RX_MODE_MASK_TEST);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool set_simulate()
	{
		if (!is_off())
		{
			uint32_t old_stuff = raw_format;
			raw_format = raw_format | (RX_MODE_MASK_SIMULATE);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool ret_simulate()
	{
		if (!is_off())
		{
			uint32_t old_stuff = raw_format;
			raw_format = raw_format & (~RX_MODE_MASK_SIMULATE);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool set_unassigned()
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format | (RX_MODE_MASK_UNASSIGNED);
		return (old_stuff != raw_format);
	}

	bool reset_unassigned()
	{
		uint32_t old_stuff = raw_format;
		raw_format = raw_format & (~RX_MODE_MASK_UNASSIGNED);
		return (old_stuff != raw_format);
	}

	bool is_good() const
	{
		return (raw_format&(RX_MODE_MASK_OFF | RX_MODE_MASK_UNASSIGNED)) != 0;
	}

	bool set_blocked()
	{
		if (!is_off())
		{
			uint32_t old_stuff = raw_format;
			raw_format = raw_format | (RX_MODE_MASK_BLOCKED);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool reset_blocked()
	{
		if (!is_off())
		{
			uint32_t old_stuff = raw_format;
			raw_format = raw_format & (~RX_MODE_MASK_BLOCKED);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool turn_on()
	{
		uint32_t old_stuff = raw_format;
		raw_format = 0;
		return (old_stuff != raw_format);
	}
	bool turn_off()
	{
		uint32_t old_stuff = raw_format;
		raw_format = RX_MODE_MASK_OFF;
		return (old_stuff != raw_format);
	}
};

///////////////////////////////////////////////////////////////

class rx_time : public rx_time_struct
{
public:
	rx_time();
	rx_time(const timeval& tv);
	rx_time(const asn_binary_time& bt);
	rx_time(const asn_generalized_time& bt);
	rx_time(const rx_time_struct& ft);
	rx_time(const uint64_t interval);
	rx_time& operator=(const rx_time_struct& right);
	rx_time& operator=(const uint64_t interval);


	void to_asn_generalized_time(asn_generalized_time& tv) const;
	void to_asn_binary_time(asn_binary_time& tv) const;
	void to_timeval(timeval& tv) const;
	std::string get_string(bool with_date = true) const;
	void get_time_string(char* buff, size_t len) const;
	std::string get_IEC_string() const;
	static rx_time_struct from_IEC_string(const char* str);
	static rx_time_struct from_SNTP_time(uint32_t seconds, uint32_t fraction);
	void to_SNTP_time(uint32_t& seconds, uint32_t& fraction);

	static uint32_t current_time_quality();
	static void set_current_time_offset(int64_t offset);
	static void set_synchronized(bool value);
	static rx_time now();
	static rx_time null_time();
	static bool is_valid_time(const rx_time_struct& arg);
	bool is_valid_time() const;

	rx_time operator+(const rx_time_struct& right) const;
	rx_time operator+(const uint64_t right) const;
	rx_time operator-(const rx_time_struct& right) const;
	rx_time operator-(const uint64_t right) const;

	bool operator==(const rx_time_struct& right) const;
	bool operator!=(const rx_time_struct& right) const;
	bool operator>(const rx_time_struct& right) const;
	bool operator>=(const rx_time_struct& right) const;
	bool operator<(const rx_time_struct& right) const;
	bool operator<=(const rx_time_struct& right) const;

	void swap_bytes() const;

	void set_as_span(uint32_t days);
	uint32_t get_as_span() const;

	rx_time& to_local();
	rx_time& to_UTC();

	/*
	rx_time& to_local_full();
	rx_time& to_UTC_full();
	*/

	uint32_t get_miliseconds() const;
	int64_t get_longlong_miliseconds() const;
	int64_t get_useconds() const;

	bool is_null() const;
};


struct time_stamp
{
	rx_time_struct rx_time;
	uint32_t quality;
	static time_stamp now();
};

void rx_dump_ticks_to_stream(std::ostream& out, rx_timer_ticks_t ticks);


// security related basics for stuff
typedef intptr_t rx_security_handle_t;
typedef intptr_t rx_thread_handle_t;
enum rx_criticalness
{
	soft,
	medium,
	hard
};
enum rx_access
{
	read	= 0x01,
	write	= 0x02,
	full	= 0xff
};


rx_thread_handle_t rx_thread_context();
bool rx_push_thread_context(rx_thread_handle_t obj);

#define RX_THREAD_NULL 0ull


void extract_next(const string_type& path, string_type& name, string_type& rest, char delimeter);



class rx_source_file
{
	sys_handle_t m_handle;
public:
	rx_source_file()
		: m_handle(0)
	{
	}
	rx_result open(const char* file_name)
	{
		m_handle = rx_file(file_name, RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		return m_handle != 0;
	}
	rx_result open_write(const char* file_name)
	{
		m_handle = rx_file(file_name, RX_FILE_OPEN_WRITE, RX_FILE_CREATE_ALWAYS);
		return m_handle != 0;
	}
	rx_result read_string(std::string& buff)
	{
		if (m_handle == 0)
		{
			RX_ASSERT(false);
			return "File not opened!";
		}
		uint64_t size;
		if (rx_file_get_size(m_handle, &size) != RX_OK)
			return "Unable to get file size!";

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
			return "Error reading file!";
		}
	}
	rx_result write_string(const std::string& buff)
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
			return "Error writing to file!";
		}
	}
	~rx_source_file()
	{
		if (m_handle != 0)
			rx_file_close(m_handle);
	}

};

}// namespace rx


namespace std
{
template<>
struct hash<rx::rx_node_id>
{
	size_t operator()(const rx::rx_node_id& id) const noexcept
	{
		switch (id.node_type_)
		{
		case rx::numeric_rx_node_id:
			return (hash<int32_t>()(id.value_.int_value)
				^ (hash<uint16_t>()(id.namespace_) << 1));
		case rx::string_rx_node_id:
			return (hash<string_type>()(*id.value_.string_value)
				^ (hash<uint16_t>()(id.namespace_) << 1));
		case rx::uuid_rx_node_id:
			return (hash<uint64_t>()(*((int64_t*)(&id.value_.uuid_value))))
				^ (hash<uint64_t>()(((int64_t*)(&id.value_.uuid_value))[1] << 1))
				^ (hash<uint16_t>()(id.namespace_) << 2);
		case rx::bytes_rx_node_id:
		{
			size_t ret = 0;
			size_t count = id.value_.bstring_value->size();
			for (size_t i = 0; i < count; i++)
			{
				ret ^= ((hash<uint8_t>()((*(id.value_.bstring_value))[i])) << i & 0xffff);
			}
			return ret;
		}
		default:
			RX_ASSERT(false);
			return 0;
		}
	}
};
}


#endif
