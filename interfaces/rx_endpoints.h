

/****************************************************************************
*
*  interfaces\rx_endpoints.h
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


#ifndef rx_endpoints_h
#define rx_endpoints_h 1


#include "protocols/ansi_c/common_c/rx_protocol_base.h"


/////////////////////////////////////////////////////////////
// logging macros for host library
#define ITF_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Interface",src,lvl,msg)
#define ITF_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Interface",src,lvl,msg)
#define ITF_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Interface",src,lvl,msg)
#define ITF_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Interface",src,lvl,msg)
#define ITF_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Interface",src,lvl,msg)
#define ITF_LOG_TRACE(src,lvl,msg) RX_TRACE("Interface",src,lvl,msg)

// rx_port_types
#include "system/runtime/rx_port_types.h"
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
	typedef std::map<string_type, rx_protocol_stack_entry*> endpoints_type;
	typedef std::map<string_type, runtime::io_types::physical_port::smart_ptr> physical_ports_type;

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

      physical_ports_type physical_ports_;


};






class rx_io_endpoint : public rx_protocol_stack_entry  
{
public:

	typedef rx_io_endpoint* smart_ptr;

  public:
      rx_io_endpoint();

      ~rx_io_endpoint();


  protected:

  private:


};







template <typename defT>
class rx_io_address 
{

  public:
      rx_io_address();

      rx_io_address (size_t value_size);


      protocol_endpoint* get_endpoint ();

      const protocol_endpoint* get_endpoint () const;

	  ~rx_io_address() = default;
  protected:

  private:


      uint8_t value_[sizeof(defT)];


};


// Parameterized Class rx_internal::interfaces::io_endpoints::rx_io_address 

template <typename defT>
rx_io_address<defT>::rx_io_address()
{
}

template <typename defT>
rx_io_address<defT>::rx_io_address (size_t value_size)
{
	rx_init_endpoint(get_endpoint(), value_size);
}



template <typename defT>
protocol_endpoint* rx_io_address<defT>::get_endpoint ()
{
	return reinterpret_cast<protocol_endpoint*>(this);
}

template <typename defT>
const protocol_endpoint* rx_io_address<defT>::get_endpoint () const
{
	return reinterpret_cast<protocol_endpoint*>(this);
}


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
