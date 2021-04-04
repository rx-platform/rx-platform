

/****************************************************************************
*
*  system\libraries\rx_plugin.h
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


#ifndef rx_plugin_h
#define rx_plugin_h 1




#include "system/storage_base/rx_storage.h"
#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_objbase.h"

/////////////////////////////////////////////////////////////
// logging macros for console library
#define PLUGIN_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Plugins",src,lvl,msg)
#define PLUGIN_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Plugins",src,lvl,msg)
#define PLUGIN_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Plugins",src,lvl,msg)
#define PLUGIN_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Plugins",src,lvl,msg)
#define PLUGIN_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Plugins",src,lvl,msg)
#define PLUGIN_LOG_TRACE(src,lvl,msg) RX_TRACE("Plugins",src,lvl,msg)


namespace rx_platform {

namespace library {





class plugin_builder 
{

  public:

      rx_directory_ptr plugin_root;


  protected:

  private:


};







class rx_plugin_base 
{

  public:
      rx_plugin_base();

      virtual ~rx_plugin_base();


      virtual string_type get_plugin_info () = 0;

      virtual rx_result init_plugin () = 0;

      virtual rx_result deinit_plugin () = 0;

      virtual rx_result build_plugin (plugin_builder& builder) = 0;

      virtual string_type get_plugin_name () = 0;


  protected:

  private:


};



//	I just love singletones



class plugin_manager 
{
    typedef std::map<rx_node_id, rx_plugin_base*> registered_plugins_type;


    template<typename typeT>
    friend rx_result register_plugin_constructor(rx_plugin_base* plugin
        , const rx_node_id& id, std::function<typename typeT::RImplPtr()> f);
    template<typename typeT>
    friend rx_result register_plugin_simple_constructor(rx_plugin_base* plugin
        , const rx_node_id& id, std::function<typename typeT::RTypePtr()> f);

  public:

      static plugin_manager& instance ();


  protected:

  private:
      plugin_manager();



      registered_plugins_type registered_plugins_;


};


} // namespace library
} // namespace rx_platform

namespace rx_platform
{
typedef library::rx_plugin_base* rx_plugin_ptr;
}


#endif
