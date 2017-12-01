

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



namespace rx_platform {

namespace constructors {





template <typename RType, typename DType>
class object_constructor_base 
{

  public:
      object_constructor_base();

      virtual ~object_constructor_base();


  protected:

      virtual DType create_runtime ();


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
DType object_constructor_base<RType,DType>::create_runtime ()
{
	return DType(pointers::_create_new);
}


} // namespace constructors
} // namespace rx_platform



#endif
