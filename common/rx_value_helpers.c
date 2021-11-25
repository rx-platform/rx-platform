

/****************************************************************************
*
*  common\rx_value_helpers.c
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

#include "rx_common.h"
#include <math.h>


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
			free(who->array_value.values);
		}
	}
	else
	{//simple union stuff
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
			free(who->complex_value);
			break;
		case RX_UUID_TYPE:
			free(who->uuid_value);
			break;
#endif
		case RX_STRING_TYPE:
			rx_destory_string_value_struct(&who->string_value);
			break;
		case RX_BYTES_TYPE:
			rx_destory_bytes_value_struct(&who->bytes_value);
			break;
		case RX_NODE_ID_TYPE:
			rx_destory_node_id(who->node_id_value);
			free(who->node_id_value);
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
	val->value.complex_value = malloc(sizeof(complex_value_struct));
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
	val->value.uuid_value = malloc(sizeof(rx_uuid_t));
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
	val->value.node_id_value = malloc(sizeof(rx_node_id_struct));
	rx_copy_node_id(val->value.node_id_value, data);
	return RX_OK;
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
			left->array_value.values = malloc(sizeof(union rx_value_union) * count);
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
			left->complex_value = malloc(sizeof(complex_value_struct));
			*left->complex_value = *right->complex_value;
			break;
		case RX_UUID_TYPE:
			left->uuid_value = malloc(sizeof(rx_uuid_t));
			*left->uuid_value = *right->uuid_value;
			break;
#endif
		case RX_STRING_TYPE:
			rx_copy_string_value(&left->string_value, &right->string_value);
			break;
		case RX_BYTES_TYPE:
			rx_copy_bytes_value(&left->bytes_value, &right->bytes_value);
			break;
		case RX_NODE_ID_TYPE:
			left->node_id_value = malloc(sizeof(rx_node_id_struct));
			rx_copy_node_id(left->node_id_value, right->node_id_value);
			break;
		}
	}
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



RX_COMMON_API int rx_compare_values(const struct typed_value_type* val, const struct typed_value_type* right)
{
	int64_t temp64_1, temp64_2;
	uint64_t utemp64_1, utemp64_2;
	double tempd_1, tempd_2;
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
			if (val->value.string_value.size == 0 && right->value.string_value.size == 0)
			{
				return 0;
			}
			else if (val->value.bytes_value.size == right->value.bytes_value.size)
			{
				return memcmp(val->value.bytes_value.value, right->value.bytes_value.value, val->value.bytes_value.size);
			}
			else
			{
				if (val->value.bytes_value.size < right->value.string_value.size)
					return -1;
				else // if (val->value.bytes_value.size > right->value.string_value.size)
					return 1;
			}
		}
		else if (val->value_type == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			return memcmp(val->value.uuid_value, right->value.uuid_value, sizeof(rx_uuid_t)) == 0;
#else
			return memcmp(&val->value.uuid_value, &right->value.uuid_value, sizeof(rx_uuid_t)) == 0;
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
				return RX_ERROR;
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
					RX_ASSERT(0);
					ret = rx_init_string_value_struct(value, NULL, -1);
					return RX_OK;
				case RX_UUID_TYPE:
					RX_ASSERT(0);// this should be done !!!
					ret = rx_init_string_value_struct(value, NULL, -1);
					return RX_OK;
				case RX_TIME_TYPE:
					ret = time_to_ISO8601(val->time_value, value);
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




