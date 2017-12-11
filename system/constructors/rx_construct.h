

/****************************************************************************
*
*  system\constructors\rx_construct.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef rx_construct_h
#define rx_construct_h 1



// rx_objbase
#include "system/meta/rx_objbase.h"

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







typedef object_constructor_base< objects::object_runtime_ptr , objects::user_object::smart_ptr  > user_object_constructor;


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
	RType ret = create_runtime(name, id);
	meta::object_class_ptr my_class = rx_gate::instance().get_manager().get_object_class(type_id);
	if (my_class)
	{
		for (auto& one : my_class->get_const_values())
		{
			rx_value temp;
			one->get_value(temp);
			ret->register_const_value(one->get_name(), temp);
		}
	}
	return ret;
}


} // namespace constructors
} // namespace rx_platform



#endif
