

/****************************************************************************
*
*  system\runtime\rx_blocks_templates.h
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


#ifndef rx_blocks_templates_h
#define rx_blocks_templates_h 1



// rx_resolvers
#include "system/runtime/rx_resolvers.h"
// rx_blocks
#include "system/runtime/rx_blocks.h"

#include "system/meta/rx_construction_templates.h"


namespace rx_platform {

namespace runtime {

namespace blocks {

namespace blocks_templates {






template <class portT>
class extern_mapper_impl : public mapper_runtime  
{
    DECLARE_REFERENCE_PTR(extern_mapper_impl);

    class mapper_resolver_user : public relation_subscriber
    {
    public:
        extern_mapper_impl<portT>* my_mapper;
        void relation_connected(const string_type& name, const platform_item_ptr& item)
        {
            my_mapper->internal_port_connected(item);
        }
        void relation_disconnected(const string_type& name)
        {
            my_mapper->internal_port_disconnected();
        }
    };
    mapper_resolver_user resolver_user_;
    friend class extern_mapper_impl::mapper_resolver_user;
public:
    typedef typename portT::smart_ptr port_ptr_t;
protected:
    port_ptr_t my_port_;

  public:
      extern_mapper_impl();


      rx_result start_mapper (runtime::runtime_start_context& ctx);

      rx_result stop_mapper (runtime::runtime_stop_context& ctx);

      virtual void port_connected (port_ptr_t port) = 0;

      virtual void port_disconnected (port_ptr_t port) = 0;


  protected:

  private:

      bool internal_port_connected (const platform_item_ptr& item);

      void internal_port_disconnected ();



      rx_thread_handle_t my_executer_;

      rx_thread_handle_t his_executer_;


};







template <class portT>
class extern_source_impl : public source_runtime  
{
    DECLARE_REFERENCE_PTR(extern_source_impl);

    class source_resolver_user : public relation_subscriber
    {
    public:
        extern_source_impl<portT>::smart_ptr my_source;
        void relation_connected(const string_type& name, const platform_item_ptr& item)
        {
            my_source->internal_port_connected(item);
        }
        void relation_disconnected(const string_type& name)
        {
            my_source->internal_port_disconnected();
        }
    };
    source_resolver_user resolver_user_;
    friend class extern_source_impl::source_resolver_user;
public:
    typedef typename portT::smart_ptr port_ptr_t;
protected:
    port_ptr_t my_port_;

  public:
      extern_source_impl();


      rx_result start_source (runtime::runtime_start_context& ctx);

      virtual rx_result stop_source (runtime::runtime_stop_context& ctx);

      virtual void port_connected (port_ptr_t port) = 0;

      virtual void port_disconnected (port_ptr_t port) = 0;


  protected:

  private:

      bool internal_port_connected (const platform_item_ptr& item);

      void internal_port_disconnected ();



      rx_thread_handle_t my_executer_;

      rx_thread_handle_t his_executer_;


};


// Parameterized Class rx_platform::runtime::blocks::blocks_templates::extern_mapper_impl 

template <class portT>
extern_mapper_impl<portT>::extern_mapper_impl()
      : my_executer_(0),
        his_executer_(0)
{
    resolver_user_.my_mapper = this;
}



template <class portT>
rx_result extern_mapper_impl<portT>::start_mapper (runtime::runtime_start_context& ctx)
{
    auto ret = mapper_runtime::start_mapper(ctx);
    if (!ret)
        return ret;

    my_executer_ = rx_thread_context();
    auto port_reference = ctx.structure.get_current_item().get_local_as<string_type>("Port", "");
    ret = ctx.register_relation_subscriber(port_reference, &resolver_user_);
    if (!ret)
    {
        RUNTIME_LOG_WARNING("extern_mapper_impl", 900, "Error starting mapper "
            + ctx.context->meta_info.get_full_path() + "." + ctx.path.get_current_path() + " " + ret.errors_line());
    }
    return true;
}

template <class portT>
rx_result extern_mapper_impl<portT>::stop_mapper (runtime::runtime_stop_context& ctx)
{
    auto ret = mapper_runtime::stop_mapper(ctx);
    return ret;
}

template <class portT>
bool extern_mapper_impl<portT>::internal_port_connected (const platform_item_ptr& item)
{
    if (!my_port_)
    {
        auto result = rx_platform::get_runtime_instance<portT>(item->meta_info().id);
        if (result)
        {
            his_executer_ = item->get_executer();
            my_port_ = result.value();
            this->port_connected(my_port_);
            map_current_value();
            return true;
        }
    }
    return false;
}

template <class portT>
void extern_mapper_impl<portT>::internal_port_disconnected ()
{
    if (my_port_)
        this->port_disconnected(my_port_);
}


// Parameterized Class rx_platform::runtime::blocks::blocks_templates::extern_source_impl 

template <class portT>
extern_source_impl<portT>::extern_source_impl()
      : my_executer_(0),
        his_executer_(0)
{
    resolver_user_.my_source = smart_this();
}



template <class portT>
rx_result extern_source_impl<portT>::start_source (runtime::runtime_start_context& ctx)
{
    auto ret = source_runtime::start_source(ctx);
    if (!ret)
        return ret;

    my_executer_ = rx_thread_context();
    auto port_reference = ctx.structure.get_current_item().get_local_as<string_type>("Port", "");
    ret = ctx.register_relation_subscriber(port_reference, &resolver_user_);
    if (!ret)
    {
        RUNTIME_LOG_WARNING("extern_source_impl", 900, "Error starting source "
            + ctx.context->meta_info.get_full_path() + "." + ctx.path.get_current_path() + " " + ret.errors_line());
    }
    return true;
}

template <class portT>
rx_result extern_source_impl<portT>::stop_source (runtime::runtime_stop_context& ctx)
{
    auto ret = source_runtime::stop_source(ctx);
    return ret;
}

template <class portT>
bool extern_source_impl<portT>::internal_port_connected (const platform_item_ptr& item)
{
    if (!my_port_)
    {
        auto result = rx_platform::get_runtime_instance<portT>(item->meta_info().id);
        if (result)
        {
            his_executer_ = item->get_executer();
            my_port_ = result.value();
            this->port_connected(my_port_);
            return true;
        }
    }
    return false;
}

template <class portT>
void extern_source_impl<portT>::internal_port_disconnected ()
{
    if (my_port_)
    {
        rx_value val;
        val.set_quality(RX_BAD_QUALITY_NOT_CONNECTED);
        val.set_time(rx_time::now());
        source_value_changed(std::move(val));
        this->port_disconnected(my_port_);
        my_port_ = portT::smart_ptr::null_ptr;
    }
}


} // namespace blocks_templates
} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
