

/****************************************************************************
*
*  model\rx_meta_internals.h
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


#ifndef rx_meta_internals_h
#define rx_meta_internals_h 1



// rx_runtime_data
#include "system/meta/rx_runtime_data.h"

namespace rx_internal {
namespace model {
namespace transactions {
class dependency_cache;
} // namespace transactions
} // namespace model

namespace discovery {
class peer_item;

} // namespace discovery
} // namespace rx_internal


#include "system/meta/rx_types.h"
#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_objbase.h"
#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"
#include "system/meta/rx_construction_templates.h"

using namespace rx_platform::meta;
using namespace rx_platform::meta::basic_types;
using namespace rx_platform::meta::object_types;


/////////////////////////////////////////////////////////////
// logging macros for console library
#define META_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Meta",src,lvl,msg)
#define META_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Meta",src,lvl,msg)
#define META_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Meta",src,lvl,msg)
#define MEAT_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Meta",src,lvl,msg)
#define META_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Meta",src,lvl,msg)
#define META_LOG_TRACE(src,lvl,msg) RX_TRACE("Meta",src,lvl,msg)


namespace rx_internal {

namespace model {

template<typename typeT, typename derivedT>
rx_result register_internal_constructor(library::rx_plugin_base* plugin
    , const rx_node_id& id, std::function<typename derivedT::smart_ptr()> f)
{
    auto* container = &derivedT::runtime_instances;
    std::function<constructed_data_t<typename typeT::RImplPtr>(const rx_node_id&)> func =
        [container, f](const rx_node_id& new_id) // full constructor function
    {
        constructed_data_t<typename typeT::RImplPtr> ret;
        auto new_ptr = f();
        if (!new_ptr)
        {
            std::ostringstream ss;
            ss << "Instance " << new_id.to_string()
                << " could not be created for class";
            return ret;
        }
        else
        {
            ret.ptr = new_ptr;
        }
        ret.register_f = [container, new_ptr](const rx_node_id& id)
        {
            locks::auto_lock_t _(rx_platform::g_runtime_lock);
            auto it = container->find(id);
            if (it == container->end())
                container->emplace(id, new_ptr);
        };
        ret.unregister_f = [container](const rx_node_id& id)
        {
            locks::auto_lock_t _(rx_platform::g_runtime_lock);
            container->erase(id);
        };
        return ret;
    };
    return ::rx_platform::register_monitored_constructor_impl<typeT>(id, func);
}

namespace algorithms
{
template <class typeT>
class types_model_algorithm;
template <class typeT>
class runtime_model_algorithm;
}

#define DECLARE_META_TYPE \
public:\
	static bool g_type_registred;\
	static void init_meta_type();\
	uint32_t get_type () const\
		{ return type_id; }\
	static uint32_t type_id;\
	static const char* type_name;\
	static const char* command_str;\
private:\




#define DECLARE_META_OBJECT \
public:\
	uint32_t get_type () const\
				{ return type_id; }\
	static uint32_t type_id;\
private:\


void init_compiled_meta_types();

typedef TYPELIST_9(variable_type, source_type, event_type, filter_type, mapper_type, program_type, struct_type, method_type, display_type) simple_rx_types;
typedef TYPELIST_4(object_type, port_type, application_type, domain_type) object_rx_types;
//typedef TYPELIST_11(reference_type, port_class, object_class, variable_class, source_class, event_class, filter_class, mapper_class, application_class, domain_class, struct_class) full_rx_types;






class relations_hash_data 
{
	relations_hash_data(const relations_hash_data&) = delete;
	relations_hash_data(relations_hash_data&&) = delete;
	void operator=(const relations_hash_data&) = delete;
	void operator=(relations_hash_data&&) = delete;

	struct relation_elements_data
	{
		std::set<rx_node_id> unordered;
		std::vector<rx_node_id> ordered;
	};

	// these are mostly static data, so we keep it ordered to find quickly
	typedef std::set<rx_node_id> relation_elements_type;
	// this here is pointer type so we don't have copying of whole set just pointer
	typedef std::map<rx_node_id, std::unique_ptr<relation_elements_type> > relation_map_type;


  public:
      relations_hash_data();

      ~relations_hash_data();


      void add_to_hash_data (const rx_node_id& new_id, const rx_node_id& first_backward);

      void remove_from_hash_data (const rx_node_id& id);

      void change_hash_data (const rx_node_id& id, const rx_node_id& first_backward_old, const rx_node_id& first_backward_new);

      bool is_backward_from (const rx_node_id& id, const rx_node_id& parent);

      void get_full_forward (const rx_node_id& id, std::vector< rx_node_id>& result) const;

      void get_full_backward (const rx_node_id& id, std::vector< rx_node_id>& result) const;

      void get_first_forward (const rx_node_id& id, std::vector< rx_node_id>& result) const;

      void get_first_backward (const rx_node_id& id, std::vector< rx_node_id>& result) const;

      void deinitialize ();


  protected:

  private:


      relation_map_type forward_hash_;

      relation_map_type backward_hash_;

      relation_map_type first_forward_hash_;

      relation_map_type first_backward_hash_;


};






class inheritance_hash 
{
	inheritance_hash(const inheritance_hash&) = delete;
	inheritance_hash(inheritance_hash&&) = delete;
	void operator=(const inheritance_hash&) = delete;
	void operator=(inheritance_hash&&) = delete;


	typedef std::set<rx_node_id> hash_elements_type;

	struct relation_elements_data
	{
        std::map<rx_node_id, size_t> unordered;
		rx_node_ids ordered;
	};

	typedef std::map<rx_node_id, relation_elements_data> relation_map_type;
	typedef std::map<rx_node_id, hash_elements_type> relation_hash_type;

  public:
      inheritance_hash();


      rx_result add_to_hash_data (const rx_node_id& new_id, const rx_node_id& base_id);

      rx_result get_base_types (const rx_node_id& id, rx_node_ids& result) const;

      rx_result get_derived_from (const rx_node_id& id, rx_node_ids& result) const;

      rx_result get_all_derived_from (const rx_node_id& id, rx_node_ids& result) const;

      rx_result remove_from_hash_data (const rx_node_id& id);

      rx_result add_to_hash_data (const std::vector<std::pair<rx_node_id, rx_node_id> >& items);

      void deinitialize ();

      bool is_derived_from (rx_node_id id, rx_node_id base_id) const;


  protected:

  private:


      //	contains base types of a type, it has ordered and
      //	unordered data
      relation_map_type hash_data_;

      //	contains all types derived from this particular type
      relation_hash_type derived_hash_;

      //	contains types derived exactly from this particular type
      relation_hash_type derived_first_hash_;

      locks::slim_lock hash_lock_;


};







class instance_hash 
{
	instance_hash(const instance_hash&) = delete;
	instance_hash(instance_hash&&) = delete;
	void operator=(const instance_hash&) = delete;
	void operator=(instance_hash&&) = delete;

	typedef std::set<rx_node_id> hash_elements_type;
	typedef std::map<rx_node_id, std::unique_ptr<hash_elements_type> > relation_hash_type;

  public:
      instance_hash();


      bool add_to_hash_data (const rx_node_id& new_id, const rx_node_id& type_id, const rx_node_ids& all_type_ids);

      bool remove_from_hash_data (const rx_node_id& new_id, const rx_node_id& type_id, const rx_node_ids& all_type_ids);

      rx_result get_instanced_from (const rx_node_id& id, rx_node_ids& result) const;

      rx_result is_instanced_from (const rx_node_id& id, rx_node_id base_id) const;

      void deinitialize ();


  protected:

  private:


      relation_hash_type instance_hash_;

      relation_hash_type instance_first_hash_;

      locks::slim_lock hash_lock_;


};



template <class typeT>
struct create_runtime_result
{
    std::vector<runtime_status_data> statuses;
    typename typeT::RTypePtr ptr;
    std::function<void(const rx_node_id&)> register_f;
    std::function<void(const rx_node_id&)> unregister_f;
    operator bool() const
    {
        return ptr;
    }
};





template <class typeT>
class types_repository 
{
	types_repository(const types_repository&) = delete;
	types_repository(types_repository&&) = delete;
	void operator=(const types_repository&) = delete;
	void operator=(types_repository&&) = delete;

public:
	typedef typeT HType;
	typedef typename typeT::RType RType;
	typedef typename typeT::RTypePtr RTypePtr;
	typedef typename typeT::smart_ptr Tptr;
	typedef rx_result_with<Tptr> TdefRes;
    typedef typename typeT::RImplType RImplType;
    typedef typename typeT::RImplPtr RImplPtr;
    typedef typename typeT::runtime_behavior_t RBeh;

    enum class runtime_state
    {
        runtime_state_created = 0,
        runtime_state_checkout = 1,
		runtime_state_initializing = 2,
		runtime_state_running = 3,
		runtime_state_deleting = 4,
		runtime_state_destroyed = 5
	};

    struct checkout_data
    {
        rx_uuid id = rx_uuid::null_uuid();
        string_type user_name;
    };

	struct runtime_data_t
	{
		RTypePtr target;
		runtime_state state;
        rx_node_id type;
        checkout_data checkout;
	};
    struct type_data_t
    {
        Tptr type_ptr;
        rx_node_id super_type;
    };
    typedef typename std::map<rx_node_id, rx_reference<discovery::peer_item> > registered_peer_types_type;
	typedef typename std::map<rx_node_id, runtime_data_t> registered_objects_type;
	typedef typename std::map<rx_node_id, type_data_t> registered_types_type;
	typedef typename std::map<rx_node_id, std::function<constructed_data_t<RImplPtr>(const rx_node_id&)> > constructors_type;
    typedef typename std::map<rx_node_id, std::function<RBeh()> > behaviors_type;

  public:
      types_repository();


      rx_result register_constructor (const rx_node_id& id, std::function<RImplPtr()> f);

      rx_result register_constructor (const rx_node_id& id, std::function<constructed_data_t<RImplPtr>(const rx_node_id&)> f);

      rx_result register_behavior (const rx_node_id& id, std::function<RBeh()> f);

      rx_result_with<create_runtime_result<typeT> > create_runtime (typename typeT::instance_data_t&& instance_data, data::runtime_values_data&& runtime_data, bool prototype = false);

      rx_result_with<typename types_repository<typeT>::RTypePtr> get_runtime (const rx_node_id& id, bool only_running = true) const;

      rx_result delete_runtime (rx_node_id id);

      api::query_result get_derived_types (const rx_node_id& id) const;

      rx_result register_type (typename types_repository<typeT>::Tptr what);

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      rx_result update_type (types_repository<typeT>::Tptr what);

      typename types_repository<typeT>::TdefRes get_type_definition (const rx_node_id& id) const;

      rx_result delete_type (rx_node_id id);

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      void deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      api::query_result get_instanced_objects (const rx_node_id& id) const;

      rx_result_with<typename typeT::RTypePtr> mark_runtime_for_delete (rx_node_id id, const rx_uuid& checkout = rx_uuid::null_uuid());

      rx_result mark_runtime_running (rx_node_id id);

      rx_result type_exists (rx_node_id id) const;

      std::vector<typename types_repository<typeT>::RTypePtr> get_active_runtimes ();

      bool is_derived_from (rx_node_id id, rx_node_id base_id) const;

      bool is_instanced_from (rx_node_id id, rx_node_id base_id) const;


  protected:

  private:

      void collect_and_add_depedencies (const typeT& what, const rx_node_id& parent_id);



      inheritance_hash inheritance_hash_;

      instance_hash instance_hash_;


      registered_objects_type registered_objects_;

      registered_types_type registered_types_;

      constructors_type constructors_;

      behaviors_type behaviors_;

      registered_peer_types_type registered_peer_types_;

	  std::function<RImplPtr()> default_constructor_;
};






template <class typeT>
class simple_types_repository 
{
	simple_types_repository(const simple_types_repository&) = delete;
	simple_types_repository(simple_types_repository&&) = delete;
	void operator=(const simple_types_repository&) = delete;
	void operator=(simple_types_repository&&) = delete;

public:
	typedef typeT HType;
	typedef typename typeT::RDataType RDataType;
	typedef typename typeT::RType RType;
	typedef typename typeT::RTypePtr RTypePtr;
	typedef typename typeT::smart_ptr Tptr;
	typedef rx_result_with<Tptr> TdefRes;

    struct type_data_t
    {
        Tptr type_ptr;
        rx_node_id super_type;
    };
    typedef typename std::map<rx_node_id, rx_reference<discovery::peer_item> > registered_peer_types_type;
	typedef typename std::map<rx_node_id, type_data_t> registered_types_type;
    typedef typename std::map<rx_node_id, std::function<RTypePtr()> > constructors_type;

  public:
      simple_types_repository();


      typename simple_types_repository<typeT>::TdefRes get_type_definition (const rx_node_id& id) const;

      rx_result register_type (typename simple_types_repository<typeT>::Tptr what);

      rx_result register_constructor (const rx_node_id& id, std::function<RTypePtr()> f);

      rx_result_with<typename simple_types_repository<typeT>::RDataType> create_simple_runtime (const rx_node_id& type_id, const string_type& rt_name, construct_context& ctx) const;

      api::query_result get_derived_types (const rx_node_id& id) const;

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      rx_result delete_type (rx_node_id id);

      rx_result type_exists (rx_node_id id) const;

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      void deinitialize ();

      rx_result update_type (typename simple_types_repository<typeT>::Tptr what);

      bool is_derived_from (rx_node_id id, rx_node_id base_id) const;

      rx_result register_peer_type (rx_reference<discovery::peer_item> what);


  protected:

  private:

      void collect_and_add_depedencies (const typeT& what, const rx_node_id& parent_id);



      inheritance_hash inheritance_hash_;


      registered_types_type registered_types_;

      constructors_type constructors_;

      registered_peer_types_type registered_peer_types_;

	  std::function<RTypePtr()> default_constructor_;
};


enum class resolver_event_verb
{
    created,
    updated,
    deleted
};
struct resolver_event_data
{
    resolver_event_verb verb;
    rx_node_id id;
    string_type path;
};




class types_resolver 
{
	struct resolver_data
	{
		rx_item_type type;
		meta_data data;
	};
	typedef std::map<rx_node_id, resolver_data> types_hash_t;
    struct resolver_subscriber_data
    {
        rx_thread_handle_t notify_thread;
        std::function<void(resolver_event_data)> callback;
    };
    std::map<rx_reference_ptr, resolver_subscriber_data> subscribers_;
  public:
    template<typename Tptr>
    void register_subscriber(Tptr who, std::function<void(resolver_event_data)> callback)
    {
        rx_post_function_to(RX_DOMAIN_META, who, [this, who, callback] {
            rx_reference_ptr anchor = who;
            auto it = subscribers_.find(anchor);
            if (it == subscribers_.end())
            {
                resolver_subscriber_data data;
                data.callback = std::move(callback);
                data.notify_thread = who->get_executer();
                subscribers_.emplace(anchor, std::move(data));
            }
            });
    }
    template<typename Tptr>
    void unregister_subscriber(Tptr who)
    {
        rx_post_function_to(RX_DOMAIN_META, who, [this, who] {
            rx_reference_ptr anchor = who;
            auto it = subscribers_.find(anchor);
            if (it != subscribers_.end())
            {
                subscribers_.erase(it);
            }
            });
    }

  public:

      rx_result add_id (const rx_node_id& id, rx_item_type type, const meta_data& data);

      rx_item_type get_item_type (const rx_node_id& id) const;

      bool is_available_id (const rx_node_id& id) const;

      rx_result remove_id (const rx_node_id& id);

      rx_item_type get_item_data (const rx_node_id& id, meta_data& data) const;

      rx_result update_id (const rx_node_id& id, const meta_data& data);

      void deinitialize ();


  protected:

  private:


      types_hash_t hash_;


};






class relations_type_repository 
{
	relations_type_repository(const relations_type_repository&) = delete;
	relations_type_repository(relations_type_repository&&) = delete;
	void operator=(const relations_type_repository&) = delete;
	void operator=(relations_type_repository&&) = delete;

public:
	typedef relation_type HType;
	typedef typename relation_type::RType RType;
	typedef typename relation_type::RTypePtr RTypePtr;
    typedef typename relation_type::RImplPtr RImplPtr;
	typedef typename relation_type::smart_ptr Tptr;
	typedef rx_result_with<Tptr> TdefRes;

	enum class runtime_state
	{
		runtime_state_created = 0,
		runtime_state_initializing = 1,
		runtime_state_running = 2,
		runtime_state_deleting = 3,
		runtime_state_destroyed = 4
	};

	struct runtime_data_t
	{
		RTypePtr target;
		runtime_state state;
	};

    struct type_data_t
    {
        Tptr type_ptr;
        rx_node_id super_type;
    };
    typedef typename std::map<rx_node_id, rx_reference<discovery::peer_item> > registered_peer_types_type;
	typedef typename std::map<rx_node_id, runtime_data_t> registered_objects_type;
	typedef typename std::map<rx_node_id, type_data_t> registered_types_type;
    //typedef typename std::map<rx_node_id, std::function<RTypePtr()> > constructors_type;
    typedef typename std::map<rx_node_id, std::function<constructed_data_t<rx_relation_impl_ptr>(const rx_node_id&)> > constructors_type;

  public:
      relations_type_repository();


      relations_type_repository::TdefRes get_type_definition (const rx_node_id& id) const;

      rx_result register_type (relations_type_repository::Tptr what);

      rx_result_with<create_runtime_result<relation_type> > create_runtime (const rx_node_id& type_id, const string_type& rt_name, runtime::relations::relation_data& data, const rx_directory_resolver& dirs);

      api::query_result get_derived_types (const rx_node_id& id) const;

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      rx_result delete_type (rx_node_id id);

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      void deinitialize ();

      rx_result update_type (relations_type_repository::Tptr what);

      rx_result type_exists (rx_node_id id) const;

      rx_result register_constructor (const rx_node_id& id, std::function<RTypePtr()> f);

      rx_result register_constructor (const rx_node_id& id, std::function<constructed_data_t<RTypePtr>(const rx_node_id&)> f);

      bool is_derived_from (rx_node_id id, rx_node_id base_id) const;


  protected:

  private:

      rx_result_with<create_runtime_result<relation_type> > create_relation_runtime (relations_type_repository::Tptr form_what, runtime::relations::relation_data& data, const rx_directory_resolver& dirs);

      void collect_and_add_depedencies (const relations_type_repository::Tptr& what, const rx_node_id& parent_id);



      inheritance_hash inheritance_hash_;

      instance_hash instance_hash_;


      registered_types_type registered_types_;

      constructors_type constructors_;

      registered_peer_types_type registered_peer_types_;

      std::function<RTypePtr()> default_constructor_;
};






class data_type_repository 
{
    data_type_repository(const data_type_repository&) = delete;
    data_type_repository(data_type_repository&&) = delete;
    void operator=(const data_type_repository&) = delete;
    void operator=(data_type_repository&&) = delete;

public:
    typedef data_type HType;
    typedef typename data_type::RDataType RDataType;
    typedef typename data_type::smart_ptr Tptr;
    typedef rx_result_with<Tptr> TdefRes;

    struct type_data_t
    {
        Tptr type_ptr;
        rx_node_id super_type;
    };
    typedef typename std::map<rx_node_id, rx_reference<discovery::peer_item> > registered_peer_types_type;
    typedef typename std::map<rx_node_id, type_data_t> registered_types_type;

  public:
      data_type_repository();


      data_type_repository::TdefRes get_type_definition (const rx_node_id& id) const;

      rx_result register_type (data_type_repository::Tptr what);

      rx_result_with<runtime::structure::block_data_result_t> create_data_type (const rx_node_id& type_id, const string_type& rt_name, construct_context& ctx, const rx_directory_resolver& dirs);

      api::query_result get_derived_types (const rx_node_id& id) const;

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      rx_result delete_type (rx_node_id id);

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      void deinitialize ();

      rx_result update_type (data_type_repository::Tptr what);

      rx_result type_exists (rx_node_id id) const;

      bool is_derived_from (rx_node_id id, rx_node_id base_id) const;


  protected:

  private:

      void collect_and_add_depedencies (const data_type_repository::Tptr& what, const rx_node_id& parent_id);



      inheritance_hash inheritance_hash_;


      registered_types_type registered_types_;

      registered_peer_types_type registered_peer_types_;


};




struct names_hash_element
{
	rx_node_id id;
	uint32_t type;
	bool is_object() const
	{
		return (type&RX_TYPE_INSTANCE) == RX_TYPE_INSTANCE;
	}
	bool is_ref_type() const
	{
		return type == RX_TYPE_REF;
	}
};
struct ids_hash_element
{
	string_type name;
	uint32_t type;
	bool is_object() const
	{
		return (type&RX_TYPE_INSTANCE) == RX_TYPE_INSTANCE;
	}
	bool is_ref_type() const
	{
		return type == RX_TYPE_REF;
	}
};





class platform_types_manager 
{
	//friend class worker_registration_object;
	template<class T>
	struct type_repositories_holder
	{
	public:
		types_repository<T>* value_;
		type_repositories_holder()
			: value_(NULL)
		{
		}
		~type_repositories_holder()
		{
            if (value_)
                delete value_;
		}
	};

	class type_repositories_container
		: public tl::gen_scatter_hierarchy<object_rx_types, type_repositories_holder>
	{
	public:
		template<class T>
		types_repository<T>& get_internal(tl::type2type<T>)
		{
			types_repository<T>* ret = (static_cast<type_repositories_holder<T>&>(*this)).value_;
			if (ret == nullptr)
			{
				ret = new types_repository<T>();
				(static_cast<type_repositories_holder<T>&>(*this)).value_ = ret;
			}
			return *ret;
		}
	};
	type_repositories_container _types_container;

	template<class T>
	struct simple_types_repositories_holder
	{
	public:
		simple_types_repository<T>* value_;
		simple_types_repositories_holder()
			: value_(NULL)
		{
		}
		~simple_types_repositories_holder()
		{
            if (value_)
                delete value_;
		}
	};

	class simple_type_repositories_container
		: public tl::gen_scatter_hierarchy<simple_rx_types, simple_types_repositories_holder>
	{
	public:
		template<class T>
		simple_types_repository<T>& get_internal(tl::type2type<T>)
		{
			simple_types_repository<T>* ret = (static_cast<simple_types_repositories_holder<T>&>(*this)).value_;
			if (ret == nullptr)
			{
				ret = new simple_types_repository<T>();
				(static_cast<simple_types_repositories_holder<T>&>(*this)).value_ = ret;
			}
			return *ret;
		}
	};
	simple_type_repositories_container _simple_types_container;

  public:

      static platform_types_manager& instance ();

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      void deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      void stop ();

      transactions::dependency_cache& get_dependecies_cache ();


      types_resolver& get_types_resolver ()
      {
        return types_resolver_;
      }


	  relations_type_repository& get_relations_repository()
	  {
		  return relations_repository_;
	  }
      data_type_repository& get_data_types_repository()
      {
          return data_types_repository_;
      }
	  template<class T>
	  types_repository<T>& get_type_repository()
	  {
		  return _types_container.get_internal<T>(tl::type2type<T>());
	  }
	  template<class T>
	  simple_types_repository<T>& get_simple_type_repository()
	  {
		  return _simple_types_container.get_internal<T>(tl::type2type<T>());
	  }
  protected:

  private:
      platform_types_manager();



      types_resolver types_resolver_;

      relations_type_repository relations_repository_;

      data_type_repository data_types_repository_;

      std::unique_ptr<transactions::dependency_cache> dependecies_cache_;

	  template <class typeT>
	  friend class algorithms::types_model_algorithm;
};


} // namespace model
} // namespace rx_internal



#endif
