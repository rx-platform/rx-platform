

/****************************************************************************
*
*  system\meta\rx_meta_algorithm.h
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


#ifndef rx_meta_algorithm_h
#define rx_meta_algorithm_h 1



// rx_meta_support
#include "system/meta/rx_meta_support.h"

namespace rx_platform
{
	namespace meta
	{
		namespace def_blocks
		{
			class variable_attribute;
		}
	}
}
#include "system/runtime/rx_blocks.h"
using namespace rx;
using namespace rx_platform::meta;
using namespace rx_platform::meta::def_blocks;
using namespace rx_platform::runtime::blocks;


namespace rx_platform {

namespace meta {
	namespace def_blocks
	{
		class variable_attribute;
	}

namespace meta_algorithm {






template <class typeT>
class meta_blocks_algorithm 
{

  public:

      static rx_result serialize_complex_attribute (const typeT& whose, base_meta_writer& stream);

      static rx_result deserialize_complex_attribute (typeT& whose, base_meta_reader& stream);

      static bool check_complex_attribute (typeT& whose, type_check_context& ctx);

      static rx_result construct_complex_attribute (const typeT& whose, construct_context& ctx);

      static rx_result resolve_complex_attribute (typeT& whose, rx_directory_ptr dir);


  protected:

  private:


};






template <class typeT>
class basic_types_algorithm 
{

  public:

      static rx_result serialize_basic_type (const typeT& whose, base_meta_writer& stream, uint8_t type);

      static rx_result deserialize_basic_type (typeT& whose, base_meta_reader& stream, uint8_t type);

      static bool check_basic_type (typeT& whose, type_check_context& ctx);

      static rx_result construct_basic_type (const typeT& whose, construct_context& ctx);

      static rx_result resolve_basic_type (typeT& whose, rx_directory_ptr dir);


  protected:

  private:


};






template <class typeT>
class object_types_algorithm 
{

  public:

      static rx_result serialize_object_type (const typeT& whose, base_meta_writer& stream, uint8_t type);

      static rx_result deserialize_object_type (typeT& whose, base_meta_reader& stream, uint8_t type);

      static bool check_object_type (typeT& whose, type_check_context& ctx);

      static rx_result construct_object (const typeT& whose, typename typeT::RTypePtr what, construct_context& ctx);

      static rx_result resolve_object_type (typeT& whose, rx_directory_ptr dir);


  protected:

  private:


};


} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform



#endif
