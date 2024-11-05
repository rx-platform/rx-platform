

/****************************************************************************
*
*  enterprise\rx_ent_alg.h
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


#ifndef rx_ent_alg_h
#define rx_ent_alg_h 1



// rx_enterprise
#include "enterprise/rx_enterprise.h"

#include "system/rx_platform_typedefs.h"
#include "system/server/rx_platform_item.h"
using rx_platform::runtime_value_type;


namespace rx_internal {

namespace enterprise {





class enterprise_interface_callback 
{

  public:
      virtual ~enterprise_interface_callback();


      virtual void read_complete (uint64_t trans_id, uint32_t result, data::runtime_values_data data) = 0;

      virtual void write_complete (uint64_t trans_id, uint32_t result, data::runtime_values_data data) = 0;

      virtual void execute_complete (uint64_t trans_id, uint32_t result, data::runtime_values_data data) = 0;

      virtual void query_complete (uint64_t trans_id, uint32_t result, data::runtime_values_data data) = 0;


  protected:

  private:


};







class enterprise_handler 
{

  public:

      void begin_read (uint64_t trans_id, string_view_type path, runtime_value_type type, enterprise_interface_callback* callback);

      void begin_write (uint64_t trans_id, string_view_type path, runtime_value_type type, data::runtime_values_data data, enterprise_interface_callback* callback);

      void begin_execute (uint64_t trans_id, string_view_type path, runtime_value_type type, data::runtime_values_data data, enterprise_interface_callback* callback);

      void begin_query (uint64_t trans_id, string_view_type path, runtime_value_type type, data::runtime_values_data data, enterprise_interface_callback* callback);


  protected:

  private:


};






class std_enterprise_interface : public enterprise_interface  
{
    struct trans_data_t
    {
        uint64_t caller_id;
        enterprise_callback* callback = nullptr;
        rx_time started;
    };
    typedef std::map<uint64_t, trans_data_t> transactions_type;

  public:
      std_enterprise_interface();

      ~std_enterprise_interface();


      void begin_read (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, enterprise_callback* callback, rx_reference_ptr anchor);

      void begin_write (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor);

      void begin_execute (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor);

      void begin_query (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor);

      virtual string_type create_error_response (int code, string_type message) = 0;

      virtual void sync_read (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, const enterprise_args_t& args, rx_reference_ptr anchor) = 0;

      virtual void sync_read (uint64_t trans_id, rx_directory_ptr dir, const rx_node_id& type_id, rx_item_type type_type, const enterprise_args_t& args, rx_reference_ptr anchor) = 0;

      virtual void sync_write (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, string_view_type data, const enterprise_args_t& args, rx_reference_ptr anchor) = 0;

      virtual void sync_execute (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, string_view_type data, const enterprise_args_t& args, rx_reference_ptr anchor) = 0;

      virtual string_view_type get_content_type () = 0;


  protected:

      void finish_read_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      void finish_write_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      void finish_execute_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      void finish_query_transaction (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data);

      uint64_t register_transaction (uint64_t trans_id, enterprise_callback* callback, rx_reference_ptr anchor);


  private:


      rx_directory_resolver resolver_dirs_;

      std::atomic<uint64_t> g_last_id_;

      locks::slim_lock transactions_lock_;

      transactions_type transactions_;


};


} // namespace enterprise
} // namespace rx_internal



#endif
