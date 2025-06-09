

/****************************************************************************
*
*  sys_internal\rx_security\rx_x509_security.h
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


#ifndef rx_x509_security_h
#define rx_x509_security_h 1



// rx_security
#include "security/rx_security.h"
// rx_platform_security
#include "sys_internal/rx_security/rx_platform_security.h"
// rx_ptr
#include "lib/rx_ptr.h"



namespace rx_internal {

namespace rx_security {

namespace x509 {





class x509_certificate : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(x509_certificate);

  public:
      x509_certificate();

      ~x509_certificate();


      rx_certificate_t* get_certificate_object ();

      rx_result open_from_thumb (const byte_string& thumb);

      rx_result open_from_bytes (const byte_string& data);

      rx_result attach (rx_certificate_t* cert);

      bool is_valid () const;

      string_type get_principal_name () const;

      byte_string get_thumbprint () const;

      byte_string get_bytes () const;

      x509_certificate(const x509_certificate& right) = delete;
      x509_certificate& operator=(const x509_certificate& right) = delete;
      x509_certificate(x509_certificate&& right) noexcept;
      x509_certificate& operator=(x509_certificate&& right)noexcept;
  protected:

  private:


      rx_certificate_t cert_;


};

typedef x509_certificate::smart_ptr x509_certificate_ptr;





class x509_certificates 
{
    typedef std::map<string_type, x509_certificate_ptr> certificates_type;

  public:
      x509_certificates();

      ~x509_certificates();


      static x509_certificates& instance ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();

      rx_result_with<x509_certificate_ptr> get_certificate (const string_type& name);


  protected:

  private:


      certificates_type certificates_;


      locks::slim_lock lock_;


};







class x509_security_context : public rx_platform::security::security_context  
{
    DECLARE_REFERENCE_PTR(x509_security_context);

  public:
      x509_security_context();

      x509_security_context (const string_view_type name, const string_view_type location, bool has_console = false);

      ~x509_security_context();


      void set_certificate (x509_certificate_ptr cert);

      void set_certificate (rx_certificate_t* cert);

      bool is_authenticated () const;

      virtual rx_result serialize (base_meta_writer& stream) const;

      virtual rx_result deserialize (base_meta_reader& stream);


      bool has_console () const;

      x509_security_context(const x509_security_context& right) = delete;
      x509_security_context& operator=(const x509_security_context& right) = delete;
      x509_security_context(x509_security_context&& right) = delete;
      x509_security_context& operator=(x509_security_context&& right) = delete;
  protected:

  private:


      x509_certificate_ptr certificate_;


      bool has_console_;


};







class certificate_security_provider : public platform_security_provider  
{

  public:

      const string_type& get_name ();

      string_type get_info ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();

      rx_result_with<security::security_context_ptr> create_host_context (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result_with<security::security_context_ptr> create_system_context (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result_with<security::security_context_ptr> create_world_context (hosting::rx_platform_host* host, configuration_data_t& data);

      rx_result start (hosting::rx_platform_host* host);

      void stop ();


  protected:

  private:


};


} // namespace x509
} // namespace rx_security
} // namespace rx_internal



#endif
