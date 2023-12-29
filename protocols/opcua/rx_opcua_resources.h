

/****************************************************************************
*
*  protocols\opcua\rx_opcua_resources.h
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


#ifndef rx_opcua_resources_h
#define rx_opcua_resources_h 1


#include "system/hosting/rx_host.h"

// rx_opcua_server
#include "protocols/opcua/rx_opcua_server.h"
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


struct registered_endpoint_data
{
    string_type url;
    byte_string certificate;
    security_mode_t security_mode = security_mode_t::none;
    string_type policy_uri;
    string_type transport_profile_uri;
    uint8_t security_level = 0;
};




class opcua_resources_repository 
{
    typedef std::map<uint32_t, opcua_security_channel::smart_ptr> server_channels_type;
    typedef std::map<uint32_t, opcua_session::smart_ptr> server_sessions_type;

    typedef std::map<string_type, std::pair<registered_endpoint_data, std::map<string_type, opcua_server_endpoint_data> > > server_endpoints_type;

  public:

      static opcua_resources_repository& instance ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();

      rx_result register_server (const string_type& port_path, const application_description& data);

      void unregister_server (const string_type& port_path, const string_type& path);

      application_descriptions_type find_servers (const string_type& ep_url, const string_type& port_path);

      rx_result register_endpoint (const string_type& port_path, registered_endpoint_data data);

      void unregister_endpoint (const string_type& port_path);

      void fill_endpoint_description (endpoint_description& descr, const string_type& port_path);


  protected:

  private:


      server_channels_type server_channels_;

      server_sessions_type server_sessions_;

      server_endpoints_type server_endpoints_;


      locks::slim_lock resources_lock_;


};


} // namespace opcua
} // namespace protocols



#endif
