

/****************************************************************************
*
*  system\runtime\rx_operational.h
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


#ifndef rx_operational_h
#define rx_operational_h 1



// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_ptr
#include "lib/rx_ptr.h"





namespace rx_platform {

namespace runtime {

namespace operational {


struct update_item
{
	runtime_handle_t handle;
	rx_value value;
};

struct write_result_item
{
	runtime_handle_t handle;
	rx_result result;
};




class rx_tags_callback : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_tags_callback);

  public:

      virtual void items_changed (const std::vector<update_item>& items) = 0;

      virtual void transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<update_item>&& items) = 0;

      virtual rx_thread_handle_t get_target () = 0;


  protected:

  private:


};







class connected_tags 
{
	typedef std::map<structure::const_value_data*, runtime_handle_t> const_values_type;
	typedef std::map<structure::value_data*, runtime_handle_t> values_type;
	typedef std::map<structure::variable_data*, runtime_handle_t> variables_type;

	typedef std::function<void(std::vector<update_item> items)> callback_function_t;
	struct one_tag_data
	{
		rt_value_ref reference;
		uint32_t reference_count;
		std::set<tags_callback_ptr> monitors;
	};
	typedef std::map<runtime_handle_t, one_tag_data> handles_map_type;
	
	typedef std::map<string_type, runtime_handle_t> referenced_tags_type;

	typedef std::map<tags_callback_ptr, std::map<runtime_handle_t, rx_value> > next_send_type;

  public:
      connected_tags();

      ~connected_tags();


      rx_result_with<runtime_handle_t> connect_tag (const string_type& path, blocks::runtime_holder* item, tags_callback_ptr monitor, const structure::hosting_object_data& state);

      rx_result disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor);

      bool process_runtime (runtime_process_context& ctx);

      rx_result read_tag (runtime_handle_t item, tags_callback_ptr monitor, const structure::hosting_object_data& state);

      void value_set (structure::value_data* whose, const rx_simple_value& val);


  protected:

  private:


      const_values_type const_values_;

      values_type values_;

      variables_type variables_;


      handles_map_type handles_map_;

      referenced_tags_type referenced_tags_;

      locks::slim_lock lock_;

      next_send_type next_send_;


};







class binded_tags 
{
	typedef std::map<structure::const_value_data*, runtime_handle_t> const_values_type;
	typedef std::map<structure::value_data*, runtime_handle_t> values_type;
	typedef std::map<runtime_handle_t, rt_value_ref> handles_map_type;

  public:
      binded_tags();

      ~binded_tags();


      rx_result_with<runtime_handle_t> bind_tag (const rt_value_ref& ref, runtime_handle_t handle);

      rx_result get_value (runtime_handle_t handle, rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, rx_simple_value&& val, connected_tags& tags);

      rx_result_with<runtime_handle_t> bind_item (const string_type& path, runtime_init_context& ctx);

      rx_result set_item (const string_type& path, rx_simple_value&& what, runtime_init_context& ctx);

      rx_result pool_value (runtime_handle_t handle, std::function<void(const rx_value&)> callback) const;

	  template<typename T>
	  rx_result set_item_static(const string_type& path, T&& value, runtime_init_context& ctx)
	  {
		  rx_simple_value temp;
		  temp.assign_static<T>(std::forward<T>(value));
		  auto result = set_item(path, std::move(temp), ctx);
		  
		  return result;
	  }
  protected:

  private:


      const_values_type const_values_;

      values_type values_;


      handles_map_type handles_map_;


};


} // namespace operational
} // namespace runtime
} // namespace rx_platform



#endif
