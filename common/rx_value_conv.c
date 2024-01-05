

/****************************************************************************
*
*  common\rx_value_conv.c
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



int is_delim(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

double complex_amplitude_helper(const complex_value_struct* val);
void destroy_union_value(union rx_value_union* who, rx_value_t type);

int strmultiequal(const char* str, const char* vals[], size_t vals_count, int* help_buffer)
{
	size_t i;
	size_t idx = 0;
	int allocated = 0;
	int has_one;
	int done = 0;
	int static_buff[0x10];
	int* not_equal = static_buff;
	if (help_buffer)
	{
		not_equal = help_buffer;
	}
	else if (vals_count > sizeof(static_buff) / sizeof(static_buff[0]))
	{
		allocated = 1;
		not_equal = malloc(sizeof(int) * vals_count);
	}
	memzero(not_equal, vals_count * sizeof(int));

	do
	{
		if (*str == '\0')
			done = 1;
		// this will explicitly include null termination
		// thus do,while loop and done flag
		has_one = 0;
		for (i = 0; i < vals_count; i++)
		{
			if (not_equal[i] == 0)
			{
				// if str is greater then vals[i]
				// null termination will exclude vals[i]!!!
				if (vals[i][idx] != *str)
				{
					not_equal[i] = 1;
				}
				else
				{
					has_one = 1;
				}
			}
		}
		if (!has_one)
			break;
		idx++;
		str++;

	} while (!done);

	if (allocated)
		free(not_equal);

	return has_one;
}

const char* true_string = "true";
const char* false_string = "false";

const char* true_strings[] = {
	"1", "true", "True", "T", "TRUE", "On", "ON", "on"
};

const char* false_strings[] = {
	"0", "false", "False", "F", "FALSE", "Off", "OFF", "off"
};

const char* skip_delims(const char* in)
{
	while (is_delim(*in))
		in++;
	return in;
}

int check_parse_end(const char* end)
{
	return end == NULL || is_delim(*end) || *end == '\0';
}

int parse_bool(const char* str, uint_fast8_t* val)
{
	str = skip_delims(str);
	if (strmultiequal(str, true_strings, sizeof(true_strings) / sizeof(true_strings[0]), NULL))
	{
		*val = 1;
		return RX_OK;
	}
	if (strmultiequal(str, true_strings, sizeof(false_strings) / sizeof(false_strings[0]), NULL))
	{
		*val = 0;
		return RX_OK;
	}
	return RX_ERROR;
}
int parse_int8(const char* str, int8_t* val)
{
	str = skip_delims(str);
	int8_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	long long valc = strtoll(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (int8_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_int16(const char* str, int16_t* val)
{
	str = skip_delims(str);
	int16_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	long long valc = strtoll(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (int16_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_int32(const char* str, int32_t* val)
{
	str = skip_delims(str);
	int32_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	long long valc = strtoll(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (int32_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_int64(const char* str, int64_t* val)
{
	str = skip_delims(str);
	int64_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	long long valc = strtoll(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (int64_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_uint8(const char* str, uint8_t* val)
{
	str = skip_delims(str);
	uint8_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	unsigned long long valc = strtoull(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (uint8_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_uint16(const char* str, uint16_t* val)
{
	str = skip_delims(str);
	uint16_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	unsigned long long valc = strtoull(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (uint16_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_uint32(const char* str, uint32_t* val)
{
	str = skip_delims(str);
	uint32_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	unsigned long long valc = strtoull(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (uint32_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_uint64(const char* str, uint64_t* val)
{
	str = skip_delims(str);
	uint64_t temp = 0;
	errno = 0;
	char* endptr = NULL;
	unsigned long long valc = strtoull(str, &endptr, 10);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	temp = (uint64_t)valc;
	*val = temp;
	return RX_OK;
}
int parse_double(const char* str, double* val)
{
	str = skip_delims(str);
	errno = 0;
	char* endptr = NULL;
	double temp = strtod(str, &endptr);
	if (!check_parse_end(endptr) || errno != 0)
		return RX_ERROR;
	*val = temp;
	return RX_OK;
}
int parse_complex(const char* str, complex_value_struct* val)
{
	int ret = parse_double(str, &val->real);
	if (ret)
		val->imag = 0;
	return ret;
}

int parse_bytes(const char* str, bytes_value_struct* val)
{
	char static_buff[0x40];
	char* pbuffer = static_buff;

	if (str == NULL || *str == '\0')
		return rx_init_bytes_value_struct(val, NULL, 0);
	size_t len = strlen(str);

	if ((len & 0x1) != 0)
		return RX_ERROR;// have to be event characters

	size_t array_size = len / 2;;

	if (array_size > sizeof(static_buff) / sizeof(static_buff[0]))
	{
		pbuffer = malloc(array_size);
		if (pbuffer == NULL)
			return RX_ERROR;
	}

	for (size_t i = 0; i < len; i += 2)
	{
		uint8_t temp = 0x00;

		if (str[i] >= '0' && str[i] <= '9')
			temp = temp + str[i] - '0';
		else if (str[i] >= 'A' || str[i] <= 'F')
			temp = temp + str[i] - 'A' + 0xa;
		else if (str[i] >= 'a' || str[i] <= 'f')
			temp = temp + str[i] - 'A' + 0xa;
		else
			return RX_ERROR;
		temp <<= 4;

		if (str[i + 1] >= '0' && str[i + 1] <= '9')
			temp = temp + str[i + 1] - '0';
		else if (str[i + 1] >= 'A' || str[i + 1] <= 'F')
			temp = temp + str[i + 1] - 'A' + 0xa;
		else if (str[i + 1] >= 'a' || str[i + 1] <= 'f')
			temp = temp + str[i + 1] - 'A' + 0xa;
		else
			return RX_ERROR;

		pbuffer[len / 2] = temp;
	}
	int ret = rx_init_bytes_value_struct(val, (uint8_t*)pbuffer, array_size);
	if (array_size > sizeof(static_buff) / sizeof(static_buff[0]))
		free(pbuffer);
	return ret;
}
int parse_time_from_ISO8601(const char* str, rx_time_struct* val)
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
		return RX_ERROR;

	if (*ptr != L'\0')
	{
		if (*ptr != L'T')
			return RX_ERROR;
		ptr++;

		if (sscanf(ptr, "%2u:%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 5;
		else if (sscanf(ptr, "%2u%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 4;
		else
			return RX_ERROR;

		if (*ptr == L':')
			ptr++;

		if (*ptr != L'\0')
		{// seconds too
			if (sscanf(ptr, "%2u", &os_time.second) == 1)
				ptr += 2;
			else
				return RX_ERROR;
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
		*val = ts;
		return RX_OK;
	}
	else
		return RX_ERROR;
}


int bool_to_str(uint_fast8_t val, string_value_struct* str)
{
	return rx_init_string_value_struct(str
		, val != 0 ? true_string : false_string, -1);
}

int int8_to_str(int8_t val, string_value_struct* str)
{
	char buff[0x10];
	sprintf(buff, "%d", (int)val);
	return rx_init_string_value_struct(str, buff, -1);
}
int int16_to_str(int16_t val, string_value_struct* str)
{
	char buff[0x10];
	sprintf(buff, "%d", (int)val);
	return rx_init_string_value_struct(str, buff, -1);
}
int int32_to_str(int32_t val, string_value_struct* str)
{
	char buff[0x10];
	sprintf(buff, "%d", val);
	return rx_init_string_value_struct(str, buff, -1);
}
int int64_to_str(int64_t val, string_value_struct* str)
{
	char buff[0x20];
	sprintf(buff, "%" PRIi64, val);
	return rx_init_string_value_struct(str, buff, -1);
}

int uint8_to_str(uint8_t val, string_value_struct* str)
{
	char buff[0x10];
	sprintf(buff, "%u", (int)val);
	return rx_init_string_value_struct(str, buff, -1);
}
int uint16_to_str(uint16_t val, string_value_struct* str)
{
	char buff[0x10];
	sprintf(buff, "%u", (int)val);
	return rx_init_string_value_struct(str, buff, -1);
}
int uint32_to_str(uint32_t val, string_value_struct* str)
{
	char buff[0x10];
	sprintf(buff, "%u", val);
	return rx_init_string_value_struct(str, buff, -1);
}
int uint64_to_str(uint64_t val, string_value_struct* str)
{
	char buff[0x20];
	sprintf(buff, "%"  PRIu64, val);
	return rx_init_string_value_struct(str, buff, -1);
}

int float_to_str(float val, string_value_struct* str)
{
	char buff[0x200];
	gcvt(val, 200, buff);
	return rx_init_string_value_struct(str, buff, -1);
}

int double_to_str(double val, string_value_struct* str)
{
	char buff[0x200];
	gcvt(val, 200, buff);
	return rx_init_string_value_struct(str, buff, -1);
}

int complex_to_str(complex_value_struct* val, string_value_struct* str)
{
	char buff[0x200];
	sprintf(buff, "%g+%gi", val->real, val->imag);
	return rx_init_string_value_struct(str, buff, -1);
}

int bytes_to_str(const bytes_value_struct* val, string_value_struct* str)
{
	if (val->size == 0)
		return rx_init_string_value_struct(str, NULL, 0);
	char hex_buff[0x40];
	char* pbuffer = hex_buff;
	size_t temp_size;
	const uint8_t* pbytes = rx_c_ptr(val, &temp_size);
	if (val->size * 2 + 1 > sizeof(hex_buff) / sizeof(hex_buff[0]))
	{
		pbuffer = malloc(val->size * 2 + 1);
		if (pbuffer == NULL)
			return RX_ERROR;
	}
	for (size_t i = 0; i < val->size; i++)
	{
		sprintf(&pbuffer[i*2], "%02X", pbytes[i]);
	}
	int ret = rx_init_string_value_struct(str, pbuffer, -1);
	if (val->size * 2 + 1 > sizeof(hex_buff) / sizeof(hex_buff[0]))
		free(pbuffer);
	return ret;
}
int time_to_ISO8601(rx_time_struct val, string_value_struct* str)
{
	char buff[0x20];

	rx_full_time os_time;
	if (rx_os_split_time(&val, &os_time))
	{
		snprintf(buff, sizeof(buff) / sizeof(buff[0]), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
			os_time.year, os_time.month, os_time.day,
			os_time.hour, os_time.minute, os_time.second, os_time.milliseconds);

		return rx_init_string_value_struct(str, buff, -1);
	}

	return RX_ERROR;

}

int convert_union(union rx_value_union* what, rx_value_t source, rx_value_t target)
{
	rx_value_t simple = target & RX_SIMPLE_VALUE_MASK;
	target = simple;
	switch (target)
	{
	case RX_BOOL_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->bool_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				return RX_OK;
			case RX_INT8_TYPE:
				what->bool_value = what->int8_value != 0;
				return RX_OK;
			case RX_INT16_TYPE:
				what->bool_value = what->int16_value != 0;
				return RX_OK;
			case RX_INT32_TYPE:
				what->bool_value = what->int32_value != 0;
				return RX_OK;
			case RX_INT64_TYPE:
				what->bool_value = what->int64_value != 0;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->bool_value = what->uint8_value != 0;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->bool_value = what->uint16_value != 0;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->bool_value = what->uint32_value != 0;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->bool_value = what->uint64_value != 0;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->bool_value = what->float_value != 0;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->bool_value = what->double_value != 0;
				return RX_OK;
			case RX_TIME_TYPE:
				what->bool_value = what->time_value.t_value != 0;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					uint_fast8_t temp = what->complex_value->real != 0 || what->complex_value->imag != 0;
					free(what->complex_value);
					what->bool_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				{
					uint_fast8_t temp = !rx_is_null_uuid(what->uuid_value);
					free(what->uuid_value);
					what->bool_value = temp;
					return RX_OK;
				}
#else
			case RX_COMPLEX_TYPE:
				{
					uint_fast8_t temp = what->complex_value.real != 0 || what->complex_value.imag != 0;
					what->bool_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				{
					uint_fast8_t temp = !rx_is_null_uuid(what->uuid_value);
					what->bool_value = temp;
					return RX_OK;
				}
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size > 0)
					{
						const char* str = rx_c_str(&what->string_value);
						if (strcmp(str, "true") == 0)
						{
							what->bool_value = (1 == 1);
							return RX_OK;
						}
						else if (strcmp(str, "false") == 0)
						{
							what->bool_value = (1 == 0);
							return RX_OK;
						}
						if (strcmp(str, "True") == 0)
						{
							what->bool_value = (1 == 1);
							return RX_OK;
						}
						else if (strcmp(str, "False") == 0)
						{
							what->bool_value = (1 == 0);
							return RX_OK;
						}
						else
						{
							char* end_ptr = NULL;
							double res = strtod(str, &end_ptr);
							if (*end_ptr == '\0')
							{
								what->bool_value = (res != 0);
								return RX_OK;
							}
						}
					}
					return RX_ERROR;
				}
			case RX_BYTES_TYPE:
				{
					uint_fast8_t temp = what->struct_value.size > 0;
					if (temp)
					{
						free(what->struct_value.values);
					}
					what->bool_value = temp;
					return RX_OK;
				}
			case RX_STRUCT_TYPE:
				{
					uint_fast8_t temp = what->bytes_value.size > 0;
					rx_destory_bytes_value_struct(&what->bytes_value);
					what->bool_value = temp;
					return RX_OK;
				}
			case RX_NODE_ID_TYPE:
				{
					uint_fast8_t temp = !rx_is_null_node_id(what->node_id_value);
					rx_destory_node_id(what->node_id_value);
					free(what->node_id_value);
					what->bool_value = temp;
					return RX_OK;
				}
			}
		}
		break;
	case RX_INT8_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->int8_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->int8_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				return RX_OK;
			case RX_INT16_TYPE:
				what->int8_value = (int8_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->int8_value = (int8_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->int8_value = (int8_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->int8_value = (int8_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->int8_value = (int8_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->int8_value = (int8_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->int8_value = (int8_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->int8_value = (int8_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->int8_value = (int8_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->int8_value = (int8_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					int8_t temp = (int8_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->int8_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					int8_t temp = (int8_t)complex_amplitude_helper(&what->complex_value);
					what->int8_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					int8_t temp = 0;
					int ret = parse_int8(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->int8_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_INT16_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->int16_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->int16_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->int16_value = (int16_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				return RX_OK;
			case RX_INT32_TYPE:
				what->int16_value = (int16_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->int16_value = (int16_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->int16_value = (int16_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->int16_value = (int16_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->int16_value = (int16_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->int16_value = (int16_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->int16_value = (int16_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->int16_value = (int16_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->int16_value = (int16_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					int16_t temp = (int16_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->int16_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					int16_t temp = (int16_t)complex_amplitude_helper(&what->complex_value);
					what->int16_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					int16_t temp = 0;
					int ret = parse_int16(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->int16_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_INT32_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->int32_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->int32_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->int32_value = (int32_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->int32_value = (int32_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				return RX_OK;
			case RX_INT64_TYPE:
				what->int32_value = (int32_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->int32_value = (int32_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->int32_value = (int32_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->int32_value = (int32_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->int32_value = (int32_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->int32_value = (int32_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->int32_value = (int32_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->int32_value = (int32_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					int32_t temp = (int32_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->int32_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					int32_t temp = (int32_t)complex_amplitude_helper(&what->complex_value);
					what->int32_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					int32_t temp = 0;
					int ret = parse_int32(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->int32_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_INT64_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->int64_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->int64_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->int64_value = (int64_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->int64_value = (int64_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->int64_value = (int64_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				return RX_OK;
			case RX_UINT8_TYPE:
				what->int64_value = (int64_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->int64_value = (int64_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->int64_value = (int64_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->int64_value = (int64_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->int64_value = (int64_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->int64_value = (int64_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->int64_value = (int64_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					int64_t temp = (int64_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->int64_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					int64_t temp = (int64_t)complex_amplitude_helper(&what->complex_value);
					what->int64_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					int64_t temp = 0;
					int ret = parse_int64(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->int64_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_UINT8_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->uint8_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->uint8_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->uint8_value = (uint8_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->uint8_value = (uint8_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->uint8_value = (uint8_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->uint8_value = (uint8_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				return RX_OK;
			case RX_UINT16_TYPE:
				what->uint8_value = (uint8_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->uint8_value = (uint8_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->uint8_value = (uint8_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->uint8_value = (uint8_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->uint8_value = (uint8_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->uint8_value = (uint8_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					uint8_t temp = (uint8_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->uint8_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					uint8_t temp = (uint8_t)complex_amplitude_helper(&what->complex_value);
					what->uint8_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					uint8_t temp = 0;
					int ret = parse_uint8(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->uint8_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_UINT16_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->uint16_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->uint16_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->uint16_value = (uint16_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->uint16_value = (uint16_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->uint16_value = (uint16_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->uint16_value = (uint16_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->uint16_value = (uint16_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				return RX_OK;
			case RX_UINT32_TYPE:
				what->uint16_value = (uint16_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->uint16_value = (uint16_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->uint16_value = (uint16_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->uint16_value = (uint16_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->uint16_value = (uint16_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					uint16_t temp = (uint16_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->uint16_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					uint16_t temp = (uint16_t)complex_amplitude_helper(&what->complex_value);
					what->uint16_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					uint16_t temp = 0;
					int ret = parse_uint16(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->uint16_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_UINT32_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->uint32_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->uint32_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->uint32_value = (uint32_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->uint32_value = (uint32_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->uint32_value = (uint32_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->uint32_value = (uint32_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->uint32_value = (uint32_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->uint32_value = (uint32_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				return RX_OK;
			case RX_UINT64_TYPE:
				what->uint32_value = (uint32_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->uint32_value = (uint32_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->uint32_value = (uint32_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->uint32_value = (uint32_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					uint32_t temp = (uint32_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->uint32_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					uint32_t temp = (uint32_t)complex_amplitude_helper(&what->complex_value);
					what->uint32_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					uint32_t temp = 0;
					int ret = parse_uint32(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->uint32_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_UINT64_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->uint64_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->uint64_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->uint64_value = (uint64_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->uint64_value = (uint64_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->uint64_value = (uint64_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->uint64_value = (uint64_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->uint64_value = (uint64_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->uint64_value = (uint64_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->uint64_value = (uint64_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->uint64_value = (uint64_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->uint64_value = (uint64_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->uint64_value = (uint64_t)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					uint64_t temp = (uint64_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->uint64_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					uint64_t temp = (uint64_t)complex_amplitude_helper(&what->complex_value);
					what->uint64_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					uint64_t temp = 0;
					int ret = parse_uint64(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->uint64_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_FLOAT_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->float_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->float_value = what->bool_value ? (float)1 : (float)0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->float_value = (float)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->float_value = (float)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->float_value = (float)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->float_value = (float)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->float_value = (float)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->float_value = (float)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->float_value = (float)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->float_value = (float)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->float_value = (float)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->float_value = (float)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					float temp = (float)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->float_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					float temp = (float)complex_amplitude_helper(&what->complex_value);
					what->float_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					double temp = 0;
					int ret = parse_double(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->float_value = (float)temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_DOUBLE_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->double_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->double_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->double_value = (double)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->double_value = (double)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->double_value = (double)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->double_value = (double)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->double_value = (double)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->double_value = (double)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->double_value = (double)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->double_value = (double)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->double_value = (double)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				return RX_OK;
			case RX_TIME_TYPE:
				what->double_value = (double)what->time_value.t_value;
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					double temp = (double)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->double_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					double temp = (double)complex_amplitude_helper(&what->complex_value);
					what->double_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					double temp = 0;
					int ret = parse_double(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->double_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
#ifndef RX_VALUE_SIZE_16
	case RX_COMPLEX_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = what->bool_value ? 1.0 : 0.0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->complex_value = malloc(sizeof(complex_value_struct));
				what->complex_value->imag = 0;
				what->complex_value->real = (double)what->time_value.t_value;
				return RX_OK;
			case RX_COMPLEX_TYPE:
				return RX_OK;
			case RX_UUID_TYPE:
				return RX_ERROR;
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					complex_value_struct temp;
					int ret = parse_complex(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->complex_value = malloc(sizeof(complex_value_struct));
						*what->complex_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
#else
	case RX_COMPLEX_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = what->bool_value ? 1.0 : 0.0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				what->complex_value.imag = 0;
				what->complex_value.real = (double)what->time_value.t_value;
				return RX_OK;
			case RX_COMPLEX_TYPE:
				return RX_OK;
			case RX_UUID_TYPE:
				return RX_ERROR;
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					complex_value_struct temp;
					int ret = parse_complex(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->complex_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
#endif
#ifndef RX_VALUE_SIZE_16
	case RX_UUID_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->uuid_value = malloc(sizeof(rx_uuid_t));
				memzero(what->uuid_value, sizeof(rx_uuid_t));
				return RX_OK;
			case RX_BOOL_TYPE:
			case RX_INT8_TYPE:
			case RX_INT16_TYPE:
			case RX_INT32_TYPE:
			case RX_INT64_TYPE:
			case RX_UINT8_TYPE:
			case RX_UINT16_TYPE:
			case RX_UINT32_TYPE:
			case RX_UINT64_TYPE:
			case RX_FLOAT_TYPE:
			case RX_DOUBLE_TYPE:
			case RX_TIME_TYPE:
			case RX_COMPLEX_TYPE:
			case RX_UUID_TYPE:
				return RX_ERROR;
			case RX_STRING_TYPE:
				{
					rx_uuid_t temp;
					if (what->string_value.size == 0)
						return RX_ERROR;

					int ret = rx_string_to_uuid(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->uuid_value = malloc(sizeof(rx_uuid_t));
						*what->uuid_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
#else
	case RX_UUID_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				memzero(&what->uuid_value, sizeof(rx_uuid_t));
				return RX_OK;
			case RX_BOOL_TYPE:
			case RX_INT8_TYPE:
			case RX_INT16_TYPE:
			case RX_INT32_TYPE:
			case RX_INT64_TYPE:
			case RX_UINT8_TYPE:
			case RX_UINT16_TYPE:
			case RX_UINT32_TYPE:
			case RX_UINT64_TYPE:
			case RX_FLOAT_TYPE:
			case RX_DOUBLE_TYPE:
			case RX_TIME_TYPE:
			case RX_COMPLEX_TYPE:
			case RX_UUID_TYPE:
				return RX_ERROR;
			case RX_STRING_TYPE:
				{
					rx_uuid_t temp;
					if (what->string_value.size == 0)
						return RX_ERROR;

					int ret = rx_string_to_uuid(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->uuid_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
#endif
	case RX_TIME_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->time_value.t_value = 0;
				return RX_OK;
			case RX_BOOL_TYPE:
				what->time_value.t_value = what->bool_value ? 1 : 0;
				return RX_OK;
			case RX_INT8_TYPE:
				what->time_value.t_value = (uint64_t)what->int8_value;
				return RX_OK;
			case RX_INT16_TYPE:
				what->time_value.t_value = (uint64_t)what->int16_value;
				return RX_OK;
			case RX_INT32_TYPE:
				what->time_value.t_value = (uint64_t)what->int32_value;
				return RX_OK;
			case RX_INT64_TYPE:
				what->time_value.t_value = (uint64_t)what->int64_value;
				return RX_OK;
			case RX_UINT8_TYPE:
				what->time_value.t_value = (uint64_t)what->uint8_value;
				return RX_OK;
			case RX_UINT16_TYPE:
				what->time_value.t_value = (uint64_t)what->uint16_value;
				return RX_OK;
			case RX_UINT32_TYPE:
				what->time_value.t_value = (uint64_t)what->uint32_value;
				return RX_OK;
			case RX_UINT64_TYPE:
				what->time_value.t_value = (uint64_t)what->uint64_value;
				return RX_OK;
			case RX_FLOAT_TYPE:
				what->time_value.t_value = (uint64_t)what->float_value;
				return RX_OK;
			case RX_DOUBLE_TYPE:
				what->time_value.t_value = (uint64_t)what->double_value;
				return RX_OK;
			case RX_TIME_TYPE:
				return RX_OK;
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					uint64_t temp = (uint64_t)complex_amplitude_helper(what->complex_value);
					free(what->complex_value);
					what->time_value.t_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#else
			case RX_COMPLEX_TYPE:
				{
					uint64_t temp = (uint64_t)complex_amplitude_helper(&what->complex_value);
					what->time_value.t_value = temp;
					return RX_OK;
				}
			case RX_UUID_TYPE:
				return RX_ERROR;
#endif
			case RX_STRING_TYPE:
				{
					if (what->string_value.size == 0)
						return RX_ERROR;
					rx_time_struct temp;
					int ret = parse_time_from_ISO8601(rx_c_str(&what->string_value), &temp);
					if (ret)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->time_value = temp;
					}
					return ret;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
				return RX_ERROR;
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_STRING_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				rx_init_string_value_struct(&what->string_value, NULL, 0);
				return RX_OK;
			case RX_BOOL_TYPE:
				return bool_to_str(what->bool_value, &what->string_value);
			case RX_INT8_TYPE:
				return int8_to_str(what->int8_value, &what->string_value);
			case RX_INT16_TYPE:
				return int16_to_str(what->int16_value, &what->string_value);
			case RX_INT32_TYPE:
				return int32_to_str(what->int32_value, &what->string_value);
			case RX_INT64_TYPE:
				return int64_to_str(what->int64_value, &what->string_value);
			case RX_UINT8_TYPE:
				return uint8_to_str(what->uint8_value, &what->string_value);
			case RX_UINT16_TYPE:
				return uint16_to_str(what->uint16_value, &what->string_value);
			case RX_UINT32_TYPE:
				return uint32_to_str(what->uint32_value, &what->string_value);
			case RX_UINT64_TYPE:
				return uint64_to_str(what->uint64_value, &what->string_value);
			case RX_FLOAT_TYPE:
				return float_to_str(what->float_value, &what->string_value);
			case RX_DOUBLE_TYPE:
				return double_to_str(what->double_value, &what->string_value);
			case RX_TIME_TYPE:
				return time_to_ISO8601(what->time_value, &what->string_value);
#ifndef RX_VALUE_SIZE_16
			case RX_COMPLEX_TYPE:
				{
					complex_value_struct* temp = what->complex_value;
					string_value_struct temp_str;
					int ret = complex_to_str(temp, &temp_str);
					if (ret == RX_OK)
					{
						free(what->complex_value);
						what->string_value = temp_str;
					}
					return ret;
				}
			case RX_UUID_TYPE:
				{
					rx_uuid_t temp = *what->uuid_value;
					char buff[0x40];
					int ret = rx_uuid_to_string(&temp, buff);
					if (ret)
					{
						free(what->uuid_value);
						rx_init_string_value_struct(&what->string_value, buff, -1);
					}
					return ret;
				}
#else
			case RX_COMPLEX_TYPE:
				{
					complex_value_struct temp = what->complex_value;
					return complex_to_str(&temp, &what->string_value);
				}
			case RX_UUID_TYPE:
				{
					rx_uuid_t temp = what->uuid_value;
					char buff[0x40];
					int ret = rx_uuid_to_string(&temp, buff);
					if (ret)
					{
						rx_init_string_value_struct(&what->string_value, buff);
					}
					return ret;
				}
#endif
			case RX_STRING_TYPE:
				return RX_OK;
			case RX_BYTES_TYPE:
				{
					bytes_value_struct temp = what->bytes_value;
					int ret = bytes_to_str(&temp, &what->string_value);
					rx_destory_bytes_value_struct(&temp);
					return ret;
				}
			case RX_STRUCT_TYPE:
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_NODE_ID_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				what->node_id_value = malloc(sizeof(rx_node_id_struct));
				rx_init_null_node_id(what->node_id_value);
				return RX_OK;
			case RX_BOOL_TYPE:
			case RX_INT8_TYPE:
			case RX_INT16_TYPE:
			case RX_INT32_TYPE:
			case RX_INT64_TYPE:
			case RX_UINT8_TYPE:
			case RX_UINT16_TYPE:
			case RX_UINT32_TYPE:
			case RX_UINT64_TYPE:
			case RX_FLOAT_TYPE:
			case RX_DOUBLE_TYPE:
			case RX_TIME_TYPE:
			case RX_COMPLEX_TYPE:
			case RX_UUID_TYPE:
				return RX_ERROR;
			case RX_STRING_TYPE:
				{
					const char* str = rx_c_str(&what->string_value);
					rx_node_id_struct temp;
					auto ret = rx_node_id_from_string(&temp, str);
					if (ret == RX_OK)
					{
						rx_destory_string_value_struct(&what->string_value);
						what->node_id_value = malloc(sizeof(rx_node_id_struct));
						*what->node_id_value = temp;
						rx_move_node_id(what->node_id_value, &temp);
						return RX_OK;
					}
					return RX_ERROR;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	case RX_BYTES_TYPE:
	case RX_STRUCT_TYPE:
		{
			switch (source)
			{
			case RX_NULL_TYPE:
				rx_init_bytes_value_struct(&what->bytes_value, NULL, 0);
				return RX_OK;
			case RX_BOOL_TYPE:
			case RX_INT16_TYPE:
			case RX_INT32_TYPE:
			case RX_INT64_TYPE:
			case RX_UINT8_TYPE:
			case RX_UINT16_TYPE:
			case RX_UINT32_TYPE:
			case RX_UINT64_TYPE:
			case RX_FLOAT_TYPE:
			case RX_DOUBLE_TYPE:
			case RX_TIME_TYPE:
			case RX_COMPLEX_TYPE:
			case RX_UUID_TYPE:
				return RX_ERROR;
			case RX_STRING_TYPE:
				{
					// TODO string to bytes
					RX_ASSERT(0);
					return RX_ERROR;
				}
			case RX_BYTES_TYPE:
			case RX_STRUCT_TYPE:
			case RX_NODE_ID_TYPE:
				return RX_ERROR;
			}
		}
		break;
	default:
		// TODO other target types
		RX_ASSERT(0);
	}
	return RX_ERROR;
}
RX_COMMON_API int rx_convert_value(struct typed_value_type* val, rx_value_t type)
{
	if (val->value_type == type)
		return RX_OK;// nothing to do
	if (type == RX_NULL_TYPE)
	{// clear value here
		destroy_union_value(&val->value, val->value_type);
		val->value_type = RX_NULL_TYPE;
		return RX_OK;
	}
	int target_is_array = (type & RX_ARRAY_VALUE_MASK) == RX_ARRAY_VALUE_MASK;
	int current_is_array = (val->value_type & RX_ARRAY_VALUE_MASK) == RX_ARRAY_VALUE_MASK;
	if (!target_is_array && !current_is_array)
	{// normal values
		if (!convert_union(&val->value, val->value_type, type))
			return RX_ERROR;
		val->value_type = type;
		return RX_OK;
	}
	else if (target_is_array && current_is_array)
	{// both are arrays convert individual items
		for (size_t i = 0; i < val->value.array_value.size; i++)
		{
			if (!convert_union(&val->value.array_value.values[i], val->value_type & RX_STRIP_ARRAY_MASK, type & RX_STRIP_ARRAY_MASK))
				return RX_ERROR;
		}
		val->value_type = type;
		return RX_OK;
	}
	else if (!target_is_array && current_is_array)
	{// from array to simple value
		if (val->value.array_value.size == 0 // empty array only to null type, checked before
			|| val->value.array_value.size > 1) // array size is greater then one, so can't do it?!?
		{
			return RX_ERROR;
		}
		if (!convert_union(&val->value.array_value.values[0], val->value_type & RX_STRIP_ARRAY_MASK, type))
			return RX_ERROR;
		union rx_value_union temp = val->value.array_value.values[0];
		if (val->value.array_value.size > 0)
			free(val->value.array_value.values);
		val->value = temp;
		val->value_type = type;
		return RX_OK;
	}
	else //(target_is_array && !current_is_array)
	{// from simple value to array
		if (!convert_union(&val->value, val->value_type, type & RX_STRIP_ARRAY_MASK))
			return RX_ERROR;
		union rx_value_union temp;
		temp = val->value;
		val->value.array_value.values = malloc(1 * sizeof(union rx_value_union));
		val->value.array_value.values[0] = temp;
		val->value.array_value.size = 1;
		val->value_type = type;
		return RX_OK;
	}
}

RX_COMMON_API int rx_parse_string(struct typed_value_type* val, const char* data)
{
	if (data == NULL)
	{
		return rx_init_null_value(val);
	}
	else if (!(*data))
	{
		return rx_init_string_value(val, NULL, 0);
	}
	else
	{
		uint_fast8_t temp;
		uint64_t temp_uint;
		int64_t temp_int;
		double temp_dbl;
		rx_uuid_t temp_uuid;
		// try bool first
		if (parse_uint64(data, &temp_uint))
		{
			if (temp_uint < UINT_MAX)
				return rx_init_uint32_value(val, (uint32_t)temp_uint);
			else
				return rx_init_uint64_value(val, temp_uint);
		}
		else if (parse_bool(data, &temp))
		{
			return rx_init_bool_value(val, temp);
		}
		else if (parse_int64(data, &temp_int))
		{
			if (temp_int < INT_MAX && temp_int > INT_MIN)
				return rx_init_int32_value(val, (int32_t)temp_int);
			else
				return rx_init_uint64_value(val, temp_int);
		}
		else if (parse_double(data, &temp_dbl))
		{
			return rx_init_double_value(val, temp_dbl);
		}
		else if (rx_string_to_uuid(data, &temp_uuid))
		{
			return rx_init_uuid_value(val, &temp_uuid);
		}
		else
		{
			return rx_init_string_value(val, data, -1);
		}
	}
}


