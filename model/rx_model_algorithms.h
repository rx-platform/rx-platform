

/****************************************************************************
*
*  model\rx_model_algorithms.h
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


meta_data resolve_reference(
    const rx_item_reference& ref
    , rx_item_type& type
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
bool is_derived_from(rx_node_id id, rx_node_id from_id);
bool is_instanced_from(rx_node_id id, rx_node_id from_id);

rx_result_with<platform_item_ptr> get_working_runtime(const rx_node_id& id);

template<class resultT, class funcT>
rx_result do_with_item(
	const rx_node_id& id
    , funcT what
    , rx_result_with_callback<resultT>&& callback)
{
	rx_platform::rx_post_function_to(RX_DOMAIN_META, callback.get_anchor()
        , [](rx_node_id id, funcT what, rx_result_with_callback<resultT>&& callback) mutable
        {
            auto result = get_platform_item_sync(id);
            if (!result)
            {
                callback(rx_result_with_callback<resultT>(rx_result_with<resultT>(result.errors())));
            }
            else
            {
                auto executer = result.value()->get_executer();
                rx_platform::rx_post_function_to(executer, callback.get_anchor()
                    , [](platform_item_ptr item, funcT&& what, rx_result_with_callback<resultT>&& callback) mutable
                    {
                        auto result = what(item);
                        callback(std::move(result));
                    }, result.value(), std::move(what), std::move(callback));
            }
        }, id, std::move, std::move(callback));
	return true;
}

template<class funcT>
rx_result do_with_item(
    const rx_node_id& id
    , funcT what
    , rx_result_callback callback
    , rx_platform::api::rx_context ctx)
{
    using remote_func_type_t = rx_platform::callback::rx_remote_function<rx_result, platform_item_ptr>;

    auto anchor = callback.get_anchor();

    remote_func_type_t remote(ctx.object, RX_DOMAIN_META, std::move(what), std::move(callback));


    rx_platform::rx_post_function_to(RX_DOMAIN_META, anchor
        , [](rx_node_id id, remote_func_type_t&& remote)
        {
            auto result = get_platform_item_sync(id);
            if (result)
            {
                auto executer = result.value()->get_executer();
                remote.set_target(executer);
                remote(result.move_value());
            }
            else
            {
                remote.send_result(result.errors());
            }
        }, std::move(id), std::move(remote));
	return true;
}
/*
template<class funcT>
rx_result do_with_item_storage(
    const rx_node_id& id
    , funcT what
    , rx_result_callback callback
    , rx_platform::api::rx_context ctx)
{

    rx_platform::rx_post_function_to(RX_DOMAIN_META, anchor
        , [](rx_node_id id, funcT what, rx_result_callback callback)
        {
            auto result = get_platform_item_sync(id);
            if (result)
            {
                auto storage_result = result.value()->meta_info().resolve_storage();
                if (storage_result)
                {
                    rx_platform::rx_post_function_to(RX_DOMAIN_SLOW, anchor
                        , [](rx_storage_ptr item, funcT what, rx_result_callback callback)
                        {
                            auto result = what(std::move(item));
                            callback(std::move(result));

                        }, result.move_value(), std::move(func), std::move(remote));
                }
                else
                {
                    callback(storage_result.errors());
                }
            }
            else
            {
                callback(result.errors());
            }
        }, std::move(id), std::move(func), std::move(remote));
    return true;
}
*/
template<class callbackT, class funcT>
rx_result do_with_runtime_item(
    const rx_node_id& id
    , funcT what
    , callbackT callback
    , rx_platform::api::rx_context ctx)
{
    auto item_result = get_working_runtime(id);
    if (!item_result)
    {
        return item_result.errors();
    }
    auto item = item_result.move_value();
    using resultT = decltype(what(std::move(item)));
    auto item_executer = item->get_executer();

    rx_platform::callback::rx_remote_function<resultT, platform_item_ptr> remote(ctx.object, item_executer, std::move(what), std::move(callback));

    remote(std::move(item));

    return true;
}


template<class funcT>
rx_result do_with_runtime_item(
    const rx_node_id& id
    , funcT what
    , rx_result_callback callback
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
    , funcT what
    , rx_platform::api::rx_context ctx)
{

    auto item_result = get_working_runtime(id);
    if (!item_result)
    {
        return item_result.errors();
    }
    auto item = item_result.move_value();

    auto item_executer = item->get_executer();

    rx_platform::rx_post_function_to<funcT, platform_item_ptr >(item_executer, ctx.object
        , std::move(what)
        , std::move(item));

    return true;
}




template<class resultT, class refT, class... Args>
rx_result do_with_items(
	const rx_node_ids& ids
	, std::function<resultT(Args...)> what
	, rx_result_callback callback
	, rx_platform::api::rx_context ctx);


template<class callbackT, class funcT>
rx_result do_in_meta_with_dir(
    funcT what
    , callbackT callback
    , rx_directory_ptr item
    , rx_platform::api::rx_context ctx)
{
    using resultT = decltype(what(item));

    rx_platform::callback::rx_remote_function<resultT, rx_directory_ptr> remote(ctx.object, RX_DOMAIN_META, std::move(what), std::move(callback));

    remote(std::move(item));

    return true;
}


template<class funcT>
rx_result do_in_meta_with_dir(
    funcT what
    , rx_result_callback callback
    , rx_directory_ptr item
    , rx_platform::api::rx_context ctx)
{

    auto meta_executer = infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_META);

    rx_platform::rx_do_with_callback<rx_reference_ptr, funcT>(meta_executer, ctx.object
        , std::forward<funcT>(what), std::move(callback)
        , std::move(item));

    return true;
}







template <class typeT>
class types_model_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void update_type (typename typeT::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback);

      static void check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (typename typeT::smart_ptr prototype);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);


  protected:

  private:

      static rx_result_with<typename typeT::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<typename typeT::smart_ptr> update_type_sync (typename typeT::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, const string_type& dir);



};






template <class typeT>
class simple_types_model_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void create_type (typename typeT::smart_ptr prototype, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void update_type (typename typeT::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback);

      static void check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (typename typeT::smart_ptr prototype);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);


  protected:

  private:

      static rx_result_with<typename typeT::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<typename typeT::smart_ptr> update_type_sync (typename typeT::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, const string_type& dir);



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

      static void delete_runtime (const rx_item_reference& item_reference, rx_result_callback&& callback);

      static rx_result init_runtime (typename typeT::RTypePtr what);

      static rx_result_with<typename typeT::RTypePtr> create_runtime_sync (instanceT&& instance_data, data::runtime_values_data&& runtime_data, bool temp = false);

      static rx_result delete_runtime_sync (const rx_item_reference& item_reference, rx_thread_handle_t result_target, rx_result_callback&& callback);


  protected:

  private:

      static rx_result_with<typename typeT::RTypePtr> get_runtime_sync (const rx_item_reference& item_reference);

      static rx_result_with<typename typeT::RTypePtr> create_prototype_sync (instanceT&& instance_data);

      static void update_runtime_sync (instanceT&& instance_data, rx_update_runtime_data update_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback, rx_thread_handle_t result_target, bool temp = false);

      static void update_runtime_with_depends_sync (instanceT&& instance_data, rx_update_runtime_data update_data, rx_result_with_callback<typename typeT::RTypePtr>&& callback, rx_thread_handle_t result_target, bool temp = false);



};






class relation_types_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename relation_type::smart_ptr>&& callback);

      static void create_type (relation_type::smart_ptr prototype, rx_result_with_callback<typename relation_type::smart_ptr>&& callback);

      static void update_type (relation_type::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename relation_type::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback);

      static void check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<relation_type::smart_ptr> create_type_sync (relation_type::smart_ptr prototype);


  protected:

  private:

      static rx_result_with<relation_type::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<relation_type::smart_ptr> update_type_sync (relation_type::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, const string_type& dir);



};






class data_types_model_algorithm 
{

  public:

      static void get_type (const rx_item_reference& item_reference, rx_result_with_callback<typename data_type::smart_ptr>&& callback);

      static void create_type (data_type::smart_ptr prototype, rx_result_with_callback<typename data_type::smart_ptr>&& callback);

      static void update_type (data_type::smart_ptr prototype, rx_update_type_data update_data, rx_result_with_callback<typename data_type::smart_ptr>&& callback);

      static void delete_type (const rx_item_reference& item_reference, rx_result_callback&& callback);

      static void check_type (const string_type& name, const string_type& dir, rx_result_with_callback<check_type_result>&& callback);

      static rx_result_with<data_type::smart_ptr> create_type_sync (data_type::smart_ptr prototype);


  protected:

  private:

      static rx_result_with<data_type::smart_ptr> get_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<data_type::smart_ptr> update_type_sync (data_type::smart_ptr prototype, rx_update_type_data update_data);

      static rx_result delete_type_sync (const rx_item_reference& item_reference);

      static rx_result_with<check_type_result> check_type_sync (const string_type& name, const string_type& dir);



};






class transaction_algorithm 
{

  public:

      static rx_result_with<api::query_result> get_dependents (rx_item_reference item, string_view_type dir);


  protected:
      template<typename T>
      static rx_result fill_simple_type_dependents(const rx_node_id& id, std::set<rx_node_id>& results);
  private:


};


} // namespace algorithms
} // namespace model
} // namespace rx_internal



#endif
