

/****************************************************************************
*
*  rx_platform.h
*
*  Copyright (c) 2018-2019 Dusan Ciric
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_platform_h
#define rx_platform_h 1


/*******************************************************************
 Preprocessor switches
 ======================

 #define PYTHON_SUPPORT			- includes the python support
 #define EXCLUDE_TEST_CODE		- exclude testing code in build
 #define RX_LOG_SOURCE_INFO		- log full source code in log
 #define RX_MIN_MEMORY          - remove some hash containers to gain smaller memory footprint
 
********************************************************************/

#include "rx_library.h"
#include "version/rx_code_macros.h"
#include "system/meta/rx_obj_types.h"


#define RX_LICENSE_MESSAGE "\
\r\n\
Copyright(C) 2018 - 2019  Dusan Ciric\r\n\r\n\
This program comes with ABSOLUTELY NO WARRANTY.\r\n\
This is free software, and you are welcome to redistribute it\r\n\
under certain conditions; type `license' for details.\r\n\
\r\n"




#endif
