

/****************************************************************************
*
*  sys_internal\rx_security\rx_aspnet_security.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef rx_aspnet_security_h
#define rx_aspnet_security_h 1



// rx_security
#include "security/rx_security.h"
// rx_platform_security
#include "sys_internal/rx_security/rx_platform_security.h"



namespace rx_internal {

namespace rx_security {

namespace aspnet_forms {





class aspnet_security_provider : public platform_security_provider  
{

  public:

      const string_type& get_name ();

      string_type get_info ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();

      rx_result_with<security::security_context_ptr> create_host_context (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result_with<security::security_context_ptr> create_system_context (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result_with<security::security_context_ptr> create_world_context (hosting::rx_platform_host* host, configuration_data_t& data);

      static string_type logon (const string_type& user, const string_type& pass);

      static bool logoff ();

      rx_result start (hosting::rx_platform_host* host);

      void stop ();


  protected:

  private:


      static std::atomic<bool> initialized_;


};






class aspnet_security_context : public rx_platform::security::security_context  
{
    DECLARE_REFERENCE_PTR(aspnet_security_context);

  public:
      aspnet_security_context();

      aspnet_security_context (const string_view_type name, const string_view_type host, const string_view_type cookie, bool system);

      ~aspnet_security_context();


      bool is_authenticated () const;

      virtual rx_result serialize (base_meta_writer& stream) const;

      virtual rx_result deserialize (base_meta_reader& stream);

      rx_result initialize (const string_view_type name, const string_view_type cookie, bool system);

      bool has_console () const;

      bool is_in_role (string_view_type role, security_mask_t access) const;


  protected:

  private:


      string_type token_;


};


} // namespace aspnet_forms
} // namespace rx_security
} // namespace rx_internal



#endif
