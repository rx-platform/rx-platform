

/****************************************************************************
*
*  protocols\opcua\rx_opcua_resources.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_opcua_resources_h
#define rx_opcua_resources_h 1


#include "system/hosting/rx_host.h"

// rx_ptr
#include "lib/rx_ptr.h"



namespace protocols {

namespace opcua {





class opcua_security_channel : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(opcua_security_channel);

  public:

  protected:

  private:


};






class opcua_session : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(opcua_session);

  public:

  protected:

  private:


};






class opcua_resources_repository 
{
    typedef std::map<uint32_t, opcua_security_channel::smart_ptr> server_channels_type;
    typedef std::map<uint32_t, opcua_session::smart_ptr> server_sessions_type;

  public:

      static opcua_resources_repository& instance ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();


  protected:

  private:


      server_channels_type server_channels_;

      server_sessions_type server_sessions_;


      locks::slim_lock resources_lock_;


};


} // namespace opcua
} // namespace protocols



#endif
