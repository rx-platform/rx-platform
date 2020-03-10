

/****************************************************************************
*
*  system\server\rx_mngt.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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

// rx_platform_item
#include "system/server/rx_platform_item.h"

namespace rx_internal {
namespace terminal {
namespace commands {
class server_command;
} // namespace commands
} // namespace terminal

namespace infrastructure {
class server_runtime;

} // namespace infrastructure
} // namespace rx_internal


#include "lib/rx_io.h"
#include "system/meta/rx_types.h"
namespace rx_internal
{
namespace builders
{
class rx_platform_builder;
}
}


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
struct management_data_t
{
    string_type telnet_addr = "127.0.0.1";
	uint16_t telnet_port = 0;
    string_type logs_directory;
	bool test_log = false;
	string_type startup_script;
	// internal stuff bellow do not change!!!
	manager_initialization_context* manager_internal_data = nullptr;
};





class server_manager 
{
    friend class rx_internal::builders::rx_platform_builder;

  public:
      server_manager();

      ~server_manager();


      rx_result initialize (hosting::rx_platform_host* host, management_data_t& data);

      void deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const management_data_t& data);

      void stop ();

      void get_directories (platform_directories_type& dirs);


      rx_object_ptr get_commands_manager ()
      {
        return commands_manager_;
      }



      rx_domain_ptr get_unassigned_domain () const
      {
        return unassigned_domain_;
      }


      rx_domain_ptr get_system_domain () const
      {
        return system_domain_;
      }


      rx_application_ptr get_unassigned_app () const
      {
        return unassigned_app_;
      }


      rx_application_ptr get_system_app () const
      {
        return system_app_;
      }


      rx_port_ptr get_telnet_listener () const
      {
        return telnet_listener_;
      }



  protected:

  private:


      rx_object_ptr commands_manager_;


      uint16_t telnet_port_;

      rx_domain_ptr unassigned_domain_;

      rx_domain_ptr system_domain_;

      rx_application_ptr unassigned_app_;

      rx_application_ptr system_app_;

      rx_port_ptr telnet_listener_;


};


} // namespace mngt
} // namespace rx_platform



#endif
