

/****************************************************************************
*
*  common\rx_common.h
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


#ifndef rx_common_h
#define rx_common_h 1



// standard return codes
#define RX_ERROR 0
#define RX_OK 1
#define RX_ASYNC 2

// Hide ugly details about RX_COMMON_API
#ifdef _MSC_VER // MSVC compiler
#include "win32/win32_common.h"
#endif
#ifdef __GNUC__ // GCC compiler
#include "gnu/gnu_common.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct rx_platform_init_data_t
{
	int rx_hd_timer; // use high definition timer

} rx_platform_init_data;




RX_COMMON_API int rx_init_common_library(const rx_platform_init_data* init_data);
RX_COMMON_API void rx_deinit_common_library();


// match pattern function
RX_COMMON_API int rx_match_pattern(const char* string, const char* pattern, int case_sensitive);

// time related functions
typedef uint64_t rx_timer_ticks_t;

typedef struct rx_time_struct_t
{
	rx_timer_ticks_t t_value;
} rx_time_struct;

typedef struct rx_full_time_t
{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t w_day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t milliseconds;

} rx_full_time;

///////////////////////////////////////////////////////////////
// TIMES FOR ASN1 MMS IEC TIME VALUES
///////////////////////////////////////////////////////////////

typedef struct asn_generalized_time_t
{
	uint16_t year;
	uint16_t month;
	uint16_t day;

	uint16_t hour;
	uint16_t minute;
	uint16_t second;

	uint32_t fraction;

} asn_generalized_time;


typedef struct asn_binary_time_t
{
	int full;
	uint32_t mstime;
	uint16_t days;

} asn_binary_time;



#define MS_IN_DAY (1000*60*60*24)
#define MS_START_1984 0x00000afe0bd9e400L

#define server_time_struct_DIFF_TIMEVAL  116444736000000000ull


#define DEFAULT_TIME_QUALITY (TIME_QUALITY_NOT_SYNCHRONIZED|TIME_QUALITY_CLASS_T1)
#define SYNCHRONIZED_TIME_QUALITY (TIME_QUALITY_CLASS_T1)


#define DEFAULT_NAMESPACE 1
#define DEFAULT_OPC_NAMESPACE 0
#define RX_USER_NAMESPACE 999


#define TIME_QUALITY_LEAP_SECONDS_KNOWN 0x80
#define TIME_QUALITY_CLOCK_FALIURE 0x40
#define TIME_QUALITY_NOT_SYNCHRONIZED 0x20

#define TIME_QUALITY_CLASS_UNKNOWN 0x1f
#define TIME_QUALITY_CLASS_T0 7
#define TIME_QUALITY_CLASS_T1 10
#define TIME_QUALITY_CLASS_T2 14
#define TIME_QUALITY_CLASS_T3 16
#define TIME_QUALITY_CLASS_T4 18
#define TIME_QUALITY_CLASS_T5 20

RX_COMMON_API int rx_os_get_system_time(struct rx_time_struct_t* st);
RX_COMMON_API int rx_os_to_local_time(struct rx_time_struct_t* st);
RX_COMMON_API int rx_os_to_utc_time(struct rx_time_struct_t* st);
RX_COMMON_API rx_timer_ticks_t rx_os_get_ms(struct rx_time_struct_t* st);
RX_COMMON_API int rx_os_split_time(const struct rx_time_struct_t* st, struct rx_full_time_t* full);
RX_COMMON_API int rx_os_collect_time(const struct rx_full_time_t* full, struct rx_time_struct_t* st);

RX_COMMON_API rx_timer_ticks_t rx_get_tick_count();
RX_COMMON_API rx_timer_ticks_t rx_get_us_ticks();


// values stuff


// union's value types
#define RX_NULL_TYPE			0x00
#define RX_BOOL_TYPE			0x01
#define RX_INT8_TYPE			0x02
#define RX_UINT8_TYPE			0x03
#define RX_INT16_TYPE			0x04
#define RX_UINT16_TYPE			0x05
#define RX_INT32_TYPE			0x06
#define RX_UINT32_TYPE			0x07
#define RX_INT64_TYPE			0x08
#define RX_UINT64_TYPE			0x09
#define RX_FLOAT_TYPE			0x0a
#define RX_DOUBLE_TYPE			0x0b
#define RX_COMPLEX_TYPE			0x0c
#define RX_STRING_TYPE			0x0d
#define RX_TIME_TYPE			0x0e
#define RX_UUID_TYPE			0x0f
#define RX_BYTES_TYPE			0x10
#define RX_STRUCT_TYPE			0x11
#define RX_TYPE_TYPE			0x12
#define RX_NODE_ID_TYPE			0x13

#define RX_SIMPLE_VALUE_MASK	0x1f



// value types for object and class types
#define RX_DEFINITION_TYPE_MASK	0x60
#define RX_BINARY_VALUE			0x00
#define RX_JSON_VALUE			0x20
#define RX_SCRIPT_VALUE			0x40


#define RX_STRIP_ARRAY_MASK		0x7f
#define RX_ARRAY_VALUE_MASK		0x80

#define RX_SIMPLE_TYPE(t) ((RX_STRIP_ARRAY_MASK&t))
#define IS_ARRAY_VALUE(t) ((t&RX_ARRAY_VALUE_MASK)==RX_ARRAY_VALUE_MASK)

#define RX_INVALID_INDEX_VALUE ((size_t)(-1l))

#define PI_CONST 3.14159265359
#define FLOAT_EPSILON 0.00000000001

#define RX_QUALITY_MASK			0xc0000000
#define RX_QUALITY_DETAILS_MASK	0x3fffffff
#define RX_ORIGIN_MASK			0xfff60000
#define RX_LIMIT_MASK			0x00030000
#define RX_LEVEL_MASK			0x0000ffff

// master quality
#define RX_GOOD_QUALITY			0x00000000
#define RX_UNCERTAIN_QUALITY	0x40000000
#define RX_BAD_QUALITY			0x80000000
#define RX_DEAD_QUALITY			0xc0000000


// bad , uncertain quality codes
#define RX_Q_OUT_OF_RANGE		0x00000004
#define RX_Q_BAD_REFERENCE		0x00000008
#define RX_Q_OSCILLATORY		0x00000010


// bad quality codes
#define RX_Q_QUALITY_INVALID	0x00000001
#define RX_Q_OVERFLOW			0x00000002
#define RX_Q_FAILURE			0x00000020
#define RX_Q_DEVICE_FAILURE		0x00000040
#define RX_Q_CONFIG_ERROR		0x00000080
#define RX_Q_NOT_CONNECTED		0x00000100
#define RX_Q_TYPE_MISMATCH		0x00000200
#define RX_Q_SYNTAX_ERROR		0x00000400
#define RX_Q_DIVISION_BY_ZERO	0x00000800
#define RX_Q_OFFLINE			0x00001000

// uncertain quality codes
#define RX_Q_OLD_DATA			0x00000001
#define RX_Q_INCONSISTENT		0x00000002
#define RX_Q_INACURATE			0x00000020
#define RX_Q_INITIAL_VALUE		0x00000040

// combined quality values

#define RX_BAD_QUALITY_QUALITY_INVALID	(RX_BAD_QUALITY|RX_Q_QUALITY_INVALID)
#define RX_BAD_QUALITY_OVERFLOW			(RX_BAD_QUALITY|RX_Q_OVERFLOW)
#define RX_BAD_QUALITY_OUT_OF_RANGE		(RX_BAD_QUALITY|RX_Q_OUT_OF_RANGE)
#define RX_BAD_QUALITY_BAD_REFERENCE	(RX_BAD_QUALITY|RX_Q_BAD_REFERENCE)
#define RX_BAD_QUALITY_OSCILLATORY		(RX_BAD_QUALITY|RX_Q_OSCILLATORY)
#define RX_BAD_QUALITY_FAILURE			(RX_BAD_QUALITY|RX_Q_FAILURE)
#define RX_BAD_QUALITY_DEVICE_FAILURE	(RX_BAD_QUALITY|RX_Q_DEVICE_FAILURE)
#define RX_BAD_QUALITY_CONFIG_ERROR		(RX_BAD_QUALITY|RX_Q_CONFIG_ERROR)
#define RX_BAD_QUALITY_NOT_CONNECTED	(RX_BAD_QUALITY|RX_Q_NOT_CONNECTED)
#define RX_BAD_QUALITY_TYPE_MISMATCH	(RX_BAD_QUALITY|RX_Q_TYPE_MISMATCH)
#define RX_BAD_QUALITY_SYNTAX_ERROR		(RX_BAD_QUALITY|RX_Q_SYNTAX_ERROR)
#define RX_BAD_QUALITY_DIVISION_BY_ZERO	(RX_BAD_QUALITY|RX_Q_DIVISION_BY_ZERO)
#define RX_BAD_QUALITY_OFFLINE			(RX_BAD_QUALITY|RX_Q_OFFLINE)

#define RX_UNCERTAIN_QUALITY_OLD_DATA		(RX_UNCERTAIN_QUALITY|RX_Q_OLD_DATA)
#define RX_UNCERTAIN_QUALITY_INCONSISTENT	(RX_UNCERTAIN_QUALITY|RX_Q_INCONSISTENT)
#define RX_UNCERTAIN_QUALITY_OUT_OF_RANGE	(RX_UNCERTAIN_QUALITY|RX_Q_OUT_OF_RANGE)
#define RX_UNCERTAIN_QUALITY_BAD_REFERENCE	(RX_UNCERTAIN_QUALITY|RX_Q_BAD_REFERENCE)
#define RX_UNCERTAIN_QUALITY_OSCILLATORY	(RX_UNCERTAIN_QUALITY|RX_Q_OSCILLATORY)
#define RX_UNCERTAIN_QUALITY_INACURATE		(RX_UNCERTAIN_QUALITY|RX_Q_INACURATE)
#define RX_UNCERTAIN_QUALITY_INITIAL_VALUE	(RX_UNCERTAIN_QUALITY|RX_Q_INITIAL_VALUE)


#define RX_DEFAULT_VALUE_QUALITY		RX_UNCERTAIN_QUALITY_INITIAL_VALUE
#define RX_CONFIG_ERROR_QUALITY			RX_BAD_QUALITY_CONFIG_ERROR
#define RX_NOT_CONNECTED_QUALITY		RX_BAD_QUALITY_NOT_CONNECTED


#define RX_DEFAULT_ORIGIN		0x0
#define RX_DEFAULT_TEST_ORIGIN	0x20000000

// origin bits
#define RX_FORCED_ORIGIN		0x80000000
#define RX_BLOCKED_ORIGIN		0x40000000
#define RX_TEST_ORIGIN			0x20000000
#define RX_CALLCUALTED_ORIGIN	0x10000000
#define RX_ESTIMATED_ORIGIN		0x08000000
#define RX_LOCAL_ORIGIN			0x04000000


typedef uint_fast8_t rx_value_t;


RX_COMMON_API int rx_parse_value_type_name(const char* strtype, rx_value_t* type);
RX_COMMON_API const char* rx_get_value_type_name(rx_value_t type);

typedef struct string_value_struct_t
{
	size_t size;
	char* value;
} string_value_struct;


RX_COMMON_API int rx_init_string_value_struct(string_value_struct* data, const char* val, int count);
RX_COMMON_API const char* rx_c_str(const string_value_struct* data);
RX_COMMON_API int rx_copy_string_value(string_value_struct* dest, const string_value_struct* src);
RX_COMMON_API void rx_destory_string_value_struct(string_value_struct* data);

typedef struct bytes_value_struct_t
{
	size_t size;
	uint8_t* value;
} bytes_value_struct;


RX_COMMON_API int rx_init_bytes_value_struct(bytes_value_struct* data, const uint8_t* bytes, size_t len);
RX_COMMON_API const uint8_t* rx_c_ptr(const bytes_value_struct* data, size_t* size);
RX_COMMON_API int rx_copy_bytes_value(bytes_value_struct* dest, const bytes_value_struct* src);
RX_COMMON_API void rx_destory_bytes_value_struct(bytes_value_struct* data);

union rx_value_union;

typedef struct array_value_struct_t
{
	size_t size;
	union rx_value_union* values;
} array_value_struct;

struct typed_value_type;

typedef struct struct_value_type_t
{
	size_t size;
	struct typed_value_type* values;
} struct_value_type;


typedef struct complex_value_struct_t
{
	double real;
	double imag;
} complex_value_struct;


// node id
union rx_node_id_union
{
	uint32_t int_value;
	string_value_struct string_value;
	bytes_value_struct bstring_value;
	rx_uuid_t uuid_value;
};
enum rx_node_id_type
{
	rx_node_id_numeric = 0,
	rx_node_id_string = 1,
	rx_node_id_uuid = 2,
	rx_node_id_bytes = 3
};
typedef struct rx_node_id_struct_t
{
	uint16_t namespace_index;
	enum rx_node_id_type node_type;
	union rx_node_id_union value;

} rx_node_id_struct;


RX_COMMON_API void rx_generate_new_uuid(rx_uuid_t* u);
RX_COMMON_API int rx_uuid_to_string(const rx_uuid_t* u, char* str);
RX_COMMON_API int rx_string_to_uuid(const char* str, rx_uuid_t* u);
RX_COMMON_API int rx_is_null_uuid(const rx_uuid_t* val);

RX_COMMON_API int rx_init_null_node_id(rx_node_id_struct* data);
RX_COMMON_API int rx_init_int_node_id(rx_node_id_struct* data, uint32_t id, uint16_t namesp);
RX_COMMON_API int rx_init_string_node_id(rx_node_id_struct* data, const char* id, int count, uint16_t namesp);
RX_COMMON_API int rx_init_uuid_node_id(rx_node_id_struct* data, const rx_uuid_t* id, uint16_t namesp);
RX_COMMON_API int rx_init_bytes_node_id(rx_node_id_struct* data, const uint8_t* id, size_t len, uint16_t namesp);

RX_COMMON_API int rx_copy_node_id(rx_node_id_struct* data, const rx_node_id_struct* src);
RX_COMMON_API int rx_move_node_id(rx_node_id_struct* data, rx_node_id_struct* src);

RX_COMMON_API int rx_compare_node_ids(const rx_node_id_struct* left, const rx_node_id_struct* right);
RX_COMMON_API int rx_is_null_node_id(const rx_node_id_struct* data);

RX_COMMON_API int rx_node_id_to_string(const rx_node_id_struct* data, string_value_struct* str);
RX_COMMON_API int rx_node_id_from_string(rx_node_id_struct* data, const char* str);

RX_COMMON_API void rx_destory_node_id(rx_node_id_struct* data);

union rx_value_union
{
	uint_fast8_t bool_value;

	uint8_t uint8_value;
	int8_t int8_value;
	uint16_t uint16_value;
	int16_t int16_value;
	uint32_t uint32_value;
	int32_t int32_value;
	uint64_t uint64_value;
	int64_t int64_value;

	float float_value;
	double double_value;

	rx_time_struct time_value;

#ifdef RX_VALUE_SIZE_16
	complex_value_struct complex_value;
	rx_uuid_t uuid_value;
#else
	complex_value_struct* complex_value;
	rx_uuid_t* uuid_value;
#endif

	string_value_struct string_value;
	bytes_value_struct bytes_value;
	rx_node_id_struct* node_id_value;

	array_value_struct array_value;
};


struct typed_value_type
{
	rx_value_t value_type;
	union rx_value_union value;
};


struct timed_value_type
{
	struct typed_value_type value;
	struct rx_time_struct_t time;
};


struct full_value_type
{
	struct typed_value_type value;
	struct rx_time_struct_t time;
	uint32_t quality;
	uint32_t origin;
};

RX_COMMON_API void rx_destroy_value(struct typed_value_type* val);

RX_COMMON_API int rx_init_null_value(struct typed_value_type* val);

RX_COMMON_API int rx_init_bool_value(struct typed_value_type* val, uint_fast8_t data);

RX_COMMON_API int rx_init_int8_value(struct typed_value_type* val, int8_t data);
RX_COMMON_API int rx_init_uint8_value(struct typed_value_type* val, uint8_t data);
RX_COMMON_API int rx_init_int16_value(struct typed_value_type* val, int16_t data);
RX_COMMON_API int rx_init_uint16_value(struct typed_value_type* val, uint16_t data);
RX_COMMON_API int rx_init_int32_value(struct typed_value_type* val, int32_t data);
RX_COMMON_API int rx_init_uint32_value(struct typed_value_type* val, uint32_t data);
RX_COMMON_API int rx_init_int64_value(struct typed_value_type* val, int64_t data);
RX_COMMON_API int rx_init_uint64_value(struct typed_value_type* val, uint64_t data);

RX_COMMON_API int rx_init_float_value(struct typed_value_type* val, float data);
RX_COMMON_API int rx_init_double_value(struct typed_value_type* val, double data);
RX_COMMON_API int rx_init_complex_value(struct typed_value_type* val, complex_value_struct data);

RX_COMMON_API int rx_init_string_value(struct typed_value_type* val, const char* data, int count);
RX_COMMON_API int rx_init_bytes_value(struct typed_value_type* val, const uint8_t* data, size_t count);

RX_COMMON_API int rx_init_uuid_value(struct typed_value_type* val, const rx_uuid_t* data);
RX_COMMON_API int rx_init_time_value(struct typed_value_type* val, const rx_time_struct data);

RX_COMMON_API int rx_init_node_id_value(struct typed_value_type* val, const rx_node_id_struct* data);


RX_COMMON_API int rx_parse_string(struct typed_value_type* val, const char* data);


RX_COMMON_API void rx_assign_value(struct typed_value_type* val, const struct typed_value_type* right);
RX_COMMON_API void rx_copy_value(struct typed_value_type* val, const struct typed_value_type* right);
RX_COMMON_API void rx_move_value(struct typed_value_type* val, struct typed_value_type* right);

RX_COMMON_API int rx_convert_value(struct typed_value_type* val, rx_value_t type);

RX_COMMON_API int rx_compare_values(const struct typed_value_type* val, const struct typed_value_type* right);


RX_COMMON_API int rx_is_null_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_float_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_complex_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_numeric_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_integer_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_unassigned_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_bool_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_string_value(const struct typed_value_type* val);
RX_COMMON_API int rx_is_array_value(const struct typed_value_type* val);
RX_COMMON_API int rx_get_array_size(const struct typed_value_type* val, size_t* size);

RX_COMMON_API int rx_get_float_value(const struct typed_value_type* val, size_t idx, double* value, rx_value_t* type);
RX_COMMON_API int rx_get_complex_value(const struct typed_value_type* val, size_t idx, complex_value_struct* value);
RX_COMMON_API int rx_get_integer_value(const struct typed_value_type* val, size_t idx, int64_t* value, rx_value_t* type);
RX_COMMON_API int rx_get_unassigned_value(const struct typed_value_type* val, size_t idx, uint64_t* value, rx_value_t* type);
RX_COMMON_API int rx_get_bool_value(const struct typed_value_type* val, size_t idx, int* value);
RX_COMMON_API int rx_get_string_value(const struct typed_value_type* val, size_t idx, string_value_struct* value);

union rx_reference_data
{
	string_value_struct path;
	rx_node_id_struct id;
};

typedef struct rx_reference_struct_t
{
	int is_path;
	union rx_reference_data data;
} rx_reference_struct;



RX_COMMON_API int rx_init_null_reference(rx_reference_struct* ref);
RX_COMMON_API int rx_init_path_reference(rx_reference_struct* ref, const char* path, int count);
RX_COMMON_API int rx_init_id_reference(rx_reference_struct* ref, const rx_node_id_struct* id);

RX_COMMON_API int rx_copy_reference(rx_reference_struct* ref, const rx_reference_struct* src);
RX_COMMON_API int rx_move_reference(rx_reference_struct* ref, rx_reference_struct* src);

RX_COMMON_API int rx_compare_references(const rx_reference_struct* left, const rx_reference_struct* right);
RX_COMMON_API int rx_is_null_reference(const rx_reference_struct* ref);

RX_COMMON_API int rx_reference_to_string(const rx_reference_struct* data, string_value_struct* str);
RX_COMMON_API int rx_reference_from_string(rx_reference_struct* data, const char* str);

RX_COMMON_API void rx_deinit_reference(rx_reference_struct* ref);

/*
RX_COMMON_API int rx_is_path_reference(const rx_reference_struct* ref); 
RX_COMMON_API int rx_is_id_reference(const rx_reference_struct* ref);

RX_COMMON_API int rx_get_path_reference(const rx_reference_struct* ref, const char** path);
RX_COMMON_API int rx_extract_id_reference(const rx_reference_struct* ref, rx_node_id_struct* id);
*/



#ifdef __cplusplus
}
#endif


#endif