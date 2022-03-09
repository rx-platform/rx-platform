

/****************************************************************************
*
*  platform_api\rx_ports.h
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


#ifndef rx_ports_h
#define rx_ports_h 1



// rx_runtime
#include "platform_api/rx_runtime.h"

#include "lib/rx_rt_data.h"
#include "lib/rx_io_addr.h"

#include "lib/rx_io_buffers.h"
using namespace rx;


namespace rx_platform_api {





class rx_port : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_port);

  public:
      rx_port();

      ~rx_port();


      virtual rx_result initialize_port (rx_init_context& ctx);

      virtual rx_result start_port (rx_start_context& ctx);

      virtual rx_result stop_port ();

      virtual rx_result deinitialize_port ();

      rx_result_with<io::rx_io_buffer> alloc_io_buffer ();

      void release_io_buffer (io::rx_io_buffer buff);

      virtual void stack_assembled ();

      virtual void stack_disassembled ();

      virtual void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);

      virtual void extern_extract_bind_address (const uint8_t* binder_data, size_t binder_data_size, protocol_address* local_addr, protocol_address* remote_addr);

      virtual rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      virtual rx_protocol_stack_endpoint* construct_initiator_endpoint ();

      virtual void destroy_endpoint (rx_protocol_stack_endpoint* what) = 0;

      rx_result unbind_stack_endpoint (rx_protocol_stack_endpoint* what);

      rx_result disconnect_stack_endpoint (rx_protocol_stack_endpoint* what);


      static const rx_item_type type_id;

      static rx_item_type runtime_type_id;

      template<typename funcT>
      runtime_handle_t create_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_REGULAR, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_calc_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_CALC, data, period);
      }
      template<typename funcT>
      runtime_handle_t create_io_timer(funcT func, uint32_t period)
      {
          callback_data::smart_ptr data = rx_create_reference<callback_data>(smart_this(), std::forward<funcT>(func));
          return create_timer_internal(RX_TIMER_IO, data, period);
      }
  protected:

      rx_result listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result connect (const protocol_address* local_address, const protocol_address* remote_address);


  private:


      plugin_port_runtime_struct impl_;

      template<class T>
      friend rx_result register_port_runtime(const rx_node_id& id);
      template<class T>
      friend rx_result register_monitored_port_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_port)(rx_platform_api::rx_port* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_port)(rx_platform_api::rx_port* self, start_ctx_ptr ctx);
};

rx_result register_port_runtime(const rx_node_id& id, rx_port_constructor_t construct_func, rx_runtime_register_func_t reg_function, rx_runtime_unregister_func_t unreg_function);
template<class T>
rx_result register_port_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_port_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_port_runtime(id, constr_lambda, rx_runtime_register_func_t(), rx_runtime_unregister_func_t());
}
template<class T>
rx_result register_monitored_port_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_port_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    rx_runtime_register_func_t reg_func = [](const rx_node_id_struct* id, lock_reference_struct* what)
    {
        using ptr_t = typename T::smart_ptr;
        rx_node_id reg_id(id);
        ptr_t ptr = ptr_t::create_from_pointer(reinterpret_cast<T*>(what->target));
        rx_runtime_manager_lock _;
        T::runtime_instances.emplace(reg_id, ptr);
    };
    rx_runtime_unregister_func_t unreg_func = [](const rx_node_id_struct* id)
    {
        rx_runtime_manager_lock _;
        T::runtime_instances.erase(rx_node_id(id));
    };
    return register_port_runtime(id, constr_lambda, reg_func, unreg_func);
}





template <typename endpointT>
class rx_transport_port : public rx_port  
{

    DECLARE_REFERENCE_PTR(rx_transport_port);

    typedef std::map<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> > active_endpoints_type;
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, std::unique_ptr<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_initiator_endpoint ();

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);


  protected:

  private:

      rx_protocol_stack_endpoint* construct_endpoint_internal ();

      virtual std::unique_ptr<endpointT> construct_endpoint () = 0;



      active_endpoints_type active_endpoints_;


};






template <typename endpointT>
class rx_client_master_port : public rx_port  
{
    DECLARE_REFERENCE_PTR(rx_client_master_port);
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, rx_reference<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:
      rx_client_master_port();


      rx_protocol_stack_endpoint* construct_initiator_endpoint ();

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      void stack_assembled ();

      void status_set_online ();

      void status_set_offline ();

      void status_write_done (uint64_t time, bool success);

      void status_read_done (uint64_t time, bool success);

      void status_received_packet ();

      void status_sent_packet ();


  protected:

      rx_reference<endpointT> active_endpoint ();

      virtual rx_result initialize_status (rx_init_context& ctx);


  private:

      virtual rx_reference<endpointT> construct_endpoint () = 0;



      rx_reference<endpointT> active_endpoint_;

      remote_owned_value<bool> online_;

      remote_owned_value<float> read_time_;

      remote_owned_value<float> write_time_;

      remote_owned_value<float> max_read_time_;

      remote_owned_value<float> max_write_time_;

      remote_owned_value<int64_t> read_count_;

      remote_owned_value<int64_t> write_count_;

      remote_owned_value<int64_t> failed_read_count_;

      remote_owned_value<int64_t> failed_write_count_;

      remote_owned_value<int32_t> queue_size_;

      remote_owned_value<int64_t> received_;

      remote_owned_value<int64_t> sent_;


};






template <typename endpointT>
class rx_server_slave_port : public rx_port  
{
    DECLARE_REFERENCE_PTR(rx_server_slave_port);
    typedef std::map<rx_protocol_stack_endpoint*, rx_reference<endpointT> > active_endpoints_type;
public:
    typedef std::function<std::pair<rx_protocol_stack_endpoint*, rx_reference<endpointT> >()> construct_func_type;
    construct_func_type construct_func;

  public:

      rx_protocol_stack_endpoint* construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      void stack_assembled ();


  protected:

      rx_reference<endpointT> get_endpoint (rx_protocol_stack_endpoint* stack);


  private:

      virtual rx_reference<endpointT> construct_endpoint () = 0;



      active_endpoints_type active_endpoints_;


};


// Parameterized Class rx_platform_api::rx_transport_port 


template <typename endpointT>
rx_protocol_stack_endpoint* rx_transport_port<endpointT>::construct_initiator_endpoint ()
{
    return construct_endpoint_internal();
}

template <typename endpointT>
rx_protocol_stack_endpoint* rx_transport_port<endpointT>::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    return construct_endpoint_internal();
}

template <typename endpointT>
void rx_transport_port<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        active_endpoints_.erase(it);
    }
}

template <typename endpointT>
rx_protocol_stack_endpoint* rx_transport_port<endpointT>::construct_endpoint_internal ()
{
    auto ep = construct_endpoint();
    if (!ep)
        return nullptr;

    rx_protocol_stack_endpoint* stack = ep->get_endpoint();
    if (!stack)
        return nullptr;

    if (stack->closed_function == nullptr)
    {
        stack->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->unbind_stack_endpoint(entry);
        };
    }
    if (stack->allocate_packet == nullptr)
    {
        stack->allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            auto result = whose->get_port()->alloc_io_buffer();
            if (result)
            {
                result.value().detach(buffer);
                return RX_PROTOCOL_OK;
            }
            else
            {
                return RX_PROTOCOL_OUT_OF_MEMORY;
            }
        };
    }
    if (stack->release_packet == nullptr)
    {
        stack->release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            io::rx_io_buffer temp;
            temp.attach(buffer);
            whose->get_port()->release_io_buffer(std::move(temp));

            return RX_PROTOCOL_OK;
        };
    }
    active_endpoints_.emplace(stack, std::move(ep));
    return stack;
}


// Parameterized Class rx_platform_api::rx_client_master_port 

template <typename endpointT>
rx_client_master_port<endpointT>::rx_client_master_port()
      : online_(false),
        read_time_(0),
        write_time_(0),
        max_read_time_(0),
        max_write_time_(0),
        read_count_(0),
        write_count_(0),
        failed_read_count_(0),
        failed_write_count_(0),
        queue_size_(0),
        received_(0),
        sent_(0)
{
}



template <typename endpointT>
rx_protocol_stack_endpoint* rx_client_master_port<endpointT>::construct_initiator_endpoint ()
{
    auto ep = this->construct_endpoint();
    auto stack = ep->get_endpoint();

    if (stack->closed_function == nullptr)
    {
        stack->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->close_endpoint();
            whose->get_port()->disconnect_stack_endpoint(entry);
        };
    }

    if (stack->allocate_packet == nullptr)
    {
        stack->allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            auto result = whose->get_port()->alloc_io_buffer();
            if (result)
            {
                result.value().detach(buffer);
                return RX_PROTOCOL_OK;
            }
            else
            {
                return RX_PROTOCOL_OUT_OF_MEMORY;
            }
        };
    }
    if (stack->release_packet == nullptr)
    {
        stack->release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            io::rx_io_buffer temp;
            temp.attach(buffer);
            whose->get_port()->release_io_buffer(std::move(temp));

            return RX_PROTOCOL_OK;
        };
    }
    active_endpoint_ = std::move(ep);
    return stack;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    if (active_endpoint_)
    {
        active_endpoint_ = rx_reference<endpointT>();
    }
}

template <typename endpointT>
rx_reference<endpointT> rx_client_master_port<endpointT>::active_endpoint ()
{
    return active_endpoint_;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::stack_assembled ()
{
    this->connect(nullptr, nullptr);
}

template <typename endpointT>
rx_result rx_client_master_port<endpointT>::initialize_status (rx_init_context& ctx)
{
    rx_result result;

    auto one_result = received_.bind("Status.RxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.RxPackets:"s + one_result.errors_line());
    one_result = sent_.bind("Status.TxPackets", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.TxPackets:"s + one_result.errors_line());
    one_result = online_.bind("Status.Online", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.Online:"s + one_result.errors_line());
    one_result = read_count_.bind("Status.Reads", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.Reads:"s + one_result.errors_line());
    one_result = write_count_.bind("Status.Writes", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.Writes:"s + one_result.errors_line());
    one_result = failed_read_count_.bind("Status.FailedReads", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.FailedReads:"s + one_result.errors_line());
    one_result = failed_write_count_.bind("Status.FailedWrites", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.FailedWrites:"s + one_result.errors_line());

    one_result = read_time_.bind("Status.LastRead", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.LastRead:"s + one_result.errors_line());
    one_result = max_read_time_.bind("Status.MaxRead", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.MaxRead:"s + one_result.errors_line());
    one_result = write_time_.bind("Status.LastWrite", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.LastWrite:"s + one_result.errors_line());
    one_result = max_write_time_.bind("Status.MaxWrite", ctx);
    if (!one_result)
        result.register_error("Error connecting Status.MaxWrite:"s + one_result.errors_line());


    return result;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::status_set_online ()
{
    online_ = true;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::status_set_offline ()
{
    online_ = false;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::status_write_done (uint64_t time, bool success)
{
    float this_time = (float)time / 1000.0f;
    write_time_ = this_time;
    if (max_write_time_ < this_time)
        max_write_time_ = this_time;
    if (!success)
    {
        failed_write_count_ += 1;
        online_ = false;
    }
    else
    {
        online_ = true;
    }
    write_count_ += 1;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::status_read_done (uint64_t time, bool success)
{
    float this_time = (float)time / 1000.0f;
    read_time_ = this_time;
    if (max_read_time_ < this_time)
        max_read_time_ = this_time;
    if (!success)
    {
        failed_read_count_ += 1;
        online_ = false;
    }
    else
    {
        online_ = true;
    }
    read_count_ += 1;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::status_received_packet ()
{
    received_ += 1;
}

template <typename endpointT>
void rx_client_master_port<endpointT>::status_sent_packet ()
{
    sent_ += 1;
}


// Parameterized Class rx_platform_api::rx_server_slave_port 


template <typename endpointT>
rx_protocol_stack_endpoint* rx_server_slave_port<endpointT>::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    auto ep = this->construct_endpoint();
    auto stack = ep->get_endpoint();

    if (stack->closed_function == nullptr)
    {
        stack->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            whose->get_port()->unbind_stack_endpoint(entry);
        };
    }
    if (stack->allocate_packet == nullptr)
    {
        stack->allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            auto result = whose->get_port()->alloc_io_buffer();
            if (result)
            {
                result.value().detach(buffer);
                return RX_PROTOCOL_OK;
            }
            else
            {
                return RX_PROTOCOL_OUT_OF_MEMORY;
            }
        };
    }
    if (stack->release_packet == nullptr)
    {
        stack->release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
        {
            endpointT* whose = reinterpret_cast<endpointT*>(entry->user_data);
            io::rx_io_buffer temp;
            temp.attach(buffer);
            whose->get_port()->release_io_buffer(std::move(temp));

            return RX_PROTOCOL_OK;
        };
    }
    active_endpoints_.emplace(stack, std::move(ep));
    return stack;
}

template <typename endpointT>
void rx_server_slave_port<endpointT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    auto it = active_endpoints_.find(what);
    if (it != active_endpoints_.end())
    {
        active_endpoints_.erase(it);
    }
}

template <typename endpointT>
rx_reference<endpointT> rx_server_slave_port<endpointT>::get_endpoint (rx_protocol_stack_endpoint* stack)
{
    auto it = active_endpoints_.find(stack);
    if (it != active_endpoints_.end())
        return it->second;
    else
        return rx_reference<endpointT>::null_ptr;
}

template <typename endpointT>
void rx_server_slave_port<endpointT>::stack_assembled ()
{
    this->listen(nullptr, nullptr);
}


} // namespace rx_platform_api



#endif
