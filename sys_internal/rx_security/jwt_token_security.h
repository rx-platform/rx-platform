

/****************************************************************************
*
*  sys_internal\rx_security\jwt_token_security.h
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


#ifndef jwt_token_security_h
#define jwt_token_security_h 1



/////////////////////////////////////////////////////////////
// logging macros for jwt library
#define JWT_LOG_INFO(src,lvl,msg) RX_LOG_INFO("JWT",src,lvl,msg)
#define JWT_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("JWT",src,lvl,msg)
#define JWT_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("JWT",src,lvl,msg)
#define JWT_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("JWT",src,lvl,msg)
#define JWT_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("JWT",src,lvl,msg)
#define JWT_LOG_TRACE(src,lvl,msg) RX_TRACE("JWT",src,lvl,msg)

// rx_platform_security
#include "sys_internal/rx_security/rx_platform_security.h"
// rx_security
#include "security/rx_security.h"



namespace rx_internal {

namespace rx_security {

namespace jwt_token {





class jwt_token_security_provider : public platform_security_provider  
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






class jwt_token_data 
{
    struct permission_data
    {
        security_mask_t allowed;
        security_mask_t denied;
    };
    typedef std::map<string_type, permission_data> permissions_type;

  public:
      jwt_token_data();

      ~jwt_token_data();


      rx_result parse (string_type header, string_type payload, string_type signature);

      bool is_in_role (string_view_type role, security_mask_t access) const;

      bool is_valid () const;


      const string_type& get_user_name () const
      {
        return user_name_;
      }


      const string_type& get_provider () const
      {
        return provider_;
      }


      const byte_string& get_signature () const
      {
        return signature_;
      }


      const string_type& get_key_id () const
      {
        return key_id_;
      }



  protected:

  private:


      locks::slim_lock lock_;

      string_type token_;

      permissions_type permissions_;

      rx_time expires_;

      bool is_system_;

      bool valid_;

      string_type user_name_;

      string_type provider_;

      byte_string signature_;

      string_type key_id_;


};






class jwt_token_security_context : public rx_platform::security::security_context  
{
    DECLARE_REFERENCE_PTR(jwt_token_security_context);

  public:
      jwt_token_security_context (std::weak_ptr<jwt_token_data> data);

      ~jwt_token_security_context();


      bool is_authenticated () const;

      virtual rx_result serialize (base_meta_writer& stream) const;

      virtual rx_result deserialize (base_meta_reader& stream);

      bool has_console () const;

      bool is_in_role (string_view_type role, security_mask_t access) const;


  protected:

  private:


      std::weak_ptr<jwt_token_data> my_data_;


};







class jwt_tokens_repository 
{
    typedef std::map<string_type, std::shared_ptr<jwt_token_data>> active_tokens_type;
    typedef std::map<string_type, crypt_key_t> public_keys_type;

  public:
      ~jwt_tokens_repository();


      static jwt_tokens_repository& instance ();

      rx_result initialize (hosting::rx_platform_host* host, configuration_data_t& data);

      void deinitialize ();

      rx_result start (hosting::rx_platform_host* host);

      void stop ();

      rx_result_with<jwt_token_security_context::smart_ptr> get_context (string_view_type token);


  protected:

  private:
      jwt_tokens_repository();


      bool check_signature (string_view_type content, const byte_string& sig, crypt_key_t key);



      active_tokens_type active_tokens_;


      public_keys_type public_keys_;

      static jwt_tokens_repository* instance_;

      locks::slim_lock tokens_lock_;

      periodic_job::smart_ptr timer_;


};


} // namespace jwt_token
} // namespace rx_security
} // namespace rx_internal



#endif
