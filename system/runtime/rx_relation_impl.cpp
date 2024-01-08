

/****************************************************************************
*
*  system\runtime\rx_relation_impl.cpp
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


#include "pch.h"


// rx_relation_impl
#include "system/runtime/rx_relation_impl.h"

#include "model/rx_model_algorithms.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "platform_api/rx_abi.h"
using namespace rx_internal::sys_runtime;

extern "C"
{

    rx_result_struct c_relation_changed(void* whose)
    {
        rx_platform::runtime::relations::extern_relation_runtime* self = (rx_platform::runtime::relations::extern_relation_runtime*)whose;
        return rx_result(RX_NOT_IMPLEMENTED).move();// self->extern_relation_changed().move();
    }
    void c_relation_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::relations::extern_relation_runtime* self = (rx_platform::runtime::relations::extern_relation_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_relation_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::relations::extern_relation_runtime* self = (rx_platform::runtime::relations::extern_relation_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_relation_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::relations::extern_relation_runtime* self = (rx_platform::runtime::relations::extern_relation_runtime*)whose;
        self->destroy_timer(timer);
    }

    host_relation_def_struct _g_relation_def_
    {
        {
            nullptr
            , nullptr
            , c_relation_start_timer
            , c_relation_suspend_timer
            , c_relation_destroy_timer
        }
    };


}


namespace rx_platform {

namespace runtime {

namespace relations {

// Class rx_platform::runtime::relations::relation_runtime

relation_runtime::relation_runtime()
{
}

relation_runtime::relation_runtime (const string_type& name, const rx_node_id& id, bool system)
{
}


relation_runtime::~relation_runtime()
{
}



rx_result relation_runtime::initialize_relation (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result relation_runtime::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result relation_runtime::start_relation (runtime::runtime_start_context& ctx, bool is_target)
{
	return true;
}

rx_result relation_runtime::stop_relation (runtime::runtime_stop_context& ctx, bool is_target)
{
	return true;
}

rx_result_with<platform_item_ptr> relation_runtime::resolve_runtime_sync (const rx_node_id& id)
{
	auto item_ptr = platform_runtime_manager::instance().get_cache().get_item(id);
	if (!item_ptr)
		return "Not a running instance!";
	return item_ptr;
}

void relation_runtime::relation_connected ()
{
}

void relation_runtime::relation_disconnected ()
{
}

rx_item_reference relation_runtime::get_implicit_reference (const meta_data& info)
{
	return rx_item_reference();
}

relation_runtime::smart_ptr relation_runtime::make_target_relation ()
{
	return rx_create_reference< relation_runtime>();
}


// Class rx_platform::runtime::relations::extern_relation_runtime

extern_relation_runtime::extern_relation_runtime (plugin_relation_runtime_struct* impl)
      : impl_(impl)
{
	impl_->host = this;
	impl_->host_def = &_g_relation_def_;
}


extern_relation_runtime::~extern_relation_runtime()
{
}



rx_result extern_relation_runtime::initialize_relation (runtime::runtime_init_context& ctx)
{
    rx_result ret = impl_->def->init_relation(impl_->anchor.target, &ctx);
    if (ret)
    {
        from_ = ctx.meta.id;
    }
    return ret;
}

rx_result extern_relation_runtime::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
    return impl_->def->deinit_relation(impl_->anchor.target);
}

rx_result extern_relation_runtime::start_relation (runtime::runtime_start_context& ctx, bool is_target)
{
    return impl_->def->start_relation(impl_->anchor.target, &ctx, is_target ? 1 : 0);
}

rx_result extern_relation_runtime::stop_relation (runtime::runtime_stop_context& ctx, bool is_target)
{
    return impl_->def->stop_relation(impl_->anchor.target, is_target ? 1 : 0);
}

rx_item_reference extern_relation_runtime::get_implicit_reference (const meta_data& info)
{
    return rx_item_reference();
}

relation_runtime::smart_ptr extern_relation_runtime::make_target_relation ()
{
    plugin_relation_runtime_struct* target_rel = nullptr;
    rx_result impl_result = impl_->def->make_target_relation(impl_->anchor.target, &target_rel);
    if (impl_result)
    {
        auto ret = rx_create_reference<extern_relation_runtime>(target_rel);
        ret->from_ = to_;
        ret->to_ = from_;
        return ret;
    }
    else
    {
        return rx_create_reference<relation_runtime>();
    }
}

rx_result_with<platform_item_ptr> extern_relation_runtime::resolve_runtime_sync (const rx_node_id& id)
{
    auto item_ptr = platform_runtime_manager::instance().get_cache().get_item(id);
    if (!item_ptr)
        return "Not a running instance!";
    to_ = id;
    return item_ptr;
}

void extern_relation_runtime::relation_connected ()
{
    if (from_ && to_)
    {
        rx_result result = impl_->def->relation_connected(impl_->anchor.target, from_.c_ptr(), to_.c_ptr());
        if (!result)
        {
            RUNTIME_LOG_TRACE("extern_relation_runtime", 100, "Extern relation connect error:"s + result.errors_line());
        }
    }
    else
    {
        RX_ASSERT(false);
        RUNTIME_LOG_ERROR("extern_relation_runtime", 900, "State machine error: connected called unexpectedly!");
    }
}

void extern_relation_runtime::relation_disconnected ()
{
    if (from_ && to_)
    {
        rx_result result = impl_->def->relation_disconnected(impl_->anchor.target, from_.c_ptr(), to_.c_ptr());
        if (!result)
        {
            RUNTIME_LOG_TRACE("extern_relation_runtime", 100, "Extern relation disconnect error:"s + result.errors_line());
        }
    }
    else
    {
        RX_ASSERT(false);
        RUNTIME_LOG_ERROR("extern_relation_runtime", 900, "State machine error: disconnected called unexpectedly!");
    }
}

void extern_relation_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_relation_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_relation_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}


const rx_node_id& extern_relation_runtime::get_from () const
{
  return from_;
}

const rx_node_id& extern_relation_runtime::get_to () const
{
  return to_;
}


} // namespace relations
} // namespace runtime
} // namespace rx_platform

