

/****************************************************************************
*
*  lib\rx_mem.h
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


#ifndef rx_mem_h
#define rx_mem_h 1



// dummy
#include "dummy.h"
// rx_ptr
#include "lib/rx_ptr.h"

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

      size_t current_read_;

      size_t next_push_;

      allocT allocator_;


  private:
	  template<typename T>
	  T* get_buffer_internal(tl::type2type<T>)
	  {
		  return allocator_.get_buffer<T>();
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
		otherT* ret = (otherT*)&this->buffer_[0];
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


      byte_string buffer_;

      static ref_counting_type g_memory_consuption;


};







typedef memory_buffer_base< std_vector_allocator  > std_buffer;

typedef pointers::reference<memory_buffer_base< std_vector_allocator> > std_buffer_ptr;






template <class allocT>
class std_strbuff : public memory_buffer_base<allocT>, 
                    	public std::streambuf  
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


typedef rx_reference<std_strbuff<memory::std_vector_allocator> > buffer_ptr;





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

      size_t current_read_;

      size_t next_push_;

      allocT allocator_;

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


      uint8_t* buffer_;

      static ref_counting_type g_memory_consuption;

      size_t size_;


};







typedef backward_memory_buffer_base< backward_simple_allocator  > back_buffer;


// Parameterized Class rx::memory::memory_buffer_base 

template <class allocT>
memory_buffer_base<allocT>::memory_buffer_base()
      : current_read_(0),
        next_push_(0)
{
	allocator_.allocate(0);// default allocation
}

template <class allocT>
memory_buffer_base<allocT>::memory_buffer_base (const void* ptr, size_t size)
      : current_read_(0),
        next_push_(0)
{
	next_push_ = (int)size;
	allocator_.allocate(size);
	memcpy(allocator_.get_buffer<uint8_t>(), ptr, size);
}


template <class allocT>
memory_buffer_base<allocT>::~memory_buffer_base()
{
}



template <class allocT>
void memory_buffer_base<allocT>::push_data (const void* ptr, size_t size)
{
	while (next_push_ + size>allocator_.get_buffer_size())
		allocator_.reallocate(next_push_ + size);
	memcpy(&allocator_.get_buffer<uint8_t>()[next_push_], ptr, size);
	next_push_ += size;
}

template <class allocT>
void memory_buffer_base<allocT>::read_data (void* ptr, size_t size)
{
	if (current_read_ + size>next_push_)
		throw std::exception("buffer end of file!");
	memcpy(ptr, &allocator_.get_buffer<uint8_t>()[current_read_], size);
	current_read_ += size;
}

template <class allocT>
bool memory_buffer_base<allocT>::eof () const
{
	RX_ASSERT(current_read_ <= next_push_);// check first
	return current_read_ >= next_push_;// just in case
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
	char* buff = &allocator_.get_buffer<char>()[current_read_];
	char* start = buff;
	while (*buff!='\r' && *buff!='\n' && !eof())
		buff++;
	if (buff - start > 0)
	{
		current_read_ += (buff - start);
		size_t len = (buff - start);
		while ((*buff == '\r' || *buff == '\n') && !eof())
		{
			buff++;
			current_read_++;
		}
		line.assign(start, len);
	}
}

template <class allocT>
void memory_buffer_base<allocT>::reinit (bool clear_memory)
{
	if (clear_memory)
		allocator_.deallocate();
	current_read_ = 0;
	next_push_ = 0;
}

template <class allocT>
size_t memory_buffer_base<allocT>::get_size () const
{
	return next_push_;
}

template <class allocT>
bool memory_buffer_base<allocT>::empty () const
{
	return next_push_ == 0;
}

template <class allocT>
void* memory_buffer_base<allocT>::get_data () const
{
  return allocator_.get_char_buffer();
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
		current_read_ = 0;
		next_push_ = (int)sz;
		allocator_.reallocate((size_t)sz);
		uint32_t readed = 0;
		if (rx_file_read(file, allocator_.get_buffer<void>(), (uint32_t)sz, &readed) && readed == sz)
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
	return this->allocator_.get_char_buffer();
}

template <class allocT>
char* std_strbuff<allocT>::pptr () const
{
	char* ret= &this->allocator_.get_char_buffer()[this->next_push_];
	return ret;
}

template <class allocT>
char* std_strbuff<allocT>::epptr () const
{
	return &this->allocator_.get_char_buffer()[this->allocator_.get_size()];
}

template <class allocT>
char* std_strbuff<allocT>::eback () const
{
  return this->allocator_.get_char_buffer();
}

template <class allocT>
char* std_strbuff<allocT>::gptr () const
{
  char* ret= &this->allocator_.get_char_buffer()[this->current_read_];
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
      : current_read_(0),
        next_push_(0)
	, allocator_(size)
{
	next_push_ = (int)size;
	allocator_.allocate(size);
}


template <class allocT>
backward_memory_buffer_base<allocT>::~backward_memory_buffer_base()
{
}



template <class allocT>
void backward_memory_buffer_base<allocT>::push_data (const void* ptr, size_t size)
{
	while (next_push_ + size>allocator_.get_buffer_size())
		allocator_.reallocate(next_push_+size);
	memcpy(&allocator_.get_buffer<uint8_t>()[allocator_.get_buffer_size() - next_push_ - size], ptr, size);
	next_push_ += ((int)size);
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
		allocator_.deallocate();
	current_read_ = 0;
	next_push_ = 0;
}

template <class allocT>
size_t backward_memory_buffer_base<allocT>::get_size () const
{
	return next_push_;
}

template <class allocT>
bool backward_memory_buffer_base<allocT>::empty () const
{
	return next_push_ == 0;
}

template <class allocT>
void* backward_memory_buffer_base<allocT>::get_data () const
{
	return &allocator_.get_buffer<uint8_t>()[allocator_.size() - next_push_];
}


} // namespace memory
} // namespace rx



#endif
