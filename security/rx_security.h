

/****************************************************************************
*
*  security\rx_security.h
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


#ifndef rx_security_h
#define rx_security_h 1


#include "lib/rx_lock.h"

// rx_ptr
#include "lib/rx_ptr.h"

#include "system/server/rx_log.h"

/////////////////////////////////////////////////////////////
// logging macros for console library
#define SECURITY_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Security",src,lvl,msg)
#define SECURITY_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Security",src,lvl,msg)
#define SECURITY_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Security",src,lvl,msg)
#define SECURITY_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Security",src,lvl,msg)
#define SECURITY_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Security",src,lvl,msg)
#define SECURITY_LOG_TRACE(src,lvl,msg) RX_TRACE("Security",src,lvl,msg)

#define ANSI_RX_USER ANSI_COLOR_GREEN

using namespace rx;


namespace rx_platform {

namespace security {
class security_context;
typedef pointers::reference<security_context> security_context_ptr;
}

typedef std::uint_fast16_t sec_error_num_t;

namespace security {
class security_context;

typedef pointers::reference<security_context> security_context_ptr;
typedef pointers::reference<security_context> execute_context_ptr;





class security_context : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(security_context);

	//DECLARE_INTERFACE_PTR(security_context);

  public:
      security_context();

      virtual ~security_context();


      void logout ();

      rx_result login ();

      virtual bool has_console () const;

      virtual bool is_system () const;

      virtual bool is_authenticated () const;

      virtual bool is_hosted () const;

      virtual bool is_interactive () const;

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream) = 0;


      const rx_security_handle_t get_handle () const
      {
        return handle_;
      }


      const string_type& get_full_name () const
      {
        return full_name_;
      }


      const string_type& get_user_name () const
      {
        return user_name_;
      }


      const string_type& get_location () const
      {
        return location_;
      }



  protected:

      string_type full_name_;

      string_type user_name_;

      string_type location_;


  private:


      rx_security_handle_t handle_;


};


// unauthorized 
security_context_ptr active_security();
security_context_ptr unauthorized_context();
void push_security(security_context_ptr ctx);
void pop_security();






class security_manager 
{
	typedef std::map<uint64_t, security_context::smart_ptr> active_contexts_type;
	typedef std::set<uint64_t> active_handles_type;

	friend security_context_ptr security_context_helper(bool get_unathorized);
	friend void push_security(security_context_ptr ctx);
	friend void pop_security();

  public:
      ~security_manager();


      static security_manager& instance ();

      rx_security_handle_t context_activated (security_context::smart_ptr who);

      rx_security_handle_t context_deactivated (security_context::smart_ptr who);

      void get_active_contexts (std::vector<security_context_ptr >& ctxs);

      void deinitialize ();


  protected:

  private:
      security_manager();

      security_manager(const security_manager &right);

      security_manager & operator=(const security_manager &right);


      security_context_ptr get_context (rx_security_handle_t handle);



      active_contexts_type active_contexts_;


      locks::lockable active_lock_;

      active_handles_type active_handles_;


};


enum security_mask_t : std::uint_fast32_t
{
    rx_security_null            = 0,

    rx_security_read_access     = 0x01,
	rx_security_write_access	= 0x02,
	rx_security_delete_access	= 0x04,
    rx_security_poll_access     = 0x04,
    rx_security_execute_access  = 0x10,

    rx_security_full            = 0x1f,
};


enum extended_security_mask_t : std::uint_fast32_t
{
	rx_security_ext_null = 0,

    rx_security_requires_system = 0x01,
    rx_security_requires_internal = 0x02,
};




class security_guard : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(security_guard);

  public:
      security_guard (const meta_data& data, security_mask_t access = security::rx_security_null);

      security_guard (security_mask_t access, const string_type& path, extended_security_mask_t extended = rx_security_ext_null);

      ~security_guard();


      bool check_permission (security_mask_t access);


  protected:

      virtual bool check_permission (security_mask_t mask, security_context_ptr ctx);


  private:


      security_mask_t access_mask_;

      extended_security_mask_t extended_mask_;

      string_type path_base_;


};

typedef security_guard::smart_ptr security_guard_ptr;






class secured_scope 
{

  public:
      secured_scope (security_context_ptr ctx);

      secured_scope (rx_security_handle_t ctx);

      ~secured_scope();


  protected:

  private:


      rx_security_handle_t ctx_;


};






class unathorized_security_context : public security_context  
{
	DECLARE_REFERENCE_PTR(unathorized_security_context);

  public:
      unathorized_security_context();

      ~unathorized_security_context();


      bool is_authenticated () const;

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);


  protected:

      void interface_bind ();

      void interface_release ();


  private:


};


} // namespace security
} // namespace rx_platform



#endif
