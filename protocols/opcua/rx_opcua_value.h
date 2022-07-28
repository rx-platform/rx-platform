

/****************************************************************************
*
*  protocols\opcua\rx_opcua_value.h
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


#ifndef rx_opcua_value_h
#define rx_opcua_value_h 1


#include "lib/rx_const_size_vector.h"


namespace protocols {
namespace opcua {

namespace binary {

class ua_binary_istream;
class ua_binary_ostream;

}

namespace common {


class variant_type;
struct data_value;
struct diagnostic_info;
struct localized_text;
struct qualified_name;
class ua_extension;

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

	bool operator==(const variant_type& right);
	bool operator!=(const variant_type& right);

	bool is_null() const;

	void clear();

	bool is_array() const;


	void to_string(string_type& str);

	bool from_rx_value(const values::rx_value& value, uint8_t hint = 0, uint16_t ns = 0);

	bool fill_rx_value(values::rx_value& value) const;

	const uint8_t get_type() const;

	const int get_array_len() const;

	const std::vector<int>& get_dimensions() const;

	void set_default(uint8_t type, int value_rank, const const_size_vector<uint32_t>& dimensions);

	static uint32_t get_opc_type_from_rx_type(rx_value_t valType);

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


struct data_value
{

public:
	data_value();
	~data_value();
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





} //namespace opcua
} //namespace common
} //namespace protocols


#endif
