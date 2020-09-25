

/****************************************************************************
*
*  system\meta\rx_meta_attr_algorithm.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef rx_meta_attr_algorithm_h
#define rx_meta_attr_algorithm_h 1



// rx_meta_support
#include "system/meta/rx_meta_support.h"
// rx_def_blocks
#include "system/meta/rx_def_blocks.h"

using namespace meta::def_blocks;


namespace rx_platform {

namespace meta {

namespace meta_algorithm {





class mapped_data_algorithm 
{

  public:

      static rx_result serialize_complex_attribute (const mapped_data_type& whose, base_meta_writer& stream);

      static rx_result deserialize_complex_attribute (mapped_data_type& whose, base_meta_reader& stream, complex_data_type& complex_data);

      static bool check_complex_attribute (mapped_data_type& whose, type_check_context& ctx);

      static rx_result construct_complex_attribute (const mapped_data_type& whose, const names_cahce_type& names, construct_context& ctx);


  protected:

  private:


};






class variable_data_algorithm 
{

  public:

      static rx_result serialize_complex_attribute (const variable_data_type& whose, base_meta_writer& stream);

      static rx_result deserialize_complex_attribute (variable_data_type& whose, base_meta_reader& stream, complex_data_type& complex_data);

      static bool check_complex_attribute (variable_data_type& whose, type_check_context& ctx);

      static rx_result construct_complex_attribute (const variable_data_type& whose, const names_cahce_type& names, construct_context& ctx);


  protected:

  private:


};







class complex_data_algorithm 
{

  public:

      static rx_result serialize_complex_attribute (const complex_data_type& whose, base_meta_writer& stream);

      static rx_result deserialize_complex_attribute (complex_data_type& whose, base_meta_reader& stream);

      static bool check_complex_attribute (complex_data_type& whose, type_check_context& ctx);

      static rx_result construct_complex_attribute (const complex_data_type& whose, construct_context& ctx);


  protected:

  private:


};







template <class typeT>
class meta_blocks_algorithm 
{

  public:

      static rx_result serialize_complex_attribute (const typeT& whose, base_meta_writer& stream);

      static rx_result deserialize_complex_attribute (typeT& whose, base_meta_reader& stream);

      static bool check_complex_attribute (typeT& whose, type_check_context& ctx);

      static rx_result construct_complex_attribute (const typeT& whose, construct_context& ctx);


  protected:

  private:


};


} // namespace meta_algorithm
} // namespace meta
} // namespace rx_platform



#endif
