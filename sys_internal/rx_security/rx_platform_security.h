

/****************************************************************************
*
*  sys_internal\rx_security\rx_platform_security.h
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


#ifndef rx_platform_security_h
#define rx_platform_security_h 1



// rx_security
#include "lib/security/rx_security.h"

#include "system/hosting/rx_host.h"

#define RX_NONE_SECURITY_NAME "none"


namespace rx_internal {

namespace rx_security {





class built_in_security_context : public rx::security::security_context  
{
    DECLARE_REFERENCE_PTR(built_in_security_context);

  public:
      built_in_security_context();

      ~built_in_security_context();


      virtual rx_result serialize (base_meta_writer& stream) const;

      virtual rx_result deserialize (base_meta_reader& stream);


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
      maintenance_context (const string_type& port, const string_type& location);

      ~maintenance_context();


      bool is_system () const;

      bool has_console () const;

      bool is_interactive () const;


  protected:

  private:


};






class host_security_context : public built_in_security_context  
{
    DECLARE_REFERENCE_PTR(host_security_context);

  public:
      host_security_context (const string_type& name, const string_type& instance);

      ~host_security_context();


      bool is_system () const;


  protected:

  private:


};






class process_context : public built_in_security_context  
{

  public:
      process_context (const string_type& port, const string_type& location);

      ~process_context();


      bool is_system () const;

      bool has_console () const;

      bool is_interactive () const;


  protected:

  private:


};






class platform_security_provider 
{

  public:

      virtual const string_type& get_name () = 0;

      virtual string_type get_info () = 0;

      virtual rx_result initialize (hosting::rx_platform_host* host) = 0;

      virtual void deinitialize () = 0;

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

      void deinitialize ();

      rx_result register_role (const string_type& role, const string_type& parent_role, hosting::rx_platform_host* host);

      rx_result register_provider (std::unique_ptr<platform_security_provider>  who, hosting::rx_platform_host* host);

      platform_security_provider* get_provider (const string_type& name);


  protected:

  private:
      platform_security();


      std::vector<std::unique_ptr<platform_security_provider> > collect_internal_providers ();



      providers_type providers_;

      platform_security_provider *default_provider_;


};






class none_security_provider : public platform_security_provider  
{

  public:

      const string_type& get_name ();

      string_type get_info ();

      rx_result initialize (hosting::rx_platform_host* host);

      void deinitialize ();


  protected:

  private:


};


} // namespace rx_security
} // namespace rx_internal



#endif
