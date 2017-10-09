

/****************************************************************************
*
*  version\rx_version.h
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


#ifndef rx_version_h
#define rx_version_h 1






#define ASSIGN_MODULE_VERSION(val,arg)  \
char temp_str[0x100];\
create_module_version_string(arg,__DATE__,__TIME__,temp_str);\
val=temp_str;\

#ifdef __cplusplus
extern "C" {
#endif


// versions helper
void create_module_compile_time_string(const char* date, const char* time, char* buffer);
void create_module_version_string(const char* prefix, const char* date, const char* time, char* buffer);


#ifdef __cplusplus
}
#endif






#endif
