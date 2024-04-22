#include "pch.h"
#include "rx_common.h"

int is_valid_base64(char ch)
{
	return (ch >= 'A' && ch <= 'Z') ||
		(ch >= 'a' && ch <= 'z') ||
		(ch >= '0' && ch <= '9') ||
		ch == '+' || ch == '/';
}

char tabel_base64[] = {
	'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L',
	'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b',
	'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r',
	's', 't', 'u', 'v',
	'w', 'x', 'y', 'z',
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', '+', '/'
};

uint8_t getFrom(char ch)
{
	for (size_t i = 0; i < sizeof(tabel_base64) / sizeof(tabel_base64[0]); i++)
	{
		if (ch == tabel_base64[i])
		{
			return (uint8_t)i;
		}
	}
	RX_ASSERT(0);
	return 0;
}

RX_COMMON_API int rx_base64_get_string(string_value_struct* result, const uint8_t* data, size_t size)
{
	if (size == 0)
	{
		rx_init_string_value_struct(result, NULL, 0);
		return RX_OK;
	}
	size_t result_size = size + ((size * 22) >> 6) + 3; //22/64 = 0.345
	if (RX_ERROR == rx_init_string_value_struct(result, NULL, (int)result_size))
		return RX_ERROR;

	char* str_data = (char*)rx_c_str(result);

	int bonus = 0;

	if (size % 3 == 1)
	{
		bonus = 2;
		size += 2;
	}
	else if (size % 3 == 2)
	{
		bonus = 1;
		size += 1;
	}

	size_t result_idx = 0;

	for (size_t i = 0; i < size - bonus; i++)
	{
		uint8_t between;

		uint8_t first = data[i] & (uint8_t)0xfc;
		first = first >> 2;

		//////////////////////////////////////////////////

		uint8_t second = data[i] & (uint8_t)0x03;
		second = second << 4;
		i++;
		if (bonus == 2 && i >= size - 2)
		{
			between = (uint8_t)0;
		}
		else
		{
			RX_ASSERT(i < size - bonus);
			between = data[i] & (uint8_t)0xf0;
			between = between >> 4;
		}

		second = second | between;

		//////////////////////////////////////////////////

		uint8_t third = 0;
		uint8_t fourth = 0;
		if (i < size - bonus)
		{
			third = data[i] & (uint8_t)0x0f;
			third = third << 2;

			i++;
			if (i < size - bonus)
			{
				RX_ASSERT(i < size - bonus);
				between = data[i] & (uint8_t)0xc0;
				between = between >> 6;
				third = third | between;
				fourth = data[i] & (uint8_t)0x3f;
			}
		}

		//////////////////////////////////////////////////


		str_data[result_idx] = tabel_base64[first];
		str_data[result_idx + 1] = tabel_base64[second];
		str_data[result_idx + 2] = tabel_base64[third];
		str_data[result_idx + 3] = tabel_base64[fourth];
		result_idx += 4;

	}

	if (bonus == 1)
	{
		str_data[result_idx-1] = '=';
	}
	else if (bonus == 2)
	{
		str_data[result_idx - 1] = '=';
		str_data[result_idx - 2] = '=';
	}

	return RX_OK;
}
uint8_t get_next_char(const char* data, size_t i, int bonus, size_t len)
{
	uint8_t one_char = data[i];
	int in_padding = (i + bonus < len);
	if (in_padding)
	{
		if (!is_valid_base64(one_char))
			return 0xff;
		one_char = getFrom(one_char);
	}
	else
	{
		if (one_char != '=')
			return 0xff;
		one_char = 0;
	}
	return one_char;
}

RX_COMMON_API int rx_base64_get_data(bytes_value_struct* result, const char* data)
{
	size_t str_size = data==NULL ? 0 : strlen(data);
	if (str_size == 0)
	{
		rx_init_bytes_value_struct(result, NULL, 0);
		return RX_OK;
	}

	if (str_size < 4)
		return RX_ERROR;
	if (str_size % 4 != 0)
		return RX_ERROR;

	int ret = rx_init_bytes_value_struct(result, NULL, str_size);
	if (ret != RX_OK)
		return ret;


	int bonus = 0;
	size_t len = str_size;
	size_t temp_size;
	uint8_t* result_data = (uint8_t*)rx_c_ptr(result, &temp_size);


	if (data[len - 2] == '=')
	{
		bonus=2;
	}
	else if (data[len - 1] == '=')
	{
		bonus = 1;
	}

	size_t result_idx = 0;
	uint8_t between = 0;
	for (size_t i = 0; i < len; i++)
	{
		uint8_t one_char = get_next_char(data, i, bonus, len);
		if (one_char == 0xff)
			return RX_ERROR;

		uint8_t first = one_char << 2;
		i++;
		one_char = get_next_char(data, i, bonus, len);
		if (one_char == 0xff)
			return RX_ERROR;
		between = one_char >> 4;
		first = first | between;

		/////////////////////////////////////////////

		uint8_t second = one_char << 4;
		i++;
		one_char = get_next_char(data, i, bonus, len);
		if (one_char == 0xff)
			return RX_ERROR;

		between = one_char >> 2;
		second = second | between;

		/////////////////////////////////////////////

		uint8_t third = one_char << 6;
		i++;

		one_char = get_next_char(data, i, bonus, len);
		if (one_char == 0xff)
			return RX_ERROR;

		between = one_char;
		third = third | between;

		result_data[result_idx++] = first;
		result_data[result_idx++] = second;
		result_data[result_idx++] = third;
	}
	size_t new_size = result_idx;
	if (bonus == 1)
	{
		new_size--;
	}
	else if (bonus == 2)
	{
		new_size -= 2;
	}
	/// this is danegrous but fuck it!!!!!!
	if (new_size > sizeof(sizeof(result->value)))
	{
		// no optimization just set size
		// free will free it whatever its size is
		result->size = new_size;
	}
	else
	{
		// has optimization, so do the copy
		// this will not alocate aditional memory
		// and will free eventual buffer that is left
		bytes_value_struct temp_struct;
		size_t temp_size = 0;
		uint8_t* temp_ptr = (uint8_t*)rx_c_ptr(result, &temp_size);
		RX_ASSERT(temp_size >= new_size);
		rx_init_bytes_value_struct(&temp_struct, temp_ptr, new_size);
		rx_destory_bytes_value_struct(result);
		*result = temp_struct;
	}

	return RX_OK;

}

