

/****************************************************************************
*
*  system\runtime\rx_rt_array_wrapper_def.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_rt_array_wrapper_def_h
#define rx_rt_array_wrapper_def_h 1




namespace rx_platform
{
namespace runtime
{
namespace structure
{


#ifdef _MSC_VER
#ifdef _DEBUG
#undef new
#endif
#endif

template <class typeT>
void array_wrapper<typeT>::array_wrapper_union_t::destroy() noexcept
{
	if (size < 0)
	{
		simple_item.~typeT();
	}
	else
	{
		if (size)
		{
			delete[] array_item;
		}
		else
		{
			RX_ASSERT(array_item == nullptr);
		}
	}
	size = -1;
}
template <class typeT>
array_wrapper<typeT>::array_wrapper_union_t::array_wrapper_union_t()
{
	size = -1;
	new(&simple_item)typeT();
}
template <class typeT>
void array_wrapper<typeT>::array_wrapper_union_t::declare_null_array(typeT&& temp)
{
	this->destroy();
	size = empty_prototype_size;
	array_item = new typeT[1];
	array_item[0] = std::move(temp);
}
template <class typeT>
array_wrapper<typeT>::array_wrapper_union_t::array_wrapper_union_t(array_wrapper_union_t&& right) noexcept
{
	size = right.size;
	right.size = 0;
	if (size < 0)
	{
		new(&simple_item)typeT(std::move(right.simple_item));
		right.array_item = nullptr;
	}
	else
	{
		array_item = right.array_item;
		right.array_item = nullptr;
	}
}

template <class typeT>
array_wrapper<typeT>::array_wrapper_union_t::array_wrapper_union_t(const array_wrapper_union_t& right)
{
	size = right.size;
	if (size < 0)
	{
		new(&simple_item)typeT(right.simple_item);
	}
	else if (size == empty_prototype_size)
	{
		array_item = new typeT[1];
		array_item[0] = right.array_item[0];
	}
	else if (size > 0)
	{
		array_item = new typeT[size];
		for (int i = 0; i < size; i++)
		{
			array_item[i] = right.array_item[i];
		}
	}
	else
	{
		array_item = 0;
	}
}
template <class typeT>
array_wrapper<typeT>::array_wrapper_union_t::array_wrapper_union_t(typeT&& right) noexcept
{
	size = -1;
	new(&simple_item)typeT(std::move(right));
}
template <class typeT>
typename array_wrapper<typeT>::array_wrapper_union_t& array_wrapper<typeT>::array_wrapper_union_t::operator=(typeT&& right) noexcept
{
	this->destroy();
	size = -1;
	new(&simple_item)typeT(std::move(right));
	return *this;
}
template <class typeT>
array_wrapper<typeT>::array_wrapper_union_t::array_wrapper_union_t(std::initializer_list<typeT> init_list)
{
	if constexpr (std::is_copy_constructible<typeT>())
	{
		size = (int)init_list.size();
		if (size)
		{
			array_item = new typeT[size];
			size_t idx = 0;
			for (auto& one : init_list)
			{
				array_item[idx++] = std::move(one);
			}
		}
		else
			array_item = nullptr;
	}
	else
	{
		size = 0;
		array_item = nullptr;
		RX_ASSERT(false);
	}
}
template <class typeT>
typename array_wrapper<typeT>::array_wrapper_union_t& array_wrapper<typeT>::array_wrapper_union_t::operator=(std::vector<typeT>&& right)
{
	this->destroy();
	size = (int)right.size();
	if (size > 0)
	{
		array_item = new typeT[size];
		size_t idx = 0;
		for (auto& one : right)
		{
			array_item[idx++] = std::move(one);
		}
	}
	else
		array_item = nullptr;
	return *this;
}
template <class typeT>
array_wrapper<typeT>::array_wrapper_union_t::array_wrapper_union_t(std::vector<typeT>&& from)
{
	size = (int)from.size();
	if (size > 0)
	{
		array_item = new typeT[size];
		size_t idx = 0;
		for (auto& one : from)
		{
			array_item[idx++] = std::move(one);
		}
	}
	else
		array_item = nullptr;
}
template <class typeT>
array_wrapper<typeT>::array_wrapper_union_t::~array_wrapper_union_t()
{
	this->destroy();
}



#ifndef RX_USE_OWN_HEAP
#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif
#endif //RX_USE_OWN_HEAP

template <class typeT>
array_wrapper<typeT>::array_wrapper()
{
}
template <class typeT>
array_wrapper<typeT>::array_wrapper(const array_wrapper& right) noexcept
	: data_(right.data_)
{
}
template <class typeT>
array_wrapper<typeT>::array_wrapper(array_wrapper&& right) noexcept
	: data_(std::move(right.data_))
{
}
template <class typeT>
array_wrapper<typeT>::array_wrapper(typeT&& right) noexcept
	: data_(std::move(right))
{
}
template <class typeT>
array_wrapper<typeT>& array_wrapper<typeT>::operator=(typeT&& right) noexcept
{
	data_ = std::move(right);
	return *this;
}
template <class typeT>
array_wrapper<typeT>& array_wrapper<typeT>::operator=(std::vector<typeT>&& right)
{
	data_ = std::move(right);
	return *this;
}
template <class typeT>
array_wrapper<typeT>::array_wrapper(std::vector<typeT>&& from)
	: data_(std::move(from))
{
}
template <class typeT>
void array_wrapper<typeT>::declare_null_array(typeT&& temp)
{
	data_.declare_null_array(std::move(temp));
}
template <class typeT>
bool array_wrapper<typeT>::is_array() const
{
	return data_.size >= 0;
}
template <class typeT>
const typeT* array_wrapper<typeT>::get_item(int array_idx) const
{
	if (array_idx < 0 && data_.size < 0)
	{
		return &data_.simple_item;
	}
	else if (data_.size > 0
		&& data_.size != array_wrapper_union_t::empty_prototype_size
		&& array_idx >= 0
		&& array_idx < data_.size)
	{
		return &data_.array_item[array_idx];
	}
	else
	{
		RX_ASSERT(false);
		return nullptr;
	}
}
template <class typeT>
typeT* array_wrapper<typeT>::get_item(int array_idx)
{
	if (array_idx < 0 && data_.size < 0)
	{
		return &data_.simple_item;
	}
	else if (data_.size > 0
		&& data_.size != array_wrapper_union_t::empty_prototype_size
		&& array_idx >= 0
		&& array_idx < data_.size)
	{
		return &data_.array_item[array_idx];
	}
	else
	{
		RX_ASSERT(false);
		return nullptr;
	}
}
template <class typeT>
const typeT* array_wrapper<typeT>::get_item() const
{
	return get_item(-1);
}
template <class typeT>
typeT* array_wrapper<typeT>::get_item()
{
	return get_item(-1);
}
template <class typeT>
typeT* array_wrapper<typeT>::get_prototype()
{
	if (data_.size < 0)
	{
		return &data_.simple_item;
	}
	else if (data_.array_item)
	{
		if (data_.size == array_wrapper_union_t::empty_prototype_size)
			return &data_.array_item[0];
		else if (data_.size > 0)
			return &data_.array_item[0];
	}
	return nullptr;
}
template <class typeT>
const typeT* array_wrapper<typeT>::get_prototype() const
{
	if (data_.size < 0)
	{
		return &data_.simple_item;
	}
	else if (data_.array_item)
	{
		if (data_.size == array_wrapper_union_t::empty_prototype_size)
			return &data_.array_item[0];
		else if (data_.size > 0)
			return &data_.array_item[0];
	}
	return nullptr;
}
template <class typeT>
int array_wrapper<typeT>::get_size() const
{
	return data_.size == array_wrapper_union_t::empty_prototype_size ? 0 : data_.size;
}


}// namespace structure
}// namespace runtime
}// namespace rx_platform




#endif
