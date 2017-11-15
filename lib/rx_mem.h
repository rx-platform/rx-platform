

/****************************************************************************
*
*  lib\rx_mem.h
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


#ifndef rx_mem_h
#define rx_mem_h 1



// rx_ptr
#include "lib/rx_ptr.h"
// dummy
#include "dummy.h"

#include <type_traits>


namespace rx {

namespace memory {





template <class allocT>
class memory_buffer_base : public pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(memory_buffer_base<allocT>);

  public:
      memory_buffer_base();

      memory_buffer_base (const void* ptr, size_t size);

      virtual ~memory_buffer_base();


      void push_data (const void* ptr, size_t size);

      void read_data (void* ptr, size_t size);

      bool eof () const;

      void push_string (const string_type& str);

      void push_line (const string_type& str);

      void read_line (string_type& line);

      void reinit (bool clear_memory = false);

      size_t get_size () const;

      bool empty () const;

      void* get_data () const;

      bool fill_with_file_content (sys_handle_t file);

	  template<typename T>
	  void read_data(T& val)
	  {
		  from_buffer(val, std::is_trivially_copyable<T>());
	  }
	  template<typename T>
	  void push_data(const T& val)
	  {
		  to_buffer(val, std::is_trivially_copyable<T>());
	  }
	  template<typename T>
	  T* get_buffer()
	  {
		  return get_buffer_internal(tl::type2type<T>());
	  }
  protected:

      size_t _current_read;

      size_t _next_push;

      allocT _allocator;


  private:
	  template<typename T>
	  T* get_buffer_internal(tl::type2type<T>)
	  {
		  return _allocator.get_buffer<T>();
	  }
	  template<typename T>
	  void to_buffer(const T& val, std::true_type&)
	  {
		  push_data(&val, sizeof(T));
	  }

};






class std_vector_allocator 
{

  public:
      std_vector_allocator();

      ~std_vector_allocator();


      void allocate (size_t size);

      void deallocate ();

      void reallocate (size_t new_size);

      size_t get_buffer_size () const;

      char* get_char_buffer () const;


private:
	template<typename otherT>
	otherT* _internal_get_buffer(tl::type2type<otherT>) const
	{
		otherT* ret = (otherT*)&this->_buffer[0];
		return ret;
	}
public:
	  template<typename otherT>
	  otherT* get_buffer() const
	  {
		  return _internal_get_buffer(tl::type2type<otherT>());
	  }
  protected:

  private:


      byte_string _buffer;

      static ref_counting_type g_memory_consuption;


};







typedef memory_buffer_base< std_vector_allocator  > std_buffer;

typedef pointers::reference<memory_buffer_base< std_vector_allocator> > std_buffer_ptr;






template <class allocT>
class std_strbuff : public memory_buffer_base<allocT>, 
                    	pu
{
	DECLARE_REFERENCE_PTR(std_strbuff<allocT>);

  public:
      std_strbuff();

      virtual ~std_strbuff();


      char* pbase () const;

      char* pptr () const;

      char* epptr () const;

      char* eback () const;

      char* gptr () const;

      char* egptr () const;


  protected:
	  virtual int_type overflow( int_type ch = traits_type::eof())
	  {
		  if(traits_type::not_eof(ch))
		  {
			  traits_type::char_type temp = traits_type::to_char_type(ch);
			  this->push_data(&temp, sizeof(temp));
		  }
		  return ch;
	  }

  private:


};


typedef std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;





template <class allocT>
class backward_memory_buffer_base : public pointers::reference_object  
{

  public:
      backward_memory_buffer_base (size_t size);

      virtual ~backward_memory_buffer_base();


      void push_data (const void* ptr, size_t size);

      void push_string (const string_type& str);

      void push_line (const string_type& str);

      void reinit (bool clear_memory = false);

      size_t get_size () const;

      bool empty () const;

      void* get_data () const;

	  template<typename T>
	  void push_data(const T& val)
	  {
		  to_buffer(val, std::is_trivially_copyable<T>());
	  }
	  template<typename T>
	  T* get_buffer()
	  {
		  return get_buffer_internal(tl::type2type<T>());
	  }
  protected:

      size_t _current_read;

      size_t _next_push;

      allocT _allocator;

	  template<typename T>
	  T* get_buffer_internal(tl::type2type<T>)
	  {
		  return get_data();
	  }
	  template<typename T>
	  void from_buffer(T& val, std::true_type&)
	  {
		  read_data(&val, sizeof(T));
	  }
	  template<typename T>
	  void to_buffer(const T& val, std::true_type&)
	  {
		  push_data(&val, sizeof(T));
	  }
  private:


};






class backward_simple_allocator 
{

  public:
      backward_simple_allocator (size_t initial);

      virtual ~backward_simple_allocator();


      void allocate (size_t size);

      void deallocate ();

      void reallocate (size_t new_size);

      size_t get_buffer_size () const;

      char* get_char_buffer () const;


  protected:

  private:


      uint8_t* _buffer;

      static ref_counting_type g_memory_consuption;

      size_t _size;


};







typedef backward_memory_buffer_base< backward_simple_allocator  > back_buffer;


// Parameterized Class rx::memory::memory_buffer_base 

template <class allocT>
memory_buffer_base<allocT>::memory_buffer_base()
      : _current_read(0),
        _next_push(0)
{
	_allocator.allocate(0);// default allocation
}

template <class allocT>
memory_buffer_base<allocT>::memory_buffer_base (const void* ptr, size_t size)
      : _current_read(0),
        _next_push(0)
{
	_next_push = (int)size;
	_allocator.allocate(size);
	memcpy(_allocator.get_buffer<uint8_t>(), ptr, size);
}


template <class allocT>
memory_buffer_base<allocT>::~memory_buffer_base()
{
}



template <class allocT>
void memory_buffer_base<allocT>::push_data (const void* ptr, size_t size)
{
	while (_next_push + size>_allocator.get_buffer_size())
		_allocator.reallocate(_next_push + size);
	memcpy(&_allocator.get_buffer<uint8_t>()[_next_push], ptr, size);
	_next_push += size;
}

template <class allocT>
void memory_buffer_base<allocT>::read_data (void* ptr, size_t size)
{
	if (_current_read + size>_next_push)
		throw std::exception("buffer end of file!");
	memcpy(ptr, &_allocator.get_buffer<uint8_t>()[_current_read], size);
	_current_read += size;
}

template <class allocT>
bool memory_buffer_base<allocT>::eof () const
{
	RX_ASSERT(_current_read <= _next_push);// check first
	return _current_read >= _next_push;// just in case
}

template <class allocT>
void memory_buffer_base<allocT>::push_string (const string_type& str)
{
	push_data(str.c_str(), str.size());
}

template <class allocT>
void memory_buffer_base<allocT>::push_line (const string_type& str)
{
	push_data(str.c_str(), str.size());
	push_data("\r\n", 2);
}

template <class allocT>
void memory_buffer_base<allocT>::read_line (string_type& line)
{
	if (eof())
		return;
	char* buff = &_allocator.get_buffer<char>()[_current_read];
	char* start = buff;
	while (*buff!='\r' && *buff!='\n' && !eof())
		buff++;
	if (buff - start > 0)
	{
		_current_read += (buff - start);
		size_t len = (buff - start);
		while ((*buff == '\r' || *buff == '\n') && !eof())
		{
			buff++;
			_current_read++;
		}
		line.assign(start, len);
	}
}

template <class allocT>
void memory_buffer_base<allocT>::reinit (bool clear_memory)
{
	if (clear_memory)
		_allocator.deallocate();
	_current_read = 0;
	_next_push = 0;
}

template <class allocT>
size_t memory_buffer_base<allocT>::get_size () const
{
	return _next_push;
}

template <class allocT>
bool memory_buffer_base<allocT>::empty () const
{
	return _next_push == 0;
}

template <class allocT>
void* memory_buffer_base<allocT>::get_data () const
{
  return _allocator.get_char_buffer();
}

template <class allocT>
bool memory_buffer_base<allocT>::fill_with_file_content (sys_handle_t file)
{
	uint64_t sz = 0;
	if (rx_file_get_size(file, &sz))
	{
		if (sz == 0)
			return true;
		// init buffer
		_current_read = 0;
		_next_push = (int)sz;
		_allocator.reallocate((size_t)sz);
		uint32_t readed = 0;
		if (rx_file_read(file, _allocator.get_buffer<void>(), (uint32_t)sz, &readed) && readed == sz)
		{
			return true;
		}
	}
	return false;
}


// Parameterized Class rx::memory::std_strbuff 

template <class allocT>
std_strbuff<allocT>::std_strbuff()
{
}


template <class allocT>
std_strbuff<allocT>::~std_strbuff()
{
}



template <class allocT>
char* std_strbuff<allocT>::pbase () const
{
	return this->_allocator.get_char_buffer();
}

template <class allocT>
char* std_strbuff<allocT>::pptr () const
{
	char* ret= &this->_allocator.get_char_buffer()[this->_next_push];
	return ret;
}

template <class allocT>
char* std_strbuff<allocT>::epptr () const
{
	return &this->_allocator.get_char_buffer()[this->_allocator.get_size()];
}

template <class allocT>
char* std_strbuff<allocT>::eback () const
{
  return this->_allocator.get_char_buffer();
}

template <class allocT>
char* std_strbuff<allocT>::gptr () const
{
  char* ret= &this->_allocator.get_char_buffer()[this->_current_read];
  return ret;
}

template <class allocT>
char* std_strbuff<allocT>::egptr () const
{
  return nullptr;
}


// Parameterized Class rx::memory::backward_memory_buffer_base 

template <class allocT>
backward_memory_buffer_base<allocT>::backward_memory_buffer_base (size_t size)
      : _current_read(0),
        _next_push(0)
	, _allocator(size)
{
	_next_push = (int)size;
	_allocator.allocate(size);
}


template <class allocT>
backward_memory_buffer_base<allocT>::~backward_memory_buffer_base()
{
}



template <class allocT>
void backward_memory_buffer_base<allocT>::push_data (const void* ptr, size_t size)
{
	while (_next_push + size>_allocator.get_buffer_size())
		_allocator.reallocate(_next_push+size);
	memcpy(&_allocator.get_buffer<uint8_t>()[_allocator.get_buffer_size() - _next_push - size], ptr, size);
	_next_push += ((int)size);
}

template <class allocT>
void backward_memory_buffer_base<allocT>::push_string (const string_type& str)
{
	push_data(str.c_str(), str.size());
}

template <class allocT>
void backward_memory_buffer_base<allocT>::push_line (const string_type& str)
{
	push_data(str.c_str(), str.size());
	push_data("\r\n", 2);
}

template <class allocT>
void backward_memory_buffer_base<allocT>::reinit (bool clear_memory)
{
	if (clear_memory)
		_allocator.deallocate();
	_current_read = 0;
	_next_push = 0;
}

template <class allocT>
size_t backward_memory_buffer_base<allocT>::get_size () const
{
	return _next_push;
}

template <class allocT>
bool backward_memory_buffer_base<allocT>::empty () const
{
	return _next_push == 0;
}

template <class allocT>
void* backward_memory_buffer_base<allocT>::get_data () const
{
	return &_allocator.get_buffer<uint8_t>()[_allocator.size() - _next_push];
}


} // namespace memory
} // namespace rx



#endif
