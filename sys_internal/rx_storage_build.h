

/****************************************************************************
*
*  sys_internal\rx_storage_build.h
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


#ifndef rx_storage_build_h
#define rx_storage_build_h 1



// rx_internal_builders
#include "sys_internal/rx_internal_builders.h"

#include "system/storage_base/rx_storage.h"


namespace sys_internal {

namespace builders {

namespace storage {





class configuration_storage_builder : public rx_platform_builder  
{

  public:
      configuration_storage_builder (rx_storage_ptr storage);

      ~configuration_storage_builder();


      rx_result do_build (rx_directory_ptr root);


  protected:

      rx_result build_from_storage (rx_directory_ptr root, rx_platform::storage_base::rx_platform_storage& storage);


  private:

      rx_result create_object_from_storage (base_meta_reader& stream, rx_storage_item_ptr&& storage, rx_directory_ptr root);

      rx_result create_type_from_storage (base_meta_reader& stream, rx_storage_item_ptr&& storage, rx_directory_ptr root);

      void dump_errors_to_log (const string_array& errors);



      rx_storage_ptr storage_;

	  template<class T>
	  rx_result create_concrete_type_from_storage(meta::meta_data& meta_data, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>);
	  template<class T>
	  rx_result create_concrete_simple_type_from_storage(meta::meta_data& meta_data, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>);
	  template<class T>
	  rx_result create_concrete_object_from_storage(meta::meta_data& meta_data, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>);
};






class directory_creator 
{

  public:

      rx_result_with<rx_directory_ptr> get_or_create_direcotry (rx_directory_ptr from, const string_type& path);


  protected:

  private:


};


} // namespace storage
} // namespace builders
} // namespace sys_internal



#endif
