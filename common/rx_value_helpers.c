

/****************************************************************************
*
*  common\rx_value_helpers.c
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

#include "rx_common.h"
#include <math.h>
#include "protocols/ansi_c/common_c/rx_packet_buffer.h"


// these are functions from "rx_value_conv.c"
int int64_to_str(int64_t val, string_value_struct* str);
int bool_to_str(uint_fast8_t val, string_value_struct* str);
int int8_to_str(int8_t val, string_value_struct* str);
int uint8_to_str(uint8_t val, string_value_struct* str);
int int16_to_str(int16_t val, string_value_struct* str);
int uint16_to_str(uint16_t val, string_value_struct* str);
int int32_to_str(int32_t val, string_value_struct* str);
int uint32_to_str(uint32_t val, string_value_struct* str);
int int64_to_str(int64_t val, string_value_struct* str);
int uint64_to_str(uint64_t val, string_value_struct* str);
int time_to_ISO8601(rx_time_struct val, string_value_struct* str);
int float_to_str(float val, string_value_struct* str);
int double_to_str(double val, string_value_struct* str);
int complex_to_str(complex_value_struct* val, string_value_struct* str);
int bytes_to_str(const bytes_value_struct* val, string_value_struct* str);

double complex_amplitude_helper(const complex_value_struct* val)
{
	return sqrt(val->real * val->real + val->imag * val->imag);
}

int is_simple_union_type(rx_value_t type)
{
	return !IS_ARRAY_VALUE(type) && type < RX_TIME_TYPE&& type != RX_STRING_TYPE;
}

int is_null_union_type(rx_value_t type)
{
	return type == RX_NULL_TYPE;
}

int is_float_union_type(rx_value_t type)
{
	return type == RX_FLOAT_TYPE || type == RX_DOUBLE_TYPE || type == RX_COMPLEX_TYPE;
}
int is_integer_union_type(rx_value_t type)
{
	return type == RX_BOOL_TYPE || type <= RX_INT8_TYPE
		|| type == RX_INT16_TYPE || type <= RX_INT32_TYPE
		|| type == RX_INT64_TYPE;
}
int is_unassigned_union_type(rx_value_t type)
{
	return type == RX_BOOL_TYPE || type <= RX_UINT8_TYPE
		|| type == RX_UINT16_TYPE || type <= RX_UINT32_TYPE
		|| type == RX_UINT64_TYPE;
}
int is_numeric_union_type(rx_value_t type)
{
	return type > RX_NULL_TYPE && type <= RX_COMPLEX_TYPE;
}
int is_complex_union_type(rx_value_t type)
{
	return type == RX_COMPLEX_TYPE;
}


RX_COMMON_API int rx_is_null_value(const struct typed_value_type* val)
{
	return is_null_union_type(val->value_type);
}
RX_COMMON_API int rx_is_float_value(const struct typed_value_type* val)
{
	return is_float_union_type((val->value_type & RX_SIMPLE_VALUE_MASK));
}
RX_COMMON_API int rx_is_complex_value(const struct typed_value_type* val)
{
	return is_complex_union_type((val->value_type & RX_SIMPLE_VALUE_MASK));
}
RX_COMMON_API int rx_is_numeric_value(const struct typed_value_type* val)
{
	return is_numeric_union_type((val->value_type & RX_SIMPLE_VALUE_MASK));
}
RX_COMMON_API int rx_is_integer_value(const struct typed_value_type* val)
{
	return is_integer_union_type((val->value_type & RX_SIMPLE_VALUE_MASK));
}
RX_COMMON_API int rx_is_unassigned_value(const struct typed_value_type* val)
{
	return is_unassigned_union_type((val->value_type & RX_SIMPLE_VALUE_MASK));
}
RX_COMMON_API int rx_is_bool_value(const struct typed_value_type* val)
{
	return (val->value_type & RX_SIMPLE_VALUE_MASK) == RX_BOOL_TYPE;
}
RX_COMMON_API int rx_is_string_value(const struct typed_value_type* val)
{
	return (val->value_type & RX_SIMPLE_VALUE_MASK) == RX_STRING_TYPE;
}
RX_COMMON_API int rx_is_bytes_value(const struct typed_value_type* val)
{
	return (val->value_type & RX_SIMPLE_VALUE_MASK) == RX_BYTES_TYPE;
}
RX_COMMON_API int rx_is_array_value(const struct typed_value_type* val)
{
	return (val->value_type & RX_ARRAY_VALUE_MASK) != 0;
}
RX_COMMON_API int rx_get_array_size(const struct typed_value_type* val, size_t* size)
{
	if ((val->value_type & RX_ARRAY_VALUE_MASK) != 0)
	{
		*size = val->value.array_value.size;
		return RX_OK;
	}
	else
	{
		*size = RX_INVALID_INDEX_VALUE;
		return RX_ERROR;
	}
}

RX_COMMON_API int rx_is_struct(const struct typed_value_type* val)
{
	return (val->value_type == RX_STRUCT_TYPE);
}
RX_COMMON_API int rx_get_struct_size(const struct typed_value_type* val, size_t* size)
{
	if (val->value_type == RX_STRUCT_TYPE)
	{
		*size = val->value.struct_value.size;
		return RX_OK;
	}
	else
	{
		*size = RX_INVALID_INDEX_VALUE;
		return RX_ERROR;
	}
}

RX_COMMON_API int rx_get_struct_value(size_t idx, struct typed_value_type* out_val, const struct typed_value_type* val)
{
	if (val->value_type == RX_STRUCT_TYPE
		&& idx < val->value.struct_value.size)
	{
		*out_val = val->value.struct_value.values[idx];
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}


void assign_value(union rx_value_union* left, const union rx_value_union* right, rx_value_t type)
{
	// copy memory first
	memcpy(left, right, sizeof(union rx_value_union));
	// now do the actual copy of pointer members
	if (type & RX_ARRAY_VALUE_MASK)
	{// we have an array
		size_t count = left->array_value.size;
		if (count > 0)
		{
			left->array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
			for (size_t i = 0; i < count; i++)
			{
				assign_value(&left->array_value.values[i], &right->array_value.values[i], RX_STRIP_ARRAY_MASK & type);
			}
		}
	}
	else
	{//simple union stuff
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
			left->complex_value = rx_heap_alloc(sizeof(complex_value_struct));
			*left->complex_value = *right->complex_value;
			break;
		case RX_UUID_TYPE:
			left->uuid_value = rx_heap_alloc(sizeof(rx_uuid_t));
			*left->uuid_value = *right->uuid_value;
			break;
#endif
		case RX_STRING_TYPE:
			rx_copy_string_value(&left->string_value, &right->string_value);
			break;
		case RX_BYTES_TYPE:
			rx_copy_bytes_value(&left->bytes_value, &right->bytes_value);
			break;
		case RX_STRUCT_TYPE:
			if (right->struct_value.size)
			{
				left->struct_value.values = rx_heap_alloc(sizeof(struct typed_value_type) * right->struct_value.size);
				for (size_t i = 0; i < right->struct_value.size; i++)
				{
					left->struct_value.values[i].value_type = right->struct_value.values[i].value_type;
					assign_value(&left->struct_value.values[i].value, &right->struct_value.values[i].value, right->struct_value.values[i].value_type);
				}
			}
			break;
		case RX_NODE_ID_TYPE:
			left->node_id_value = rx_heap_alloc(sizeof(rx_node_id_struct));
			rx_copy_node_id(left->node_id_value, right->node_id_value);
			break;
		}
	}
}

void destroy_union_value(union rx_value_union* who, rx_value_t type)
{
	if (type == RX_NULL_TYPE)
		return;

	if (type & RX_ARRAY_VALUE_MASK)
	{// we have an array
		if (who->array_value.size > 0)
		{
			for (size_t i = 0; i < who->array_value.size; i++)
			{
				destroy_union_value(&who->array_value.values[i], RX_STRIP_ARRAY_MASK & type);
			}
			rx_heap_free(who->array_value.values);
		}
	}
	else
	{//simple union stuff
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
			rx_heap_free(who->complex_value);
			break;
		case RX_UUID_TYPE:
			rx_heap_free(who->uuid_value);
			break;
#endif
		case RX_STRING_TYPE:
			rx_destory_string_value_struct(&who->string_value);
			break;
		case RX_BYTES_TYPE:
			rx_destory_bytes_value_struct(&who->bytes_value);
			break;
		case RX_STRUCT_TYPE:
			if (who->struct_value.size > 0)
			{
				for (size_t i = 0; i < who->struct_value.size; i++)
				{
					destroy_union_value(&who->struct_value.values[i].value, who->struct_value.values[i].value_type);
				}
				rx_heap_free(who->struct_value.values);
			}
			break;
		case RX_NODE_ID_TYPE:
			rx_destory_node_id(who->node_id_value);
			rx_heap_free(who->node_id_value);
			break;
		}
	}
	memzero(&who, sizeof(who));
}


RX_COMMON_API void rx_destroy_value(struct typed_value_type* val)
{
	destroy_union_value(&val->value, val->value_type);
}

RX_COMMON_API int rx_init_null_value(struct typed_value_type* val)
{
	memzero(val, sizeof(struct typed_value_type));
	return RX_OK;
}

RX_COMMON_API int rx_init_bool_value(struct typed_value_type* val, uint_fast8_t data)
{
	val->value.bool_value = data;
	val->value_type = RX_BOOL_TYPE;
	return RX_OK;
}

RX_COMMON_API int rx_init_int8_value(struct typed_value_type* val, int8_t data)
{
	val->value.int8_value = data;
	val->value_type = RX_INT8_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_uint8_value(struct typed_value_type* val, uint8_t data)
{
	val->value.uint8_value = data;
	val->value_type = RX_UINT8_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_int16_value(struct typed_value_type* val, int16_t data)
{
	val->value.int16_value = data;
	val->value_type = RX_INT16_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_uint16_value(struct typed_value_type* val, uint16_t data)
{
	val->value.uint16_value = data;
	val->value_type = RX_UINT16_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_int32_value(struct typed_value_type* val, int32_t data)
{
	val->value.int32_value = data;
	val->value_type = RX_INT32_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_uint32_value(struct typed_value_type* val, uint32_t data)
{
	val->value.uint32_value = data;
	val->value_type = RX_UINT32_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_int64_value(struct typed_value_type* val, int64_t data)
{
	val->value.int64_value = data;
	val->value_type = RX_INT64_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_uint64_value(struct typed_value_type* val, uint64_t data)
{
	val->value.uint64_value = data;
	val->value_type = RX_UINT64_TYPE;
	return RX_OK;
}

RX_COMMON_API int rx_init_float_value(struct typed_value_type* val, float data)
{
	val->value.float_value = data;
	val->value_type = RX_FLOAT_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_double_value(struct typed_value_type* val, double data)
{
	val->value.double_value = data;
	val->value_type = RX_DOUBLE_TYPE;
	return RX_OK;
}
RX_COMMON_API int rx_init_complex_value(struct typed_value_type* val, complex_value_struct data)
{
	val->value_type = RX_COMPLEX_TYPE;
#ifndef RX_VALUE_SIZE_16
	val->value.complex_value = rx_heap_alloc(sizeof(complex_value_struct));
	*val->value.complex_value = data;
#else
	val->value.double_value = data;
#endif
	return RX_OK;
}

RX_COMMON_API int rx_init_string_value(struct typed_value_type* val, const char* data, int count)
{
	val->value_type = RX_STRING_TYPE;
	rx_init_string_value_struct(&val->value.string_value, data, count);
	return RX_OK;
}
RX_COMMON_API int rx_init_bytes_value(struct typed_value_type* val, const uint8_t* data, size_t count)
{
	val->value_type = RX_BYTES_TYPE;
	rx_init_bytes_value_struct(&val->value.bytes_value, data, count);
	return RX_OK;
}

RX_COMMON_API int rx_init_uuid_value(struct typed_value_type* val, const rx_uuid_t* data)
{
	val->value_type = RX_UUID_TYPE;
#ifndef RX_VALUE_SIZE_16
	val->value.uuid_value = rx_heap_alloc(sizeof(rx_uuid_t));
	*val->value.uuid_value = *data;
#else
	val->value.double_value = data;
#endif
	return RX_OK;
}
RX_COMMON_API int rx_init_time_value(struct typed_value_type* val, const rx_time_struct data)
{
	val->value.time_value = data;
	val->value_type = RX_TIME_TYPE;
	return RX_OK;
}

RX_COMMON_API int rx_init_node_id_value(struct typed_value_type* val, const rx_node_id_struct* data)
{
	val->value_type = RX_NODE_ID_TYPE;
	val->value.node_id_value = rx_heap_alloc(sizeof(rx_node_id_struct));
	rx_copy_node_id(val->value.node_id_value, data);
	return RX_OK;
}


RX_COMMON_API int rx_init_string_array_value(struct typed_value_type* val, const char** data, size_t size)
{
	val->value_type = RX_STRING_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = size;
	if (size)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * size);
		for (int i = 0; i < size; i++)
		{
			rx_init_string_value_struct(&val->value.array_value.values[i].string_value, data[i], -1);
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_bytes_array_value(struct typed_value_type* val, const uint8_t** data, const size_t* sizes, size_t count)
{
	val->value_type = RX_BYTES_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			rx_init_bytes_value_struct(&val->value.array_value.values[i].bytes_value, data[i], sizes[i]);
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_uuid_array_value(struct typed_value_type* val, const rx_uuid_t* data, size_t count)
{
	val->value_type = RX_UUID_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
#ifndef RX_VALUE_SIZE_16
			val->value.array_value.values[i].uuid_value = rx_heap_alloc(sizeof(rx_uuid_t));
			*val->value.array_value.values[i].uuid_value = data[i];
#else
			val->value.array_value.values[i].uuid_value = data[i];
#endif
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_time_array_value(struct typed_value_type* val, const rx_time_struct* data, size_t count)
{
	val->value_type = RX_TIME_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].time_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_node_id_array_value(struct typed_value_type* val, const rx_node_id_struct* data, size_t count)
{
	val->value_type = RX_NODE_ID_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].node_id_value = rx_heap_alloc(sizeof(rx_node_id_struct));
			rx_copy_node_id(val->value.array_value.values[i].node_id_value, &data[i]);
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}


RX_COMMON_API int rx_init_bool_array_value(struct typed_value_type* val, const uint_fast8_t* data, size_t count)
{
	val->value_type = RX_BOOL_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].bool_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_struct_value(struct typed_value_type* val, const struct typed_value_type* data, size_t count)
{
	val->value_type = RX_STRUCT_TYPE;
	val->value.struct_value.size = count;
	if (count)
	{
		val->value.struct_value.values = rx_heap_alloc(sizeof(struct typed_value_type) * count);
		for (int i = 0; i < count; i++)
		{
			rx_copy_value(&val->value.struct_value.values[i], &data[i]);
		}
	}
	else
	{
		val->value.struct_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_array_value(struct typed_value_type* val, rx_value_t type, const union rx_value_union* data, size_t count)
{
	val->value_type = type | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			assign_value(&val->value.array_value.values[i], &data[i], type);
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_array_value_with_ptrs(struct typed_value_type* val, rx_value_t type, const union rx_value_union** data, size_t count)
{
	val->value_type = type | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			assign_value(&val->value.array_value.values[i], data[i], type);
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_struct_value_with_ptrs(struct typed_value_type* val, const struct typed_value_type** data, size_t count)
{
	val->value_type = RX_STRUCT_TYPE;
	val->value.struct_value.size = count;
	if (count)
	{
		val->value.struct_value.values = rx_heap_alloc(sizeof(struct typed_value_type) * count);
		for (int i = 0; i < count; i++)
		{
			rx_copy_value(&val->value.struct_value.values[i], data[i]);
		}
	}
	else
	{
		val->value.struct_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_int8_array_value(struct typed_value_type* val, const int8_t* data, size_t count)
{
	val->value_type = RX_INT8_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].int8_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_uint8_array_value(struct typed_value_type* val, const uint8_t* data, size_t count)
{
	val->value_type = RX_UINT8_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].uint8_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_int16_array_value(struct typed_value_type* val, const int16_t* data, size_t count)
{
	val->value_type = RX_INT16_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].int16_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_uint16_array_value(struct typed_value_type* val, const uint16_t* data, size_t count)
{
	val->value_type = RX_UINT16_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].uint16_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_int32_array_value(struct typed_value_type* val, const int32_t* data, size_t count)
{
	val->value_type = RX_INT32_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].int32_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_uint32_array_value(struct typed_value_type* val, const uint32_t* data, size_t count)
{
	val->value_type = RX_UINT32_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].int32_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_int64_array_value(struct typed_value_type* val, const int64_t* data, size_t count)
{
	val->value_type = RX_INT64_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].int64_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_uint64_array_value(struct typed_value_type* val, const uint64_t* data, size_t count)
{
	val->value_type = RX_UINT64_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].uint64_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_float_array_value(struct typed_value_type* val, const float* data, size_t count)
{
	val->value_type = RX_FLOAT_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].float_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_double_array_value(struct typed_value_type* val, const double* data, size_t count)
{
	val->value_type = RX_DOUBLE_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].double_value = data[i];
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}

RX_COMMON_API int rx_init_complex_array_value(struct typed_value_type* val, const complex_value_struct* data, size_t count)
{
	val->value_type = RX_COMPLEX_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
#ifndef RX_VALUE_SIZE_16
			val->value.array_value.values[i].complex_value = rx_heap_alloc(sizeof(complex_value_struct));
			*val->value.array_value.values[i].complex_value = data[i];
#else
			val->value.array_value.values[i].double_value = data;
#endif
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}




RX_COMMON_API void rx_assign_value(struct typed_value_type* val, const struct typed_value_type* right)
{
	val->value_type = right->value_type;
	assign_value(&val->value, &right->value, val->value_type);
}
RX_COMMON_API void rx_copy_value(struct typed_value_type* val, const struct typed_value_type* right)
{
	val->value_type = right->value_type;
	assign_value(&val->value, &right->value, val->value_type);
}
RX_COMMON_API void rx_move_value(struct typed_value_type* val, struct typed_value_type* right)
{
	// hard core move with memcpy and memzero
	memcpy(val, right, sizeof(struct typed_value_type));
	memzero(right, sizeof(struct typed_value_type));
}



RX_COMMON_API int rx_init_struct_array_value(struct typed_value_type* val, const struct_value_type* data, size_t count)
{
	val->value_type = RX_STRUCT_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].struct_value.values = rx_heap_alloc(sizeof(struct typed_value_type) * data[i].size);
			val->value.array_value.values[i].struct_value.size = data[i].size;
			for (size_t j = 0; j < data[i].size; j++)
			{
				val->value.array_value.values[i].struct_value.values[j].value_type = data[i].values[j].value_type;
				assign_value(&val->value.array_value.values[i].struct_value.values[j].value, &data[i].values[j].value, data[i].values[j].value_type);
			}
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}
RX_COMMON_API int rx_init_struct_array_value_with_ptrs(struct typed_value_type* val, const struct_value_type** data, size_t count)
{
	val->value_type = RX_STRUCT_TYPE | RX_ARRAY_VALUE_MASK;
	val->value.array_value.size = count;
	if (count)
	{
		val->value.array_value.values = rx_heap_alloc(sizeof(union rx_value_union) * count);
		for (int i = 0; i < count; i++)
		{
			val->value.array_value.values[i].struct_value.values = rx_heap_alloc(sizeof(struct typed_value_type) * data[i]->size);
			val->value.array_value.values[i].struct_value.size = data[i]->size;
			for (size_t j = 0; j < data[i]->size; j++)
			{
				val->value.array_value.values[i].struct_value.values[j].value_type = data[i]->values[j].value_type;
				assign_value(&val->value.array_value.values[i].struct_value.values[j].value, &data[i]->values[j].value, data[i]->values[j].value_type);
			}
		}
	}
	else
	{
		val->value.array_value.values = NULL;
	}
	return RX_OK;
}


RX_COMMON_API int rx_get_array_value(size_t index, struct typed_value_type* val, const struct typed_value_type* right)
{
	if (!rx_is_array_value(right))
		return RX_ERROR;
	size_t size = 0;
	if (RX_OK != rx_get_array_size(right, &size))
		return RX_ERROR;
	if (index >=size)
		return RX_ERROR;
	val->value_type = right->value_type&RX_STRIP_ARRAY_MASK;
	assign_value(&val->value, &right->value.array_value.values[index], right->value_type & RX_STRIP_ARRAY_MASK);

	return RX_OK;
}


RX_COMMON_API int rx_compare_values(const struct typed_value_type* val, const struct typed_value_type* right)
{
	int64_t temp64_1, temp64_2;
	uint64_t utemp64_1, utemp64_2;
	double tempd_1, tempd_2;
	if (rx_is_array_value(val) && rx_is_array_value(right))
	{// two arrays
		size_t count_val = 0;
		size_t count_right = 0;
		rx_get_array_size(val, &count_val);
		rx_get_array_size(right, &count_right);
		if (count_val == count_right)
		{// same array sizes
			if (is_simple_union_type(RX_SIMPLE_TYPE(val->value_type)) && is_simple_union_type(RX_SIMPLE_TYPE(right->value_type)))
			{
				if (is_numeric_union_type(RX_SIMPLE_TYPE(val->value_type)) && is_numeric_union_type(right->value_type & RX_STRIP_ARRAY_MASK))
				{
					if (is_integer_union_type(val->value_type & RX_STRIP_ARRAY_MASK) && is_integer_union_type(RX_SIMPLE_TYPE(right->value_type)))
					{
						for (size_t i = 0; i < count_right; i++)
						{
							rx_get_integer_value(val, i, &temp64_1, NULL);
							rx_get_integer_value(right, i, &temp64_2, NULL);

							if (temp64_1 == temp64_2)
								continue;
							else if (temp64_1 < temp64_2)
								return -1;
							else //if (temp64_1 > temp64_2)
								return 1;
						}
						return 0;
					}
					if (is_unassigned_union_type(RX_SIMPLE_TYPE(val->value_type)) && is_integer_union_type(RX_SIMPLE_TYPE(right->value_type)))
					{
						for (size_t i = 0; i < count_right; i++)
						{
							rx_get_unassigned_value(val, i, &utemp64_1, NULL);
							rx_get_unassigned_value(right, i, &utemp64_2, NULL);
							if (utemp64_1 == utemp64_2)
								continue;
							else if (utemp64_1 < utemp64_2)
								return -1;
							else //if (utemp64_1 > utemp64_2)
								return 1;
						}
						return 0;
					}
					else// is_float() || is_complex()
					{
						for (size_t i = 0; i < count_right; i++)
						{
							rx_get_float_value(val, i, &tempd_1, NULL);
							rx_get_float_value(right, i, &tempd_2, NULL);
							if (tempd_1 == tempd_2)
								continue;
							else if (tempd_1 < tempd_2)
								return -1;
							else //if (tempd_1 > tempd_2)
								return 1;
						}
						return 0;
					}
				}
				else
					return RX_SIMPLE_TYPE(val->value_type) - RX_SIMPLE_TYPE(right->value_type);
			}
			else if (RX_SIMPLE_TYPE(val->value_type) == RX_SIMPLE_TYPE(right->value_type))
			{
				if (RX_SIMPLE_TYPE(val->value_type) == RX_STRING_TYPE)
				{
					for (size_t i = 0; i < count_right; i++)
					{
						if (val->value.array_value.values[i].string_value.size == 0 && right->value.array_value.values[i].string_value.size == 0)
						{
							continue;
						}
						else if (val->value.array_value.values[i].string_value.size != 0 && right->value.array_value.values[i].string_value.size != 0)
						{
							int temp_ret = strcmp(rx_c_str(&val->value.array_value.values[i].string_value), rx_c_str(&right->value.array_value.values[i].string_value));
							if (temp_ret != 0)
								return temp_ret;
						}
						else // one of strings is not empty!!!
							return val->value.array_value.values[i].string_value.size == 0 ? -1 : 1;
					}
					return 0;
				}
				else if (RX_SIMPLE_TYPE(val->value_type) == RX_BYTES_TYPE)
				{
					for (size_t i = 0; i < count_right; i++)
					{
						if (val->value.array_value.values[i].bytes_value.size == 0 && right->value.array_value.values[i].bytes_value.size == 0)
						{
							continue;
						}
						else if (val->value.array_value.values[i].bytes_value.size == right->value.array_value.values[i].bytes_value.size)
						{

							size_t right_count = 0;
							const uint8_t* right_ptr = rx_c_ptr(&right->value.array_value.values[i].bytes_value, &right_count);
							size_t left_count = 0;
							const uint8_t* left_ptr = rx_c_ptr(&val->value.array_value.values[i].bytes_value, &left_count);
							RX_ASSERT(right_count == left_count);

							int temp_ret = memcmp(left_ptr, right_ptr, min(right_count, left_count));
							if (temp_ret != 0)
								return temp_ret;
						}
						else
						{
							if (val->value.array_value.values[i].bytes_value.size < right->value.array_value.values[i].bytes_value.size)
								return -1;
							else // if (val->value.bytes_value.size > right->value.string_value.size)
								return 1;
						}
					}
					return 0;
				}

				else if (RX_SIMPLE_TYPE(val->value_type) == RX_STRUCT_TYPE)
				{
					for (size_t i = 0; i < count_right; i++)
					{
						if (val->value.array_value.values[i].struct_value.size == 0 && right->value.array_value.values[i].struct_value.size == 0)
						{
							continue;
						}
						else if (val->value.array_value.values[i].struct_value.size == right->value.array_value.values[i].struct_value.size)
						{
							for (size_t j = 0; j < right->value.array_value.values[i].struct_value.size; j++)
							{
								int ret = rx_compare_values(&val->value.array_value.values[i].struct_value.values[j], &right->value.array_value.values[i].struct_value.values[j]);
								if (ret != 0)
									return ret;
							}
						}
						else
						{
							if (val->value.array_value.values[i].struct_value.size < right->value.array_value.values[i].struct_value.size)
								return -1;
							else // if (val->value.bytes_value.size > right->value.string_value.size)
								return 1;
						}
					}
					return 0;
				}

				else if (RX_SIMPLE_TYPE(val->value_type) == RX_TIME_TYPE)
				{
					for (size_t i = 0; i < count_right; i++)
					{
						if (val->value.array_value.values[i].time_value.t_value == right->value.array_value.values[i].time_value.t_value)
							continue;
						if (val->value.array_value.values[i].time_value.t_value < right->value.array_value.values[i].time_value.t_value)
							return -1;
						else
							return 1;
					}
					return 0;
				}
				else if (RX_SIMPLE_TYPE(val->value_type) == RX_UUID_TYPE)
				{
					for (size_t i = 0; i < count_right; i++)
					{
#ifndef RX_VALUE_SIZE_16
						int temp_ret = memcmp(val->value.array_value.values[i].uuid_value, right->value.array_value.values[i].uuid_value, sizeof(rx_uuid_t));
#else
						int temp_ret = memcmp(&val->value.array_value.values[i].uuid_value, &right->value.array_value.values[i].uuid_value, sizeof(rx_uuid_t));
#endif
						if (temp_ret != 0)
							return temp_ret;
					}
					return 0;
				}
				else if (RX_SIMPLE_TYPE(val->value_type) == RX_NODE_ID_TYPE)
				{
					for (size_t i = 0; i < count_right; i++)
					{
						int temp_ret = rx_compare_node_ids(val->value.array_value.values[i].node_id_value
							, right->value.array_value.values[i].node_id_value);
						if (temp_ret != 0)
							return temp_ret;
					}
					return 0;
				}
			}
			return RX_SIMPLE_TYPE(val->value_type) - RX_SIMPLE_TYPE(right->value_type);
		}
		else
		{
			return (int)count_val - (int)count_right;
		}
	}
	else if(!rx_is_array_value(val) && rx_is_array_value(right))
	{
		return -1;
	}
	else if (rx_is_array_value(val) && !rx_is_array_value(right))
	{
		return 1;
	}
	if (is_simple_union_type(val->value_type) && is_simple_union_type(right->value_type))
	{
		if (is_numeric_union_type(val->value_type) && is_numeric_union_type(right->value_type))
		{
			if (is_integer_union_type(val->value_type) && is_integer_union_type(right->value_type))
			{
				rx_get_integer_value(val, 0, &temp64_1, NULL);
				rx_get_integer_value(right, 0, &temp64_2, NULL);

				if (temp64_1 == temp64_2)
					return 0;
				else if (temp64_1 < temp64_2)
					return -1;
				else //if (temp64_1 > temp64_2)
					return 1;
			}
			if (is_unassigned_union_type(val->value_type) && is_integer_union_type(right->value_type))
			{
				rx_get_unassigned_value(val, 0, &utemp64_1, NULL);
				rx_get_unassigned_value(right, 0, &utemp64_2, NULL);
				if (utemp64_1 == utemp64_2)
					return 0;
				else if (utemp64_1 < utemp64_2)
					return -1;
				else //if (utemp64_1 > utemp64_2)
					return 1;
			}
			else// is_float() || is_complex()
			{
				rx_get_float_value(val, 0, &tempd_1, NULL);
				rx_get_float_value(right, 0, &tempd_2, NULL);
				if (tempd_1 == tempd_2)
					return 0;
				else if (tempd_1 < tempd_2)
					return -1;
				else //if (tempd_1 > tempd_2)
					return 1;
			}
		}
		else
			return val->value_type - right->value_type;
	}
	else if (val->value_type == right->value_type)
	{
		if (val->value_type == RX_STRING_TYPE)
		{
			if (val->value.string_value.size == 0 && right->value.string_value.size == 0)
				return 0;
			else if (val->value.string_value.size != 0 && right->value.string_value.size != 0)
				return strcmp(rx_c_str(&val->value.string_value), rx_c_str(&right->value.string_value));
			else // one of strings is not empty!!!
				return val->value.string_value.size == 0 ? -1 : 1;
		}
		else if (val->value_type == RX_BYTES_TYPE)
		{
			if (val->value.bytes_value.size == 0 && right->value.bytes_value.size == 0)
			{
				return 0;
			}
			else if (val->value.bytes_value.size == right->value.bytes_value.size)
			{
				size_t right_count = 0;
				const uint8_t* right_ptr = rx_c_ptr(&right->value.bytes_value, &right_count);
				size_t left_count = 0;
				const uint8_t* left_ptr = rx_c_ptr(&val->value.bytes_value, &left_count);
				RX_ASSERT(right_count == left_count);

				return memcmp(left_ptr, right_ptr, min(right_count, left_count));
			}
			else
			{
				if (val->value.bytes_value.size < right->value.bytes_value.size)
					return -1;
				else // if (val->value.bytes_value.size > right->value.string_value.size)
					return 1;
			}
		}

		else if (val->value_type == RX_STRUCT_TYPE)
		{
			if (val->value.struct_value.size == 0 && right->value.struct_value.size == 0)
			{
				return 0;
			}
			else if (val->value.struct_value.size == right->value.struct_value.size)
			{
				for (size_t i = 0; i < right->value.struct_value.size; i++)
				{
					int ret = rx_compare_values(&val->value.struct_value.values[i], &right->value.struct_value.values[i]);
					if (ret != 0)
						return ret;
				}
				return 0;
			}
			else
			{
				if (val->value.struct_value.size < right->value.struct_value.size)
					return -1;
				else // if (val->value.bytes_value.size > right->value.string_value.size)
					return 1;
			}
		}

		else if (val->value_type == RX_TIME_TYPE)
		{
			if (val->value.time_value.t_value == right->value.time_value.t_value)
				return 0;
			if (val->value.time_value.t_value < right->value.time_value.t_value)
				return -1;
			else
				return 1;
		}
		else if (val->value_type == RX_NODE_ID_TYPE)
		{
			return rx_compare_node_ids(val->value.node_id_value, right->value.node_id_value);
		}
		else if (val->value_type == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			return memcmp(val->value.uuid_value, right->value.uuid_value, sizeof(rx_uuid_t));
#else
			return memcmp(&val->value.uuid_value, &right->value.uuid_value, sizeof(rx_uuid_t));
#endif
		}
	}
	return val->value_type - right->value_type;
}


int get_float_value(const union rx_value_union* val, rx_value_t type, size_t idx, double* value, rx_value_t* ret_type)
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < val->array_value.size)
		{
			return get_float_value(&val->array_value.values[idx], RX_SIMPLE_TYPE(type), 0, value, ret_type);
		}
		else
		{
			return RX_ERROR;
		}
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			*value = 0;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_BOOL_TYPE:
			*value = val->bool_value ? 1 : 0;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_INT8_TYPE:
			*value = val->int8_value;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_UINT8_TYPE:
			*value = val->uint8_value;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_INT16_TYPE:
			*value = val->int16_value;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_UINT16_TYPE:
			*value = val->uint16_value;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_INT32_TYPE:
			*value = val->int32_value;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_UINT32_TYPE:
			*value = val->uint32_value;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_INT64_TYPE:
			*value = (double)val->int64_value;
			if (ret_type)
				*ret_type = RX_DOUBLE_TYPE;
			return RX_OK;
		case RX_UINT64_TYPE:
			*value = (double)val->uint64_value;
			if (ret_type)
				*ret_type = RX_DOUBLE_TYPE;
			return RX_OK;
		case RX_DOUBLE_TYPE:
			*value = val->double_value;
			if (ret_type)
				*ret_type = RX_DOUBLE_TYPE;
			return RX_OK;
		case RX_COMPLEX_TYPE:

#ifndef RX_VALUE_SIZE_16
			*value = complex_amplitude_helper(val->complex_value);
#else
			*value = complex_amplitude_helper(&val->complex_value);
#endif
			if (ret_type)
				*ret_type = RX_DOUBLE_TYPE;
			return RX_OK;
		case RX_FLOAT_TYPE:
			*value = val->float_value;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_OK;
		case RX_STRING_TYPE:
			*value = (double)val->string_value.size;
			if (ret_type)
				*ret_type = RX_DOUBLE_TYPE;
			return RX_OK;
		case RX_BYTES_TYPE:
			*value = (double)val->bytes_value.size;
			if (ret_type)
				*ret_type = RX_DOUBLE_TYPE;
			return RX_OK;
		case RX_UUID_TYPE:
			*value = 0;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_ERROR;
		case RX_TIME_TYPE:
			*value = (double)(val->time_value.t_value/1000ull);
			if (ret_type)
				*ret_type = RX_DOUBLE_TYPE;
			return RX_OK;
		case RX_STRUCT_TYPE:
			*value = 0;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_ERROR;
		default:
			*value = 0;
			if (ret_type)
				*ret_type = RX_FLOAT_TYPE;
			return RX_ERROR;
		}
	}
}

int get_integer_value(const union rx_value_union* val, rx_value_t type, size_t idx, int64_t* value, rx_value_t* ret_type)
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < val->array_value.size)
		{
			return get_integer_value(&val->array_value.values[idx], RX_SIMPLE_TYPE(type), 0, value, ret_type);
		}
		else
		{
			return RX_ERROR;
		}
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			if (ret_type)
				*ret_type = RX_INT8_TYPE;
			*value = 0;
			return RX_OK;
		case RX_BOOL_TYPE:
			*value = val->bool_value ? 1 : 0;
			if (ret_type)
				*ret_type = RX_INT8_TYPE;
			return RX_OK;
		case RX_INT8_TYPE:
			*value = val->int8_value;
			if (ret_type)
				*ret_type = RX_INT8_TYPE;
			return RX_OK;
		case RX_UINT8_TYPE:
			*value = val->uint8_value;
			if (ret_type)
				*ret_type = RX_INT16_TYPE;
			return RX_OK;
		case RX_INT16_TYPE:
			*value = val->int16_value;
			if (ret_type)
				*ret_type = RX_INT16_TYPE;
			return RX_OK;
		case RX_UINT16_TYPE:
			*value = val->uint16_value;
			if (ret_type)
				*ret_type = RX_INT32_TYPE;
			return RX_OK;
		case RX_INT32_TYPE:
			*value = val->int32_value;
			if (ret_type)
				*ret_type = RX_INT32_TYPE;
			return RX_OK;
		case RX_UINT32_TYPE:
			*value = val->uint32_value;
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_INT64_TYPE:
			*value = val->int64_value;
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_UINT64_TYPE:
			*value = (int64_t)val->uint64_value;
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_DOUBLE_TYPE:
			*value = (int64_t)val->double_value;
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_COMPLEX_TYPE:

#ifndef RX_VALUE_SIZE_16
			* value = (int64_t)sqrt(val->complex_value->real * val->complex_value->real + val->complex_value->imag * val->complex_value->imag);
#else
			* value = (int64_t)sqrt(val->complex_value.real * val->complex_value.real + val->complex_value.imag * val->complex_value.imag);
#endif
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_FLOAT_TYPE:
			*value = (int64_t)val->float_value;
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_STRING_TYPE:
			*value = (int64_t)val->string_value.size;
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_BYTES_TYPE:
			*value = (int64_t)val->bytes_value.size;
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_UUID_TYPE:
			*value = 0;
			if (ret_type)
				*ret_type = RX_INT8_TYPE;
			return RX_ERROR;
		case RX_TIME_TYPE:
			*value = (int64_t)(val->time_value.t_value / 1000ull);
			if (ret_type)
				*ret_type = RX_INT64_TYPE;
			return RX_OK;
		case RX_STRUCT_TYPE:
			*value = (int64_t)val->struct_value.size;
			if (ret_type)
				*ret_type = RX_INT8_TYPE;
			return RX_OK;
		default:
			*value = 0;
			if (ret_type)
				*ret_type = RX_INT8_TYPE;
			return RX_ERROR;
		}
	}
}

int get_unassigned_value(const union rx_value_union* val, rx_value_t type, size_t idx, uint64_t* value, rx_value_t* ret_type)
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < val->array_value.size)
		{
			return get_unassigned_value(&val->array_value.values[idx], RX_SIMPLE_TYPE(type), 0, value, ret_type);
		}
		else
		{
			return RX_ERROR;
		}
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			*value = 0;
			if (ret_type)
				*ret_type = RX_UINT8_TYPE;
			return RX_OK;
		case RX_BOOL_TYPE:
			*value = val->bool_value ? 1 : 0;
			if (ret_type)
				*ret_type = RX_UINT8_TYPE;
			return RX_OK;
		case RX_INT8_TYPE:
			*value = val->int8_value;
			if (ret_type)
				*ret_type = RX_UINT8_TYPE;
			return RX_OK;
		case RX_UINT8_TYPE:
			*value = val->uint8_value;
			if (ret_type)
				*ret_type = RX_UINT8_TYPE;
			return RX_OK;
		case RX_INT16_TYPE:
			*value = val->int16_value;
			if (ret_type)
				*ret_type = RX_UINT16_TYPE;
			return RX_OK;
		case RX_UINT16_TYPE:
			*value = val->uint16_value;
			if (ret_type)
				*ret_type = RX_UINT16_TYPE;
			return RX_OK;
		case RX_INT32_TYPE:
			*value = val->int32_value;
			if (ret_type)
				*ret_type = RX_UINT32_TYPE;
			return RX_OK;
		case RX_UINT32_TYPE:
			*value = val->uint32_value;
			if (ret_type)
				*ret_type = RX_UINT32_TYPE;
			return RX_OK;
		case RX_INT64_TYPE:
			*value = (uint64_t)val->int64_value;
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		case RX_UINT64_TYPE:
			*value = val->uint64_value;
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		case RX_DOUBLE_TYPE:
			*value = (uint64_t)val->double_value;
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		case RX_COMPLEX_TYPE:

#ifndef RX_VALUE_SIZE_16
			* value = (uint64_t)sqrt(val->complex_value->real * val->complex_value->real + val->complex_value->imag * val->complex_value->imag);
#else
			* value = (int64_t)sqrt(val->complex_value.real * val->complex_value.real + val->complex_value.imag * val->complex_value.imag);
#endif
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		case RX_FLOAT_TYPE:
			*value = (uint64_t)val->float_value;
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		case RX_STRING_TYPE:
			*value = (uint64_t)val->string_value.size;
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		case RX_BYTES_TYPE:
		case RX_STRUCT_TYPE:
			*value = (uint64_t)val->bytes_value.size;
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		case RX_UUID_TYPE:
			*value = 0;
			if (ret_type)
				*ret_type = RX_UINT8_TYPE;
			return RX_ERROR;
		case RX_TIME_TYPE:
			*value = (uint64_t)(val->time_value.t_value / 1000ull);
			if (ret_type)
				*ret_type = RX_UINT64_TYPE;
			return RX_OK;
		default:
			*value = 0;
			if (ret_type)
				*ret_type = RX_UINT8_TYPE;
			return RX_ERROR;
		}
	}
}
int get_complex_value(const union rx_value_union* val, rx_value_t type, size_t idx, complex_value_struct* value)
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < val->array_value.size)
		{
			return get_complex_value(&val->array_value.values[idx], RX_SIMPLE_TYPE(type), 0, value);
		}
		else
		{
			return RX_ERROR;
		}
	}
	else
	{
		if (type == RX_COMPLEX_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			* value = *val->complex_value;
#else
			* value = val->complex_value;
#endif
			return RX_OK;
		}
		else
		{
			value->imag = 0;
			return get_float_value(val, type, 0, &value->real, NULL);
		}
	}
} // complex

int get_string_value(const union rx_value_union* val, rx_value_t type, size_t idx, string_value_struct* value)
{
	if (type == RX_STRING_TYPE)
	{
		rx_copy_string_value(value, &val->string_value);
		return RX_OK;
	}
	else
	{
		if (IS_ARRAY_VALUE(type))
		{
			if (idx < val->array_value.size)
			{
				return get_string_value(&val->array_value.values[idx], RX_SIMPLE_TYPE(type), 0, value);
			}
			else
			{

				rx_packet_buffer buffer;
				rx_init_packet_buffer(&buffer, 0x100, 0x00);
				rx_push_to_packet(&buffer, "[", 1);
				int first = 1;
				for (size_t i = 0; i < val->array_value.size; i++)
				{
					string_value_struct temp;
					if (RX_OK == get_string_value(&val->array_value.values[i], RX_SIMPLE_TYPE(type), -1, &temp))
					{
						if (first)
							first = 0;
						else
							rx_push_to_packet(&buffer, ", ", 2);

						const char* temp_str = rx_c_str(&temp);
						if (temp_str == NULL)
							rx_push_to_packet(&buffer, "null", 4);
						else
							rx_push_to_packet(&buffer, temp_str, strlen(temp_str));
					}

				}
				rx_push_to_packet(&buffer, "]", 2);
				auto ret = rx_init_string_value_struct(value, (const char*)buffer.buffer_ptr, -1);
				rx_deinit_packet_buffer(&buffer);
				return ret;
			}
		}
		else
		{
			// this might be a static assert i don't know yet?
			// it should be checked before in this function in the code above
			if (type == RX_STRING_TYPE)
			{
				int ret = rx_copy_string_value(value, &val->string_value);
				return ret;
			}
			else
			{
				int ret = RX_ERROR;
				switch (type)
				{
				case RX_NULL_TYPE:
					ret = rx_init_string_value_struct(value, NULL, -1);
					return ret;
				case RX_BOOL_TYPE:
					ret = bool_to_str(val->bool_value, value);
					return ret;
				case RX_INT8_TYPE:
					ret = int8_to_str(val->int8_value, value);
					return ret;
				case RX_UINT8_TYPE:
					ret = uint8_to_str(val->uint8_value, value);
					return ret;
				case RX_INT16_TYPE:
					ret = int16_to_str(val->int16_value, value);
					return ret;
				case RX_UINT16_TYPE:
					ret = uint16_to_str(val->uint16_value, value);
					return ret;
				case RX_INT32_TYPE:
					ret = int32_to_str(val->int32_value, value);
					return ret;
				case RX_UINT32_TYPE:
					ret = uint32_to_str(val->uint32_value, value);
					return ret;
				case RX_INT64_TYPE:
					ret = int64_to_str(val->int64_value, value);
					return ret;
				case RX_UINT64_TYPE:
					ret = uint64_to_str(val->uint64_value, value);
					return ret;
				case RX_FLOAT_TYPE:
					ret = float_to_str(val->float_value, value);
					return ret;
				case RX_DOUBLE_TYPE:
					ret = double_to_str(val->double_value, value);
					return ret;
				case RX_COMPLEX_TYPE:

#ifndef RX_VALUE_SIZE_16
					ret = complex_to_str(val->complex_value, value);
#else
					ret = complex_to_str(&val->complex_value, value);
#endif
					return ret;

				case RX_STRING_TYPE:
					RX_ASSERT(0);
					return RX_ERROR;
				case RX_BYTES_TYPE:
					ret = bytes_to_str(&val->bytes_value, value);
					return ret;
				case RX_UUID_TYPE:
					{
						char uuid_buff[0x50];
#ifndef RX_VALUE_SIZE_16
						ret = rx_uuid_to_string(val->uuid_value, uuid_buff);
#else
						ret = rx_uuid_to_string(&val->uuid_value, uuid_buff);
#endif
						if (ret == RX_OK)
						{
							ret = rx_init_string_value_struct(value, uuid_buff, -1);
						}
					}
					return ret;
				case RX_TIME_TYPE:
					ret = time_to_ISO8601(val->time_value, value);
					return ret;
				case RX_STRUCT_TYPE:
					{
						rx_packet_buffer buffer;
						rx_init_packet_buffer(&buffer, 0x100, 0x00);
						rx_push_to_packet(&buffer, "{", 1);
						int first = 1;
						for (size_t i = 0; i < val->struct_value.size; i++)
						{
							string_value_struct temp;
							if (RX_OK == rx_get_string_value(&val->struct_value.values[i], -1, &temp))
							{
								if (first)
									first = 0;
								else
									rx_push_to_packet(&buffer, ", ", 2);

								const char* temp_str = rx_c_str(&temp);
								if(temp_str ==NULL)
									rx_push_to_packet(&buffer, "null", 4);
								else
									rx_push_to_packet(&buffer, temp_str, strlen(temp_str));
							}

						}
						rx_push_to_packet(&buffer, "}", 2);
						ret = rx_init_string_value_struct(value, (const char*)buffer.buffer_ptr, -1);
						rx_deinit_packet_buffer(&buffer);
					}
					return RX_OK;
				default:
					ret = rx_init_string_value_struct(value, NULL, -1);
					return ret;
				}
			}
		}
	}
}
RX_COMMON_API int rx_get_float_value(const struct typed_value_type* val, size_t idx, double* value, rx_value_t* type)
{
	return get_float_value(&val->value, val->value_type, idx, value, type);
}
RX_COMMON_API int rx_get_complex_value(const struct typed_value_type* val, size_t idx, complex_value_struct* value)
{
	return get_complex_value(&val->value, val->value_type, idx, value);
}
RX_COMMON_API int rx_get_integer_value(const struct typed_value_type* val, size_t idx, int64_t* value, rx_value_t* type)
{
	return get_integer_value(&val->value, val->value_type, idx, value, type);
}
RX_COMMON_API int rx_get_unassigned_value(const struct typed_value_type* val, size_t idx, uint64_t* value, rx_value_t* type)
{
	return get_unassigned_value(&val->value, val->value_type, idx, value, type);

}
RX_COMMON_API int rx_get_bool_value(const struct typed_value_type* val, size_t idx, int* value)
{
	int64_t temp_val;
	int ret = get_integer_value(&val->value, val->value_type, idx, &temp_val, NULL);
	*value = (temp_val != 0);
	return ret;
}
RX_COMMON_API int rx_get_string_value(const struct typed_value_type* val, size_t idx, string_value_struct* value)
{
	if (value == NULL)
	{
		RX_ASSERT(0);
		return RX_ERROR;
	}
	int ret = get_string_value(&val->value, val->value_type, idx, value);
	return ret;
}
RX_COMMON_API int rx_get_bytes_value(const struct typed_value_type* val, size_t idx, bytes_value_struct* value)
{
	if (value == NULL && val->value_type != RX_BYTES_TYPE)
	{
		RX_ASSERT(0);
		return RX_ERROR;
	}
	return rx_copy_bytes_value(value, &val->value.bytes_value);
}




