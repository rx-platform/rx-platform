

/****************************************************************************
*
*  system\runtime\rx_rt_func.h
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_rt_func_h
#define rx_rt_func_h 1




#include "lib/rx_ser_lib.h"
using namespace rx;


namespace rx_platform {

namespace runtime {

namespace blocks {

namespace func {





class runtime_item 
{
  public:
	  typedef std::unique_ptr<runtime_item> smart_ptr;

  public:

      virtual bool serialize (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const = 0;

      virtual bool deserialize (base_meta_reader& stream, uint8_t type) = 0;


  protected:

  private:


};


} // namespace func
} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
