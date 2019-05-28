

/****************************************************************************
*
*  system\runtime\rx_operational.h
*
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


#ifndef rx_operational_h
#define rx_operational_h 1



// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"

namespace rx_platform {
namespace runtime {
namespace blocks {
class runtime_object;

} // namespace blocks
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {
typedef uint_fast32_t runtime_handle_t;
typedef uint_fast32_t runtime_transaction_id_t;

namespace runtime {

namespace operational {

struct write_result_type
{
	runtime_handle_t handle;
	rx_result result;
};




class rx_write_task_callback 
{

  public:

      virtual void write_done (runtime_transaction_id_t transaction_id, const std::vector<write_result_type>& results) = 0;


  protected:

  private:


};






class rx_execute_task 
{

  public:

  protected:

  private:


};






class rx_read_task 
{

  public:

  protected:

  private:


};







class connected_tags 
{
	typedef std::map<structure::const_value_data*, runtime_handle_t> const_values_type;
	typedef std::map<structure::value_data*, runtime_handle_t> values_type;
	typedef std::map<structure::variable_data*, runtime_handle_t> variables_type;
	struct one_tag_data
	{
		rt_value_ref reference;
		uint32_t reference_count;
	};
	typedef std::map<runtime_handle_t, one_tag_data> handles_map_type;
	
	typedef std::map<string_type, runtime_handle_t> referenced_tags_type;

  public:
      connected_tags();

      ~connected_tags();


      rx_result_with<runtime_handle_t> connect_tag (const string_type& path, blocks::runtime_object* item);

      rx_result disconnect_tag (runtime_handle_t handle);

      rx_result_with<runtime_handle_t> bind_tag (const string_type& path, blocks::runtime_object* item);

      rx_result local_get_value (runtime_handle_t handle, rx_simple_value& val) const;

      rx_result local_set_value (runtime_handle_t handle, rx_simple_value&& val);


  protected:

  private:

      static runtime_handle_t get_new_handle ();



      const_values_type const_values_;

      values_type values_;

      variables_type variables_;


      handles_map_type handles_map_;

      referenced_tags_type referenced_tags_;

      locks::slim_lock lock_;


};


} // namespace operational
} // namespace runtime
} // namespace rx_platform



#endif
