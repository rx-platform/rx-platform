

/****************************************************************************
*
*  system\server\rx_endpoints.h
*
*  Copyright (c) 2018 Dusan Ciric
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



// rx_io
#include "lib/rx_io.h"
// rx_objbase
#include "system/meta/rx_objbase.h"

#include "system/hosting/rx_host.h"


namespace rx_platform {

namespace io_endpoints {





class rx_io_endpoint : public rx::io::dispatcher_subscriber  
{
	DECLARE_REFERENCE_PTR(rx_io_endpoint);

  public:
      rx_io_endpoint();

      ~rx_io_endpoint();


  protected:

  private:


};


struct io_manager_data_t
{
	io_manager_data_t()
	{
		memzero(this, sizeof(io_manager_data_t));
	}
};




class rx_io_manager : public objects::object_types::server_object  
{
	typedef std::map<string_type, rx_io_endpoint::smart_ptr> endpoints_type;

  public:
      rx_io_manager();

      virtual ~rx_io_manager();


      uint32_t initialize (hosting::rx_platform_host* host, io_manager_data_t& data);

      uint32_t deinitialize ();

      uint32_t start (hosting::rx_platform_host* host, const io_manager_data_t& data);

      uint32_t stop ();


  protected:

  private:


      endpoints_type endpoints_;


};


} // namespace io_endpoints
} // namespace rx_platform



#endif
