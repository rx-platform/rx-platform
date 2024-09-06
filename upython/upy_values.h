

/****************************************************************************
*
*  upython\upy_values.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef upy_values_h
#define upy_values_h 1


#ifdef UPYTHON_SUPPORT
#include "upy_internal.h"




namespace rx_platform {

namespace python {





class upy_convertor 
{

  public:

      static rx_result upy_to_simple (mp_obj_t py, values::rx_simple_value& val);

      static rx_result upy_to_data (mp_obj_t py, data::runtime_values_data data);

      static rx_result value_to_upy (const values::rx_value& val, mp_obj_t* py);


  protected:

  private:


};


} // namespace python
} // namespace rx_platform

#endif


#endif
