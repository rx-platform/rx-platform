

/****************************************************************************
*
*  interfaces\rx_endpoints.h
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


#ifndef rx_endpoints_h
#define rx_endpoints_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"


/////////////////////////////////////////////////////////////
// logging macros for host library
#define ITF_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Interface",src,lvl,msg)
#define ITF_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Interface",src,lvl,msg)
#define ITF_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Interface",src,lvl,msg)
#define ITF_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Interface",src,lvl,msg)
#define ITF_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Interface",src,lvl,msg)
#define ITF_LOG_TRACE(src,lvl,msg) RX_TRACE("Interface",src,lvl,msg)

// rx_objbase
#include "system/runtime/rx_objbase.h"
// dummy
#include "dummy.h"

#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"

using namespace rx_platform;
using namespace rx_platform::runtime;


namespace rx_internal {

namespace interfaces {

namespace io_endpoints {





class rx_io_manager : public rx_platform::runtime::items::object_runtime  
{
	typedef std::map<string_type, rx_protocol_stack_endpoint*> endpoints_type;
	typedef std::map<string_type, rx_port_ptr> physical_ports_type;

  public:
      rx_io_manager();

      ~rx_io_manager();


      rx_result initialize (hosting::rx_platform_host* host, io_manager_data_t& data);

      void deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const io_manager_data_t& data);

      void stop ();


  protected:

  private:


      //	These endpoints include COM ports, Console and UDP
      //	communications
      endpoints_type endpoints_;


};


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
