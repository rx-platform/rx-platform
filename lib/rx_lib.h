

/****************************************************************************
*
*  lib\rx_lib.h
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_lib_h
#define rx_lib_h 1




////////////////////////////////////////////////////////////////////////////////////////////
// main server library file
////////////////////////////////////////////////////////////////////////////////////////////
namespace rx
{
void list_files(const std::string& dir, const std::string& pattern, std::vector<std::string>& files);
void combine_paths(const std::string& path1, const std::string& path2, std::string& path);

bool create_directory(const std::string& dir, bool fail_on_exsists);
bool delete_all_files(const std::string& dir, const std::string& pattern);

bool file_exist(const std::string& path, const std::string& file);
bool file_exist(const std::string& file);
void get_full_path(const std::string& base, std::string& path);

class svr_except : public std::exception
{
public:
	svr_except(const char* what, dword code) throw() : m_what(what), m_code(code) { }
	const char *what() const throw() { return m_what.c_str(); }
	dword code() throw() { return m_code; }

private:
	std::string m_what;
	dword m_code;

};

extern const char* g_complie_time;
extern const char* g_lib_version;


string_type& to_upper(string_type& str);

string_type get_code_module(const string_type& full);


///////////////////////////////////////////////////////////////
// TIMES FOR ASN1 MMS IEC TIME VALUES
///////////////////////////////////////////////////////////////

struct asn_generalized_time
{
	word year;
	word month;
	word day;

	word hour;
	word minute;
	word second;

	dword fraction;
};


struct asn_binary_time
{
	bool full;
	dword mstime;
	word days;
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

};

typedef rx_uuid rx_id;


union rx_node_id_union
{
	dword int_value;
	string_type* string_value;
	byte_string* bstring_value;
	rx_uuid_t uuid_value;
};



enum rx_node_id_type
{
	numeric_rx_node_id = 0,
	string_rx_node_id = 1,
	guid_rx_node_id = 2,
	bytes_rx_node_id = 3
};


class rx_node_id
{

public:
	rx_node_id();

	rx_node_id(const rx_node_id &right);

	rx_node_id(const dword& id, const word& namesp = DEFAULT_NAMESPACE);

	rx_node_id(const char* id, const word& namesp = DEFAULT_NAMESPACE);

	rx_node_id(const rx_uuid_t& id, const word& namesp = DEFAULT_NAMESPACE);

	rx_node_id(rx_node_id&& right);

	~rx_node_id();

	rx_node_id & operator=(const rx_node_id &right);

	bool operator==(const rx_node_id &right) const;

	bool operator!=(const rx_node_id &right) const;


	bool operator < (const rx_node_id& right) const;

	void to_string(string_type& val) const;

	static rx_node_id from_string(const char* value);

	bool is_null() const;

	bool is_standard() const;

	bool is_opc() const;

	void set_string_id(const char* strid);

	bool is_guid() const;

	bool get_uuid(rx_uuid_t& id) const;

	bool get_numeric(dword& id) const;

	bool get_string(string_type& id) const;

	const word get_namespace() const;
	void set_namespace(word value);

	const rx_node_id_type get_node_type() const;
	void set_node_type(rx_node_id_type value);

	static const rx_node_id null_id;
private:
	bool is_simple() const;
	void clear_content();
	word m_namespace;
	rx_node_id_type m_node_type;
	rx_node_id_union m_value;

};
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
	dword raw_format;
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
			dword old_stuff = raw_format;
			raw_format = raw_format | (RX_MODE_MASK_TEST);
			return (old_stuff != raw_format);
		}
		return false;
	}

	bool reset_test()
	{
		if (!is_off())
		{
			dword old_stuff = raw_format;
			raw_format = raw_format & (!RX_MODE_MASK_TEST);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool set_simulate()
	{
		if (!is_off())
		{
			dword old_stuff = raw_format;
			raw_format = raw_format | (RX_MODE_MASK_SIMULATE);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool ret_simulate()
	{
		if (!is_off())
		{
			dword old_stuff = raw_format;
			raw_format = raw_format & (~RX_MODE_MASK_SIMULATE);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool set_unassigned()
	{
		dword old_stuff = raw_format;
		raw_format = raw_format | (RX_MODE_MASK_UNASSIGNED);
		return (old_stuff != raw_format);
	}

	bool reset_unassigned()
	{
		dword old_stuff = raw_format;
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
			dword old_stuff = raw_format;
			raw_format = raw_format | (RX_MODE_MASK_BLOCKED);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool reset_blocked()
	{
		if (!is_off())
		{
			dword old_stuff = raw_format;
			raw_format = raw_format & (~RX_MODE_MASK_BLOCKED);
			return (old_stuff != raw_format);
		}
		return false;
	}
	bool turn_on()
	{
		dword old_stuff = raw_format;
		raw_format = 0;
		return (old_stuff != raw_format);
	}
	bool turn_off()
	{
		dword old_stuff = raw_format;
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
	rx_time(const qword interval);
	rx_time& operator=(const rx_time_struct& right);
	rx_time& operator=(const qword interval);


	void to_asn_generalized_time(asn_generalized_time& tv) const;
	void to_asn_binary_time(asn_binary_time& tv) const;
	void to_timeval(timeval& tv) const;
	std::string get_string() const;
	void get_time_string(char* buff, size_t len) const;
	std::string get_IEC_string() const;
	static rx_time_struct from_IEC_string(const char* str);
	static rx_time_struct from_SNTP_time(dword seconds, dword fraction);
	void to_SNTP_time(dword& seconds, dword& fraction);

	static dword current_time_quality();
	static void set_current_time_offset(sqword offset);
	static void set_synchronized(bool value);
	static rx_time now();
	static rx_time null_time();
	static bool is_valid_time(const rx_time_struct& arg);
	bool is_valid_time() const;

	rx_time operator+(const rx_time_struct& right) const;
	rx_time operator+(const qword right) const;
	rx_time operator-(const rx_time_struct& right) const;
	rx_time operator-(const qword right) const;

	bool operator==(const rx_time_struct& right) const;
	bool operator!=(const rx_time_struct& right) const;
	bool operator>(const rx_time_struct& right) const;
	bool operator>=(const rx_time_struct& right) const;
	bool operator<(const rx_time_struct& right) const;
	bool operator<=(const rx_time_struct& right) const;


	void set_as_span(dword days);
	dword get_as_span() const;

	rx_time& to_local();
	rx_time& to_UTC();

	/*
	rx_time& to_local_full();
	rx_time& to_UTC_full();
	*/

	dword get_miliseconds() const;
	sqword get_longlong_miliseconds() const;
	sqword get_useconds() const;

	bool is_null() const;
};


struct time_stamp
{
	rx_time_struct rx_time;
	dword quality;
	static time_stamp now();
};

// security related basics for stuff
typedef qword rx_security_handle_t;
typedef qword rx_thread_handle_t;

#define RX_THREAD_NULL 0ull



}// namespace rx




#endif
