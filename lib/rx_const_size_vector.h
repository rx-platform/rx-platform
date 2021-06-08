

/****************************************************************************
*
*  lib\rx_const_size_vector.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_const_size_vector_h
#define rx_const_size_vector_h 1





#ifdef _MSC_VER 
#ifdef _DEBUG
#undef new
#endif
#endif
namespace rx
{
template<class T, class Allocator = std::allocator<T> >
class const_size_vector : protected Allocator
{
	T* data_;
	std::size_t size_ = 0;
public:
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef T* iterator;
	typedef const T* const_iterator;

	// constructors
	explicit const_size_vector()
	{
		size_ = 0;
		data_ = nullptr;
	}
	explicit const_size_vector(size_t size, const T& val = T())
	{
		size_ = size;
		if (size_)
		{
			data_ = this->allocate(size_);
			for (size_t i = 0; i < size_; i++)
			{
				new(&data_[i++]) T(val);
			}
		}
		else
			data_ = nullptr;
	}
	explicit const_size_vector(const T* vals, size_t size)
	{
		size_ = size;
		if (size_)
		{
			data_ = this->allocate(size_);
			for (size_t i = 0; i < size_; i++)
			{
				new(&data_[i++]) T(vals[i]);
			}
		}
		else
			data_ = nullptr;
	}
	const_size_vector(std::initializer_list<T> init_list)
	{
		size_ = init_list.size();
		if (size_)
		{
			data_ = this->allocate(size_);
			size_t idx = 0;
			for (auto one : init_list)
			{
				new(&data_[idx++]) T(std::move(one));
			}
		}
		else
			data_ = nullptr;
	}

	explicit const_size_vector(std::vector<T>&& from)
	{
		size_ = from.size();
		if (size_)
		{
			data_ = this->allocate(size_);
			size_t idx = 0;
			for (auto& one : from)
			{
				new(&data_[idx++]) T(std::move(one));
			}
		}
		else
			data_ = nullptr;
	}
	explicit const_size_vector(const std::vector<T>& from)
	{
		size_ = from.size();
		if (size_)
		{
			data_ = this->allocate(size_);
			size_t idx = 0;
			for (const auto& one : from)
			{
				new(&data_[idx++]) T(one);
			}
		}
		else
			data_ = nullptr;
	}
	// move copy assign operators
	const_size_vector(const const_size_vector &right)
	{
		size_ = right.size_;
		if (size_)
		{
			data_ = this->allocate(size_);
			for (size_t i = 0; i < size_; i++)
			{
				new(&data_[i]) T(right.const_reference_at(i));
			}
		}
		else
			data_ = nullptr;
	}
	const_size_vector & operator=(const const_size_vector &right)
	{
		destroy_content();
		size_ = right.size_;
		if (size_)
		{
			data_ = this->allocate(size_);
			for (size_t i = 0; i < size_; i++)
			{
				new(&data_[i]) T(right.const_reference_at(i));
			}
		}
		else
			data_ = nullptr;

		return *this;
	}
	// use 'memmove' to move object around
	const_size_vector(const_size_vector &&right) noexcept
	{
		data_ = right.data_;
		size_ = right.size_;
		right.data_ = nullptr;
		right.size_ = 0;
	}
	const_size_vector & operator=(const_size_vector &&right) noexcept
	{
		destroy_content();

		data_ = right.data_;
		size_ = right.size_;
		right.data_ = nullptr;
		right.size_ = 0;

		return *this;
	}
	// Create an object in aligned storage
	template<typename ...Args>
	void emplace_at(size_t idx, Args&&... args)
	{
		//if (size_ >= N) // possible error handling
		//	throw std::bad_alloc{};

		// construct value in memory of aligned storage
		// using in place operator new
		(&data_[idx])->~T();
		new(&data_[idx]) T(std::forward<Args>(args)...);
	}

	// Access an object in aligned storage
	const T& operator[](std::size_t pos) const
	{
		return const_reference_at(pos);
	}
	T& operator[](std::size_t pos)
	{
		return reference_at(pos);
	}
	// Delete objects from aligned storage
	~const_size_vector()
	{
		destroy_content();
	}
	// iterators
	iterator begin() noexcept
	{
		return &data_[0];
	}
	const_iterator begin() const noexcept
	{
		return &data_[0];
	}
	const_iterator cbegin() const noexcept
	{
		return &data_[0];
	}
	iterator end() noexcept
	{
		return &data_[size_];
	}
	const_iterator end() const noexcept
	{
		return &data_[size_];
	}
	const_iterator cend() const noexcept
	{
		return &data_[size_];
	}
	size_t size() const
	{
		return size_;
	}
	bool empty() const
	{
		return size_ == 0;
	}
private:
	// helper functions, ugly stuff
	void destroy_content()
	{
		if (data_)
		{
			for (std::size_t i = 0; i < size_; ++i) {
				// note: needs std::launder as of C++17
				(&data_[i])->~T();
			}
			this->deallocate(data_, size_);
		}
	}
	T& reference_at(std::size_t pos)
	{
		// note: needs std::launder as of C++17
		return data_[pos];
	}
	const T& const_reference_at(std::size_t pos) const
	{
		// note: needs std::launder as of C++17
		return data_[pos];
	}
};


}



#ifdef _MSC_VER 
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif


#endif
