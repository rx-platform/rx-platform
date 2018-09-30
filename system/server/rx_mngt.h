

/****************************************************************************
*
*  system\server\rx_mngt.h
*
*  Copyright (c) 2018 Dusan Ciric
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

// rx_objbase
#include "system/meta/rx_objbase.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_inf
#include "system/server/rx_inf.h"
// rx_io
#include "lib/rx_io.h"

#include "lib/rx_io.h"
#include "system/meta/rx_classes.h"


namespace rx_platform {

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
        return to_register_;
      }



  protected:

  private:
      manager_initialization_context(const manager_initialization_context &right);

      manager_initialization_context & operator=(const manager_initialization_context &right);



      to_register_type to_register_;


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
	template<class clsT>
	typename clsT::smart_ptr get_class(const rx_node_id& id);

  public:
      server_manager();

      virtual ~server_manager();


      uint32_t initialize (hosting::rx_platform_host* host, managment_data_t& data);

      uint32_t deinitialize ();

      uint32_t start (hosting::rx_platform_host* host, const managment_data_t& data);

      uint32_t stop ();

      void get_directories (server_directories_type& dirs);

      meta::object_class_ptr get_object_class (const rx_node_id& id);


      rx_reference<objects::object_types::server_object> get_commands_manager ()
      {
        return commands_manager_;
      }


      rx_reference<objects::object_types::domain_runtime> get_unassigned_domain ()
      {
        return unassigned_domain_;
      }


      rx_reference<objects::object_types::application_runtime> get_unassigned_app ()
      {
        return unassigned_app_;
      }


      rx_reference<objects::object_types::application_runtime> get_system_app ()
      {
        return system_app_;
      }


      rx_reference<objects::object_types::domain_runtime> get_system_domain ()
      {
        return system_domain_;
      }



  protected:

      void virtual_bind ();

      void virtual_release ();


  private:


      rx_reference<rx::io::tcp_listent_std_buffer> telnet_listener_;

      rx_reference<objects::object_types::server_object> commands_manager_;

      rx_reference<objects::object_types::domain_runtime> unassigned_domain_;

      rx_reference<objects::object_types::application_runtime> unassigned_app_;

      rx_reference<objects::object_types::application_runtime> system_app_;

      rx_reference<objects::object_types::domain_runtime> system_domain_;


      uint16_t telnet_port_;


};


} // namespace mngt
} // namespace rx_platform



#endif
