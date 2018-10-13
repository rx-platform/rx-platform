

/****************************************************************************
*
*  classes\rx_meta.h
*
*  Copyright (c) 2018 Dusan Ciric
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


#ifndef rx_meta_h
#define rx_meta_h 1



// rx_construct
#include "system/constructors/rx_construct.h"
// rx_thread
#include "lib/rx_thread.h"

#include "system/meta/rx_classes.h"
#include "system/meta/rx_obj_classes.h"
#include "system/meta/rx_objbase.h"
using namespace rx_platform::meta;
using namespace rx_platform::meta::basic_defs;
using namespace rx_platform::meta::object_defs;


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

typedef TYPELIST_9(object_class, variable_class, source_class, event_class, filter_class, mapper_class, application_class, domain_class, struct_class) regular_rx_types;
typedef TYPELIST_10(reference_type, object_class, variable_class, source_class, event_class, filter_class, mapper_class, application_class, domain_class, struct_class) full_rx_types;








class relations_hash_data 
{
	relations_hash_data(const relations_hash_data&) = delete;
	relations_hash_data(relations_hash_data&&) = delete;
	void operator=(const relations_hash_data&) = delete;
	void operator=(relations_hash_data&&) = delete;

	// these are mostly static data, so we keep it ordered to find quickly
	typedef std::set<rx_node_id> relation_elements_type;
	// this here is pointer type so we don't have copying of whole set just pointer
	typedef std::map<rx_node_id, std::unique_ptr<relation_elements_type> > relation_map_type;


  public:
      relations_hash_data();

      virtual ~relations_hash_data();


      void add_to_hash_data (const rx_node_id& new_id, const rx_node_id& first_backward);

      void remove_from_hash_data (const rx_node_id& id);

      void change_hash_data (const rx_node_id& id, const rx_node_id& first_backward_old, const rx_node_id& first_backward_new);

      bool is_backward_from (const rx_node_id& id, const rx_node_id& parent);

      void get_full_forward (const rx_node_id& id, std::vector< rx_node_id>& result);

      void get_full_backward (const rx_node_id& id, std::vector< rx_node_id>& result);


  protected:

  private:


      relation_map_type forward_hash_;

      relation_map_type backward_hash_;

      relation_map_type first_forward_hash_;

      relation_map_type first_backward_hash_;


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
	typedef typename constructors::object_constructor_base<RType,RType> constructorType;

	typedef typename std::map<rx_node_id, RType> registered_objects_type;
	typedef typename std::map<rx_node_id, Tptr> registered_classes_type;
	typedef typename std::map<rx_node_id, std::function<RTypePtr()> > object_constructors_type;


  public:
      type_hash();

      virtual ~type_hash();


      typename type_hash<typeT>::Tptr get_class_definition (const rx_node_id& id);

      bool register_class (typename type_hash<typeT>::Tptr what);

      bool register_constructor (const rx_node_id& id, std::function<RType()> f);

      typename type_hash<typeT>::RTypePtr create_runtime (const string_type& name, rx_node_id&& id, rx_node_id&& type_id, bool system = false);

      typename type_hash<typeT>::RTypePtr create_runtime (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system = false);


  protected:

  private:


      object_constructors_type object_constructors_;

      relations_hash_data hash_;


      registered_objects_type registered_objects_;

      registered_classes_type registered_classes_;


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





class internal_classes_manager 
{
	typedef std::map<rx_node_id, ids_hash_element> ids_hash_type;
	typedef std::map<string_type, names_hash_element> names_hash_type;

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
		: public tl::gen_scatter_hierarchy<regular_rx_types, type_cache_holder>
	{
	public:
		template<class T>
		type_hash<T>& get_internal(internal_classes_manager* manager, tl::type2type<T>)
		{
			type_hash<T>* ret = (static_cast<type_cache_holder<T>&>(*this)).value_;
			if (ret == nullptr)
			{
				ret = new type_hash<T>();
				//ret->set_manager(manager);
				//manager->_cache_types.insert(cache_types_type::value_type(T::type_id, ret));
				(static_cast<type_cache_holder<T>&>(*this)).value_ = ret;
			}
			return *ret;
		}
	};
	type_cache_list_container _types_container;
	/*typedef std::map<uint32_t, inheritance_cache_base*> cache_types_type;
	typedef std::map<uint32_t, inheritance_cache_base*>::iterator cache_types_iterator;
	typedef std::map<uint32_t, inheritance_cache_base*>::const_iterator const_cache_types_iterator;
	cache_types_type _cache_types;*/

	friend class type_cache_list_container;

public:
	template<class T>
	type_hash<T>& get_type_cache()
	{
		return _types_container.get_internal<T>(this, tl::type2type<T>());
	}

  public:
      virtual ~internal_classes_manager();


      static internal_classes_manager& instance ();

      uint32_t initialize (hosting::rx_platform_host* host, meta_data_t& data);

      uint32_t deinitialize ();

      uint32_t start (hosting::rx_platform_host* host, const meta_data_t& data);

      uint32_t stop ();

      platform_item_ptr create_type (base_meta_reader& stream);

      platform_item_ptr create_object (base_meta_reader& stream);

	  template<class T>
	  struct get_arg_data
	  {
		  T data;
	  };
	  //template<class T>
	  //void get_class(std::function<void(T))
	  //{
		 // //worker_.append();
	  //}
  protected:

  private:
      internal_classes_manager();



      rx::threads::physical_job_thread worker_;


};


// Parameterized Class model::type_hash 

template <class typeT>
type_hash<typeT>::type_hash()
{
}


template <class typeT>
type_hash<typeT>::~type_hash()
{
}



template <class typeT>
typename type_hash<typeT>::Tptr type_hash<typeT>::get_class_definition (const rx_node_id& id)
{
	auto it = registered_classes_.find(id);
	if (it != registered_classes_.end())
	{
		return it->second;
	}
	else
	{
		return Tptr::null_ptr;
	}
}

template <class typeT>
bool type_hash<typeT>::register_class (typename type_hash<typeT>::Tptr what)
{
	auto it = registered_classes_.find(what->meta_data().get_id());
	if (it == registered_classes_.end())
	{
		registered_classes_.emplace(what->meta_data().get_id(), what);
		return true;
	}
	else
	{
		return false;
	}
}

template <class typeT>
bool type_hash<typeT>::register_constructor (const rx_node_id& id, std::function<RType()> f)
{
	object_constructors_.emplace(id, f);
	return true;
}

template <class typeT>
typename type_hash<typeT>::RTypePtr type_hash<typeT>::create_runtime (const string_type& name, rx_node_id&& id, rx_node_id&& type_id, bool system)
{
	rx_node_id to_create;
	if (id.is_null())
		to_create = rx_node_id::generate_new(RX_USER_NAMESPACE);
	else
		to_create = std::move(id);

	auto ret = typeT::create_runtime_ptr();

	std::vector<rx_node_id> base;
	base.emplace_back(type_id);
	hash_.get_full_backward(type_id,base);
	for(const auto& one : base)
	{

		auto it = object_constructors_.find(one);
		if (it != object_constructors_.end())
		{
			ret = (it->second)();
			break;
		}

	}
	auto my_class = rx_gate::instance().get_manager().get_class<typename RType::definition_t>(type_id);
	if (my_class)
	{
		ret->meta_data().construct(name, to_create, std::move(type_id), system);
		my_class->construct(ret);

	}
	return ret;
}

template <class typeT>
typename type_hash<typeT>::RTypePtr type_hash<typeT>::create_runtime (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system)
{
	rx_node_id to_create(rx_uuid::create_new().uuid());
	if (id.is_null())
		to_create = rx_node_id::generate_new(RX_USER_NAMESPACE);
	else
		to_create = id;

	auto ret = typeT::create_runtime_ptr();

	std::vector<rx_node_id> base;
	base.emplace_back(type_id);
	hash_.get_full_backward(type_id, base);
	for (const auto& one : base)
	{

		auto it = object_constructors_.find(one);
		if (it != object_constructors_.end())
		{
			ret = (it->second)();
			break;
		}

	}
	auto my_class = rx_gate::instance().get_manager().get_class<typename RType::definition_t>(type_id);
	if (my_class)
	{
		ret->meta_data().construct(name, to_create, type_id, system);
		my_class->construct(ret);

	}
	return ret;
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
} // namespace model



#endif
