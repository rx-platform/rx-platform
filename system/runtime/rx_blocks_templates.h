

/****************************************************************************
*
*  system\runtime\rx_blocks_templates.h
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


#ifndef rx_blocks_templates_h
#define rx_blocks_templates_h 1



// rx_resolvers
#include "system/runtime/rx_resolvers.h"
// rx_blocks
#include "system/runtime/rx_blocks.h"



namespace rx_platform {

namespace runtime {

namespace blocks {

namespace blocks_templates {





template <class portT>
class extern_mapper_impl : public mapper_runtime  
{
    DECLARE_REFERENCE_PTR(extern_mapper_impl);

    class mapper_resolver_user : public resolvers::resolver_user<meta::object_types::port_type>
    {
    public:
        extern_mapper_impl<portT>::smart_ptr my_mapper;
        bool runtime_connected(platform_item_ptr&& item, rx_port_impl_ptr port)
        {
            return my_mapper->internal_port_connected(port, item->meta_info().id);
        }
        void runtime_disconnected()
        {
            my_mapper->internal_port_disconnected();
        }
        rx_reference_ptr get_reference()
        {
            return my_mapper;
        }
    };
    mapper_resolver_user resolver_user_;
    friend class extern_mapper_impl::mapper_resolver_user;
    typedef typename portT::smart_ptr port_ptr_t;
protected:
    port_ptr_t my_port_;

  public:
      extern_mapper_impl();


      rx_result start_mapper (runtime::runtime_start_context& ctx);

      virtual void port_connected (port_ptr_t port) = 0;

      virtual void port_disconnected (port_ptr_t port) = 0;

      rx_result stop_mapper (runtime::runtime_stop_context& ctx);


  protected:

  private:

      bool internal_port_connected (rx_port_impl_ptr port, rx_node_id id);

      void internal_port_disconnected ();



      resolvers::runtime_resolver<meta::object_types::port_type> resolver_;


      rx_thread_handle_t my_executer_;

      rx_thread_handle_t his_executer_;


};






template <class portT>
class extern_source_impl : public source_runtime  
{
    DECLARE_REFERENCE_PTR(extern_source_impl);

    class source_resolver_user : public resolvers::resolver_user<meta::object_types::port_type>
    {
    public:
        extern_source_impl<portT>::smart_ptr my_source;
        bool runtime_connected(platform_item_ptr&& item, rx_port_impl_ptr port)
        {
            return true;
        }
        void runtime_connected()
        {
        }
        rx_reference_ptr get_reference()
        {
            return my_source;
        }
    };
    source_resolver_user resolver_user_;
    friend class extern_source_impl::source_resolver_user;
    typedef typename portT::smart_ptr port_ptr_t;
    port_ptr_t my_port_;

  public:
      extern_source_impl();


      rx_result start_source (runtime::runtime_start_context& ctx);

      virtual rx_result stop_source (runtime::runtime_stop_context& ctx);


  protected:

  private:


      resolvers::runtime_resolver<meta::object_types::port_type> resolver_;


      rx_thread_handle_t my_executer_;

      rx_thread_handle_t his_executer_;


};


// Parameterized Class rx_platform::runtime::blocks::blocks_templates::extern_mapper_impl 

template <class portT>
extern_mapper_impl<portT>::extern_mapper_impl()
      : my_executer_(0),
        his_executer_(0)
{
    resolver_user_.my_mapper = smart_this();
}



template <class portT>
rx_result extern_mapper_impl<portT>::start_mapper (runtime::runtime_start_context& ctx)
{
    auto ret = mapper_runtime::start_mapper(ctx);
    if (!ret)
        return ret;

    my_executer_ = rx_thread_context();
    auto port_reference = rx_item_reference(ctx.structure.get_current_item().get_local_as<string_type>("Port", ""));
    ret = this->resolver_.start_resolver(port_reference, &resolver_user_, ctx.directories);
    return ret;
}

template <class portT>
bool extern_mapper_impl<portT>::internal_port_connected (rx_port_impl_ptr port, rx_node_id id)
{
    auto result = rx_platform::get_runtime_instance<portT>(id);
    if (result)
    {
        RUNTIME_LOG_DEBUG("extern_mapper_impl", 100, "Resolved port reference");
        his_executer_ = port->get_executer();
        my_port_ = result.value();
        this->port_connected(my_port_);
        map_current_value();
        return true;
    }
    return false;
}

template <class portT>
void extern_mapper_impl<portT>::internal_port_disconnected ()
{
    if (my_port_)
        this->port_disconnected(my_port_);
}

template <class portT>
rx_result extern_mapper_impl<portT>::stop_mapper (runtime::runtime_stop_context& ctx)
{
    this->resolver_.stop_resolver();
    auto ret = mapper_runtime::stop_mapper(ctx);
    return ret;
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
    auto ret = this->resolver_.start_resolver(ctx, &resolver_user_, ctx.directories);
    return ret;
}

template <class portT>
rx_result extern_source_impl<portT>::stop_source (runtime::runtime_stop_context& ctx)
{
    this->resolver_.stop_resolver();
    return true;
}


} // namespace blocks_templates
} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif