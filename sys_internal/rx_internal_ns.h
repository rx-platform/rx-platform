

/****************************************************************************
*
*  sys_internal\rx_internal_ns.h
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


#ifndef rx_internal_ns_h
#define rx_internal_ns_h 1



// rx_internal_objects
#include "sys_internal/rx_internal_objects.h"
// rx_host
#include "host/rx_host.h"
// rx_ns
#include "system/server/rx_ns.h"

using namespace server::ns;


namespace sys_internal {

namespace internal_ns {






class root_server_directory : public server::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(root_server_directory);

	DECLARE_CODE_INFO("rx", "0.5.0", "\
root server directory:\r\n\
contains root server folders\
");

  public:
      root_server_directory();

      virtual ~root_server_directory();


      static void initialize (host::rx_server_host* host, namespace_data_t& data);

      static void deinitialize ();

      namespace_item_attributes get_attributes () const;

      bool generate_json (std::ostream& def, std::ostream& err);


  protected:

  private:


      static server_directories_type _root_directories;

      static server_items_type _root_items;


};






class namespace_directory : public server::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(namespace_directory);


	DECLARE_CODE_INFO("rx", "0.5.0", "\
server directory:\r\n\
used to create system defined folders...\
");

  public:
      namespace_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items);

      virtual ~namespace_directory();


      namespace_item_attributes get_attributes () const;

      bool generate_json (std::ostream& def, std::ostream& err);


  protected:

  private:


};






class unassigned_directory : public server::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(unassigned_directory);


	DECLARE_CODE_INFO("rx", "0.5.0", "\
storing unassigned domains and applications\r\n\
All objects here are with unassgned state and hawing a bad quality\
");

  public:
      unassigned_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items);

      virtual ~unassigned_directory();


      namespace_item_attributes get_attributes () const;

      bool generate_json (std::ostream& def, std::ostream& err);


  protected:

  private:


};






class world_directory : public server::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(world_directory);


	DECLARE_CODE_INFO("rx", "0.5.0", "\
user directory:\r\n\
used to create user defined folders...\
");

  public:
      world_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items);

      virtual ~world_directory();


      namespace_item_attributes get_attributes () const;

      bool generate_json (std::ostream& def, std::ostream& err);


  protected:

  private:


};






class system_server_item : public server::ns::rx_server_item  
{

  public:
      system_server_item();

      virtual ~system_server_item();


  protected:

  private:


};


} // namespace internal_ns
} // namespace sys_internal



#endif
