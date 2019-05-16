

/****************************************************************************
*
*  system\constructors\rx_construct.h
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


#ifndef rx_construct_h
#define rx_construct_h 1




#include "system/server/rx_server.h"


namespace rx_platform {

namespace constructors {





template <typename RType, typename DType>
class object_constructor_base 
{

  public:
      object_constructor_base();

      virtual ~object_constructor_base();


      RType create_object (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system = false);


  protected:

      virtual DType create_runtime (const string_type& name, const rx_node_id& id, bool system = false);


  private:
      object_constructor_base(const object_constructor_base< RType,DType > &right);

      object_constructor_base< RType,DType > & operator=(const object_constructor_base< RType,DType > &right);



};






template <typename RType, typename DType>
class item_constructor_base 
{

  public:
      item_constructor_base();

      virtual ~item_constructor_base();


      RType create_item (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system = false);


  protected:

      virtual DType create_runtime (bool system = false);


  private:
      item_constructor_base(const item_constructor_base< RType,DType > &right);

      item_constructor_base< RType,DType > & operator=(const item_constructor_base< RType,DType > &right);



};


// Parameterized Class rx_platform::constructors::object_constructor_base 

template <typename RType, typename DType>
object_constructor_base<RType,DType>::object_constructor_base()
{
}

template <typename RType, typename DType>
object_constructor_base<RType,DType>::object_constructor_base(const object_constructor_base<RType,DType> &right)
{
}


template <typename RType, typename DType>
object_constructor_base<RType,DType>::~object_constructor_base()
{
}


template <typename RType, typename DType>
object_constructor_base<RType,DType> & object_constructor_base<RType,DType>::operator=(const object_constructor_base<RType,DType> &right)
{
}



template <typename RType, typename DType>
DType object_constructor_base<RType,DType>::create_runtime (const string_type& name, const rx_node_id& id, bool system)
{
	return DType(name, id);
}

template <typename RType, typename DType>
RType object_constructor_base<RType,DType>::create_object (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system)
{
	RType ret = create_runtime(name, id, system);
	typename RType::pointee_type::definition_t::smart_ptr my_type =
		rx_gate::instance().get_manager().get_type<typename RType::pointee_type::definition_t>(type_id);
	if (my_type)
	{
		my_type->construct(ret);

	}
	return ret;
}


// Parameterized Class rx_platform::constructors::item_constructor_base 

template <typename RType, typename DType>
item_constructor_base<RType,DType>::item_constructor_base()
{
}

template <typename RType, typename DType>
item_constructor_base<RType,DType>::item_constructor_base(const item_constructor_base<RType,DType> &right)
{
}


template <typename RType, typename DType>
item_constructor_base<RType,DType>::~item_constructor_base()
{
}


template <typename RType, typename DType>
item_constructor_base<RType,DType> & item_constructor_base<RType,DType>::operator=(const item_constructor_base<RType,DType> &right)
{
}



template <typename RType, typename DType>
DType item_constructor_base<RType,DType>::create_runtime (bool system)
{
	return DType();
}

template <typename RType, typename DType>
RType item_constructor_base<RType,DType>::create_item (const string_type& name, const rx_node_id& id, const rx_node_id& type_id, bool system)
{
	RType ret = create_runtime();
	typename RType::pointee_type::definition_t::smart_ptr my_type =
		rx_gate::instance().get_manager().get_type<typename RType::pointee_type::definition_t>(type_id);
	if (my_type)
	{
		my_type->construct(ret);

	}
	return ret;
}


} // namespace constructors
} // namespace rx_platform



#endif
