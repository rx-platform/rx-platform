

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



// rx_blocks
#include "system/runtime/rx_blocks.h"



namespace rx_platform {

namespace runtime {

namespace blocks {

namespace blocks_templates {





class resolver_user 
{

  public:

      virtual void port_connected (rx_port_impl_ptr port) = 0;

      virtual void port_disconnected (rx_port_impl_ptr port) = 0;

      virtual rx_reference_ptr get_reference () = 0;


  protected:

  private:


};






class item_port_resolver 
{
    enum class resolver_state
    {
        idle = 0,
        querying = 1,
        same_thread = 2,
        other_thread = 3,
        stopping = 4
    };
    resolver_state my_state_ = resolver_state::idle;

  public:

      rx_result init (runtime::runtime_start_context& ctx, resolver_user* user);

      void deinit ();


  protected:

      rx_port_impl_ptr try_get_port ();


      rx_port_impl_ptr my_port_;


  private:

      void resolve_port ();

      virtual rx_result_with<rx_port_impl_ptr> resolve_port_sync (const rx_node_id& id);



      resolver_user *user_;


      string_type port_path_;

      rx_directory_resolver directories_;


};






template <class portT>
class extern_source_impl : public source_runtime  
{
    DECLARE_REFERENCE_PTR(extern_source_impl);

    class source_resolver_user : public resolver_user
    {
    public:
        extern_source_impl<portT>::smart_ptr my_source;
        void port_connected(rx_port_impl_ptr port)
        {
        }
        void port_disconnected(rx_port_impl_ptr port)
        {
        }
        rx_reference_ptr get_reference()
        {
            return my_source;
        }
    };
    source_resolver_user resolver_user_;
    friend class source_resolver_user;
    typedef typename portT::smart_ptr port_ptr_t;
    port_ptr_t my_port_;

  public:
      extern_source_impl();


      rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


  protected:

  private:


      item_port_resolver resolver_;


};






template <class portT>
class extern_mapper_impl : public mapper_runtime  
{
    DECLARE_REFERENCE_PTR(extern_mapper_impl);

    class mapper_resolver_user : public resolver_user
    {
    public:
        extern_mapper_impl<portT>::smart_ptr my_mapper;
        void port_connected(rx_port_impl_ptr port)
        {
            my_mapper->internal_port_connected(port);
        }
        void port_disconnected(rx_port_impl_ptr port)
        {
            my_mapper->internal_port_disconnected(port);
        }
        rx_reference_ptr get_reference()
        {
            return my_mapper;
        }
    };
    mapper_resolver_user resolver_user_;
    friend class mapper_resolver_user;
    typedef typename portT::smart_ptr port_ptr_t;
    port_ptr_t my_port_;

  public:
      extern_mapper_impl();


      rx_result start_mapper (runtime::runtime_start_context& ctx);

      virtual void port_connected (port_ptr_t port) = 0;

      virtual void port_disconnected (port_ptr_t port) = 0;

      rx_result stop_mapper (runtime::runtime_stop_context& ctx);


  protected:

  private:

      void internal_port_connected (rx_port_impl_ptr port);

      void internal_port_disconnected (rx_port_impl_ptr port);



      item_port_resolver resolver_;


};


// Parameterized Class rx_platform::runtime::blocks::blocks_templates::extern_source_impl 

template <class portT>
extern_source_impl<portT>::extern_source_impl()
{
    resolver_user_.my_source = smart_this();
}



template <class portT>
rx_result extern_source_impl<portT>::start_runtime (runtime::runtime_start_context& ctx)
{
    auto ret = source_runtime::start_runtime(ctx);
    if (!ret)
        return ret;
    ret = this->resolver_.init(ctx, &resolver_user_);
    return ret;
}

template <class portT>
rx_result extern_source_impl<portT>::stop_runtime (runtime::runtime_stop_context& ctx)
{
    this->resolver_.deinit();
    auto ret = source_runtime::stop_runtime(ctx);
    return ret;
}


// Parameterized Class rx_platform::runtime::blocks::blocks_templates::extern_mapper_impl 

template <class portT>
extern_mapper_impl<portT>::extern_mapper_impl()
{
    resolver_user_.my_mapper = smart_this();
}



template <class portT>
rx_result extern_mapper_impl<portT>::start_mapper (runtime::runtime_start_context& ctx)
{
    auto ret = mapper_runtime::start_mapper(ctx);
    if (!ret)
        return ret;
    ret = this->resolver_.init(ctx, &resolver_user_);
    return ret;
}

template <class portT>
void extern_mapper_impl<portT>::internal_port_connected (rx_port_impl_ptr port)
{
    auto my_port = port.cast_to<port_ptr_t>();
    if (my_port)
    {
        map_current_value();
        this->port_connected(my_port);
    }
}

template <class portT>
void extern_mapper_impl<portT>::internal_port_disconnected (rx_port_impl_ptr port)
{
    auto my_port = port.cast_to<port_ptr_t>();
    if (my_port)
        this->port_disconnected(my_port);
}

template <class portT>
rx_result extern_mapper_impl<portT>::stop_mapper (runtime::runtime_stop_context& ctx)
{
    this->resolver_.deinit();
    auto ret = mapper_runtime::stop_mapper(ctx);
    return ret;
}


} // namespace blocks_templates
} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
