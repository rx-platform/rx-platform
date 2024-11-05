

/****************************************************************************
*
*  enterprise\rx_json_ent.h
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


#ifndef rx_json_ent_h
#define rx_json_ent_h 1



// rx_ent_alg
#include "enterprise/rx_ent_alg.h"



namespace rx_internal {

namespace enterprise {





class json_enterprise_interface : public std_enterprise_interface  
{

  public:
      json_enterprise_interface();

      ~json_enterprise_interface();


      void sync_read (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, const enterprise_args_t& args, rx_reference_ptr anchor);

      void sync_read (uint64_t trans_id, rx_directory_ptr dir, const rx_node_id& type_id, rx_item_type type_type, const enterprise_args_t& args, rx_reference_ptr anchor);

      void sync_write (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, string_view_type data, const enterprise_args_t& args, rx_reference_ptr anchor);

      void sync_execute (uint64_t trans_id, platform_item_ptr item, string_view_type sub_path, string_view_type data, const enterprise_args_t& args, rx_reference_ptr anchor);

      string_type create_error_response (int code, string_type message);

      string_view_type get_content_type ();

      string_view_type get_name ();


  protected:

  private:

      rx_result recursive_list_directory (rx_directory_ptr item, std::vector<platform_item_ptr>& rt_items, const std::set<rx_node_id>& by_type);



};


} // namespace enterprise
} // namespace rx_internal



#endif
