

/****************************************************************************
*
*  lib\rx_iso_8601.h
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


#ifndef rx_iso_8601_h
#define rx_iso_8601_h 1





////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// VAZNO!!!!! //////////////////////////////////////////////////////////////////////////////////////////
// OVDE PISI KOD!!!
// objasnicu ti zasto kad se vidimo sledeci put, sorry moram da ti pokazem

#define RX_ISO_8601_NO_TIME 1
#define RX_ISO_8601_NO_SECONDS 8
#define RX_ISO_8601_NO_MILLISECONDS 16


rx_time_struct iso_8601_to_rx_time(const char* iso_8601_str);
int rx_time_to_iso_8601(const rx_time_struct* rx_time, char* buffer, size_t buff_len, int options = 0);





// DOVDE JE KOD!!!
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////





#endif
