

/****************************************************************************
*
*  model\rx_meta_internals.h
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


#ifndef rx_meta_internals_h
#define rx_meta_internals_h 1



// rx_thread
#include "lib/rx_thread.h"

#include "system/meta/rx_types.h"
#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_objbase.h"
#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"

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


namespace model {

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

typedef TYPELIST_6(variable_type, source_type, event_type, filter_type, mapper_type, struct_type) simple_rx_types;
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
		hash_elements_type unordered;
		rx_node_ids ordered;
	};

	// this here is pointer type so we don't have copying of whole set just pointer
	typedef std::map<rx_node_id, relation_elements_data> relation_map_type;

	typedef std::map<rx_node_id, hash_elements_type> relation_hash_type;

  public:
      inheritance_hash();


      rx_result add_to_hash_data (const rx_node_id& new_id, const rx_node_id& base_id);

      rx_result get_base_types (const rx_node_id& id, rx_node_ids& result) const;

      rx_result get_derived_from (const rx_node_id& id, rx_node_ids& result) const;

      rx_result get_all_derived_from (const rx_node_id& id, rx_node_ids& result) const;

      rx_result remove_from_hash_data (const rx_node_id& new_id, const rx_node_id& base_id);

      rx_result add_to_hash_data (const std::vector<std::pair<rx_node_id, rx_node_id> >& items);


  protected:

  private:


      //	contains base types of a type, it has ordered and
      //	unordered data
      relation_map_type hash_data_;

      //	contains all types derived from this particular type
      relation_hash_type derived_hash_;

      //	contains types derived exactly from this particular type
      relation_hash_type derived_first_hash_;


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


  protected:

  private:


      relation_hash_type instance_hash_;

      relation_hash_type instance_first_hash_;


};






template <class typeT>
class type_hash 
{
	type_hash(const type_hash&) = delete;
	type_hash(type_hash&&) = delete;
	void operator=(const type_hash&) = delete;
	void operator=(type_hash&&) = delete;

public:
	typedef typename typeT::RType RType;
	typedef typename typeT::RTypePtr RTypePtr;
	typedef typename typeT::smart_ptr Tptr;

	enum runtime_state
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

	typedef typename std::unordered_map<rx_node_id, runtime_data_t> registered_objects_type;
	typedef typename std::unordered_map<rx_node_id, Tptr> registered_types_type;
	typedef typename std::map<rx_node_id, std::function<RTypePtr()> > constructors_type;

  public:
      type_hash();


      typename type_hash<typeT>::Tptr get_type_definition (const rx_node_id& id) const;

      rx_result register_type (typename type_hash<typeT>::Tptr what);

      rx_result register_constructor (const rx_node_id& id, std::function<RTypePtr()> f);

      rx_result_with<typename type_hash<typeT>::RTypePtr> create_runtime (meta_data& meta, typename typeT::instance_data_t&& type_data, data::runtime_values_data* init_data = nullptr, bool prototype = false);

      api::query_result get_derived_types (const rx_node_id& id) const;

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      typename type_hash<typeT>::RTypePtr get_runtime (const rx_node_id& id) const;

      rx_result delete_runtime (rx_node_id id);

      rx_result delete_type (rx_node_id id);

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      rx_result update_type (typename type_hash<typeT>::Tptr what);

      api::query_result get_instanced_objects (const rx_node_id& id) const;

      rx_result_with<typename typeT::RTypePtr> mark_runtime_for_delete (rx_node_id id);

      rx_result_with<typename typeT::RTypePtr> mark_runtime_running (rx_node_id id);


  protected:

  private:


      inheritance_hash inheritance_hash_;

      instance_hash instance_hash_;


      registered_objects_type registered_objects_;

      registered_types_type registered_types_;

      constructors_type constructors_;

	  std::function<RTypePtr()> default_constructor_;
};






template <class typeT>
class simple_type_hash 
{
	simple_type_hash(const simple_type_hash&) = delete;
	simple_type_hash(simple_type_hash&&) = delete;
	void operator=(const simple_type_hash&) = delete;
	void operator=(simple_type_hash&&) = delete;

public:
	typedef typename typeT::RDataType RDataType;
	typedef typename typeT::RType RType;
	typedef typename typeT::RTypePtr RTypePtr;
	typedef typename typeT::smart_ptr Tptr;

	typedef typename std::unordered_map<rx_node_id, Tptr> registered_types_type;
	typedef typename std::map<rx_node_id, std::function<RTypePtr()> > constructors_type;

  public:
      simple_type_hash();


      typename simple_type_hash<typeT>::Tptr get_type_definition (const rx_node_id& id) const;

      rx_result register_type (typename simple_type_hash<typeT>::Tptr what);

      rx_result register_constructor (const rx_node_id& id, std::function<RTypePtr()> f);

      rx_result_with<typename simple_type_hash<typeT>::RDataType> create_simple_runtime (const rx_node_id& type_id) const;

      api::query_result get_derived_types (const rx_node_id& id) const;

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      rx_result delete_type (rx_node_id id);

      rx_result type_exists (rx_node_id id) const;

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);


  protected:

  private:


      inheritance_hash inheritance_hash_;


      registered_types_type registered_types_;

      constructors_type constructors_;

	  std::function<RTypePtr()> default_constructor_;
};






class types_resolver 
{
	struct resolver_data
	{
		rx_item_type type;
		meta_data data;
	};
	typedef std::map<rx_node_id, resolver_data> types_hash_t;

  public:

      rx_result add_id (const rx_node_id& id, rx_item_type type, const meta_data& data);

      rx_item_type get_item_type (const rx_node_id& id) const;

      bool is_available_id (const rx_node_id& id) const;

      rx_result remove_id (const rx_node_id& id);

      rx_item_type get_item_data (const rx_node_id& id, meta_data& data) const;


  protected:

  private:


      types_hash_t hash_;


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
	struct type_cache_holder
	{
	public:
		type_hash<T>* value_;
		type_cache_holder()
			: value_(NULL)
		{
		}
		~type_cache_holder()
		{
		}
	};

	class type_cache_list_container
		: public tl::gen_scatter_hierarchy<object_rx_types, type_cache_holder>
	{
	public:
		template<class T>
		type_hash<T>& get_internal(tl::type2type<T>)
		{
			type_hash<T>* ret = (static_cast<type_cache_holder<T>&>(*this)).value_;
			if (ret == nullptr)
			{
				ret = new type_hash<T>();
				(static_cast<type_cache_holder<T>&>(*this)).value_ = ret;
			}
			return *ret;
		}
		template<class T>
		const type_hash<T>& get_internal_const(tl::type2type<T>) const
		{
			const type_hash<T>* ret = (static_cast<const type_cache_holder<T>&>(*this)).value_;
			return *ret;
		}
	};
	type_cache_list_container _types_container;

	template<class T>
	struct simple_type_cache_holder
	{
	public:
		simple_type_hash<T>* value_;
		simple_type_cache_holder()
			: value_(NULL)
		{
		}
		~simple_type_cache_holder()
		{
		}
	};

	class simple_type_cache_list_container
		: public tl::gen_scatter_hierarchy<simple_rx_types, simple_type_cache_holder>
	{
	public:
		template<class T>
		simple_type_hash<T>& get_internal(tl::type2type<T>)
		{
			simple_type_hash<T>* ret = (static_cast<simple_type_cache_holder<T>&>(*this)).value_;
			if (ret == nullptr)
			{
				ret = new simple_type_hash<T>();
				(static_cast<simple_type_cache_holder<T>&>(*this)).value_ = ret;
			}
			return *ret;
		}
		template<class T>
		const simple_type_hash<T>& get_internal_const(tl::type2type<T>) const
		{
			const simple_type_hash<T>* ret = (static_cast<const simple_type_cache_holder<T>&>(*this)).value_;
			return *ret;
		}
	};
	simple_type_cache_list_container _simple_types_container;

  public:

      static platform_types_manager& instance ();

      rx_result initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      rx_result deinitialize ();

      rx_result start (hosting::rx_platform_host* host, const meta_configuration_data_t& data);

      rx_result stop ();


      rx::threads::physical_job_thread& get_worker ()
      {
        return worker_;
      }


      types_resolver& get_types_resolver ()
      {
        return types_resolver_;
      }


	  template<class T>
	  type_hash<T>& internal_get_type_cache()
	  {
		  return _types_container.get_internal<T>(tl::type2type<T>());
	  }
	  template<class T>
	  simple_type_hash<T>& internal_get_simple_type_cache()
	  {
		  return _simple_types_container.get_internal<T>(tl::type2type<T>());
	  }
	  template<class T>
	  const type_hash<T>& get_type_cache() const
	  {
		  return _types_container.get_internal_const<T>(tl::type2type<T>());
	  }
	  template<class T>
	  const simple_type_hash<T>& get_simple_type_cache() const
	  {
		  return _simple_types_container.get_internal_const<T>(tl::type2type<T>());
	  }
	  template<class T>
	  typename T::smart_ptr get_type_sync(const string_type& path, rx_directory_ptr dir)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(path);
		  if (!item)
		  {// TODO error, item does not exists
			  return T::smart_ptr::null_ptr;
		  }
		  auto id = item->meta_info().get_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  return T::smart_ptr::null_ptr;
		  }
		  auto ret = internal_get_type_cache<T>().get_type_definition(std::move(id));
		  if (!ret)
		  {// TODO error, invalid node id
			  return T::smart_ptr::null_ptr;
		  }
		  return ret;
	  }
	  template<class T>
	  typename T::RTypePtr get_runtime_sync(const string_type& path, rx_directory_ptr dir)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(path);
		  if (!item)
		  {// TODO error, item does not exists
			  return T::RTypePtr::null_ptr;
		  }
		  auto id = item->meta_info().get_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  return T::RTypePtr::null_ptr;
		  }
		  auto ret = internal_get_type_cache<T>().get_runtime(std::move(id));
		  if (!ret)
		  {// TODO error, invalid node id
			  return T::RTypePtr::null_ptr;
		  }
		  return ret;
	  }
  protected:

  private:
      platform_types_manager();



      rx::threads::physical_job_thread worker_;

      types_resolver types_resolver_;

	  template <class typeT>
	  friend class algorithms::types_model_algorithm;
};


} // namespace model



#endif
