

/****************************************************************************
*
*  protocols\opcua\rx_opcua_params.h
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


#ifndef rx_opcua_params_h
#define rx_opcua_params_h 1


#include "lib/rx_const_size_vector.h"




namespace protocols {
namespace opcua {

namespace opcua_addr_space
{
class reference_data;
}

namespace binary {

class ua_binary_istream;
class ua_binary_ostream;

}

namespace common {

enum class change_trigger_type
{
	status_trigger = 0,
	status_value_trigger = 1,
	status_value_time_trigger = 2
};

enum class node_class_type
{
	node = 0,
	object = 1,
	variable = 2,
	method = 4,
	object_type = 8,
	variable_type = 16,
	reference_type = 32,
	data_type = 64,
	view_type = 128
};
enum class browse_direction_type
{
	browse_direction_forward = 0,
	browse_direction_backward = 1,
	browse_direction_both = 2
};


enum class timestamps_return_type : uint32_t
{
	source = 0,
	server = 1,
	both = 2,
	neither = 3
};

enum class application_type_t
{
	server_application_type = 0,
	client_application_type = 1,
	client_server_application_type = 2,
	discrovery_application_type = 3
};


enum class security_mode_t : uint32_t
{
	invalid = 0,
	none = 1,
	sign = 2,
	sign_encr = 3
};

struct localized_text
{
	string_type text;
	string_type localeid;
};

enum class permission_type : uint32_t
{
	none				= 0x00000,
	browse				= 0x00001,
	read_role			= 0x00002,
	write_attribute		= 0x00004,
	write_role			= 0x00008,
	write_historizing	= 0x00010,
	read				= 0x00020,
	write				= 0x00040,
	read_history		= 0x00080,

	insert_history		= 0x00100,
	modify_history		= 0x00200,
	delete_history		= 0x00400,
	receive_events		= 0x00800,
	call				= 0x01000,
	add_reference		= 0x02000,
	remove_reference	= 0x04000,
	delete_node			= 0x08000,

	add_node			= 0x10000

};


enum class write_mask_type : uint32_t
{
	none				= 0x0000000,

	access_level		= 0x0000001,
	array_dimensions	= 0x0000002,
	browse_name			= 0x0000004,
	contains_no_loop	= 0x0000008,
	data_type			= 0x0000010,
	description			= 0x0000020,
	display_name		= 0x0000040,
	event_notifier		= 0x0000080,

	executable			= 0x0000100,
	historizing			= 0x0000200,
	inverse_name		= 0x0000400,
	is_abstract			= 0x0000800,
	minimum_sampling	= 0x0001000,
	node_class			= 0x0002000,
	node_id				= 0x0004000,
	symetric			= 0x0008000,

	user_access_level	= 0x0010000,
	user_executable		= 0x0020000,
	user_write_mask		= 0x0040000,
	value_rank			= 0x0080000,
	write_mask			= 0x0100000,
	value_for_variable	= 0x0200000,
	data_type_def		= 0x0400000,
	role_permissions	= 0x0800000,

	access_restrictions	= 0x1000000,
	access_level_ex		= 0x2000000

};


enum class attribute_id : uint32_t
{
	min_id					= 1,

	node_id					= 1,
	node_class				= 2,
	browse_name				= 3,
	display_name			= 4,
	description				= 5,
	write_mask				= 6,
	user_write_mask			= 7,

	is_abstract				= 8,
	symetric				= 9,
	inverse_name			= 10,
	contains_no_loop		= 11,
	event_notifier			= 12,
	value					= 13,
	data_type				= 14,
	value_rank				= 15,

	array_dimensions		= 16,
	access_level			= 17,
	user_access_level		= 18,
	minimum_sampling		= 19,
	historizing				= 20,
	executable				= 21,
	user_executable			= 22,
	data_type_def			= 23,

	role_permissions		= 24,
	user_role_permissions	= 25,
	access_restrictions		= 26,
	access_level_ex			= 27,

	max_id					= 27

};

enum class opc_access_level : uint8_t
{
	none				= 0x00,
	current_read		= 0x01,
	current_write		= 0x02,
	history_read		= 0x04,
	history_write		= 0x08,
	semantic_change		= 0x10,
	status_write		= 0x20,
	timestamp_write		= 0x40
};

enum class opc_access_level_ex : uint32_t
{
	none				= 0x000,
	current_read		= 0x001,
	current_write		= 0x002,
	history_read		= 0x004,
	history_write		= 0x008,
	semantic_change		= 0x010,
	status_write		= 0x020,
	timestamp_write		= 0x040,

	non_atomic_read		= 0x080,
	non_atomic_write	= 0x100,
	write_full_array	= 0x200
};



enum class access_restrictions_type : uint16_t
{
	none = 0,
	signing_required = 1,
	encryiption_required = 2,
	session_required = 4,
};

struct role_permission
{
	rx_node_id role_id;
	permission_type permissions;
};

typedef std::vector<role_permission> role_permissions_type;

struct qualified_name
{
	uint16_t namesp = 0;
	string_type name;
};

class ua_extension;
typedef std::unique_ptr<ua_extension> opcua_extension_ptr;

class ua_extension
{
	friend class binary::ua_binary_istream;
	friend class binary::ua_binary_ostream;

public:
	ua_extension(rx_node_id class_id, rx_node_id binary_id, rx_node_id xml_id);
	ua_extension() = default;
	virtual ~ua_extension() = default;
	ua_extension(const ua_extension&) = delete;
	ua_extension(ua_extension&&) = delete;

	rx_node_id class_id;
	rx_node_id binary_id;
	rx_node_id xml_id;

	virtual opcua_extension_ptr make_copy();

private:
	virtual void internal_serialize_extension(binary::ua_binary_ostream& stream) const;
	virtual void internal_deserialize_extension(binary::ua_binary_istream& stream);


};


struct application_description
{
	string_type application_uri;
	string_type product_uri;
	localized_text application_name;
	application_type_t application_type;
	string_type gateway_uri;
	string_type discovery_profile_uri;
	string_array discovery_urls;
	void deserialize(binary::ua_binary_istream& stream);
	void serialize(binary::ua_binary_ostream& stream) const;
};

class variant_type;
struct data_value;
struct diagnostic_info;

union vunion_type
{
	bool		b_val;
	char		sc_val;
	uint8_t		c_val;
	int16_t		sw_val;
	uint16_t		w_val;
	int32_t		sdw_val;
	uint32_t		dw_val;
	int64_t		sqw_val;
	uint64_t qw_val;
	float		f_val;
	double		d_val;
	string_type* str_val;
	rx_time_struct	ft_val;
	rx_uuid_t* guid_val;
	byte_string* bstr_val;
	rx_node_id* node_val;
	qualified_name* qname_val;
	localized_text* ltext_val;
	ua_extension* ext_val;
	data_value* data_val;
	variant_type* var_val;
	diagnostic_info* di_val;
	vunion_type* array_val;
};

void assign_vunion(vunion_type& what, bool val);
void assign_vunion(vunion_type& what, int8_t val);
void assign_vunion(vunion_type& what, uint8_t val);
void assign_vunion(vunion_type& what, int16_t val);
void assign_vunion(vunion_type& what, uint16_t val);
void assign_vunion(vunion_type& what, int32_t val);
void assign_vunion(vunion_type& what, uint32_t val);
void assign_vunion(vunion_type& what, int64_t val);
void assign_vunion(vunion_type& what, uint64_t val);
void assign_vunion(vunion_type& what, rx_time_struct val);
void assign_vunion(vunion_type& what, float val);
void assign_vunion(vunion_type& what, double val);

void assign_vunion(vunion_type& what, const string_type& val);

template<typename T>
void assign_vunion(vunion_type& what, const std::vector<T>& val);

void assign_vunion(vunion_type& what, const qualified_name& val);
void assign_vunion(vunion_type& what, const localized_text& val);


const uint32_t opcid_Null = 0;

class variant_type
{
	friend class binary::ua_binary_istream;
	friend class binary::ua_binary_ostream;
public:
	variant_type();

	variant_type(const variant_type& right);
	variant_type(variant_type&& right) noexcept;
	variant_type(bool val);
	variant_type(char val);
	variant_type(uint8_t val);
	variant_type(int16_t val);
	variant_type(uint16_t val);
	variant_type(int32_t val);
	variant_type(uint32_t val);
	variant_type(int64_t val);

	variant_type(uint64_t val);

	variant_type(float val);

	variant_type(double val);

	variant_type(const string_array& val);

	variant_type(const qualified_name& val);
	variant_type(const localized_text& val);
	variant_type(const rx_node_id& val);
	variant_type(const data_value& val);

	variant_type(rx_time val);

	~variant_type();

	variant_type& operator=(const variant_type& right);
	variant_type& operator=(variant_type&& right) noexcept;

	bool is_null() const;

	void clear();

	bool is_array() const;


	void to_string(string_type& str);

	bool from_rx_value(const values::rx_value& value, uint8_t hint = 0, uint16_t ns = 0);

	void fill_rx_value(values::rx_value& value) const;

	const uint8_t get_type() const;

	const int get_array_len() const;

	const std::vector<int>& get_dimensions() const;

	void set_default(uint8_t type, int value_rank, const const_size_vector<uint32_t>& dimensions);

private:
	void copy_from(const variant_type& right);
	void clear_union(uint8_t type, vunion_type& vu);
	void copy_union_from(uint8_t type, vunion_type& to, const vunion_type& from);

private:
	uint8_t type_{ opcid_Null };
	vunion_type union_;
	int array_len_{ -1 };
	std::vector<int> dimensions_;

};





struct diagnostic_info
{
	void fill_diagnostics_strings(string_array& strings, uint32_t mask) const;
	void retrive_diagnostics_strings(const string_array& strings, uint32_t result);

	diagnostic_info* make_clone();

	string_type namespace_uri;
	string_type symbolic_id;
	string_type locale;
	string_type localized_text;
	string_type additional_info;
	std::unique_ptr<diagnostic_info> internal_diagnostic_info;
	uint32_t internal_result = 0;
	mutable int namespace_uri_idx = -1;
	mutable int symbolic_id_idx = -1;
	mutable int locale_idx = -1;
	mutable int localized_text_idx = -1;

};



struct data_value
{

public:
	data_value& operator=(const data_value& right);
	rx_result fill_rx_value(values::rx_value& vvalue) const;
	rx_result from_rx_value(values::rx_value&& vvalue);
	rx_result from_rx_value(const values::rx_value& vvalue);
	void set_timestamp(rx_time val);
	uint32_t status_code = 0;
	variant_type value;
	rx_time_struct server_ts{ 0 };
	rx_time_struct source_ts{ 0 };
	uint16_t server_ps{ 0 };
	uint16_t source_ps{ 0 };

};


/*!!!!TODO
template<class ua_istream, class ua_ostream>
typedef std::vector<application_description> servers_type;

*/

typedef string_type numeric_range;

typedef uint32_t opcua_result_t;

struct read_value_id
{
	read_value_id(attribute_id vattribute_id = attribute_id::value)
	{
		attr_id = vattribute_id;
	}
	rx_node_id node_id;
	attribute_id attr_id;
	numeric_range range;
	qualified_name data_encoding;
	void serialize(binary::ua_binary_ostream& stream) const;
	void deserialize(binary::ua_binary_istream& stream);
};

template<class ua_istream, class ua_ostream>
struct write_value
{
	write_value(attribute_id vattribute_id = attribute_id::value)
	{
		attr_id = vattribute_id;
	}
	rx_node_id node_id;
	attribute_id attr_id;
	numeric_range range;
	data_value value;
	void serialize(ua_ostream& stream) const
	{
		stream << node_id;
		stream << attr_id;
		stream << range;
		stream << value;
	}
	void deserialize(ua_istream& stream)
	{
		stream >> node_id;
		stream >> attr_id;
		stream >> range;
		stream >> value;
	}
};

struct opcua_view_description
{
	rx_node_id view_id;
	rx_time timestamp;
	uint32_t view_version = 0;


	void serialize(binary::ua_binary_ostream& stream) const;
	void deserialize(binary::ua_binary_istream& stream);
};

struct opcua_browse_description
{
	rx_node_id node_id;
	browse_direction_type direction;
	rx_node_id reference_type_id;
	bool sub_types = false;
	uint32_t node_class_mask = 0xff;
	uint32_t result_mask = 0xff;

	std::set<rx_node_id> resolved_reference_ids;


	void serialize(binary::ua_binary_ostream& stream) const;
	void deserialize(binary::ua_binary_istream& stream);
};


struct reference_description
{
	rx_node_id reference_id;
	bool is_forward = false;
	rx_node_id target_id;
	qualified_name browse_name;
	localized_text display_name;
	node_class_type node_class;
	rx_node_id type_id;


	void serialize(binary::ua_binary_ostream& stream) const;
	void deserialize(binary::ua_binary_istream& stream);
};


struct browse_result_internal
{
	rx_result result;
	opcua_result_t status_code;
	std::vector<reference_description> references;
	void add_reference_data(bool forward, const opcua_addr_space::reference_data& data, const opcua_browse_description& to_browse);
};

struct opcua_browse_result
{
	opcua_result_t status_code;
	byte_string continuation_point;
	std::vector<reference_description> references;

	void serialize(binary::ua_binary_ostream& stream) const;
	void deserialize(binary::ua_binary_istream& stream);
};

struct user_token_policy
{
	string_type policy_id;
	int token_type;
	string_type issued_token_type;
	string_type issuer_endpoint_url;
	string_type policy_uri;
	void deserialize(binary::ua_binary_istream& stream);
	void serialize(binary::ua_binary_ostream& stream) const;
};

template<class ua_istream, class ua_ostream>
struct application_certificate
{
	string_type version;
	byte_string serial;
	string_type algorithm;
	byte_string signature;
	rx_time_struct valid_from;
	rx_time_struct valid_to;
	string_type application_uri;
	string_array host_names;
	byte_string public_key;
	string_array key_usage;
	void deserialize(ua_istream& stream)
	{
		stream >> version;
		stream >> serial;
		stream >> algorithm;
		stream >> signature;
		stream >> valid_from;
		stream >> valid_to;
		stream >> application_uri;
		stream >> host_names;
		stream >> public_key;
		stream >> key_usage;
	}
	void serialize(ua_ostream& stream) const
	{
		stream << version;
		stream << serial;
		stream << algorithm;
		stream << signature;
		stream << valid_from;
		stream << valid_to;
		stream << application_uri;
		stream << host_names;
		stream << public_key;
		stream << key_usage;
	}
};

struct endpoint_description
{
	string_type url;
	application_description application;
	byte_string certificate;
	security_mode_t security_mode = security_mode_t::none;
	string_type policy_uri;
	std::vector<user_token_policy> user_tokens;
	string_type transport_profile_uri;
	uint8_t security_level = 0;
	void deserialize(binary::ua_binary_istream& stream);
	void serialize(binary::ua_binary_ostream& stream) const;
};

/*TODO!!!!
typedef std::vector<endpoint_description> endpoints_description_type;
*/




} //namespace opcua
} //namespace common
} //namespace protocols


#endif
