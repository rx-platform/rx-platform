

/****************************************************************************
*
*  system\server\rx_mngt.h
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


#ifndef rx_mngt_h
#define rx_mngt_h 1


#include "lib/rx_lib.h"

// rx_io
#include "lib/rx_io.h"
// rx_objbase
#include "system/meta/rx_objbase.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_inf
#include "system/server/rx_inf.h"

#include "lib/rx_io.h"


namespace server {

namespace mngt {






class manager_initialization_context 
{
	typedef std::vector<server_command_base_ptr> to_register_type;

  public:
      manager_initialization_context();

      virtual ~manager_initialization_context();


      void register_command (server_command_base_ptr cmd);


      const to_register_type& get_to_register () const
      {
        return _to_register;
      }



  protected:

  private:
      manager_initialization_context(const manager_initialization_context &right);

      manager_initialization_context & operator=(const manager_initialization_context &right);



      to_register_type _to_register;


};


class manager_initialization_context;
struct managment_data_t
{
	managment_data_t()
	{
		telnet_port = 0;
		manager_internal_data = nullptr;
	}
	uint16_t telnet_port;
	// internal stuff bellow do not change!!!
	manager_initialization_context* manager_internal_data;
};





class server_manager 
{
	DECLARE_VIRTUAL_REFERENCE_PTR(server_manager);

  public:
      server_manager();

      virtual ~server_manager();


      uint32_t initialize (host::rx_server_host* host, managment_data_t& data);

      uint32_t deinitialize ();

      uint32_t start (host::rx_server_host* host, const managment_data_t& data);

      uint32_t stop ();

      void get_directories (server_directories_type& dirs);


      server_item_ptr get_commands_manager ()
      {
        return _commands_manager;
      }


      objects::domain_runtime::smart_ptr get_unassigned_domain ()
      {
        return _unassigned_domain;
      }


      objects::application_runtime::smart_ptr get_unassigned_app ()
      {
        return _unassigned_app;
      }


      objects::application_runtime::smart_ptr get_system_app ()
      {
        return _system_app;
      }


      objects::domain_runtime::smart_ptr get_system_domain ()
      {
        return _system_domain;
      }



  protected:

      void virtual_bind ();

      void virtual_release ();


  private:


      rx::io::tcp_listent_std_buffer::smart_ptr _telnet_listener;

      server_item_ptr _commands_manager;

      objects::domain_runtime::smart_ptr _unassigned_domain;

      objects::application_runtime::smart_ptr _unassigned_app;

      objects::application_runtime::smart_ptr _system_app;

      objects::domain_runtime::smart_ptr _system_domain;


      uint16_t _telnet_port;


};


} // namespace mngt
} // namespace server



#endif
