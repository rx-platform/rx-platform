

/****************************************************************************
*
*  model\rx_model_algorithms.h
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


#ifndef rx_model_algorithms_h
#define rx_model_algorithms_h 1


#include "system/server/rx_async_functions.h"

// rx_meta_internals
#include "model/rx_meta_internals.h"

#include "sys_internal/rx_internal_ns.h"


namespace model {

namespace algorithms {

rx_result_with<rx_node_id> resolve_reference(
	const item_reference& ref
	, ns::rx_directory_resolver& directories);

template<typename typeT>
rx_result_with<rx_node_id> resolve_type_reference(
	const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<typeT>);

rx_result_with<rx_node_id> resolve_relation_reference(
	const item_reference& ref
	, ns::rx_directory_resolver& directories);

template<typename typeT>
rx_result_with<rx_node_id> resolve_simple_type_reference(
	const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<typeT>);

template<typename typeT>
rx_result_with<rx_node_id> resolve_runtime_reference(
	const item_reference& ref
	, ns::rx_directory_resolver& directories, tl::type2type<typeT>);

rx_result_with<platform_item_ptr> get_platform_item_sync(rx_item_type type, rx_node_id id);

rx_result_with<platform_item_ptr> get_working_runtime_sync(const rx_node_id& id);

template<class resultT, class refT>
rx_result do_with_item(
	const rx_node_id& id
	, std::function<resultT(rx_result_with<platform_item_ptr>&&)> what
	, std::function<void(resultT)> callback
	, rx_platform::api::rx_context ctx)
{
	auto ret_executer = rx_thread_context();

	std::function<void(rx_result_with<platform_item_ptr>&&)> func2 = [what, ret_executer, callback, ctx](rx_result_with<platform_item_ptr>&& who)
	{
		auto ret_val = what(std::move(who.move_value()));
		rx_platform::rx_post_function_to<rx_reference_ptr, resultT>(ret_executer, callback, ctx.object, ret_val);
	};

	std::function<void(const rx_node_id&)> func = [func2, ctx](const rx_node_id& id) {
		auto result = get_working_runtime_sync(id);

		auto executer = result.value()->get_executer();
		if (executer == RX_DOMAIN_META)
			func2(std::move(result));
		else
			rx_post_result_to(executer, func2, ctx.object, std::move(result));

	};
	rx_platform::rx_post_function_to<rx_reference_ptr, const rx_node_id&>(RX_DOMAIN_META, func, ctx.object, id);
	return true;
}



template<class resultT, class refT, class... Args>
rx_result do_with_items(
	const rx_node_ids& ids
	, std::function<resultT(Args...)> what
	, std::function<void(resultT)> callback
	, rx_platform::api::rx_context ctx);






template <class typeT>
class types_model_algorithm
{

  public:

      static void check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref);

      static void create_type (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes);

      static void delete_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref);

      static void update_type (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref);

      static void get_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref);


  protected:

  private:

      static type_check_context check_type_sync (const string_type& name, rx_directory_ptr dir);

      static rx_result delete_type_sync (const item_reference& item_reference, rx_directory_ptr dir);

      static rx_result_with<typename typeT::smart_ptr> update_type_sync (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version);

      static rx_result_with<typename typeT::smart_ptr> get_type_sync (const item_reference& item_reference, rx_directory_ptr dir);



};






template <class typeT>
class simple_types_model_algorithm
{

  public:

      static void check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref);

      static void create_type (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (const string_type& name, const item_reference& base_reference, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes);

      static void delete_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref);

      static void update_type (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref);

      static void get_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_reference_ptr ref);


  protected:

  private:

      static type_check_context check_type_sync (const string_type& name, rx_directory_ptr dir);

      static rx_result delete_type_sync (const item_reference& item_reference, rx_directory_ptr dir);

      static rx_result_with<typename typeT::smart_ptr> update_type_sync (typename typeT::smart_ptr prototype, rx_directory_ptr dir, bool increment_version);

      static rx_result_with<typename typeT::smart_ptr> get_type_sync (const item_reference& item_reference, rx_directory_ptr dir);



};






template <class typeT>
class runtime_model_algorithm
{

  public:

      static void delete_runtime (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref);

      static void create_runtime (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref);

      static rx_result_with<typename typeT::RTypePtr> create_runtime_sync (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, rx_reference_ptr ref);

      static void create_prototype (const meta_data& info, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref);

      static void create_runtime_implicit (const string_type& name, const item_reference& base_reference, namespace_item_attributes attributes, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref);

      static rx_result_with<typename typeT::RTypePtr> create_runtime_implicit_sync (const string_type& name, const item_reference& base_reference, namespace_item_attributes attributes, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, rx_directory_ptr dir, rx_reference_ptr ref);

      static rx_result init_runtime (typename typeT::RTypePtr what);

      static void update_runtime (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, bool increment_version, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref);

      static rx_result update_runtime_sync (const meta_data& info, data::runtime_values_data* init_data, typename typeT::instance_data_t instance_data, bool increment_version, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref, rx_thread_handle_t result_target);

      static void get_runtime (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_reference_ptr ref);


  protected:

  private:

      static rx_result delete_runtime_sync (const item_reference& item_reference, rx_directory_ptr dir, rx_thread_handle_t result_target, std::function<void(rx_result)> callback, rx_reference_ptr ref);

      static rx_result_with<typename typeT::RTypePtr> create_prototype_sync (const meta_data& info, typename typeT::instance_data_t instance_data, rx_directory_ptr dir);

      static rx_result helper_delete_runtime_sync (meta_data_t info);

      static rx_result delete_runtime_sync (meta_data_t info, rx_thread_handle_t result_target, std::function<void(rx_result)> callback, rx_reference_ptr ref);

      static rx_result_with<typename typeT::RTypePtr> get_runtime_sync (const item_reference& item_reference, rx_directory_ptr dir);



};






class relation_types_algorithm
{

  public:

      static void check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_reference_ptr ref);

      static void create_type (const string_type& name, const item_reference& base_reference, relation_type::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<relation_type::smart_ptr>&&)> callback, rx_reference_ptr ref);

      static rx_result_with<relation_type::smart_ptr> create_type_sync (const string_type& name, const item_reference& base_reference, relation_type::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes);

      static void delete_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_reference_ptr ref);

      static void update_type (relation_type::smart_ptr prototype, rx_directory_ptr dir, bool increment_version, std::function<void(rx_result_with<relation_type::smart_ptr>&&)> callback, rx_reference_ptr ref);

      static void get_type (const item_reference& item_reference, rx_directory_ptr dir, std::function<void(rx_result_with<relation_type::smart_ptr>&&)> callback, rx_reference_ptr ref);


  protected:

  private:

      static type_check_context check_type_sync (const string_type& name, rx_directory_ptr dir);

      static rx_result delete_type_sync (const item_reference& item_reference, rx_directory_ptr dir);

      static rx_result_with<relation_type::smart_ptr> update_type_sync (relation_type::smart_ptr prototype, rx_directory_ptr dir, bool increment_version);

      static rx_result_with<relation_type::smart_ptr> get_type_sync (const item_reference& item_reference, rx_directory_ptr dir);



};


} // namespace algorithms
} // namespace model



#endif
