

/****************************************************************************
*
*  runtime_internal\rx_runtime_algorithms.h
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


#ifndef rx_runtime_algorithms_h
#define rx_runtime_algorithms_h 1



// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "system/server/rx_ns.h"
#include "system/runtime/rx_objbase.h"


namespace sys_runtime {

namespace algorithms {
template<class typeT>
rx_result create_runtime_structure(typename typeT::RTypePtr what);
template<class typeT>
rx_result delete_runtime_structure(typename typeT::RTypePtr what);
template<class typeT>
rx_result init_runtime(typename typeT::RTypePtr what, runtime::runtime_init_context& ctx);
template<class typeT>
rx_result deinit_runtime(typename typeT::RTypePtr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx);






class object_algorithms 
{

  public:

      static rx_result init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx);

      static rx_result deinit_runtime (rx_object_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx);

      static rx_result connect_domain (rx_object_ptr what);

      static rx_result disconnect_domain (rx_object_ptr what);


  protected:

  private:

      static rx_result start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx);

      static rx_result stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx);



};






class domain_algorithms 
{

  public:

      static rx_result init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx);

      static rx_result deinit_runtime (rx_domain_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx);

      static rx_result connect_application (rx_domain_ptr what);

      static rx_result disconnect_application (rx_domain_ptr what);


  protected:

  private:

      static rx_result start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx);

      static rx_result stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx);



};






class port_algorithms 
{

  public:

      static rx_result init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx);

      static rx_result deinit_runtime (rx_port_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx);

      static rx_result connect_application (rx_port_ptr what);

      static rx_result disconnect_application (rx_port_ptr what);


  protected:

  private:

      static rx_result start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx);

      static rx_result stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx);



};






class application_algorithms 
{

  public:

      static rx_result init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx);

      static rx_result start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx);

      static rx_result deinit_runtime (rx_application_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx);

      static rx_result stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx);


  protected:

  private:


};






class relations_algorithms 
{

  public:

      static rx_result init_runtime (rx_relation_ptr what, runtime::runtime_init_context& ctx);

      static rx_result deinit_runtime (rx_relation_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx);


  protected:

  private:

      static rx_result start_runtime (rx_relation_ptr what, runtime::runtime_start_context& ctx);

      static rx_result stop_runtime (rx_relation_ptr what, runtime::runtime_stop_context& ctx);



};


} // namespace algorithms
} // namespace sys_runtime



#endif
