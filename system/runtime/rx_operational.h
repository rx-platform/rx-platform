

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
namespace relations {
class relation_runtime;

} // namespace relations
} // namespace runtime
} // namespace rx_platform




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

    // IMPORTANT!!!!
    // relations::relation_runtime* is a pointer because of a object lifetime!!!
    // since the runtime object is holding a reference smart pointer
    // and this class is part of an object structure we can be sure that
    // lifetime of an object is shorter than lifetime of the reference to relation runtime
    // this is why i'm considering hiding this functionality to a different part of platform?
    typedef std::map<relations::relation_runtime*, runtime_handle_t> relations_type;

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

    typedef std::map<string_type, relations::relation_runtime*> mapped_relations_type;

    typedef std::map<runtime_handle_t, relations::relation_runtime*> relation_handles_map_type;


  public:
      connected_tags();

      ~connected_tags();


      rx_result_with<runtime_handle_t> connect_tag (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor, const structure::hosting_object_data& state);

      rx_result disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor);

      bool process_runtime (algorithms::runtime_process_context& ctx);

      rx_result read_tag (runtime_handle_t item, tags_callback_ptr monitor, const structure::hosting_object_data& state);

      void binded_tags_change (structure::value_data* whose, const rx_value& val, structure::hosting_object_data& state);

      rx_result write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, const structure::hosting_object_data& state);

      void relation_tags_change (relations::relation_runtime* whose, const rx_value& val, structure::hosting_object_data& state);

      rx_result_with<runtime_handle_t> connect_tag_from_relations (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor, const structure::hosting_object_data& state);

      void runtime_stopped (const rx_time& now);

      void variable_change (structure::variable_data* whose, const rx_value& val, structure::hosting_object_data& state);


  protected:

  private:


      const_values_type const_values_;

      values_type values_;

      variables_type variables_;

      relations_type relations_;


      handles_map_type handles_map_;

      referenced_tags_type referenced_tags_;

      next_send_type next_send_;

      mapped_relations_type mapped_relations_;

      relation_handles_map_type relations_handles_map_;


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

      rx_result set_value (runtime_handle_t handle, rx_simple_value&& val, connected_tags& tags, structure::hosting_object_data& state);

      rx_result_with<runtime_handle_t> bind_item (const string_type& path, runtime_init_context& ctx);

      rx_result set_item (const string_type& path, rx_simple_value&& what, runtime_init_context& ctx);

      rx_result pool_value (runtime_handle_t handle, std::function<void(const rx_value&)> callback) const;

      void connected_tags_change (structure::value_data* whose, const rx_value& val);

      rx_result set_item (const string_type& path, rx_simple_value&& what, runtime_start_context& ctx);

	  template<typename T>
	  rx_result set_item_static(const string_type& path, T&& value, runtime_init_context& ctx)
	  {
		  rx_simple_value temp;
		  temp.assign_static<T>(std::forward<T>(value));
		  auto result = set_item(path, std::move(temp), ctx);
		  
		  return result;
	  }
      template<typename T>
      rx_result set_item_static(const string_type& path, T&& value, runtime_start_context& ctx)
      {
          rx_simple_value temp;
          temp.assign_static<T>(std::forward<T>(value));
          auto result = set_item(path, std::move(temp), ctx);

          return result;
      }
  protected:

  private:

      rx_result internal_set_item (const string_type& path, rx_simple_value&& what, runtime_structure_resolver& structure);



      const_values_type const_values_;

      values_type values_;


      handles_map_type handles_map_;


};


} // namespace operational
} // namespace runtime
} // namespace rx_platform



#endif
