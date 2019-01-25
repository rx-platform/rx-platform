

/****************************************************************************
*
*  interfaces\rx_endpoints.h
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


#ifndef rx_endpoints_h
#define rx_endpoints_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"

#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"

using namespace rx_platform;
using namespace rx_platform::runtime;


namespace interfaces {

namespace io_endpoints {





class rx_io_endpoint 
{
public:

	typedef rx_io_endpoint* smart_ptr;

  public:
      rx_io_endpoint();

      ~rx_io_endpoint();


  protected:

  private:


};






class physical_port : public rx_platform::runtime::object_types::port_runtime  
{
	DECLARE_CODE_INFO("rx", 0, 0, 1, "\
physical port class. basic implementation of a port");

	DECLARE_REFERENCE_PTR(physical_port);


  public:
      physical_port (object_types::port_creation_data&& data);


  protected:

  private:


      rx_io_endpoint *my_endpoint_;


};






class rx_io_manager : public rx_platform::runtime::object_types::server_object  
{
	typedef std::map<string_type, rx_io_endpoint::smart_ptr> endpoints_type;
	typedef std::map<string_type, physical_port::smart_ptr> physical_ports_type;

  public:
      rx_io_manager();

      virtual ~rx_io_manager();


      uint32_t initialize (hosting::rx_platform_host* host, io_manager_data_t& data);

      uint32_t deinitialize ();

      uint32_t start (hosting::rx_platform_host* host, const io_manager_data_t& data);

      uint32_t stop ();


  protected:

  private:


      //	These endpoints include COM ports, Console and UDP
      //	communications
      endpoints_type endpoints_;

      physical_ports_type physical_ports_;


};


} // namespace io_endpoints
} // namespace interfaces



#endif
