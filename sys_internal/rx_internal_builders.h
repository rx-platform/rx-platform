

/****************************************************************************
*
*  sys_internal\rx_internal_builders.h
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_internal_builders_h
#define rx_internal_builders_h 1



// rx_server
#include "system/server/rx_server.h"

#include "sys_internal/rx_internal_ns.h"
using namespace sys_internal::internal_ns;


namespace sys_internal {

namespace builders {






class rx_platform_builder 
{

  public:
      rx_platform_builder();

      virtual ~rx_platform_builder();


      virtual void build (platform_root::smart_ptr root) = 0;

      static server_directory_ptr buid_platform_system (hosting::rx_platform_host* host, namespace_data_t& data);


  protected:

  private:
      rx_platform_builder(const rx_platform_builder &right);

      rx_platform_builder & operator=(const rx_platform_builder &right);



};






class root_folder_builder : public rx_platform_builder  
{

  public:

      void build (platform_root::smart_ptr root);


  protected:

  private:


};






class basic_types_builder : public rx_platform_builder  
{

  public:

      void build (platform_root::smart_ptr root);


  protected:

  private:
	    template<class T>
		void build_basic_object_type(server_directory_ptr dir, rx_reference<T> what);
		template<class T>
		void build_basic_domain_type(server_directory_ptr dir, rx_reference<T> what);
		template<class T>
		void build_basic_application_type(server_directory_ptr dir, rx_reference<T> what);
		template<class T>
		void build_basic_port_type(server_directory_ptr dir, rx_reference<T> what);

		template<class T>
		void build_basic_type(server_directory_ptr dir, rx_reference<T> what);

};






class system_classes_builder : public rx_platform_builder  
{

  public:

      void build (platform_root::smart_ptr root);


  protected:

  private:


};






class port_classes_builder : public rx_platform_builder  
{

  public:

      void build (platform_root::smart_ptr root);


  protected:

  private:


};






class system_objects_builder : public rx_platform_builder  
{

  public:

      void build (platform_root::smart_ptr root);


  protected:

  private:


};


} // namespace builders
} // namespace sys_internal



#endif
