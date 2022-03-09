

/****************************************************************************
*
*  model\rx_construction_list.h
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


#ifndef rx_construction_list_h
#define rx_construction_list_h 1






namespace rx_internal {

namespace model {

namespace construction {

int do_types_testing();

template <size_t Lower, size_t Upper>
struct for_bounds {
	static constexpr const size_t lower = Lower;
	static constexpr const size_t upper = Upper;
};

template <size_t Index, size_t Upper>
struct for_symmetric {};


namespace for_constexpr_detail {
template <size_t lower, size_t... Is, class F>
constexpr void for_constexpr_impl(F&& f,
	std::index_sequence<Is...> /*meta*/) {
	(void)std::initializer_list<char>{
		((void)f(std::integral_constant<size_t, Is + lower>{}),
			'0')...};
}
}  // namespace for_constexpr_detail

template <class Bounds0, class F>
constexpr void for_constexpr(F&& f) {
	for_constexpr_detail::for_constexpr_impl<Bounds0::lower>(
		std::forward<F>(f),
		std::make_index_sequence<Bounds0::upper - Bounds0::lower>{});
}

constexpr size_t type_index_none = (size_t)-1;

template <class typeT, class tupleT>
constexpr size_t get_index_of_helper()
{
	size_t ret = type_index_none;
	for_constexpr< for_bounds<0, std::tuple_size<tupleT>::value> >([&ret](auto i) constexpr
		{
			if (std::is_same <typeT, typename std::tuple_element<i, tupleT>::type::type>::value)
				ret = i;
		});
	return ret;
}


template<class typeT>
struct construction_list_adapter
{
	construction_list_adapter()
	{
	}
	typedef typeT type;
	typedef typename typeT::smart_ptr ptr_type;
	typedef std::map<rx_node_id, ptr_type> list_type;
	list_type my_list;
};

template <class prevType, class... Args>
struct construction_list_types_collector
{
	typedef decltype(std::tuple_cat(typename prevType::resulting_type(), std::tuple<construction_list_adapter<Args>...>())) resulting_type;
};

template<>
struct construction_list_types_collector<void>
{
	typedef std::tuple<> resulting_type;
};

template <class typeT, class tupleT>
class construction_list
{
public:
	tupleT tuple_item;
	template<class cType>
	static constexpr size_t get_rx_type_index()
	{
		return get_index_of_helper<cType, tupleT>();
	}
	template<class cType, size_t idx>
	typename cType::smart_ptr construct()
	{
		return rx_create_reference<typename std::tuple_element<idx, tupleT>::type::type>();
	}
	template<class cType>
	void register_object(const rx_node_id id, typename cType::smart_ptr what)
	{
		auto& item = std::get<construction_list_adapter<cType> >(tuple_item);
		item.my_list.emplace(id, what);
	}
	template<class cType>
	typename cType::smart_ptr get_object(const rx_node_id id) const
	{
		auto& item = std::get<construction_list_adapter<cType> >(tuple_item);
		auto it = item.my_list.find(id);
		if (it != item.my_list.end())
			return it->second;
		else
			return cType::smart_ptr();
	}

};





template <class typeT, class... Args>
class construction_list_dummy 
{

  public:

  protected:

  private:


};



class base_class : public rx::pointers::reference_object
{
	DECLARE_REFERENCE_PTR(base_class);

};

class my_type1 : public base_class
{
	DECLARE_REFERENCE_PTR(my_type1);
};


class my_type2 : public base_class
{
	DECLARE_REFERENCE_PTR(my_type2);
};


class runtimes_constructor
{
public:
	virtual base_class::smart_ptr construct_object(size_t idx) = 0;
};

template <class tupleT>
class runtimes_constructor_implementation
{
	typedef construction_list<base_class,tupleT> object_list_t;
	object_list_t items;
public:
	template<class cType>
	rx_result_with<base_class::smart_ptr> construct_object(rx_node_id id)
	{
		constexpr size_t idx = construction_list<base_class, tupleT>::template get_rx_type_index<cType>();
		auto ret = this->items.template construct<cType, idx>();
		if (ret)
			this->items.template register_object<cType>(id, ret);
		return rx_result_with<base_class::smart_ptr>(ret);
	}
	template<class cType>
	typename cType::smart_ptr get_object(const rx_node_id& id) const
	{
		return this->items.template get_object<cType>(id);
	}
};





} // namespace construction
} // namespace model
} // namespace rx_internal



#endif
