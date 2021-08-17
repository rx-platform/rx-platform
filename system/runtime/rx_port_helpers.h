

/****************************************************************************
*
*  system\runtime\rx_port_helpers.h
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


#ifndef rx_port_helpers_h
#define rx_port_helpers_h 1



// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "rx_value_templates.h"
using namespace rx_platform::runtime;


namespace rx_platform {

namespace runtime {

namespace io_types {






class master_port_status 
{

  public:
      master_port_status();


      virtual rx_result initialize (runtime_init_context& ctx);

      void set_online ();

      void set_offline ();

      void write_done (uint64_t time, bool success);

      void read_done (uint64_t time, bool success);

      void received_packet ();

      void sent_packet ();


  protected:

  private:


      remote_owned_value<bool> online_;

      remote_owned_value<float> read_time_;

      remote_owned_value<float> write_time_;

      remote_owned_value<float> max_read_time_;

      remote_owned_value<float> max_write_time_;

      remote_owned_value<int64_t> read_count_;

      remote_owned_value<int64_t> write_count_;

      remote_owned_value<int64_t> failed_read_count_;

      remote_owned_value<int64_t> failed_write_count_;

      remote_owned_value<int32_t> queue_size_;

      remote_owned_value<int64_t> received_;

      remote_owned_value<int64_t> sent_;


};






class simple_port_status 
{

  public:
      simple_port_status();


      rx_result initialize (runtime_init_context& ctx);

      void received_packet ();

      void sent_packet ();


  protected:

  private:


      remote_owned_value<int64_t> received_;

      remote_owned_value<int64_t> sent_;


};






class external_port_status 
{

  public:
      external_port_status();


      void received_packet (size_t bytes);

      void sent_packet (size_t bytes);

      rx_result initialize (runtime_init_context& ctx);


  protected:

  private:


      remote_owned_value<int64_t> received_bytes_;

      remote_owned_value<int64_t> sent_bytes_;

      remote_owned_value<int64_t> received_;

      remote_owned_value<int64_t> sent_;


};


} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
