

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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
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
#define META_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Meta",src,lvl,msg)
#define META_LOG_TRACE(src,lvl,msg) RX_TRACE("Meta",src,lvl,msg)


namespace model {



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


struct query_result_detail
{
	rx_node_id id;
	string_type name;
};

struct query_result
{
	uint32_t error_code;
	std::vector<query_result_detail> details;
};







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

	struct relation_elements_data
	{
		std::set<rx_node_id> unordered;
		rx_node_ids ordered;
	};

	// these are mostly static data, so we keep it ordered to find quickly
	typedef std::set<rx_node_id> hash_elements_type;
	// this here is pointer type so we don't have copying of whole set just pointer
	typedef std::map<rx_node_id, std::unique_ptr<relation_elements_data> > relation_map_type;

	typedef std::map<rx_node_id, std::unique_ptr<hash_elements_type> > relation_hash_type;

  public:
      inheritance_hash();


      rx_result add_to_hash_data (const rx_node_id& new_id, const rx_node_id& base_id);

      rx_result get_base_types (const rx_node_id& id, rx_node_ids& result) const;

      rx_result get_derived_from (const rx_node_id& id, rx_node_ids& result) const;

      rx_result get_all_derived_from (const rx_node_id& id, rx_node_ids& result) const;

      rx_result remove_from_hash_data (const rx_node_id& new_id, const rx_node_id& base_id);


  protected:

  private:


      relation_map_type hash_data_;

      relation_hash_type derived_hash_;

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

	typedef typename std::unordered_map<rx_node_id, RTypePtr> registered_objects_type;
	typedef typename std::unordered_map<rx_node_id, Tptr> registered_types_type;
	typedef typename std::map<rx_node_id, std::function<RTypePtr()> > constructors_type;

  public:
      type_hash();


      typename type_hash<typeT>::Tptr get_type_definition (const rx_node_id& id) const;

      rx_result register_type (typename type_hash<typeT>::Tptr what);

      rx_result register_constructor (const rx_node_id& id, std::function<RType()> f);

      rx_result_with<typename type_hash<typeT>::RTypePtr> create_runtime (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system, const data::runtime_values_data* init_data = nullptr);

      rx_result_with<typename type_hash<typeT>::RTypePtr> create_runtime (const string_type& name, const rx_node_id& type_id, const data::runtime_values_data* init_data = nullptr);

      query_result get_derived_types (const rx_node_id& id) const;

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      typename type_hash<typeT>::RTypePtr get_runtime (const rx_node_id& id) const;

      rx_result delete_runtime (rx_node_id id);

      rx_result delete_type (rx_node_id id);


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


      typename type_hash<typeT>::Tptr get_type_definition (const rx_node_id& id) const;

      rx_result register_type (typename type_hash<typeT>::Tptr what);

      rx_result register_constructor (const rx_node_id& id, std::function<RType()> f);

      rx_result_with<typename simple_type_hash<typeT>::RDataType> create_simple_runtime (const rx_node_id& type_id) const;

      query_result get_derived_types (const rx_node_id& id) const;

      rx_result check_type (const rx_node_id& id, type_check_context& ctx) const;

      rx_result delete_type (rx_node_id id);


  protected:

  private:


      inheritance_hash inheritance_hash_;


      registered_types_type registered_types_;

      constructors_type constructors_;

	  std::function<RTypePtr()> default_constructor_;
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

  public:

      static platform_types_manager& instance ();

      uint32_t initialize (hosting::rx_platform_host* host, meta_data_t& data);

      uint32_t deinitialize ();

      uint32_t start (hosting::rx_platform_host* host, const meta_data_t& data);

      uint32_t stop ();


      rx::threads::physical_job_thread& get_worker ()
      {
        return worker_;
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
	  typename T::smart_ptr get_type(const string_type& path, rx_directory_ptr dir)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(path);
		  if (!item)
		  {// TODO error, item does not exists
			  return T::smart_ptr::null_ptr;
		  }
		  auto id = item->get_node_id();
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
	  typename T::RTypePtr get_runtime(const string_type& path, rx_directory_ptr dir)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(path);
		  if (!item)
		  {// TODO error, item does not exists
			  return T::RTypePtr::null_ptr;
		  }
		  auto id = item->get_node_id();
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
	  template<class T, class refT>
	  void create_runtime(const string_type& name, const string_type& type_name, const data::runtime_values_data* init_data, rx_directory_ptr dir, std::function<void(rx_result_with<typename T::RTypePtr>&&)> callback, refT ref)
	  {
		  std::function<rx_result_with<typename T::RTypePtr>()> func = [=]() mutable {
			  return create_runtime_helper<T>(name, type_name, init_data, dir, tl::type2type<T>());
		  };
		  rx_do_with_callback<rx_result_with<typename T::RTypePtr>, refT>(func, RX_DOMAIN_META, callback, ref);
	  }
	  template<class T>
	  rx_result_with<typename T::RTypePtr> create_runtime_helper(const string_type& name, const string_type& type_name, const data::runtime_values_data* init_data, rx_directory_ptr dir, tl::type2type<T>)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(type_name);
		  if (!item)
		  {// TODO error, type does not exists
			  return "Type "s + type_name + " does not exists!";
		  }
		  auto id = item->get_node_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  return type_name + " does not have valid Id!";
		  }
		  auto ret = internal_get_type_cache<T>().create_runtime(name, id, init_data);
		  if (ret)
		  {
			  if (!dir->add_item(ret.value()->get_item_ptr()))
			  {
				  internal_get_type_cache<T>().delete_runtime(id);
				  return "Unable to add "s + name + " to directory!";
				  // TODO error, can't add this name
			  }
		  }
		  return ret;
	  }

	  template<class T, class refT>
	  void create_type(typename T::smart_ptr what, rx_directory_ptr dir, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback, refT ref)
	  {
		  using result_t = rx_result_with<typename T::smart_ptr>;
		  std::function<result_t(void)> func = [=]() {
			  return create_type_helper<T>(what, dir, tl::type2type<T>());
		  };
		  rx_do_with_callback<result_t, refT>(func, RX_DOMAIN_META, callback, ref);
	  }
	  template<class T>
	  rx_result_with<typename T::smart_ptr> create_type_helper(typename T::smart_ptr what, rx_directory_ptr dir, tl::type2type<T>)
	  {
		  auto ret = internal_get_type_cache<T>().register_type(what);
		  if (!ret)
		  {// TODO error, didn't created runtime
			  return T::smart_ptr::null_ptr;
		  }
		  if (!dir->add_item(what->get_item_ptr()))
		  {
			  internal_get_type_cache<T>().delete_type(what->meta_data().get_id());
			  // TODO error, can't add this name
			  return T::smart_ptr::null_ptr;
		  }
		  return what;
	  }
	  template<class T>
	  rx_result_with<typename T::smart_ptr> create_simple_type(typename T::smart_ptr what, rx_directory_ptr dir)
	  {
		  auto ret = internal_get_simple_type_cache<T>().register_type(what);
		  if (!ret)
		  {// TODO error, didn't created runtime
			  return T::smart_ptr::null_ptr;
		  }
		  if (!dir->add_item(what->get_item_ptr()))
		  {
			  if (internal_get_simple_type_cache<T>().delete_type(what->meta_data().get_id()))
			  {

			  }
			  // TODO error, can't add this name
			  return T::smart_ptr::null_ptr;
		  }
		  return what;
	  }

	  template<class T, class refT>
	  void check_type(const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, refT ref)
	  {
		  std::function<type_check_context(const string_type, rx_directory_ptr)> func = [=](const string_type loc_name, rx_directory_ptr loc_dir) mutable {
			  return check_type_helper<T>(loc_name, loc_dir, tl::type2type<T>());
		  };
		  rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
	  }
	  template<class T, class refT>
	  void check_simple_type(const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, refT ref)
	  {
		  std::function<type_check_context(const string_type, rx_directory_ptr)> func = [=](const string_type loc_name, rx_directory_ptr loc_dir) mutable {
			  return check_simple_type_helper<T>(loc_name, loc_dir, tl::type2type<T>());
		  };
		  rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
	  }

	  template<class T, class refT>
	  void delete_runtime(const string_type& name, rx_directory_ptr dir, std::function<void(rx_result&&)> callback, refT ref)
	  {
		  std::function<rx_result()> func = [=]() {
			  return delete_runtime_helper<T>(name, dir, tl::type2type<T>());
		  };
		  rx_do_with_callback<rx_result, refT>(func, RX_DOMAIN_META, callback, ref);
	  }
	  template<class T, class refT>
	  void delete_type(const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, refT ref)
	  {
		  std::function<rx_result(string_type, rx_directory_ptr)> func = [=](string_type name, rx_directory_ptr dir) {
			  return delete_type_helper<T>(name, dir, tl::type2type<T>());
		  };
		  rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
	  }
	  template<class T, class refT>
	  void delete_simple_type(const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, refT ref)
	  {
		  std::function<rx_result(string_type, rx_directory_ptr)> func = [=](string_type name, rx_directory_ptr dir) {
			  return delete_simple_type_helper<T>(name, dir, tl::type2type<T>());
		  };
		  rx_do_with_callback(func, RX_DOMAIN_META, callback, ref, name, dir);
	  }
  protected:

  private:
      platform_types_manager();

	  template<class T>
	  rx_result delete_runtime_helper(const string_type& name, rx_directory_ptr dir, tl::type2type<T>)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(name);
		  if (!item)
		  {
			  return name + " does not exists!";
		  }
		  auto id = item->get_node_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  return name + " does not have valid " + T::type_name + " id!";
		  }
		  auto ret = internal_get_type_cache<T>().delete_runtime(id);
		  if (!ret)
		  {// TODO error, didn't deleted runtime
			  return ret;
		  }
		  dir->delete_item(name);
		  return true;
	  }
	  template<class T>
	  rx_result delete_type_helper(const string_type& name, rx_directory_ptr dir, tl::type2type<T>)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(name);
		  if (!item)
		  {// TODO error, item does not exists
			  return false;
		  }
		  auto id = item->get_node_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  return false;
		  }
		  auto ret = internal_get_type_cache<T>().delete_type(id);
		  if (!ret)
		  {// TODO error, didn't deleted runtime
			  return false;
		  }
		  dir->delete_item(name);
		  return true;
	  }
	  template<class T>
	  rx_result delete_simple_type_helper(const string_type& name, rx_directory_ptr dir, tl::type2type<T>)
	  {
		  rx_platform_item::smart_ptr item = dir->get_sub_item(name);
		  if (!item)
		  {// TODO error, item does not exists
			  return false;
		  }
		  auto id = item->get_node_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  return false;
		  }
		  auto ret = internal_get_simple_type_cache<T>().delete_type(id);
		  if (!ret)
		  {// TODO error, didn't deleted runtime
			  return false;
		  }
		  dir->delete_item(name);
		  return true;
	  }
	  template<class T>
	  type_check_context check_type_helper(const string_type& name, rx_directory_ptr dir, tl::type2type<T>)
	  {
		  type_check_context ret;
		  rx_platform_item::smart_ptr item = dir->get_sub_item(name);
		  if (!item)
		  {
			  ret.add_error(name + " does not exists!");
			  return ret;
		  }
		  auto id = item->get_node_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  ret.add_error(name + " does not have valid " + T::type_name + " id!");
			  return ret;
		  }
		  internal_get_type_cache<T>().check_type(id, ret);
		  return ret;
	  }
	  template<class T>
	  type_check_context check_simple_type_helper(const string_type& name, rx_directory_ptr dir, tl::type2type<T>)
	  {
		  type_check_context ret;
		  rx_platform_item::smart_ptr item = dir->get_sub_item(name);
		  if (!item)
		  {
			  ret.add_error(name + " does not exists!");
			  return ret;
		  }
		  auto id = item->get_node_id();
		  if (id.is_null())
		  {// TODO error, item does not have id
			  ret.add_error(name + " does not have valid " + T::type_name + " id!");
			  return ret;
		  }
		  internal_get_simple_type_cache<T>().check_type(id, ret);
		  return ret;
	  }

      rx::threads::physical_job_thread worker_;


};


// Parameterized Class model::type_hash 

template <class typeT>
type_hash<typeT>::type_hash()
{
	default_constructor_ = []()
	{
		return rx_create_reference<RType>();
	};
}



template <class typeT>
typename type_hash<typeT>::Tptr type_hash<typeT>::get_type_definition (const rx_node_id& id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return it->second;
	}
	else
	{
		return Tptr::null_ptr;
	}
}

template <class typeT>
rx_result type_hash<typeT>::register_type (typename type_hash<typeT>::Tptr what)
{
	const auto& id = what->meta_data().get_id();
	auto it = registered_types_.find(id);
	if (it == registered_types_.end())
	{
		registered_types_.emplace(what->meta_data().get_id(), what);
		inheritance_hash_.add_to_hash_data(id, what->meta_data().get_parent());
		return true;
	}
	else
	{
		return false;
	}
}

template <class typeT>
rx_result type_hash<typeT>::register_constructor (const rx_node_id& id, std::function<RType()> f)
{
	constructors_.emplace(id, f);
	return true;
}

template <class typeT>
rx_result_with<typename type_hash<typeT>::RTypePtr> type_hash<typeT>::create_runtime (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system, const data::runtime_values_data* init_data)
{
	rx_node_id to_create;
	if (id.is_null())
		to_create = rx_node_id::generate_new(RX_USER_NAMESPACE);
	else
	{
		if (registered_objects_.find(id) != registered_objects_.end())
		{
			return "Duplicate Id!";
		}
		to_create = id;
	}

	RTypePtr ret;

	rx_node_ids base;
	base.emplace_back(type_id);
	auto base_result = inheritance_hash_.get_base_types(type_id,base);
	if (!base_result)
		return base_result.errors();
	for(const auto& one : base)
	{
		auto it = constructors_.find(one);
		if (it != constructors_.end())
		{
			ret = (it->second)();
			break;
		}
	}
	if (!ret)
		ret = default_constructor_();

	construct_context ctx;
	ret->meta_data().construct(name, to_create, type_id, system);
	for (auto one_id : base)
	{
		auto my_class = get_type_definition(one_id);
		if (my_class)
		{
			auto result = my_class->construct(ret, ctx);
			if (!result)
			{// error constructing object
				return result.errors();
			}
		}
	}
	object_type::set_object_runtime_data(ctx.runtime_data, ret);
	if (init_data)
	{
		ret->fill_data(*init_data);
	}
	registered_objects_.emplace(to_create, ret);
	instance_hash_.add_to_hash_data(to_create, type_id, base);
	return ret;
}

template <class typeT>
rx_result_with<typename type_hash<typeT>::RTypePtr> type_hash<typeT>::create_runtime (const string_type& name, const rx_node_id& type_id, const data::runtime_values_data* init_data)
{
	return create_runtime(name, rx_node_id::null_id, type_id, false, init_data);
}

template <class typeT>
query_result type_hash<typeT>::get_derived_types (const rx_node_id& id) const
{
	query_result ret;
	std::vector<rx_node_id> temp;
	inheritance_hash_.get_derived_from(id, temp);
	for (auto one : temp)
	{
		auto type = registered_types_.find(one);

		if (type!=registered_types_.end())
		{
			ret.details.emplace_back(query_result_detail { one, type->second->meta_data().get_name() });
		}
	}
	return ret;
}

template <class typeT>
rx_result type_hash<typeT>::check_type (const rx_node_id& id, type_check_context& ctx) const
{
	auto temp = get_type_definition(id);
	if (temp)
	{
		return temp->check_type(ctx);
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< typeT::type_name
			<< " with node_id "
			<< id;
		ctx.add_error(ss.str());
		return false;
	}
}

template <class typeT>
typename type_hash<typeT>::RTypePtr type_hash<typeT>::get_runtime (const rx_node_id& id) const
{
	auto it = registered_objects_.find(id);
	if (it != registered_objects_.end())
	{
		return it->second;
	}
	else
	{
		return Tptr::null_ptr;
	}
}

template <class typeT>
rx_result type_hash<typeT>::delete_runtime (rx_node_id id)
{
	auto it = registered_objects_.find(id);
	if (it != registered_objects_.end())
	{
		auto type_id = it->second->meta_data().get_parent();
		rx_node_ids base;
		base.emplace_back(type_id);
		inheritance_hash_.get_base_types(type_id, base);
		registered_objects_.erase(it);
		instance_hash_.remove_from_hash_data(id, type_id, base);
		return true;
	}
	else
	{
		return "Object does not exists!";
	}
}

template <class typeT>
rx_result type_hash<typeT>::delete_type (rx_node_id id)
{
    auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		auto type_id = it->second->meta_data().get_parent();
		rx_node_ids base;
		base.emplace_back(type_id);
		inheritance_hash_.get_base_types(type_id, base);
		registered_types_.erase(it);
		inheritance_hash_.remove_from_hash_data(id, type_id);
		return true;
	}
	else
	{
		return false;
	}
}

/*
class type_safe_class
{
	static int32_t next_id()
	{
		static int32_t g_id(0);
		return g_id++;
	}
	template<class T, class Tbase>
	int32_t id_for()
	{
		static int result(next_id());
	}
};

private:
	template <typename T_>
	struct type_safe_object_type
	{
		virtual ~SomethingValueBase()
		{
		}
	};

	struct SomethingValueBase
	{
		std::string type_info_name;

		SomethingValueBase(const std::string & t) :
			type_info_name(t)
		{
		}
	};

	template <typename T_>
	struct SomethingValue :
		SomethingValueBase
	{
		T_ value;

		SomethingValue(const T_ & v) :
			SomethingValueBase(typeid(type_safe_object_type<T_>()).name()),
			value(v)
		{
		}
	};

	std::shared_ptr<SomethingValueBase> _value;

public:
	template <typename T_>
	type_safe_class(const T_ & t) :
		_value(new SomethingValue<T_>(t))
	{
	}

	template <typename T_>
	const T_ & as() const
	{
		if (typeid(type_safe_object_type<T_>()).name() != _value->type_info_name)
			throw SomethingIsSomethingElse();
		return std::static_pointer_cast<const SomethingValue<T_> >(_value)->value;
	}
};
*/
// Parameterized Class model::simple_type_hash 

template <class typeT>
simple_type_hash<typeT>::simple_type_hash()
{
	default_constructor_ = []()
	{
		return rx_create_reference<RType>();
	};
}



template <class typeT>
typename type_hash<typeT>::Tptr simple_type_hash<typeT>::get_type_definition (const rx_node_id& id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return it->second;
	}
	else
	{
		return Tptr::null_ptr;
	}
}

template <class typeT>
rx_result simple_type_hash<typeT>::register_type (typename type_hash<typeT>::Tptr what)
{
	const auto& id = what->meta_data().get_id();
	auto it = registered_types_.find(id);
	if (it == registered_types_.end())
	{
		registered_types_.emplace(what->meta_data().get_id(), what);
		inheritance_hash_.add_to_hash_data(id, what->meta_data().get_parent());
		return true;
	}
	else
	{
		return false;
	}
}

template <class typeT>
rx_result simple_type_hash<typeT>::register_constructor (const rx_node_id& id, std::function<RType()> f)
{
	constructors_.emplace(id, f);
	return true;
}

template <class typeT>
rx_result_with<typename simple_type_hash<typeT>::RDataType> simple_type_hash<typeT>::create_simple_runtime (const rx_node_id& type_id) const
{
	RTypePtr ret;

	rx_node_ids base;
	base.emplace_back(type_id);
	auto base_result = inheritance_hash_.get_base_types(type_id, base);
	if (!base_result)
		return base_result.errors();

	for (const auto& one : base)
	{
		auto it = constructors_.find(one);
		if (it != constructors_.end())
		{
			ret = (it->second)();
			break;
		}
	}
	if (!ret)
		ret = default_constructor_();

	construct_context ctx;
	for (auto one_id : base)
	{
		auto my_class = get_type_definition(one_id);
		if (my_class)
		{
			auto result = my_class->construct(ret, ctx);
			if (!result)
			{// error constructing object
				return result.errors();
			}
		}
	}

	return RDataType{ std::move(create_runtime_data(ctx.runtime_data)), std::move(ret) };
}

template <class typeT>
query_result simple_type_hash<typeT>::get_derived_types (const rx_node_id& id) const
{
	query_result ret;
	return ret;
}

template <class typeT>
rx_result simple_type_hash<typeT>::check_type (const rx_node_id& id, type_check_context& ctx) const
{
	auto temp = get_type_definition(id);
	if (temp)
	{
		return temp->check_type(ctx);
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< typeT::type_name
			<< " with node_id "
			<< id;
		ctx.add_error(ss.str());
		return false;
	}
}

template <class typeT>
rx_result simple_type_hash<typeT>::delete_type (rx_node_id id)
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		auto type_id = it->second->meta_data().get_parent();
		rx_node_ids base;
		base.emplace_back(type_id);
		inheritance_hash_.get_base_types(type_id, base);
		registered_types_.erase(it);
		inheritance_hash_.remove_from_hash_data(id, type_id);
		return true;
	}
	else
	{
		return false;
	}
}


} // namespace model



#endif