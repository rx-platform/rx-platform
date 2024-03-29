

/****************************************************************************
*
*  sys_internal\rx_namespace_algorithms.h
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


#ifndef rx_namespace_algorithms_h
#define rx_namespace_algorithms_h 1



// rx_ns
#include "system/server/rx_ns.h"



namespace rx_internal {

namespace internal_ns {






class namespace_algorithms 
{

  public:

      static rx_result_with<rx_directory_ptr> get_or_create_direcotry (rx_directory_ptr from, const string_type& path);

      static rx_result translate_path (const string_type& ref_path, const string_type& rel_path, string_type& result);

      static rx_result recursive_remove_directories (rx_directory_ptr from, const string_type& path);


  protected:

  private:


};


} // namespace internal_ns
} // namespace rx_internal



#endif
