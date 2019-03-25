

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


/////////////////////////////////////////////////////////////
// logging macros for building
#define BUILD_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Build",src,lvl,msg)
#define BUILD_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Build",src,lvl,msg)
#define BUILD_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Build",src,lvl,msg)
#define BUILD_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Build",src,lvl,msg)
#define BUILD_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Build",src,lvl,msg)
#define BUILD_LOG_TRACE(src,lvl,msg) RX_TRACE("Build",src,lvl,msg)

// rx_internal_builders
#include "sys_internal/rx_internal_builders.h"

#include "system/storage_base/rx_storage.h"


namespace sys_internal {

namespace builders {

namespace storage {





class configuration_storage_builder : public rx_platform_builder  
{

  public:
      configuration_storage_builder (meta::rx_storage_type storage_type);

      ~configuration_storage_builder();


      rx_result do_build (platform_root::smart_ptr root);


  protected:

      rx_result build_from_storage (platform_root::smart_ptr root, rx_platform::storage_base::rx_platform_storage& storage, meta::rx_storage_type storage_type);


  private:

      rx_result create_object_from_storage (base_meta_reader& stream, rx_storage_item_ptr&& storage, meta::rx_storage_type storage_type, platform_root::smart_ptr root);

      rx_result create_type_from_storage (base_meta_reader& stream, rx_storage_item_ptr&& storage, meta::rx_storage_type storage_type, platform_root::smart_ptr root);



      meta::rx_storage_type storage_type_;

	  template<class T>
	  rx_result create_concrete_type_from_storage(meta::meta_data& meta_data, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>);
	  template<class T>
	  rx_result create_concrete_simple_type_from_storage(meta::meta_data& meta_data, base_meta_reader& stream, rx_directory_ptr dir, rx_storage_item_ptr&& storage, tl::type2type<T>);
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
