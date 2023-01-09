

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_headless.h
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


#ifndef rx_headless_h
#define rx_headless_h 1






#ifdef RX_HEADLESS_EXPORTS
#define RX_HEADLESS_API __declspec(dllexport) 
#else
#define RX_HEADLESS_API __declspec(dllimport) 
#endif

#ifdef __cplusplus
extern "C" {
#endif

	RX_HEADLESS_API int rx_headless_init_platform(int argc, char* argv[], const char* help_name, const char* host_name, const char* local_dir_override);
	RX_HEADLESS_API int rx_headless_start_platform();
	RX_HEADLESS_API int rx_headless_stop_platform();
	RX_HEADLESS_API int rx_headless_deinit_platform();


#ifdef __cplusplus
}
#endif





#endif
