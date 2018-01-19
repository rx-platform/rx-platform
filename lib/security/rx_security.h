

/****************************************************************************
*
*  lib\security\rx_security.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef rx_security_h
#define rx_security_h 1


#include "lib/rx_lock.h"

// rx_ptr
#include "lib/rx_ptr.h"


#include "lib/rx_log.h"

/////////////////////////////////////////////////////////////
// logging macros for console library
#define SECURITY_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Security",src,lvl,msg)
#define SECURITY_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Security",src,lvl,msg)
#define SECURITY_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Security",src,lvl,msg)
#define SECURITY_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Security",src,lvl,msg)
#define SECURITY_LOG_TRACE(src,lvl,msg) RX_TRACE("Security",src,lvl,msg)


using namespace rx;


namespace rx {

namespace security {
class security_context;





class security_context : public pointers::interface_object  
{
	DECLARE_INTERFACE_PTR(security_context);

	//DECLARE_INTERFACE_PTR(security_context);

  public:
      security_context();

      virtual ~security_context();


      void logout ();

      void login ();

      virtual bool has_console () const;

      virtual bool is_system () const;

      virtual bool is_authenticated () const;

      virtual bool is_hosted () const;

      virtual bool is_interactive () const;


      const rx_security_handle_t get_handle () const
      {
        return handle_;
      }


      const string_type& get_location () const
      {
        return location_;
      }


      const string_type& get_full_name () const
      {
        return full_name_;
      }


      const string_type& get_user_name () const
      {
        return user_name_;
      }


      const string_type& get_port () const
      {
        return port_;
      }



  protected:

      string_type location_;

      string_type full_name_;

      string_type user_name_;

      string_type port_;


  private:


      rx_security_handle_t handle_;


};

typedef pointers::interface_reference<security_context> security_context_ptr;


security_context_ptr active_security();
void push_security(security_context_ptr ctx);
void pop_security();






class security_manager 
{
	typedef std::map<uint64_t, security_context::smart_ptr> active_contexts_type;

	friend security_context_ptr active_security();
	friend void push_security(security_context_ptr ctx);
	friend void pop_security();

  public:
      virtual ~security_manager();


      static security_manager& instance ();

      rx_security_handle_t context_activated (security_context::smart_ptr who);

      rx_security_handle_t context_deactivated (security_context::smart_ptr who);

      void get_active_contexts (std::vector<security_context_ptr >& ctxs);


  protected:

  private:
      security_manager();

      security_manager(const security_manager &right);

      security_manager & operator=(const security_manager &right);


      security_context_ptr get_context (rx_security_handle_t handle);



      active_contexts_type active_contexts_;


      locks::lockable active_lock_;

      uint64_t last_id_;


};


enum security_mask_t : std::uint_fast32_t
{
	rx_security_execute_access	= 0x01,
	rx_security_read_access		= 0x02,
	rx_security_write_access	= 0x04,
	rx_security_delete_access	= 0x08
};

enum extended_security_mask_t : std::uint_fast32_t
{
	rx_security_ext_null = 0
};




class security_guard 
{
  public:
	typedef std::unique_ptr<security_guard> smart_ptr;

  public:
      security_guard();

      virtual ~security_guard();


      bool check_premissions (security_mask_t mask, extended_security_mask_t extended_mask);


  protected:

      virtual bool check_premissions (security_mask_t mask, extended_security_mask_t extended_mask, security_context_ptr ctx);


  private:


};

typedef security_guard::smart_ptr security_guard_ptr;






class security_auto_context 
{

  public:
      security_auto_context (security_context_ptr ctx);

      virtual ~security_auto_context();


  protected:

  private:


      rx_security_handle_t ctx_;


};






class built_in_security_context : public security_context, 
                                  	public pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(built_in_security_context);

	DECLARE_DERIVED_FROM_INTERFACE;

  public:
      built_in_security_context();

      virtual ~built_in_security_context();


  protected:

  private:


};






class unathorized_security_context : public security_context, 
                                     	public pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(unathorized_security_context);
	DECLARE_DERIVED_FROM_INTERFACE;

  public:
      unathorized_security_context();

      virtual ~unathorized_security_context();


      bool is_authenticated () const;


  protected:

      ///void interface_bind ();

      //void interface_release ();


  private:


};






class loose_security_guard : public security_guard  
{
public:
	typedef std::unique_ptr<security_guard> smart_ptr;

  public:
      loose_security_guard();

      virtual ~loose_security_guard();


  protected:

      bool check_premissions (security_mask_t mask, extended_security_mask_t extended_mask, security_context_ptr ctx);


  private:


};


} // namespace security
} // namespace rx



#endif
