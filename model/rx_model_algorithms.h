

/****************************************************************************
*
*  model\rx_model_algorithms.h
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


#ifndef rx_model_algorithms_h
#define rx_model_algorithms_h 1



// rx_meta_internals
#include "model/rx_meta_internals.h"

#include "system/server/rx_platform_item.h"
#include "sys_internal/rx_async_functions.h"
#include "api/rx_platform_api.h"


namespace rx_internal {

namespace model {

namespace algorithms {

rx_result_with<rx_node_id> resolve_reference(
	const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories);

template<typename typeT>
rx_result_with<rx_node_id> resolve_type_reference(
	const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<typeT>);

rx_result_with<rx_node_id> resolve_relation_reference(
	const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories);

rx_result_with<rx_node_id> resolve_data_type_reference(
    const rx_item_reference& ref
    , const ns::rx_directory_resolver& directories);

template<typename typeT>
rx_result_with<rx_node_id> resolve_simple_type_reference(
	const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<typeT>);

template<typename typeT>
rx_result_with<rx_node_id> resolve_runtime_reference(
	const rx_item_reference& ref
	, const ns::rx_directory_resolver& directories, tl::type2type<typeT>);

rx_result_with<platform_item_ptr> get_platform_item_sync(rx_item_type type, rx_node_id id);
rx_result_with<platform_item_ptr> get_platform_item_sync(rx_node_id id);

rx_result_with<platform_item_ptr> get_working_runtime(const rx_node_id& id);

template<class resultT, class funcT>
rx_result do_with_item(
	const rx_node_id& id
    , funcT&& what
    , rx_result_with_callback<resultT>&& callback
	, rx_platform::api::rx_context ctx)
{
    using my_result_type = rx_function_to_go<rx_result_with<platform_item_ptr>&&, rx_result_with_callback<resultT>&&>;
	auto ret_executer = rx_thread_context();

	auto func2 = my_result_type(ctx.object,
        [what = std::forward<funcT>(what), ret_executer, ctx]
        (rx_result_with<platform_item_ptr>&& who, rx_result_with_callback<resultT> && callback) mutable
	    {
		    auto ret_val = what(std::move(who));
            callback.set_arguments(std::move(ret_val));
		    rx_platform::rx_post_packed_to(ret_executer, ctx.object, std::move(callback));
	    });

	rx_platform::rx_post_function_to(RX_DOMAIN_META, ctx.object
        , [ret_executer](rx_node_id id, my_result_type&& func2, rx_result_callback&& callback) mutable
        {
            auto result = get_platform_item_sync(id);
            if (!result)
            {
                rx_platform::rx_post_packed_to(ret_executer, std::move(callback));
            }
            else
            {
                auto executer = result.value()->get_executer();
                func2.set_arguments(std::move(result), std::move(callback));
                rx_post_packed_to(executer, std::move(func2));
            }
        }, id, std::move(func2), std::move(callback));
	return true;
}

template<class funcT>
rx_result do_with_item(
    const rx_node_id& id
    , funcT&& what
    , rx_result_callback&& callback
    , rx_platform::api::rx_context ctx)
{
    using my_result_type = rx_function_to_go<rx_result_with<platform_item_ptr>&&, rx_result_callback&&>;
    auto ret_executer = rx_thread_context();

    auto func2 = my_result_type(ctx.object,
        [what = std::forward<funcT>(what), ret_executer, ctx]
        (rx_result_with<platform_item_ptr>&& who, rx_result_callback&& callback) mutable
        {
            auto ret_val = what(std::move(who));
            callback.set_arguments(std::move(ret_val));
            rx_platform::rx_post_packed_to(ret_executer, std::move(callback));
        });

    rx_platform::rx_post_function_to(RX_DOMAIN_META, ctx.object
        , [ret_executer](rx_node_id id, my_result_type&& func2, rx_result_callback&& callback) mutable
        {
            auto result = get_platform_item_sync(id);
            if (!result)
            {
                callback.set_arguments(rx_result(result.errors()));
                rx_platform::rx_post_packed_to(ret_executer, std::move(callback));
            }
            else
            {
                auto executer = result.value()->get_executer();
                func2.set_arguments(std::move(result), std::move(callback));
                rx_post_packed_to(executer, std::move(func2));
            }
        }, id, std::move(func2), std::move(callback));
    return true;
}

template<class resultT, class funcT>
rx_result do_with_runtime_item(
    const rx_node_id& id
    , funcT&& what
    , rx_result_with_callback<resultT>&& callback
    , rx_platform::api::rx_context ctx)
{

    auto item_result = get_working_runtime(id);
    if (!item_result)
    {
        return item_result.errors();
    }
    auto item = item_result.move_value();

    auto item_executer = item->get_executer();

    rx_platform::rx_do_with_callback<rx_reference_ptr, funcT>(item_executer, ctx.object
        , std::forward<funcT>(what), std::move(callback)
        , std::move(item));

    return true;
}


template<class funcT>
rx_result do_with_runtime_item(
    const rx_node_id& id
    , funcT&& what
    , rx_result_callback&& callback
    , rx_platform::api::rx_context ctx)
{

    auto item_result = get_working_runtime(id);
    if (!item_result)
    {
        return item_result.errors();
    }
    auto item = item_result.move_value();

    auto item_executer = item->get_executer();

    rx_platform::rx_do_with_callback<rx_reference_ptr, funcT>(item_executer, ctx.object
        , std::forward<funcT>(what), std::move(callback)
        , std::move(item));

    return true;
}



template<class resultT, class refT, class... Args>
rx_result do_with_items(
	const rx_node_ids& ids
	, std::function<resultT(Args...)> what
	, rx_result_callback&& callback
	, rx_platform::api::rx_context ctx);








template <class typeT>
class types_model_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void update_type (typename typeT::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback);

      static void check_type (const string_type& name, rx_directory_ptr dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (typename typeT::smart_ptr prototype);


  protected:

  private:

      static rx_result_with<typename typeT::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<typename typeT::smart_ptr> update_type_sync (typename typeT::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, rx_directory_ptr dir);



};






template <class typeT>
class simple_types_model_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void update_type (typename typeT::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback);

      static void check_type (const string_type& name, rx_directory_ptr dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (typename typeT::smart_ptr prototype);


  protected:

  private:

      static rx_result_with<typename typeT::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<typename typeT::smart_ptr> update_type_sync (typename typeT::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, rx_directory_ptr dir);



};






template <class typeT>
class runtime_model_algorithm 
{
  public:
      using instanceT = typename typeT::instance_data_t;

  public:

      static void get_runtime (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::RTypePtr>&& callback);

      static void create_runtime (instanceT&& instance_data, data::runtime_values_data&& runtime_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback);

      static void create_prototype (instanceT&& instance_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback);

      static void update_runtime (instanceT&& instance_data, rx_update_runtime_data update_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback);

      static void delete_runtime (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback);

      static rx_result init_runtime (typename typeT::RTypePtr what);

      static rx_result_with<typename typeT::RTypePtr> create_runtime_sync (instanceT&& instance_data, data::runtime_values_data&& runtime_data);


  protected:

  private:

      static rx_result_with<typename typeT::RTypePtr> get_runtime_sync (const rx_item_reference& item_reference);

      static rx_result_with<typename typeT::RTypePtr> create_prototype_sync (instanceT&& instance_data);

      static void update_runtime_sync (instanceT&& instance_data, rx_update_runtime_data update_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback, rx_thread_handle_t result_target);

      static rx_result delete_runtime_sync (const rx_item_reference& item_reference, rx_thread_handle_t result_target, rx_function_to_go<rx_result&&>&& callback);



};






class relation_types_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename relation_type::smart_ptr>&& callback);

      static void create_type (relation_type::smart_ptr prototype, rx_result_with_callback<typename relation_type::smart_ptr>&& callback);

      static void update_type (relation_type::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename relation_type::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback);

      static void check_type (const string_type& name, rx_directory_ptr dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<relation_type::smart_ptr> create_type_sync (relation_type::smart_ptr prototype);


  protected:

  private:

      static rx_result_with<relation_type::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<relation_type::smart_ptr> update_type_sync (relation_type::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, rx_directory_ptr dir);



};






class data_types_model_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename data_type::smart_ptr>&& callback);

      static void create_type (data_type::smart_ptr prototype, rx_result_with_callback<typename data_type::smart_ptr>&& callback);

      static void update_type (data_type::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename data_type::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_function_to_go<rx_result&&>&& callback);

      static void check_type (const string_type& name, rx_directory_ptr dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<data_type::smart_ptr> create_type_sync (data_type::smart_ptr prototype);


  protected:

  private:

      static rx_result_with<data_type::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<data_type::smart_ptr> update_type_sync (data_type::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, rx_directory_ptr dir);



};


} // namespace algorithms
} // namespace model
} // namespace rx_internal



#endif
