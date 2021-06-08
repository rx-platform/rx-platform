

/****************************************************************************
*
*  system\runtime\rx_internal_objects.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform}
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_internal_objects_h
#define rx_internal_objects_h 1


#include "rx_configuration.h"

// rx_objbase
#include "system/runtime/rx_objbase.h"



namespace rx_platform {

namespace sys_objects {





class system_application : public runtime::items::application_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,2, "\
system application. contains system objects and internal system communication");

	DECLARE_REFERENCE_PTR(system_application);

  public:
      ~system_application();


      namespace_item_attributes get_attributes () const;

      static system_application::smart_ptr instance ();

      void deinitialize ();


  protected:

  private:
      system_application();



};






class system_domain : public runtime::items::domain_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
this domain is used by system objects");
	DECLARE_REFERENCE_PTR(system_domain);

  public:
      ~system_domain();


      namespace_item_attributes get_attributes () const;

      static system_domain::smart_ptr instance ();

      void deinitialize ();


  protected:

  private:
      system_domain();



};






class unassigned_application : public runtime::items::application_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
this application is for unassigned domains.it allways has bad quality");

	DECLARE_REFERENCE_PTR(unassigned_application);

  public:
      ~unassigned_application();


      namespace_item_attributes get_attributes () const;

      static unassigned_application::smart_ptr instance ();

      void deinitialize ();


  protected:

  private:
      unassigned_application();



};






class unassigned_domain : public runtime::items::domain_runtime  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
this domain is for unassigned objects.it always has bad quality");

	DECLARE_REFERENCE_PTR(unassigned_domain);

  public:
      ~unassigned_domain();


      namespace_item_attributes get_attributes () const;

      static unassigned_domain::smart_ptr instance ();

      void deinitialize ();


  protected:

  private:
      unassigned_domain();



};






class system_object : public runtime::items::object_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
System object implementation");
    DECLARE_REFERENCE_PTR(system_object);

  public:
      system_object();

      ~system_object();


      namespace_item_attributes get_attributes () const;

      static system_object::smart_ptr instance ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      void deinitialize ();


  protected:

  private:


      runtime::owned_value<rx_time> current_time_;

      rx_timer_ptr timer_;


};


} // namespace sys_objects
} // namespace rx_platform



#endif
