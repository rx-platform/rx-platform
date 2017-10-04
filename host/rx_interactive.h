

/****************************************************************************
*
*  host\rx_interactive.h
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


#ifndef rx_interactive_h
#define rx_interactive_h 1



// rx_cmds
#include "system/server/rx_cmds.h"
// rx_host
#include "host/rx_host.h"
// rx_security
#include "system/security/rx_security.h"

namespace host {
namespace interactive {
class interactive_console_host;

} // namespace interactive
} // namespace host


using namespace rx;
using namespace server;

void test_timers();


namespace host {

namespace interactive {





class interactive_security_context : public server::security::built_in_security_context  
{
	DECLARE_REFERENCE_PTR(interactive_security_context);


  public:
      interactive_security_context();

      virtual ~interactive_security_context();


      bool has_console () const;

      bool is_system () const;


  protected:

  private:


};







class interactive_console_client : public server::prog::console_client  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(interactive_console_client);


  public:
      interactive_console_client();

      virtual ~interactive_console_client();


      void run_interactive (const interactive_console_host& host);

      security::security_context::smart_ptr get_current_security_context ();


  protected:

      const string_type& get_console_name ();

      void virtual_bind ();

      void virtual_release ();

      void exit_console ();


  private:


      interactive_security_context::smart_ptr m_security_context;


      bool m_exit;


};







class interactive_console_host : public rx_server_host  
{

  public:
      interactive_console_host();

      virtual ~interactive_console_host();


      static void main (interactive_console_host& host);

      const string_type& get_host_info ();

      void server_started_event ();

      bool shutdown (const string_type& msg);

      bool exit () const;

      void get_host_objects (std::vector<server::objects::object_runtime_ptr>& items);

      void get_host_classes (std::vector<server::meta::object_class_ptr>& items);


  protected:

  private:


      bool m_exit;


};


} // namespace interactive
} // namespace host



#endif
