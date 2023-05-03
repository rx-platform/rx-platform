

/****************************************************************************
*
*  first_plugin\first_plugin.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef first_plugin_h
#define first_plugin_h 1



// rx_basic_types
#include "rx_basic_types.h"
// rx_var_types
#include "rx_var_types.h"
// rx_objects
#include "rx_objects.h"
// rx_runtime
#include "rx_runtime.h"
// rx_api
#include "rx_api.h"

using namespace rx_platform_api;






class first_filter : public rx_platform_api::rx_filter  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Filter. Filter implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_filter);

  public:
      first_filter();

      ~first_filter();


      rx_result initialize_filter (rx_platform_api::rx_init_context& ctx);

      rx_result start_filter (rx_platform_api::rx_start_context& ctx);

      rx_result stop_filter ();

      rx_result deinitialize_filter ();

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);


  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;


};






class first_mapper : public rx_platform_api::rx_mapper  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Mapper. Mapper implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_mapper);

  public:
      first_mapper();

      ~first_mapper();


      rx_result initialize_mapper (rx_platform_api::rx_init_context& ctx);

      rx_result start_mapper (rx_platform_api::rx_start_context& ctx);

      rx_result stop_mapper ();

      rx_result deinitialize_mapper ();

      void mapped_value_changed (rx_value&& val, rx_process_context& ctx);


  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;


};






class first_plugin : public rx_platform_api::rx_platform_plugin  
{

  public:

      rx_result init_plugin ();

      rx_result deinit_plugin ();

      rx_result build_plugin ();


  protected:

  private:


};






class first_source : public rx_platform_api::rx_source  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Source. Source implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_source);

  public:
      first_source();

      ~first_source();


      rx_result initialize_source (rx_platform_api::rx_init_context& ctx);

      rx_result start_source (rx_platform_api::rx_start_context& ctx);

      rx_result stop_source ();

      rx_result deinitialize_source ();

      rx_result source_write (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx);


  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;

      local_value<double> amplitude_;

      local_value<uint32_t> period_;

      local_value<uint32_t> sample_;

      rx_timer_ticks_t ticks_module_;


};






class first_object : public rx_platform_api::rx_object  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Object. Object implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_object);

  public:
      first_object();

      ~first_object();


      rx_result initialize_object (rx_platform_api::rx_init_context& ctx);

      rx_result start_object (rx_platform_api::rx_start_context& ctx);

      rx_result stop_object ();

      rx_result deinitialize_object ();


  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;


};






class first_domain : public rx_platform_api::rx_domain  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Domain. Domain implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_domain);

  public:
      first_domain();

      ~first_domain();


      rx_result initialize_domain (rx_platform_api::rx_init_context& ctx);

      rx_result start_domain (rx_platform_api::rx_start_context& ctx);

      rx_result stop_domain ();

      rx_result deinitialize_domain ();


  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;


};






class first_application : public rx_platform_api::rx_application  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Application. Application implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_application);

  public:
      first_application();

      ~first_application();


      rx_result initialize_application (rx_platform_api::rx_init_context& ctx);

      rx_result start_application (rx_platform_api::rx_start_context& ctx);

      rx_result stop_application ();

      rx_result deinitialize_application ();


  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;


};






class first_struct : public rx_platform_api::rx_struct  
{
    DECLARE_PLUGIN_CODE_INFO(1, 0, 0, "\
First Struct. Struct implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_struct);

  public:
      first_struct();

      ~first_struct();


      rx_result initialize_struct (rx_platform_api::rx_init_context& ctx);

      rx_result start_struct (rx_platform_api::rx_start_context& ctx);

      rx_result stop_struct ();

      rx_result deinitialize_struct ();


  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;


};






class first_relation : public rx_platform_api::rx_relation  
{
    DECLARE_PLUGIN_CODE_INFO(0, 1, 0, "\
First Relation. Relation implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_relation);

  public:
      first_relation();

      ~first_relation();


      rx_result initialize_relation (rx_platform_api::rx_init_context& ctx);

      rx_result start_relation (rx_platform_api::rx_start_context& ctx, bool is_target);

      rx_result stop_relation (bool is_target);

      rx_result deinitialize_relation ();

      rx_relation::smart_ptr make_target_relation ();

      static constexpr rx_item_type type_id = rx_item_type::rx_relation_type;
  protected:

  private:

      void relation_connected (rx_node_id from, rx_node_id to);

      void relation_disconnected (rx_node_id from, rx_node_id to);



};






class first_singleton : public rx_platform_api::rx_object  
{
    DECLARE_PLUGIN_CODE_INFO(0, 1, 0, "\
First Singleton. Singleton implementation in test plugin.");

    DECLARE_REFERENCE_PTR(first_singleton);

  public:
      first_singleton();

      ~first_singleton();


      rx_result initialize_object (rx_platform_api::rx_init_context& ctx);

      rx_result start_object (rx_platform_api::rx_start_context& ctx);

      rx_result stop_object ();

      rx_result deinitialize_object ();

      static first_singleton::smart_ptr instance ();

  protected:

  private:

      void timer_tick ();



      runtime_handle_t timer_;


};




#endif
