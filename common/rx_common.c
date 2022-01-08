

/****************************************************************************
*
*  common\rx_common.c
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


#include "pch.h"


// rx_common
#include "common/rx_common.h"


int parse_uint16(const char* str, uint16_t* val);
int parse_uint32(const char* str, uint32_t* val);

RX_COMMON_API int rx_init_string_value_struct(string_value_struct* data, const char* val, int count)
{
	if (val)
	{
		if (count < 0)
		{
			size_t len = strlen(val) + 1;
			data->size = len - 1;
			if (len <= sizeof(data->value))
			{// optimize string
				memcpy(&data->value, val, len);
			}
			else
			{
				data->value = malloc(len);
				if (data->value == NULL)
				{
					data->size = 0;
					return RX_ERROR;
				}
				memcpy(data->value, val, len);
			}
		}
		else
		{// this might be non null terminated string
			size_t len = count;
			int has_null = count > 0 && val[count - 1] == '\0';
			if (!has_null)
			{
				data->size = len;
				len++;
			}
			else
			{
				data->size = len - 1;
			}
			if (len <= sizeof(data->value))
			{// optimize string
				memcpy(&data->value, val, len);
				if(!has_null)
					((char*)&data->value)[len - 1] = '\0';
			}
			else
			{
				data->value = malloc(len);
				if (data->value == NULL)
				{
					data->size = 0;
					return RX_ERROR;
				}
				memcpy(data->value, val, len);
				if (!has_null)
					data->value[len - 1] = '\0';
			}
		}
	}
	else
	{
		memzero(data, sizeof(string_value_struct));
	}
	return RX_OK;
}
RX_COMMON_API const char* rx_c_str(const string_value_struct* data)
{
	if (data->size == 0)
		return NULL;
	else if (data->size + 1 <= sizeof(data->value)) // optimized string
		return (const char*)&data->value;
	else
		return data->value;
}


RX_COMMON_API int rx_copy_string_value(string_value_struct* dest, const string_value_struct* src)
{
	*dest = *src;
	if (dest->size > 0)
	{
		size_t len = dest->size + 1;
		if (len > sizeof(dest->value))
		{
			dest->value = malloc(len);
			if (dest->value)
				memcpy(dest->value, src->value, len);
			else
				return RX_ERROR;
		}
	}
	else
	{
		dest->value = NULL;
	}
	return RX_OK;
}
RX_COMMON_API void rx_destory_string_value_struct(string_value_struct* data)
{
	if (data->size > sizeof(data->value))
		free(data->value);
	memzero(data, sizeof(string_value_struct));
}
RX_COMMON_API int rx_init_bytes_value_struct(bytes_value_struct* data, const uint8_t* bytes, size_t len)
{
	if (bytes && len)
	{
		data->size = len;
		if (len < sizeof(data->value))
		{// optimize string
			memcpy(&data->value, bytes, len);
		}
		else
		{
			data->value = malloc(len);
			if (data->value == NULL)
			{
				data->size = 0;
				return RX_ERROR;
			}
			memcpy(data->value, bytes, len);
		}
	}
	else
	{
		memzero(data, sizeof(bytes_value_struct));
	}
	return RX_OK;
}


RX_COMMON_API int rx_copy_bytes_value(bytes_value_struct* dest, const bytes_value_struct* src)
{
	*dest = *src;
	if (dest->size > 0)
	{
		if (dest->size > sizeof(dest->value))
		{
			dest->value = malloc(src->size);
			if (dest->value == NULL)
			{
				dest->size = 0;
				return RX_ERROR;
			}
			memcpy(dest->value, src->value, dest->size);
		}
	}
	else
	{
		dest->value = NULL;
	}
	return RX_OK;
}

RX_COMMON_API const uint8_t* rx_c_ptr(const bytes_value_struct* data, size_t* size)
{
	*size = data->size;
	if (data->size == 0)
		return NULL;
	else if (data->size <= sizeof(data->value)) // optimized buffer
		return (const uint8_t*)&data->value;
	else
		return data->value;
}
RX_COMMON_API void rx_destory_bytes_value_struct(bytes_value_struct* data)
{
	if (data->size > sizeof(data->value))
		free(data->value);
	memzero(data, sizeof(bytes_value_struct));
}


RX_COMMON_API int rx_parse_value_type_name(const char* strtype, rx_value_t* type)
{

	if (strcmp(strtype, "null") == 0)
	{
		*type = RX_NULL_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "bit") == 0)
	{
		*type = RX_BOOL_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "int8") == 0)
	{
		*type = RX_INT8_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "uint8") == 0)
	{
		*type = RX_UINT8_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "int16") == 0)
	{
		*type = RX_INT16_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "uint16") == 0)
	{
		*type = RX_UINT16_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "int32") == 0)
	{
		*type = RX_INT32_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "uint32") == 0)
	{
		*type = RX_UINT32_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "int64") == 0)
	{
		*type = RX_INT64_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "uint64") == 0)
	{
		*type = RX_UINT64_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "float32") == 0)
	{
		*type = RX_FLOAT_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "float64") == 0)
	{
		*type = RX_DOUBLE_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "string") == 0)
	{
		*type = RX_STRING_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "time") == 0)
	{
		*type = RX_TIME_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "uuid") == 0)
	{
		*type = RX_UUID_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "nodeid") == 0)
	{
		*type = RX_NODE_ID_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "complex") == 0)
	{
		*type = RX_COMPLEX_TYPE;
		return RX_OK;
	}
	else if (strcmp(strtype, "bytes") == 0)
	{
		*type = RX_BYTES_TYPE;
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}
RX_COMMON_API const char* rx_get_value_type_name(rx_value_t type)
{
	switch (type)
	{
	case RX_NULL_TYPE:
		return "null";
	case RX_BOOL_TYPE:
		return "bit";
	case RX_INT8_TYPE:
		return "int8";
	case RX_UINT8_TYPE:
		return "uint8";
	case RX_INT16_TYPE:
		return "int16";
	case RX_UINT16_TYPE:
		return "uint16";
	case RX_INT32_TYPE:
		return "int32";
	case RX_UINT32_TYPE:
		return "uint32";
	case RX_INT64_TYPE:
		return "int64";
	case RX_UINT64_TYPE:
		return "uint64";
	case RX_FLOAT_TYPE:
		return "float32";
	case RX_DOUBLE_TYPE:
		return "float64";
	case RX_STRING_TYPE:
		return "string";
	case RX_TIME_TYPE:
		return "time";
	case RX_UUID_TYPE:
		return "uuid";
	case RX_BYTES_TYPE:
		return "bytes";
	case RX_NODE_ID_TYPE:
		return "nodeid";
	case RX_COMPLEX_TYPE:
		return "complex";
	default:
		return "internal error unknown type!!!";
	}
}
RX_COMMON_API int rx_is_null_uuid(const rx_uuid_t* val)
{
	uint8_t* data = (uint8_t*)val;// uuid have no align so this is legit!!!
	for (size_t i = 0; i < sizeof(rx_uuid_t); i++)
		if (data[i] != 0)
			return 0;
	return 1;
}

RX_COMMON_API int rx_init_null_node_id(rx_node_id_struct* data)
{
	memzero(data, sizeof(rx_node_id_struct));
	return RX_OK;
}
RX_COMMON_API int rx_init_int_node_id(rx_node_id_struct* data, uint32_t id, uint16_t namesp)
{
	data->node_type = rx_node_id_numeric;
	data->namespace_index = id ? namesp : 0;
	data->value.int_value = id;
	return RX_OK;
}
RX_COMMON_API int rx_init_string_node_id(rx_node_id_struct* data, const char* id, int count, uint16_t namesp)
{
	data->node_type = rx_node_id_string;
	data->namespace_index = namesp;
	return rx_init_string_value_struct(&data->value.string_value, id, count);
}
RX_COMMON_API int rx_init_uuid_node_id(rx_node_id_struct* data, const rx_uuid_t* id, uint16_t namesp)
{
	data->node_type = rx_node_id_uuid;
	data->namespace_index = namesp;
	data->value.uuid_value = *id;
	return RX_OK;
}
RX_COMMON_API int rx_init_bytes_node_id(rx_node_id_struct* data, const uint8_t* id, size_t len, uint16_t namesp)
{
	data->node_type = rx_node_id_bytes;
	data->namespace_index = namesp;
	return rx_init_bytes_value_struct(&data->value.bstring_value, id, len);

}
RX_COMMON_API int rx_copy_node_id(rx_node_id_struct* data, const rx_node_id_struct* src)
{
	memcpy(data, src, sizeof(rx_node_id_struct));
	// handle the types that are not plain copy
	switch (src->node_type)
	{
	case rx_node_id_string:
		return rx_init_string_value_struct(&data->value.string_value, rx_c_str(&src->value.string_value), -1);
	case rx_node_id_bytes:
		{
			size_t count = 0;
			const uint8_t* ptr = rx_c_ptr(&src->value.bstring_value, &count);
			return rx_init_bytes_value_struct(&data->value.bstring_value, ptr, count);
		}
    default:;
	}
	return RX_OK;
}
RX_COMMON_API int rx_move_node_id(rx_node_id_struct* data, rx_node_id_struct* src)
{
	memcpy(data, src, sizeof(rx_node_id_struct));
	memzero(src, sizeof(rx_node_id_struct));
	return RX_OK;
}

RX_COMMON_API int rx_compare_node_ids(const rx_node_id_struct* left, const rx_node_id_struct* right)
{
	int left_null = rx_is_null_node_id(left);
	int right_null = rx_is_null_node_id(right);
	if (left_null && right_null)
	{
		return 0;
	}
	else if (left_null && !right_null)
	{
		return -1;
	}
	else if (!left_null && right_null)
	{
		return 1;
	}
	if (left->namespace_index != right->namespace_index)
	{
		// unassigned, be careful
		if (left->namespace_index < right->namespace_index)
			return -1;
		else // (left->namespace_index > right->namespace_index) has to be!!!
			return 1;
	}
	if (left->node_type != right->node_type)
	{
		// signed so cool
		return left->node_type - right->node_type;
	}
	// at this point we know that namespace indexes are the same and types are the same
	switch (left->node_type)
	{
	case rx_node_id_numeric:
		{
			// unassigned, be careful
			if (left->value.int_value == right->value.int_value)
				return 0;
			else if (left->value.int_value < right->value.int_value)
				return -1;
			else // (left->namespace_index > right->namespace_index) has to be!!!
				return 1;
		}
	case rx_node_id_uuid:
		return memcmp(&left->value.uuid_value, &right->value.uuid_value, sizeof(uuid_t));
	case rx_node_id_string:
		{
			const char* str_left = rx_c_str(&left->value.string_value);
			const char* str_right = rx_c_str(&right->value.string_value);

			if (str_left && str_right)
			{// both nonzero
				return strcmp(str_left, str_right);
			}
			else if (str_left)
			{// right is zero
				return 1;
			}
			else if (str_right)
			{// left is zero
				return -1;
			}
			else
			{// both are zero, so equal
				return 0;
			}
		}
	case rx_node_id_bytes:
		{
			size_t left_size = 0;
			size_t right_size = 0;
			const uint8_t* left_ptr = rx_c_ptr(&left->value.bstring_value, &left_size);
			const uint8_t* right_ptr = rx_c_ptr(&right->value.bstring_value, &right_size);

			if (left_size && right_size)
			{// both nonzero
				if (left_size == right_size)
				{
					return memcmp(left_ptr, right_ptr, left_size);
				}
				else
				{// unassigned be careful
					if (left_size < right_size)
						return -1;
					else // (left_size > right_size) has to be!!!
						return 1;
				}
			}
			else if (left_size)
			{// right is zero
				return 1;
			}
			else if (right_size)
			{// left is zero
				return -1;
			}
			else
			{// both are zero, so equal
				return 0;
			}
		}
	default:
		RX_ASSERT(0);
		return 0;
	}
}
RX_COMMON_API int rx_is_null_node_id(const rx_node_id_struct* data)
{
	return data->node_type == rx_node_id_numeric
		&& data->namespace_index == 0 // does this matter, i guess not!!!
		&& data->value.int_value == 0;
}

size_t get_node_id_string_size(const rx_node_id_struct* data)
{
	if (rx_is_null_node_id(data))
	{
		return 0;
	}
	size_t buffer_size = 3;// type + ':' + '\0'
	// calculate total buffer size
	switch (data->node_type)
	{
	case rx_node_id_numeric:
		buffer_size += 0x10;
		break;
	case rx_node_id_string:
		buffer_size += data->value.string_value.size + 1;
		break;
	case rx_node_id_uuid:
		buffer_size += 40;// uuid string size
		break;
	case rx_node_id_bytes:
		buffer_size += data->value.string_value.size * 2 + 1;
		break;
	}
	if (data->namespace_index != DEFAULT_NAMESPACE)
	{
		buffer_size += 7;// namespace + ':'
	}
	return buffer_size;
}

int internal_node_id_to_string(const rx_node_id_struct* data, char* buffer)
{
	int ret_value = RX_OK;
	switch (data->node_type)
	{
	case rx_node_id_numeric:
		{
			int ret;
			if(data->namespace_index== DEFAULT_NAMESPACE)
				ret = sprintf(buffer, "i:%u", data->value.int_value);
			else
				ret = sprintf(buffer, "%d:i:%u", (int)data->namespace_index, data->value.int_value);
			ret_value = ret > 0 ? RX_OK : RX_ERROR;
		}
		break;
	case rx_node_id_string:
		{
			const char* str = rx_c_str(&data->value.string_value);
			int ret = -1;
			if (data->namespace_index == DEFAULT_NAMESPACE)
			{
				if (str)
					ret = sprintf(buffer, "s:%s", str);
				else
					ret = sprintf(buffer, "s:");
			}
			else
			{
				if (str)
					ret = sprintf(buffer, "%d:s:%s", (int)data->namespace_index, str);
				else
					ret = sprintf(buffer, "%d:s:", (int)data->namespace_index);
			}
			ret_value = ret > 0 ? RX_OK : RX_ERROR;
		}
		break;
	case rx_node_id_uuid:
		{
			int ret;
			if (data->namespace_index == DEFAULT_NAMESPACE)
				ret = sprintf(buffer, "g:");
			else
				ret = sprintf(buffer, "%d:g:", (int)data->namespace_index);
			if (ret <= 0)
				return RX_ERROR;
			size_t idx = ret;
			ret_value = rx_uuid_to_string(&data->value.uuid_value, &buffer[idx]);
		}
		break;
	case rx_node_id_bytes:
		{
			int ret;
			if (data->namespace_index == DEFAULT_NAMESPACE)
				ret = sprintf(buffer, "b:");
			else
				ret = sprintf(buffer, "%d:b:", (int)data->namespace_index);
			if (ret <= 0)
				return RX_ERROR;
			size_t idx = ret;
			for (size_t i = 0; i < data->value.bstring_value.size; i++)
			{
				ret = sprintf(&buffer[idx], "%02X", (int)data->value.bstring_value.value[i]);
				if (ret <= 0)
				{
					ret_value = RX_ERROR;
					break;
				}
				idx += ret;
			}
		}
		break;
	}
	return ret_value;
}

RX_COMMON_API int rx_node_id_to_string(const rx_node_id_struct* data, string_value_struct* str)
{
	if (rx_is_null_node_id(data))
	{
		return rx_init_string_value_struct(str, NULL, -1);
	}
	int ret_value = RX_OK;
	char static_buff[0x10];
	int allocated = 0;
	char* buffer = NULL;
	size_t buffer_size = get_node_id_string_size(data);
	if (buffer_size == 0)
	{
		return RX_ERROR;
	}
	
	// now we have the size, check to see for allocation
	if (buffer_size > sizeof(static_buff) / sizeof(static_buff[0]))
	{
		buffer = malloc(buffer_size);
		if (buffer == NULL)
			return RX_ERROR;
		allocated = 1;
	}
	else
	{
		buffer = static_buff;
	}
	// now create the string
	ret_value = internal_node_id_to_string(data, buffer);

	if (ret_value)
	{
		ret_value = rx_init_string_value_struct(str, buffer, -1);
	}

	if (allocated)
		free(buffer);

	return ret_value;
}

int internal_node_id_from_string(rx_node_id_struct* data, const char* str)
{
	char temp_buffer[0x20];
	if (str == NULL)
		return RX_ERROR;
	size_t len = strlen(str);
	if (len < 2)
		return RX_ERROR;
	size_t idx = 0;
	uint16_t namespace_index = DEFAULT_NAMESPACE;
	if (str[0] != 'i' && str[0] != 's' && str[0] != 'g' && str[0] != 'b')
	{
		for (; idx < len; idx++)
		{
			if (str[idx] == ':')
			{// we found first separator
				if (idx > 8)
					return RX_ERROR;
				memcpy(temp_buffer, str, idx);
				temp_buffer[idx] = '\0';
				break;
			}
		}
		if (idx == len)
			return RX_ERROR;

		if (!parse_uint16(temp_buffer, &namespace_index))
			return RX_ERROR;
		idx++;
	}
	if (idx == len)
		return RX_ERROR;
	char type = str[idx];
	idx++;
	if (idx == len || str[idx] != ':')
		return RX_ERROR;
	idx++;
	if (idx == len)
		return RX_ERROR;
	switch (type)
	{
	case 'i':
		{// integer
			uint32_t val;
			if (!parse_uint32(&str[idx], &val))
				return RX_ERROR;
			return rx_init_int_node_id(data, val, namespace_index);
		}
		break;
	case 's':
		{
			return rx_init_string_node_id(data, &str[idx], -1, namespace_index);
		}
		break;
	case 'g':
		{
			rx_uuid_t temp_uuid;
			int ret = rx_string_to_uuid(&str[idx], &temp_uuid);
			if (ret)
			{
				ret = rx_init_uuid_node_id(data, &temp_uuid, namespace_index);
			}
			return ret;
		}
		break;
	case 'b':
		// TODO parsing byte array
	default:
		RX_ASSERT(0);
		return RX_ERROR;
	}
}

RX_COMMON_API int rx_node_id_from_string(rx_node_id_struct* data, const char* str)
{
	if (str == NULL || *str == '\0')
		return rx_init_null_node_id(data);
	else
		return internal_node_id_from_string(data, str);
}

RX_COMMON_API void rx_destory_node_id(rx_node_id_struct* data)
{
	if (data->node_type == rx_node_id_string)
		rx_destory_string_value_struct(&data->value.string_value);
	if (data->node_type == rx_node_id_bytes)
		rx_destory_bytes_value_struct(&data->value.bstring_value);
}

/*
union rx_reference_data
{
	string_value_struct path;
	rx_node_id_struct id;
};

typedef struct rx_reference_struct_t
{
	int is_id;
	union rx_reference_data data;
} rx_reference_struct;
*/


RX_COMMON_API int rx_init_null_reference(rx_reference_struct* ref)
{
	memzero(ref, sizeof(rx_reference_struct));
	return RX_OK;
}
RX_COMMON_API int rx_init_path_reference(rx_reference_struct* ref, const char* path, int count)
{
	ref->is_path = 1;
	return rx_init_string_value_struct(&ref->data.path, path, count);
}
RX_COMMON_API int rx_init_id_reference(rx_reference_struct* ref, const rx_node_id_struct* id)
{
	ref->is_path = 0;
	return rx_copy_node_id(&ref->data.id, id);
}
RX_COMMON_API int rx_copy_reference(rx_reference_struct* ref, const rx_reference_struct* src)
{
	memcpy(ref, src, sizeof(rx_reference_struct));
	if (src->is_path)
	{
		return rx_init_string_value_struct(&ref->data.path, rx_c_str(&src->data.path), -1);
	}
	else
	{
		return rx_copy_node_id(&ref->data.id, &src->data.id);
	}
}
RX_COMMON_API int rx_move_reference(rx_reference_struct* ref, rx_reference_struct* src)
{
	memcpy(ref, src, sizeof(rx_reference_struct));
	memzero(src, sizeof(rx_reference_struct));
	return RX_OK;
}


RX_COMMON_API int rx_reference_to_string(const rx_reference_struct* data, string_value_struct* str)
{
	if (data->is_path)
	{
		return rx_init_string_value_struct(str, rx_c_str(&data->data.path), -1);
	}
	else
	{
		char static_buff[0x48];
		int allocated = 0;
		char* buffer = NULL;
		int ret_value = RX_OK;

		size_t buffer_size = get_node_id_string_size(&data->data.id);
		if (buffer_size == 0)
		{
			return rx_init_string_value_struct(str, NULL, -1);
		}
		buffer_size += 3;// place for "id:"
		// now we have the size, check to see for allocation
		if (buffer_size > sizeof(static_buff) / sizeof(static_buff[0]))
		{
			buffer = malloc(buffer_size);
			if (buffer == NULL)
				return RX_ERROR;
			allocated = 1;
		}
		else
		{
			buffer = static_buff;
		}
		// now create the string
		ret_value = internal_node_id_to_string(&data->data.id, &buffer[3]);

		if (ret_value)
		{
			ret_value = rx_init_string_value_struct(str, &buffer[3], -1);
			if (ret_value)
				memcpy(buffer, "id:", 3);
		}

		if (allocated)
			free(buffer);

		return ret_value;
	}
}
RX_COMMON_API int rx_reference_from_string(rx_reference_struct* data, const char* str)
{
	if (str == NULL)
		return rx_init_null_reference(data);
	if (str[0] == 'i' && str[1] == 'd' && str[2] == ':') // no need for testing string size it is null terminated!!!
	{// id stuff
		data->is_path = 0;
		return internal_node_id_from_string(&data->data.id, &str[3]);
	}
	else
	{// string stuff
		data->is_path = 1;
		return rx_init_string_value_struct(&data->data.path, &str[3], -1);
	}
}

RX_COMMON_API void rx_deinit_reference(rx_reference_struct* ref)
{
	if (ref->is_path)
	{
		rx_destory_string_value_struct(&ref->data.path);
	}
	else
	{
		rx_destory_node_id(&ref->data.id);
	}
}
RX_COMMON_API int rx_compare_references(const rx_reference_struct* left, const rx_reference_struct* right)
{
	if (left->is_path && right->is_path)
	{// both are strings
		const char* str_left = rx_c_str(&left->data.path);
		const char* str_right = rx_c_str(&right->data.path);

		if (str_left && str_right)
		{// both nonzero
			return strcmp(str_left, str_right);
		}
		else if (str_left)
		{// right is zero
			return 1;
		}
		else if (str_right)
		{// left is zero
			return -1;
		}
		else
		{// both are zero, so equal
			return 0;
		}
	}
	else if (!left->is_path && !right->is_path)
	{
		return rx_compare_node_ids(&left->data.id, &right->data.id);
	}
	else if (left->is_path) // && !right->is_path
	{
		return 1;
	}
	else //!left->is_path && right->is_path
	{
		return -1;
	}
}
RX_COMMON_API int rx_is_null_reference(const rx_reference_struct* ref)
{
	return !ref->is_path && rx_is_null_node_id(&ref->data.id);
}



