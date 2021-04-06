

/****************************************************************************
*
*  system\meta\rx_meta_algorithm.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_meta_algorithm_h
#define rx_meta_algorithm_h 1



// rx_obj_types
#include "system/meta/rx_obj_types.h"

namespace rx_platform {
namespace meta {
namespace basic_types {
class data_type;

} // namespace basic_types
} // namespace meta
} // namespace rx_platform


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
using namespace rx_platform::meta::object_types;
using namespace rx_platform::runtime::blocks;
using rx_platform::meta::basic_types::data_type;


namespace rx_platform {
namespace runtime
{
namespace relations
{
class relation_data;
}
}

namespace meta {
	namespace def_blocks
	{
		class variable_attribute;
	}

namespace meta_algorithm {






class data_types_algorithm 
{

  public:

      static rx_result serialize_type (const data_type& whose, base_meta_writer& stream, uint8_t type);

      static rx_result deserialize_type (data_type& whose, base_meta_reader& stream, uint8_t type);

      static bool check_type (data_type& whose, type_check_context& ctx);

      static rx_result construct_runtime (const data_type& whose, data_blocks_prototype& what, construct_context& ctx);


  protected:

  private:


};






template <class typeT>
class basic_types_algorithm 
{

  public:

      static rx_result serialize_type (const typeT& whose, base_meta_writer& stream, uint8_t type);

      static rx_result deserialize_type (typeT& whose, base_meta_reader& stream, uint8_t type);

      static bool check_type (typeT& whose, type_check_context& ctx);

      static rx_result construct (const typeT& whose, construct_context& ctx, typename typeT::RDataType& prototype);


  protected:

  private:


};






template <class typeT>
class object_types_algorithm 
{

  public:

      static rx_result serialize_type (const typeT& whose, base_meta_writer& stream, uint8_t type);

      static rx_result deserialize_type (typeT& whose, base_meta_reader& stream, uint8_t type);

      static bool check_type (typeT& whose, type_check_context& ctx);

      static rx_result construct_runtime (const typeT& whose, typename typeT::RTypePtr what, construct_context& ctx);


  protected:

  private:


};







class relation_type_algorithm 
{

  public:

      static rx_result serialize_type (const relation_type& whose, base_meta_writer& stream, uint8_t type);

      static rx_result deserialize_type (relation_type& whose, base_meta_reader& stream, uint8_t type);

      static bool check_type (relation_type& whose, type_check_context& ctx);

      static rx_result construct_runtime (const relation_type& whose, relation_type::RTypePtr what, construct_context& ctx);


  protected:

  private:


};







template <class typeT>
class object_data_algorithm 
{

  public:

      static rx_result serialize_object_data (const object_types::object_data_type& whose, base_meta_writer& stream);

      static rx_result deserialize_object_data (object_types::object_data_type& whose, base_meta_reader& stream, complex_data_type& complex_data);

      static bool check_object_data (object_types::object_data_type& whose, type_check_context& ctx);

      static rx_result construct_object_data (const object_types::object_data_type& whose, typename typeT::RTypePtr what, const names_cahce_type& names, construct_context& ctx);


  protected:

  private:


};







class relation_blocks_algorithm 
{

  public:

      static rx_result serialize_relation_attribute (const object_types::relation_attribute& whose, base_meta_writer& stream);

      static rx_result deserialize_relation_attribute (object_types::relation_attribute& whose, base_meta_reader& stream);

      static bool check_relation_attribute (object_types::relation_attribute& whose, type_check_context& ctx);

      static rx_result construct_relation_attribute (const object_types::relation_attribute& whose, runtime::relations::relation_data& data, construct_context& ctx);


  protected:

  private:


};


} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform



#endif
