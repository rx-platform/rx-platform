

/****************************************************************************
*
*  system\meta\rx_runtime_data.h
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


#ifndef rx_runtime_data_h
#define rx_runtime_data_h 1



// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_rt_data
#include "lib/rx_rt_data.h"

#include "system/runtime/rx_runtime_helpers.h"


namespace rx_platform {
enum class rx_domain_priority : uint8_t
{
    low = 0,
    normal = 1,
    high = 2,
    realtime = 3,
    // default value
    standard = 1,
    priority_count = 4
};

namespace meta {

namespace runtime_data {





struct application_data 
{


      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      int processor;

      rx_domain_priority priority;

      byte_string identity;

  public:

  protected:

  private:


};






struct domain_data 
{


      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      rx_item_reference app_ref;

      int processor;

      rx_domain_priority priority;

  public:

  protected:

  private:


};






struct object_data 
{


      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      rx_item_reference domain_ref;

  public:

  protected:

  private:


};






struct port_data 
{


      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      rx_item_reference app_ref;

      byte_string identity;

      bool simulation;

      bool process;

  public:

  protected:

  private:


};






struct basic_runtime_data 
{


      rx::data::runtime_values_data overrides;

      meta_data meta_info;

  public:

  protected:

  private:


};






class object_runtime_data : public basic_runtime_data  
{
public:
    typedef object_types::object_type targetType;

  public:

      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta);

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      object_data instance_data;


  protected:

  private:


};






class domain_runtime_data : public basic_runtime_data  
{
public:
    typedef object_types::domain_type targetType;

  public:

      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta);

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      domain_data instance_data;


  protected:

  private:


};






class port_runtime_data : public basic_runtime_data  
{
public:
    typedef object_types::port_type targetType;

  public:

      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta);

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      port_data instance_data;


  protected:

  private:


};






class application_runtime_data : public basic_runtime_data  
{
public:
    typedef object_types::application_type targetType;

  public:

      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type, const meta_data& meta);

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


      application_data instance_data;


  protected:

  private:


};


} // namespace runtime_data
} // namespace meta
} // namespace rx_platform



#endif
