

/****************************************************************************
*
*  host\rx_host.h
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


#ifndef rx_host_h
#define rx_host_h 1


#include "system/meta/rx_objbase.h"
#include "lib/rx_mem.h"
/////////////////////////////////////////////////////////////
// logging macros for host library
#define HOST_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Host",src,lvl,msg)
#define HOST_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Host",src,lvl,msg)
#define HOST_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Host",src,lvl,msg)
#define HOST_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Host",src,lvl,msg)
#define HOST_LOG_TRACE(src,lvl,msg) RX_TRACE("Host",src,lvl,msg)

// rx_security
#include "system/security/rx_security.h"



namespace server {
namespace security
{
typedef rx::pointers::reference<security_context> execute_context_ptr;
}
}


namespace host {






class rx_server_host 
{

	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      rx_server_host();

      virtual ~rx_server_host();


      virtual const string_type& get_host_info () = 0;

      virtual void server_started_event ();

      virtual void server_stopping_event ();

      virtual bool shutdown (const string_type& msg) = 0;

      virtual void get_host_objects (std::vector<server::objects::object_runtime_ptr>& items) = 0;

      virtual void get_host_classes (std::vector<server::meta::object_class_ptr>& items) = 0;

      virtual bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);


  protected:

  private:
      rx_server_host(const rx_server_host &right);

      rx_server_host & operator=(const rx_server_host &right);



};






class host_security_context : public server::security::built_in_security_context  
{
	DECLARE_REFERENCE_PTR(host_security_context);

  public:
      host_security_context();

      virtual ~host_security_context();


      bool is_system () const;


  protected:

  private:


};


} // namespace host



#endif
