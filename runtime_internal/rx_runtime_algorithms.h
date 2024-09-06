

/****************************************************************************
*
*  runtime_internal\rx_runtime_algorithms.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_runtime_algorithms_h
#define rx_runtime_algorithms_h 1



// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "system/runtime/rx_objbase.h"
#include "system/runtime/rx_holder_algorithms.h"
#include "lib/rx_func_to_go.h"


namespace rx_internal {

namespace sys_runtime {

namespace algorithms {
template<class typeT>
rx_result create_runtime_structure(typename typeT::RTypePtr what);
template<class typeT>
rx_result delete_runtime_structure(typename typeT::RTypePtr what);
template<class typeT>
rx_result init_runtime(typename typeT::RTypePtr what, runtime::runtime_init_context& ctx);
template<class typeT>
rx_result just_init_runtime(typename typeT::RTypePtr what, runtime::runtime_init_context& ctx);
template<class typeT>
void just_start_runtime(typename typeT::RTypePtr what, const_callbacks_type callbacks, pending_connections_type pending_connections, status_data_type status);
template<class typeT>
rx_result deinit_runtime(typename typeT::RTypePtr what, rx_result_callback&& callback);






class object_algorithms 
{

  public:

      static rx_result init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx);

      static rx_result just_init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx);

      static void just_start_runtime (rx_object_ptr what, const_callbacks_type callbacks, pending_connections_type pending_connections, status_data_type status);

      static rx_result deinit_runtime (rx_object_ptr what, rx_result_callback&& callback);

      static rx_result stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx);

      static rx_result connect_domain (rx_object_ptr what);

      static rx_result disconnect_domain (rx_object_ptr what);


  protected:

  private:

      static rx_result start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx);



};






class domain_algorithms 
{

  public:

      static rx_result init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx);

      static rx_result just_init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx);

      static void just_start_runtime (rx_domain_ptr what, const_callbacks_type callbacks, pending_connections_type pending_connections, status_data_type status);

      static rx_result deinit_runtime (rx_domain_ptr what, rx_result_callback&& callback);

      static rx_result stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx);

      static rx_result connect_application (rx_domain_ptr what);

      static rx_result disconnect_application (rx_domain_ptr what);


  protected:

  private:

      static rx_result start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx);



};






class port_algorithms 
{

  public:

      static rx_result init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx);

      static rx_result just_init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx);

      static void just_start_runtime (rx_port_ptr what, const_callbacks_type callbacks, pending_connections_type pending_connections, status_data_type status);

      static rx_result deinit_runtime (rx_port_ptr what, rx_result_callback&& callback);

      static rx_result stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx);

      static rx_result connect_application (rx_port_ptr what);

      static rx_result disconnect_application (rx_port_ptr what);


  protected:

  private:

      static rx_result start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx);



};






class application_algorithms 
{

  public:

      static rx_result init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx);

      static rx_result just_init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx);

      static void just_start_runtime (rx_application_ptr what, const_callbacks_type callbacks, pending_connections_type pending_connections, status_data_type status);

      static rx_result deinit_runtime (rx_application_ptr what, rx_result_callback&& callback);

      static rx_result stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx);


  protected:

  private:

      static rx_result start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx);



};






class relations_algorithms 
{

  public:

      static rx_result init_runtime (rx_relation_ptr what, runtime::runtime_init_context& ctx);

      static rx_result deinit_runtime (rx_relation_ptr what, rx_result_callback&& callback);


  protected:

  private:

      static rx_result start_runtime (rx_relation_ptr what, runtime::runtime_start_context& ctx);

      static rx_result stop_runtime (rx_relation_ptr what, runtime::runtime_stop_context& ctx);



};






class shutdown_algorithms 
{

  public:

      static void stop_applications (std::vector<rx_application_ptr> apps);

      static void stop_domains (std::vector<rx_domain_ptr> domains);

      static void stop_ports (std::vector<rx_port_ptr> ports);

      static void stop_objects (std::vector<rx_object_ptr> objects);

      static void deinit_applications (std::vector<rx_application_ptr> apps);

      static void deinit_domains (std::vector<rx_domain_ptr> domains);

      static void deinit_ports (std::vector<rx_port_ptr> ports);

      static void deinit_objects (std::vector<rx_object_ptr> objects);


  protected:

  private:


};






class startup_algorithms 
{

  public:

      static void start_applications (std::vector<rx_application_ptr> apps);

      static void start_domains (std::vector<rx_domain_ptr> domains);

      static void start_ports (std::vector<rx_port_ptr> ports);

      static void start_objects (std::vector<rx_object_ptr> objects);


  protected:

  private:


};


} // namespace algorithms
} // namespace sys_runtime
} // namespace rx_internal



#endif
