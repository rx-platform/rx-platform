

/****************************************************************************
*
*  sys_internal\rx_security\rx_platform_roles.h
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


#ifndef rx_platform_roles_h
#define rx_platform_roles_h 1




#include "security/rx_security.h"
#include "system/hosting/rx_host.h"
using namespace rx_platform;


namespace rx_internal {

namespace rx_security {





class platform_principal_ref 
{

  public:

      string_type name;

      byte_string opaque;


  protected:

  private:


};


class platform_roles;




class platform_role 
{
    struct role_permission
    {
        string_type path;
        security_mask_t allow_access;
        security_mask_t deny_access;
    };

    typedef std::map<string_type, platform_principal_ref> users_type;
    typedef std::vector<role_permission> permissions_type;
    friend class platform_roles;


  public:
      platform_role();


      bool is_in_role (security::security_context_ptr& ctx);

      std::optional<bool> check_permissions (security_mask_t mask, const string_type& obj_path, security::security_context_ptr ctx);

      rx_result serialize_role (base_meta_writer& stream);

      rx_result deserialize_role (base_meta_reader& stream);


  protected:

  private:


      users_type users_;

      platform_principal_ref denied_users_;


      permissions_type permissions_;


};






class platform_roles 
{
    typedef std::vector<platform_role> roles_type;
    typedef std::map<string_type, std::vector<platform_role*> > user_roles_cache_type;

  public:

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();

      rx_result initialize_roles (std::vector<rx_roles_storage_item_ptr> storages);

      bool check_permissions (security_mask_t mask, const string_type& path, security::security_context_ptr ctx);


  protected:

  private:

      std::vector<platform_role*>& get_roles_for_user (security::security_context_ptr& ctx);

      void invalidate_cache ();



      roles_type roles_;


      locks::slim_lock roles_lock_;

      user_roles_cache_type user_roles_cache_;


};


} // namespace rx_security
} // namespace rx_internal



#endif
