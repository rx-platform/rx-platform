

/****************************************************************************
*
*  common\rx_basic.c
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



// rx_result_struct functions


RX_COMMON_API void rx_init_result_struct(rx_result_struct* res)
{
	memzero(res, sizeof(rx_result_struct));
}
RX_COMMON_API int rx_init_result_struct_with_error(rx_result_struct* res, uint32_t code, const char* text, int count)
{
	memzero(res, sizeof(rx_result_struct));
	res->count = 1;
	res->data.static_data[0].code = code;
	return rx_init_string_value_struct(&res->data.static_data[0].text, text, count);
}
RX_COMMON_API int rx_init_result_struct_with_errors(rx_result_struct* res, uint32_t* codes, const char** texts, size_t errors_count)
{
	int ret = RX_OK;
	const char* temp_str;
	rx_result_data* temp_ptr;
	res->count = errors_count;
	if (errors_count)
	{
		if (res->count <= RESULT_STATIC_SIZE)
		{
			temp_ptr = res->data.static_data;
		}
		else
		{
			temp_ptr = malloc(sizeof(rx_result_data) * (res->count + 1));
			if (!temp_ptr)
				return RX_ERROR;
			res->data.ptr_data = temp_ptr;
		}
		for (size_t i = 0; i < errors_count; i++)
		{
			if (codes)
				temp_ptr[i].code = codes[i];
			else
				temp_ptr[i].code = 113;
			if (texts)
				temp_str = texts[i];
			else
				temp_str = NULL;
			if (!temp_str)
				temp_str = UNDEFINED_RESULT_TEXT;
			ret = rx_init_string_value_struct(&temp_ptr[i].text, temp_str, -1);
			if (ret != RX_OK)
			{// cleanup earlier allocations on errors
				for (size_t j = 0; j < i; j++)
				{
					rx_destory_string_value_struct(&temp_ptr[j].text);
				}
				free(temp_ptr);
				return ret;
			}
			
		}
	}
	return ret;
}
RX_COMMON_API int rx_result_add_error(rx_result_struct* res, uint32_t code, const char* text, int count)
{
	int ret;
	rx_result_data* temp_ptr;
	if (res->count < RESULT_STATIC_SIZE)
	{
		res->data.static_data[res->count].code = code;
		ret = rx_init_string_value_struct(&res->data.static_data[res->count].text, text, count);
	}
	else if (res->count == RESULT_STATIC_SIZE)
	{// transfer from static to dynamic array
		temp_ptr = malloc(sizeof(rx_result_data) * (RESULT_STATIC_SIZE + 1));
		if (temp_ptr)
		{
			memcpy(temp_ptr, res->data.static_data, sizeof(rx_result_data) * RESULT_STATIC_SIZE);

			temp_ptr[RESULT_STATIC_SIZE].code = code;
			ret = rx_init_string_value_struct(&temp_ptr[RESULT_STATIC_SIZE].text, text, count);

			res->data.ptr_data = temp_ptr;
		}
		else
		{
			ret = RX_ERROR;
		}
	}
	else
	{// full dynamic

		temp_ptr = malloc(sizeof(rx_result_data) * (res->count + 1));
		if (temp_ptr)
		{
			memcpy(temp_ptr, res->data.ptr_data, sizeof(rx_result_data) * (res->count));

			temp_ptr[res->count].code = code;
			ret = rx_init_string_value_struct(&temp_ptr[res->count].text, text, count);

			free(res->data.ptr_data);
			res->data.ptr_data = temp_ptr;
		}
		else
		{
			ret = RX_ERROR;
		}
	}
	res->count++;
	return ret;
}

RX_COMMON_API void rx_move_result_struct(rx_result_struct* res, rx_result_struct* src)
{
	memcpy(res, src, sizeof(rx_result_struct));
	memzero(src, sizeof(rx_result_struct));
}
RX_COMMON_API int rx_copy_result_struct(rx_result_struct* res, const rx_result_struct* src)
{
	int ret = RX_OK;
	uint32_t i;
	memcpy(res, src, sizeof(rx_result_struct));
	if (src->count)
	{
		if (src->count <= RESULT_STATIC_SIZE)
		{
			for (i = 0; i < src->count; i++)
			{
				ret = rx_copy_string_value(&res->data.static_data[i].text, &src->data.static_data[i].text);
				if (ret != RX_OK)
					break;
			}
		}
		else
		{
			res->data.ptr_data = malloc(sizeof(rx_result_data) * src->count);
			for (i = 0; i < src->count; i++)
			{
				ret = rx_copy_string_value(&res->data.ptr_data[i].text, &src->data.ptr_data[i].text);
				if (ret != RX_OK)
					break;
			}
		}
	}
	return ret;
}

RX_COMMON_API int rx_result_ok(const rx_result_struct* res)
{
	return res->count == 0;
}


RX_COMMON_API size_t rx_result_errors_count(const rx_result_struct* res)
{
	return res->count;
}
RX_COMMON_API const char* rx_result_get_error(const rx_result_struct* res, size_t idx, uint32_t* code)
{
	if (idx < res->count)
	{
		if (res->count <= RESULT_STATIC_SIZE)
		{
			*code = res->data.static_data[idx].code;
			return rx_c_str(&res->data.static_data[idx].text);
		}
		else
		{
			*code = res->data.ptr_data[idx].code;
			return rx_c_str(&res->data.ptr_data[idx].text);
		}
	}
	else
	{
		return NULL;
	}
}

RX_COMMON_API void rx_destroy_result_struct(rx_result_struct* res)
{
	uint32_t i;
	if (res->count)
	{
		if (res->count <= RESULT_STATIC_SIZE)
		{
			for (i = 0; i < res->count; i++)
			{
				rx_destory_string_value_struct(&res->data.static_data[i].text);
			}
		}
		else
		{
			for (i = 0; i < res->count; i++)
			{
				rx_destory_string_value_struct(&res->data.ptr_data[i].text);
			}
			free(res->data.ptr_data);
		}
	}
}


RX_COMMON_API void rx_init_meta_data(rx_meta_data_struct* what)
{
	memzero(what, sizeof(rx_meta_data_struct));
}
RX_COMMON_API void rx_deinit_meta_data(rx_meta_data_struct* what)
{
	rx_destory_string_value_struct(&what->name);
	rx_destory_string_value_struct(&what->path);
	rx_deinit_reference(&what->parent);
}



