

/****************************************************************************
*
*  sys_internal\rx_security\rx_platform_security.h
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


#ifndef rx_platform_security_h
#define rx_platform_security_h 1



// rx_platform_roles
#include "sys_internal/rx_security/rx_platform_roles.h"
// rx_security
#include "security/rx_security.h"

#include "system/hosting/rx_host.h"

#define RX_NONE_SECURITY_NAME "none"
#define RX_CERT_SECURITY_NAME "x509"


namespace rx_internal {

namespace rx_security {





class built_in_security_context : public rx_platform::security::security_context  
{
    DECLARE_REFERENCE_PTR(built_in_security_context);

  public:
      built_in_security_context();

      ~built_in_security_context();


      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


      const string_type& get_description () const
      {
        return description_;
      }



  protected:

      string_type description_;


  private:


};






class maintenance_context : public built_in_security_context  
{
    DECLARE_REFERENCE_PTR(maintenance_context);

  public:
      maintenance_context();

      ~maintenance_context();


      bool has_console () const;


  protected:

  private:


};






class host_security_context : public built_in_security_context  
{
    DECLARE_REFERENCE_PTR(host_security_context);

  public:
      host_security_context();

      ~host_security_context();


      bool is_system () const;

      bool is_hosted () const;


  protected:

  private:


};






class process_context : public built_in_security_context  
{

  public:
      process_context (const string_type& name = "", bool sys = false);

      ~process_context();


      bool is_system () const;

      bool has_console () const;

      bool is_interactive () const;


  protected:

  private:


      bool system_;


};






class platform_security_provider 
{

  public:

      virtual const string_type& get_name () = 0;

      virtual string_type get_info () = 0;

      virtual rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data) = 0;

      virtual void deinitialize () = 0;

      virtual rx_result_with<security::security_context_ptr> create_host_context (hosting::rx_platform_host* host, configuration_data_t& data) = 0;

      virtual rx_result_with<security::security_context_ptr> create_system_context (hosting::rx_platform_host* host, configuration_data_t& data) = 0;

      virtual rx_result_with<security::security_context_ptr> create_world_context (hosting::rx_platform_host* host, configuration_data_t& data) = 0;

      platform_security_provider() = default;
      platform_security_provider(const platform_security_provider&) = delete;
      platform_security_provider(platform_security_provider&&) = delete;
      virtual ~platform_security_provider() = default;
  protected:

  private:


};






class platform_security 
{
    typedef std::map<string_type, std::unique_ptr<platform_security_provider> > providers_type;

  public:
      ~platform_security();


      static platform_security& instance ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result initialize_roles (std::vector<rx_roles_storage_item_ptr> storages);

      void deinitialize ();

      rx_result register_role (const string_type& role, const string_type& parent_role, hosting::rx_platform_host* host);

      rx_result register_provider (std::unique_ptr<platform_security_provider>  who, hosting::rx_platform_host* host, configuration_data_t& data);

      platform_security_provider* get_provider (const string_type& name);

      bool check_permissions (security::security_mask_t mask, const string_type& path, security::security_context_ptr ctx);


  protected:

  private:
      platform_security();


      std::vector<std::unique_ptr<platform_security_provider> > collect_internal_providers ();



      providers_type providers_;

      platform_security_provider *default_provider_;

      platform_roles roles_manager_;


      security::security_context_ptr host_ctx_;

      security::security_context_ptr system_ctx_;

      security::security_context_ptr world_ctx_;

      security::security_context_ptr unassigned_ctx_;


};






class none_security_provider : public platform_security_provider  
{

  public:

      const string_type& get_name ();

      string_type get_info ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();

      rx_result_with<security::security_context_ptr> create_host_context (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result_with<security::security_context_ptr> create_system_context (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result_with<security::security_context_ptr> create_world_context (hosting::rx_platform_host* host, configuration_data_t& data);


  protected:

  private:


};


} // namespace rx_security
} // namespace rx_internal



#endif
