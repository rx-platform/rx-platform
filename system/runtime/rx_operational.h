

/****************************************************************************
*
*  system\runtime\rx_operational.h
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


#ifndef rx_operational_h
#define rx_operational_h 1



// rx_relations
#include "system/runtime/rx_relations.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace runtime {
class runtime_process_context;
namespace tag_blocks {
class binded_tags;
} // namespace tag_blocks

namespace logic_blocks {
class method_data;

} // namespace logic_blocks
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {
namespace logic_blocks {
class logic_holder;
} // namespace logic_blocks
namespace display_blocks {
class displays_holder;
} // namespace logic_blocks

namespace tag_blocks {





class rx_tags_callback : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_tags_callback);

  public:

      virtual void items_changed (const std::vector<update_item>& items) = 0;

      virtual void execute_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, data::runtime_values_data data) = 0;

      virtual void write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result) = 0;


  protected:

  private:


};







class connected_tags 
{
    using relation_ptr = rx_reference<relations::relation_data>;
	typedef std::map<structure::const_value_data*, runtime_handle_t> const_values_type;
	typedef std::map<structure::value_data*, runtime_handle_t> values_type;
    typedef std::map<structure::full_value_data*, runtime_handle_t> full_values_type;
    typedef std::map<logic_blocks::method_data*, runtime_handle_t> methods_type;
    typedef std::map<structure::indirect_value_data*, runtime_handle_t> indirect_values_type;
	typedef std::map<structure::variable_data*, runtime_handle_t> variables_type;
    typedef std::map<relation_ptr, runtime_handle_t> relations_type;
    typedef std::map<relations::relation_value_data*, runtime_handle_t> relation_values_type;

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
 
    typedef std::map<tags_callback_ptr, std::vector<write_result_data> > write_results_type;
    typedef std::vector<write_tag_data> write_requests_type;

    typedef std::map<tags_callback_ptr, std::vector<execute_result_data> > execute_results_type;
    typedef std::vector<execute_tag_data> execute_requests_type;

    typedef std::map<string_type, relation_ptr> mapped_relations_type;

    friend class algorithms::runtime_relation_algorithms;

  public:
      connected_tags();

      ~connected_tags();


      void init_tags (runtime_process_context* ctx, relations::relations_holder* relations, logic_blocks::logic_holder* logic, display_blocks::displays_holder* displays, binded_tags* binded);

      void runtime_stopped (const rx_time& now);

      rx_result_with<runtime_handle_t> connect_tag (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor = tags_callback_ptr::null_ptr);

      rx_result read_tag (runtime_handle_t item, tags_callback_ptr monitor);

      rx_result write_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor);

      rx_result execute_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, data::runtime_values_data data, tags_callback_ptr monitor);

      rx_result disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor = tags_callback_ptr::null_ptr);

      bool process_runtime ();

      bool process_transactions ();

      void binded_value_change (structure::value_data* whose, const rx_value& val);

      void variable_change (structure::variable_data* whose, const rx_value& val);

      void relation_value_change (relations::relation_value_data* whose, const rx_value& val);

      void write_result_arrived (tags_callback_ptr whose, write_result_data&& data);

      void execute_result_arrived (tags_callback_ptr whose, execute_result_data&& data);

      void target_relation_removed (relation_ptr&& whose);

      void full_value_changed (structure::full_value_data* whose, const rx_value& val);


  protected:

  private:

      rx_result_with<runtime_handle_t> connect_tag_from_relations (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor);

      rx_result internal_write_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor);

      rx_result internal_execute_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, data::runtime_values_data args, tags_callback_ptr monitor, rx_security_handle_t identity);

      connected_tags::relation_ptr get_parent_relation (const string_type& name);

      rx_result_with<runtime_handle_t> register_new_tag_ref (const string_type& path, rt_value_ref ref, tags_callback_ptr monitor);



      values_type values_;

      variables_type variables_;

      runtime_process_context *context_;

      indirect_values_type indirect_values_;

      relation_values_type relation_values_;

      binded_tags *binded_;

      structure::const_value_data const_values_;

      full_values_type full_values_;

      methods_type methods_;


      handles_map_type handles_map_;

      referenced_tags_type referenced_tags_;

      next_send_type next_send_;

      mapped_relations_type mapped_relations_;

      relations::relations_holder* parent_relations_;

      write_results_type write_results_;

      write_requests_type write_requests_;

      execute_results_type execute_results_;

      execute_requests_type execute_requests_;

      logic_blocks::logic_holder* parent_logic_;

      display_blocks::displays_holder* parent_displays_;


};







class binded_tags 
{
	typedef std::map<structure::const_value_data*, runtime_handle_t> const_values_type;
	typedef std::map<structure::value_data*, std::pair<runtime_handle_t, std::vector<binded_callback_t> > > values_type;
    typedef std::map<structure::full_value_data*, std::pair<runtime_handle_t, std::vector<binded_callback_t> > > full_values_type;
    typedef std::map<logic_blocks::method_data*, std::pair<runtime_handle_t, std::vector<binded_callback_t> > > methods_type;
	typedef std::map<runtime_handle_t, rt_value_ref> handles_map_type;

  public:
      binded_tags();

      ~binded_tags();


      rx_result get_value (runtime_handle_t handle, rx_simple_value& val) const;

      rx_result set_value (runtime_handle_t handle, rx_simple_value&& val, connected_tags& tags, runtime_process_context* ctx);

      rx_result_with<runtime_handle_t> bind_item (const string_type& path, runtime_init_context& ctx, binded_callback_t callback);

      rx_result set_item (const string_type& path, rx_simple_value&& what, runtime_init_context& ctx);

      rx_result pool_value (runtime_handle_t handle, std::function<void(const rx_value&)> callback) const;

      void value_change (structure::value_data* whose, const rx_value& val);

      void full_value_changed (structure::full_value_data* whose, const rx_value& val, connected_tags& tags);

      rx_result set_item (const string_type& path, rx_simple_value&& what, runtime_start_context& ctx);

      rx_result get_item (const string_type& path, rx_simple_value& what, runtime_start_context& ctx);

      rx_result get_item (const string_type& path, rx_simple_value& what, runtime_init_context& ctx);

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

      rx_result internal_get_item (const string_type& path, rx_simple_value& what, runtime_structure_resolver& structure, runtime_process_context* ctx);



      values_type values_;

      structure::const_value_data *const_values_;

      full_values_type full_values_;

      methods_type methods_;


      handles_map_type handles_map_;


};






class connected_write_task : public structure::write_task  
{

  public:
      connected_write_task (connected_tags* parent, tags_callback_ptr callback, runtime_transaction_id_t id, runtime_handle_t item);


      void process_result (rx_result&& result);


  protected:

  private:


      connected_tags *parent_;


      runtime_transaction_id_t id_;

      tags_callback_ptr callback_;

      runtime_handle_t item_;


};






class connected_execute_task : public structure::execute_task  
{

  public:
      connected_execute_task (connected_tags* parent, tags_callback_ptr callback, runtime_transaction_id_t id, runtime_handle_t item);


      void process_result (rx_result&& result, data::runtime_values_data&& data);


  protected:

  private:


      connected_tags *parent_;


      runtime_transaction_id_t id_;

      tags_callback_ptr callback_;

      runtime_handle_t item_;


};


} // namespace tag_blocks
} // namespace runtime
} // namespace rx_platform



#endif
