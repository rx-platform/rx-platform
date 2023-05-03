

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_general.h
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


#ifndef rx_general_h
#define rx_general_h 1


#include "rx_api.h"

// rx_ptr
#include "lib/rx_ptr.h"

//////////////////////////////////////////////////////////////////////////////
// log macros defined here

#define RX_LOG_SELF_PRIORITY 9999
#define RELEASE_CODE_INFO ""

/* if*/
#ifdef RX_LOG_SOURCE_INFO

#define LOG_CODE_INFO ___code_name.c_str()
#define LOG_CODE_PREFIX { std::string ___code_name(_rx_func_);___code_name+="@" __FILE__"\r\n";
#define LOG_CODE_POSTFIX  ; }

#else
#define LOG_CODE_INFO RELEASE_CODE_INFO
#define LOG_CODE_PREFIX
#define LOG_CODE_POSTFIX
#endif


#define RX_LOG_INFO(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform_api::write_log(rx_platform_api::log_event_type::info, lib, src, lvl,LOG_CODE_INFO,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_WARNING(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform_api::write_log(rx_platform_api::log_event_type::warning, lib, src, lvl,LOG_CODE_INFO,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_ERROR(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform_api::write_log(rx_platform_api::log_event_type::error, lib, src, lvl,LOG_CODE_INFO,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_CRITICAL(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform_api::write_log(rx_platform_api::log_event_type::critical, lib, src, lvl,LOG_CODE_INFO,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_DEBUG(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform_api::write_log(rx_platform_api::log_event_type::debug, lib, src, lvl,LOG_CODE_INFO, (msg)\
) LOG_CODE_POSTFIX
#define RX_TRACE(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform_api::write_log(rx_platform_api::log_event_type::trace, lib, src, lvl,LOG_CODE_INFO,(msg))\
 LOG_CODE_POSTFIX
//////////////////////////////////////////////////////////////////////////////

namespace rx_platform_api
{

enum class log_event_type
{
	debug = 0,
	trace = 1,
	info = 2,
	warning = 3,
	error = 4,
	critical = 5
};

class rx_runtime_manager_lock
{
public:
    rx_runtime_manager_lock();
    ~rx_runtime_manager_lock();
};

void write_log(log_event_type type, const char* library, const char* source, uint16_t level, const char* code, const char* message);

rx_result register_item_binary(rx_item_type type, const string_type& name, const string_type& path
    , const rx_node_id& id, const rx_node_id& parent
    , uint32_t version, rx_time modified, const uint8_t* data, size_t count, uint32_t stream_version = 0);

template<class T>
rx_result register_item_binary_with_code(const string_type& name, const string_type& path, const rx_node_id& id, const rx_node_id& parent, const uint8_t* data, size_t count, uint32_t stream_version = 0)
{
    uint32_t version = (((uint32_t)(T::code_version()[0])) << 16) | (uint16_t)(T::code_version()[1]);
    return register_item_binary(T::type_id, name, path, id, parent, version, T::compile_time(), data, count, stream_version);
}

rx_result register_item_binary(rx_item_type type, const string_type& name, const string_type& path
    , const rx_node_id& id, const rx_node_id& parent, const uint8_t* data, size_t count, uint32_t stream_version = 0);


rx_result register_runtime_binary(rx_item_type type, const string_type& name, const string_type& path
    , const rx_node_id& id, const rx_node_id& parent
    , uint32_t version, rx_time modified, const uint8_t* data, size_t count, uint32_t stream_version = 0);

template<class T>
rx_result register_runtime_binary_with_code(const string_type& name, const string_type& path, const rx_node_id& id, const rx_node_id& parent, const uint8_t* data, size_t count, uint32_t stream_version = 0)
{
    uint32_t version = (((uint32_t)(T::code_version()[0])) << 16) | (uint16_t)(T::code_version()[1]);
    return register_runtime_binary(T::runtime_type_id, name, path, id, parent, version, T::compile_time(), data, count, stream_version);
}

rx_result register_runtime_binary(rx_item_type type, const string_type& name, const string_type& path
    , const rx_node_id& id, const rx_node_id& parent, const uint8_t* data, size_t count, uint32_t stream_version = 0);

}



namespace rx_platform_api {





class callback_data : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(callback_data);

  public:

      void process ();

      plugin_job_struct_t* c_ptr ();

      template<typename funcT>
      callback_data(rx_reference_ptr anchor, funcT&& func)
          : callback_(std::forward<funcT>(func))
          , anchor_(anchor)
      {
          init_api_data();
      }
      ~callback_data()
      {
      }
  protected:

  private:

      void init_api_data ();



      plugin_job_struct_t api_data_;

      rx_reference_ptr anchor_;

      std::function<void()> callback_;


};


} // namespace rx_platform_api



#endif
