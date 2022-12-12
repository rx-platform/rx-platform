

/****************************************************************************
*
*  upython\upythonc.h
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


#ifndef upythonc_h
#define upythonc_h 1




#include <windows.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

#ifdef __cplusplus
extern "C" {
#endif
	
typedef int (*pget_modules)(int timeout, size_t* count, mp_obj_t** objs);
typedef void (*pmodule_done)(mp_obj_t result);
typedef mp_obj_t(*pmodule_read_tag)(const char* path);
typedef mp_obj_t(*pmodule_write_tag)(const char* path, mp_obj_t what);

typedef struct host_data_str
{
	HANDLE hevent;
	pget_modules modules_callback;
	pmodule_done module_done;
	pmodule_read_tag module_read;
	pmodule_write_tag module_write;

} host_data;
	
void register_host(host_data* data);

#ifdef __cplusplus
}
#endif







#endif
