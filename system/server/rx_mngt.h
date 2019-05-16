

/****************************************************************************
*
*  system\server\rx_mngt.h
*
*  Copyright (c) 2018-2019 Dusan Ciric
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_mngt_h
#define rx_mngt_h 1


#include "lib/rx_lib.h"

// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_inf
#include "system/server/rx_inf.h"

#include "lib/rx_io.h"
#include "system/meta/rx_types.h"


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
	uint16_t telnet_port = 0;
	// internal stuff bellow do not change!!!
	manager_initialization_context* manager_internal_data = nullptr;
};





class server_manager 
{
public:
	template<class clsT>
	typename clsT::smart_ptr get_type(const rx_node_id& id);
	template<class clsT>
	typename clsT::smart_ptr get_simple_type(const rx_node_id& id);

  public:
      server_manager();

      ~server_manager();


      rx_result initialize (hosting::rx_platform_host* host, managment_data_t& data);

      rx_result deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const managment_data_t& data);

      rx_result stop ();

      void get_directories (platform_directories_type& dirs);


      rx_object_ptr get_commands_manager ()
      {
        return commands_manager_;
      }


      rx_reference<runtime::objects::domain_runtime> get_unassigned_domain ()
      {
        return unassigned_domain_;
      }


      rx_reference<runtime::objects::application_runtime> get_unassigned_app ()
      {
        return unassigned_app_;
      }


      rx_reference<runtime::objects::application_runtime> get_system_app ()
      {
        return system_app_;
      }


      rx_reference<runtime::objects::domain_runtime> get_system_domain ()
      {
        return system_domain_;
      }



  protected:

  private:


      rx_reference<runtime::objects::port_runtime> telnet_listener_;

      rx_object_ptr commands_manager_;

      rx_reference<runtime::objects::domain_runtime> unassigned_domain_;

      rx_reference<runtime::objects::application_runtime> unassigned_app_;

      rx_reference<runtime::objects::application_runtime> system_app_;

      rx_reference<runtime::objects::domain_runtime> system_domain_;


      uint16_t telnet_port_;


};


} // namespace mngt
} // namespace rx_platform



#endif
