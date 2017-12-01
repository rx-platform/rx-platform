

/****************************************************************************
*
*  sys_internal\rx_internal_objects.h
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


#ifndef rx_internal_objects_h
#define rx_internal_objects_h 1


#include "rx_configuration.h"

// rx_objbase
#include "system/meta/rx_objbase.h"



namespace sys_internal {

namespace sys_objects {





class system_application : public rx_platform::objects::application_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system application. contains system objects and internal system communication");

	DECLARE_REFERENCE_PTR(system_application);

  public:
      system_application();

      virtual ~system_application();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class system_domain : public rx_platform::objects::domain_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
this domain is used by system objects");
	DECLARE_REFERENCE_PTR(system_domain);

  public:
      system_domain();

      virtual ~system_domain();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class unassigned_application : public rx_platform::objects::application_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
this application is for unassigned domains.it allways has bad quality");

	DECLARE_REFERENCE_PTR(unassigned_application);

  public:
      unassigned_application();

      virtual ~unassigned_application();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class unssigned_domain : public rx_platform::objects::domain_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
this domain is for uassigned objects.it allways has bad quality");

	DECLARE_REFERENCE_PTR(unssigned_domain);

  public:
      unssigned_domain();

      virtual ~unssigned_domain();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};


} // namespace sys_objects
} // namespace sys_internal



#endif
