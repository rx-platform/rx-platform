

/****************************************************************************
*
*  sys_internal\rx_internal_builders.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_internal_builders_h
#define rx_internal_builders_h 1



// rx_server
#include "system/server/rx_server.h"

#include "system/runtime/rx_runtime_holder.h"

/////////////////////////////////////////////////////////////
// logging macros for building
#define BUILD_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Build",src,lvl,msg)
#define BUILD_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Build",src,lvl,msg)
#define BUILD_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Build",src,lvl,msg)
#define BUILD_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Build",src,lvl,msg)
#define BUILD_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Build",src,lvl,msg)
#define BUILD_LOG_TRACE(src,lvl,msg) RX_TRACE("Build",src,lvl,msg)


#include "sys_internal/rx_internal_ns.h"
using namespace rx_internal::internal_ns;


namespace rx_internal {

namespace builders {






class rx_platform_builder 
{

  public:
      rx_platform_builder();

      virtual ~rx_platform_builder();


      static rx_result build_platform (hosting::rx_platform_host* host, configuration_data_t& config);

      virtual rx_result do_build (configuration_data_t& config) = 0;

      static void deinitialize ();


  protected:

  private:
      rx_platform_builder(const rx_platform_builder &right);

      rx_platform_builder & operator=(const rx_platform_builder &right);


      static std::vector<std::unique_ptr<rx_platform_builder> > get_system_builders (namespace_data_t& data, const meta_configuration_data_t& meta_data, hosting::rx_platform_host* host);

      static std::vector<std::unique_ptr<rx_platform_builder> > get_user_builders (namespace_data_t& data, hosting::rx_platform_host* host);

      static std::vector<std::unique_ptr<rx_platform_builder> > get_test_builders (namespace_data_t& data, hosting::rx_platform_host* host);

      static std::vector<std::unique_ptr<rx_platform_builder> > get_other_builders (namespace_data_t& data, hosting::rx_platform_host* host);

      static rx_result register_system_constructors ();

      static rx_result buid_unassigned (hosting::rx_platform_host* host, namespace_data_t& data);

      static std::vector<std::unique_ptr<rx_platform_builder> > get_plugin_builders (namespace_data_t& data, hosting::rx_platform_host* host);

      static void recursive_destory_fs (rx_directory_ptr dir);



};






class root_folder_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class basic_object_types_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& onfcig);


  protected:

  private:
        template<class T>
        void build_standard_basic_object_type(configuration_data_t& config, rx_directory_ptr dir, rx_reference<T> what);
		void build_basic_object_type(configuration_data_t& config, rx_directory_ptr dir, rx_object_type_ptr what);
		void build_basic_domain_type(configuration_data_t& config, rx_directory_ptr dir, rx_domain_type_ptr what);
		void build_basic_application_type(configuration_data_t& config, rx_directory_ptr dir, rx_application_type_ptr what);
		void build_basic_port_type(configuration_data_t& config, rx_directory_ptr dir, rx_port_type_ptr what);

};






class system_types_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:
      void build_instance_info_struct_type(rx_directory_ptr dir, struct_type_ptr what);
      void build_host_info_struct_type(rx_directory_ptr dir, struct_type_ptr what);

};






class port_types_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class system_objects_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class support_types_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class relation_types_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class simulation_types_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class system_ports_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class terminal_commands_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class http_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:


};






class basic_types_builder : public rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config);


  protected:

  private:
      void build_object_data_struct_type(rx_directory_ptr dir, struct_type_ptr what);

      template<class T>
      void build_basic_type(rx_directory_ptr dir, rx_reference<T> what);

};


} // namespace builders
} // namespace rx_internal



#endif
