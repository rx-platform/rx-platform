

/****************************************************************************
*
*  upython\upython.h
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


#ifndef upython_h
#define upython_h 1


#ifdef UPYTHON_SUPPORT
#include "system/server/rx_server.h"


/////////////////////////////////////////////////////////////
// logging macros
#define UPYTHON_LOG_INFO(src,lvl,msg) RX_LOG_INFO("upython",src,lvl,msg)
#define UPYTHON_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("upython",src,lvl,msg)
#define UPYTHON_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("upython",src,lvl,msg)
#define UPYTHON_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("upython",src,lvl,msg)
#define UPYTHON_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("upython",src,lvl,msg)
#define UPYTHON_LOG_TRACE(src,lvl,msg) RX_TRACE("upython",src,lvl,msg)



namespace rx_platform {
namespace python {
class upy_module;

} // namespace python
} // namespace rx_platform




namespace rx_platform {
namespace hosting
{
class rx_platform_host;
}

namespace python {
typedef rx_reference<upy_module> upy_module_ptr;






class upython 
{

  public:
      upython();

      ~upython();


      static rx_result register_logic_handlers ();

      static rx_result start_script (hosting::rx_platform_host* host, const rx_platform::configuration_data_t& data);

      static void stop_script ();


  protected:

  private:


};


} // namespace python
} // namespace rx_platform

#endif


#endif
