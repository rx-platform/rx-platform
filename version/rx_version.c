

/****************************************************************************
*
*  version\rx_version.c
*
*  Copyright (c) 2017 Dusan Ciric
*
*
*  This file is part of rx-platform
*
*
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/



#define _CRT_SECURE_NO_WARNINGS

// rx_version
#include "version/rx_version.h"




#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

void create_module_compile_time_string(const char* date, const char* time,char* buffer)
{

    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    char* time_ptr= (char*)time;

    char* endptr=time_ptr;
    hour = strtol(time_ptr,&endptr,10);
	time_ptr = endptr;
	time_ptr++;
    minute = strtol(time_ptr,&endptr,10);
	time_ptr = endptr;

	char str[4];
	memcpy(str, date, 3);
	str[3] = '\0';


	char* date_ptr = (char*)date;
	day = strtol(&date_ptr[3], &endptr, 10);
	date_ptr = endptr;
	date_ptr++;
	year = strtol(date_ptr, &endptr, 10);

    if (strcmp(str , "Jan") == 0)
        month = 1;
    else if (strcmp(str , "Feb") == 0)
        month = 2;
	else if (strcmp(str , "Mar") == 0)
        month = 3;
	else if (strcmp(str , "Apr") == 0)
        month = 4;
	else if (strcmp(str , "May") == 0)
        month = 5;
	else if (strcmp(str , "Jun") == 0)
        month = 6;
	else if (strcmp(str , "Jul") == 0)
        month = 7;
	else if (strcmp(str , "Aug") == 0)
        month = 8;
	else if (strcmp(str , "Sep") == 0)
        month = 9;
	else if (strcmp(str , "Oct") == 0)
        month = 10;
	else if (strcmp(str , "Nov") == 0)
        month = 11;
	else if (strcmp(str , "Dec") == 0)
        month = 12;

    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
        year, month,
        day, hour, minute, minute);
}
void create_module_version_string(const char* prefix, int major, int minor, int build, const char* date, const char* time, char* buffer)
{


	int year = 0;
	int month = 0;
	int day = 0;


	char* date_ptr = (char*)date;

	char* endptr = date_ptr;

	day = strtol(&date_ptr[3], &endptr, 10);
	date_ptr = endptr;
	date_ptr++;
	year = strtol(date_ptr, &endptr, 10);


	char str[4];
	memcpy(str, date, 3);
	str[3] = '\0';

	if (strcmp(str, "Jan") == 0)
		month = 1;
	else if (strcmp(str, "Feb") == 0)
		month = 2;
	else if (strcmp(str, "Mar") == 0)
		month = 3;
	else if (strcmp(str, "Apr") == 0)
		month = 4;
	else if (strcmp(str, "May") == 0)
		month = 5;
	else if (strcmp(str, "Jun") == 0)
		month = 6;
	else if (strcmp(str, "Jul") == 0)
		month = 7;
	else if (strcmp(str, "Aug") == 0)
		month = 8;
	else if (strcmp(str, "Sep") == 0)
		month = 9;
	else if (strcmp(str, "Oct") == 0)
		month = 10;
	else if (strcmp(str, "Nov") == 0)
		month = 11;
	else if (strcmp(str, "Dec") == 0)
		month = 12;

	sprintf(buffer, "%s Ver %d.%d.%d.%d%04d", prefix,
		major,minor,build,
		(year), month * 100 + day);


}




#ifdef __cplusplus
}
#endif




