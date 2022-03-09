

/****************************************************************************
*
*  platform_api\rx_var_types.h
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


#ifndef rx_var_types_h
#define rx_var_types_h 1



// rx_runtime
#include "platform_api/rx_runtime.h"



namespace rx_platform_api {





class rx_filter : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_filter);

  public:
      rx_filter (bool input, bool output);

      ~rx_filter();


      virtual rx_result initialize_filter (rx_init_context& ctx);

      virtual rx_result start_filter (rx_start_context& ctx);

      virtual rx_result stop_filter ();

      virtual rx_result deinitialize_filter ();

      virtual rx_result filter_input (rx_value& val);

      virtual rx_result filter_output (rx_simple_value& val);


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


      plugin_filter_runtime_struct impl_;

      template<class T>
      friend rx_result register_filter_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_filter)(rx_platform_api::rx_filter* self, init_ctx_ptr ctx);
      friend rx_result_struct(::c_start_filter)(rx_platform_api::rx_filter* self, start_ctx_ptr ctx);
};

rx_result register_filter_runtime(const rx_node_id& id, rx_filter_constructor_t construct_func);
template<class T>
rx_result register_filter_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_filter_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_filter_runtime(id, constr_lambda);
}





class rx_mapper : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_mapper);

  public:
      rx_mapper (bool read, bool write);

      ~rx_mapper();


      virtual rx_result initialize_mapper (rx_init_context& ctx);

      virtual rx_result start_mapper (rx_start_context& ctx);

      virtual rx_result stop_mapper ();

      virtual rx_result deinitialize_mapper ();

      virtual void mapped_value_changed (rx_value&& val, rx_process_context& ctx);

      virtual void mapper_result_received (rx_result&& result, runtime_transaction_id_t id, rx_process_context& ctx);


      rx_value_t get_value_type () const;


      static rx_item_type type_id;


  protected:

      void mapper_write_pending (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val);

      void map_current_value () const;


  private:


      plugin_mapper_runtime_struct impl_;

      rx_value_t value_type_;

      template<class T>
      friend rx_result register_mapper_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_mapper)(rx_platform_api::rx_mapper* self, init_ctx_ptr ctx, uint8_t value_type);
      friend rx_result_struct(::c_start_mapper)(rx_platform_api::rx_mapper* self, start_ctx_ptr ctx);
};

rx_result register_mapper_runtime(const rx_node_id& id, rx_mapper_constructor_t construct_func);
template<class T>
rx_result register_mapper_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_mapper_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_mapper_runtime(id, constr_lambda);
}





class rx_source : public rx_runtime  
{
    DECLARE_REFERENCE_PTR(rx_source);

  public:
      rx_source (bool input, bool output);

      virtual ~rx_source();


      virtual rx_result initialize_source (rx_init_context& ctx);

      virtual rx_result start_source (rx_start_context& ctx);

      virtual rx_result stop_source ();

      virtual rx_result deinitialize_source ();

      virtual rx_result source_write (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx);


      rx_value_t get_value_type () const;

      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

      rx_result source_value_changed (rx_value&& val);

      void source_result_received (rx_result&& result, runtime_transaction_id_t id);


  private:


      plugin_source_runtime_struct impl_;

      rx_value_t value_type_;

      template<class T>
      friend rx_result register_source_runtime(const rx_node_id& id);
      friend rx_result_struct(::c_init_source)(rx_platform_api::rx_source* self, init_ctx_ptr ctx, uint8_t value_type);
      friend rx_result_struct(::c_start_source)(rx_platform_api::rx_source* self, start_ctx_ptr ctx);
};

rx_result register_source_runtime(const rx_node_id& id, rx_source_constructor_t construct_func);
template<class T>
rx_result register_source_runtime(const rx_node_id& id)
{
    auto constr_lambda = []() -> plugin_source_runtime_struct_t*
    {
        T* temp = new T;
        return &temp->impl_;
    };
    return register_source_runtime(id, constr_lambda);
}





template <class portT>
class rx_extern_mapper : public rx_mapper  
{
    DECLARE_REFERENCE_PTR(rx_extern_mapper);

    class mapper_relation_subscriber : public rx_relation_subscriber
    {
    public:
        relation_subscriber_data subscriber_data_;
        rx_extern_mapper<portT>* my_mapper;
        mapper_relation_subscriber()
        {
            subscriber_data_.connected_callback = [](void* whose, const char* name, const rx_node_id_struct* id)
                {
                    mapper_relation_subscriber* self = (mapper_relation_subscriber*)whose;
                    self->relation_connected(name, id);
                };
            subscriber_data_.disconnected_callback = [](void* whose, const char* name)
                {
                    mapper_relation_subscriber* self = (mapper_relation_subscriber*)whose;
                    self->relation_disconnected(name);
                };
            subscriber_data_.target = this;
        }
        void relation_connected(const char* name, const rx_node_id_struct* id)
        {
            my_mapper->internal_port_connected(id);
        }
        void relation_disconnected(const char* name)
        {
            my_mapper->internal_port_disconnected();
        }
    };
    mapper_relation_subscriber subscriber_;
    friend class rx_extern_mapper::mapper_relation_subscriber;
    typedef typename portT::smart_ptr port_ptr_t;
protected:
    port_ptr_t my_port_;

  public:
      rx_extern_mapper (bool input, bool output);


      virtual void port_connected (port_ptr_t port) = 0;

      virtual void port_disconnected (port_ptr_t port) = 0;


  protected:

      rx_result subscribe_relation (rx_start_context& ctx, const string_type& name);


  private:

      void internal_port_connected (const rx_node_id& id);

      void internal_port_disconnected ();



};






template <class portT>
class rx_extern_source : public rx_source  
{

    DECLARE_REFERENCE_PTR(rx_extern_source);

    class source_relation_subscriber : public rx_relation_subscriber
    {
    public:
        relation_subscriber_data subscriber_data_;
        rx_extern_source<portT>* my_source;
        source_relation_subscriber()
        {
            subscriber_data_.connected_callback = [](void* whose, const char* name, const rx_node_id_struct* id)
            {
                source_relation_subscriber* self = (source_relation_subscriber*)whose;
                self->relation_connected(name, id);
            };
            subscriber_data_.disconnected_callback = [](void* whose, const char* name)
            {
                source_relation_subscriber* self = (source_relation_subscriber*)whose;
                self->relation_disconnected(name);
            };
            subscriber_data_.target = this;
        }
        void relation_connected(const char* name, const rx_node_id_struct* id)
        {
            my_source->internal_port_connected(id);
        }
        void relation_disconnected(const char* name)
        {
            my_source->internal_port_disconnected();
        }
    };
    source_relation_subscriber subscriber_;
    friend class rx_extern_source::source_relation_subscriber;
    typedef typename portT::smart_ptr port_ptr_t;
protected:
    port_ptr_t my_port_;

  public:
      rx_extern_source (bool input, bool output);


      virtual void port_connected (port_ptr_t port) = 0;

      virtual void port_disconnected (port_ptr_t port) = 0;


  protected:

      rx_result subscribe_relation (rx_start_context& ctx, const string_type& name);


  private:

      void internal_port_connected (const rx_node_id& id);

      void internal_port_disconnected ();



};


// Parameterized Class rx_platform_api::rx_extern_mapper 

template <class portT>
rx_extern_mapper<portT>::rx_extern_mapper (bool input, bool output)
    : rx_mapper(input, output)
{
    subscriber_.my_mapper = this;
}



template <class portT>
rx_result rx_extern_mapper<portT>::subscribe_relation (rx_start_context& ctx, const string_type& name)
{
    if (name.empty())
        return RX_INVALID_ARGUMENT;
    auto ret = ctx.register_relation_subscriber(name, &subscriber_.subscriber_data_);
    return ret;
}

template <class portT>
void rx_extern_mapper<portT>::internal_port_connected (const rx_node_id& id)
{
    if (!this->my_port_)
    {
        auto result = rx_platform_api::get_runtime_instance<portT>(id);
        if (result)
        {
            my_port_ = result.value();
            this->port_connected(my_port_);
            this->map_current_value();
        }
    }
}

template <class portT>
void rx_extern_mapper<portT>::internal_port_disconnected ()
{
    if (this->my_port_)
        this->port_disconnected(my_port_);
}


// Parameterized Class rx_platform_api::rx_extern_source 

template <class portT>
rx_extern_source<portT>::rx_extern_source (bool input, bool output)
    : rx_source(input, output)
{
    subscriber_.my_source = this;
}



template <class portT>
rx_result rx_extern_source<portT>::subscribe_relation (rx_start_context& ctx, const string_type& name)
{
    if (name.empty())
        return RX_INVALID_ARGUMENT;
    auto ret = ctx.register_relation_subscriber(name, &subscriber_.subscriber_data_);
    return ret;
}

template <class portT>
void rx_extern_source<portT>::internal_port_connected (const rx_node_id& id)
{
    if (!this->my_port_)
    {
        auto result = rx_platform_api::get_runtime_instance<portT>(id);
        if (result)
        {
            my_port_ = result.value();
            this->port_connected(my_port_);
        }
    }
}

template <class portT>
void rx_extern_source<portT>::internal_port_disconnected ()
{
    if (this->my_port_)
    {
        rx_value val;
        val.set_quality(RX_BAD_QUALITY_NOT_CONNECTED);
        val.set_time(rx_time::now());
        this->source_value_changed(std::move(val));
        this->port_disconnected(my_port_);
        this->my_port_ = portT::smart_ptr::null_ptr;
    }
}


} // namespace rx_platform_api



#endif
