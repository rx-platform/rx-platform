

/****************************************************************************
*
*  lib\rx_iso_8601.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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

#include"common/rx_common.h"

// rx_iso_8601
#include "lib/rx_iso_8601.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// VAZNO!!!!! //////////////////////////////////////////////////////////////////////////////////////////
// OVDE PISI KOD!!!
// objasnicu ti zasto kad se vidimo sledeci put, sorry moram da ti pokazem


rx_time_struct iso_8601_to_rx_time(const char* str)
{
	const char* ptr = str;

	rx_full_time os_time;

	os_time.year = 0;
	os_time.month = 0;
	os_time.day = 0;
	os_time.hour = 0;
	os_time.minute = 0;
	os_time.second = 0;
	os_time.milliseconds = 0;

	if (sscanf(ptr, "%4u-%2u-%2u", &os_time.year, &os_time.month, &os_time.day) == 3)
		ptr += 10;
	else if (sscanf(ptr, "%4u%2u%2u", &os_time.year, &os_time.month, &os_time.day) == 3)
		ptr += 8;
	else
		return rx_time_struct{ 0 };

	if (*ptr != L'\0')
	{
		if (*ptr != L'T')
			return rx_time_struct{ 0 };
		ptr++;

		if (sscanf(ptr, "%2u:%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 5;
		else if (sscanf(ptr, "%2u%2u", &os_time.hour, &os_time.minute) == 2)
			ptr += 4;
		else
			return rx_time_struct{ 0 };

		if (*ptr == L':')
			ptr++;

		if (*ptr != L'\0')
		{// seconds too
			if (sscanf(ptr, "%2u", &os_time.second) == 1)
				ptr += 2;
			else
				return rx_time_struct{ 0 };
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
		return ts;
	}
	else
		return rx_time_struct{ 0 };

}
int rx_time_to_iso_8601(const rx_time_struct* rx_time, char* buffer, size_t buff_len, int options)
{
	if (rx_time == nullptr || buffer == nullptr)
		return RX_ERROR;

	rx_full_time os_time;
	if (rx_os_split_time(rx_time, &os_time))
	{
		if (options & RX_ISO_8601_NO_TIME)
		{
			return snprintf(buffer, buff_len, "%04u-%02u-%02u", os_time.year, os_time.month, os_time.day);
		}
		else if (options & RX_ISO_8601_NO_MILLISECONDS)
		{
			if (snprintf(buffer, buff_len, "%04u-%02u-%02uT%02u:%02u:%02u", os_time.year, os_time.month, os_time.day,
				os_time.hour, os_time.minute, os_time.second))
				return RX_OK;
		}
		else if (options & RX_ISO_8601_NO_SECONDS)
		{
			if (snprintf(buffer, buff_len, "%04u-%02u-%02uT%02u:%02u", os_time.year, os_time.month, os_time.day,
				os_time.hour, os_time.minute))
				return RX_OK;
		}
		else
		{
			if(snprintf(buffer, buff_len, "%04u-%02u-%02uT%02u:%02u:%02u.%03u", os_time.year, os_time.month, os_time.day,
				os_time.hour, os_time.minute, os_time.second, os_time.milliseconds) > 0)
				return RX_OK;
		}
	}
	return RX_ERROR;

}

// DOVDE JE KOD!!!
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


