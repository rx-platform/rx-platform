

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

      size_t m_current_read;

      size_t m_next_push;

      allocT m_allocator;


  private:
	  template<typename T>
	  T* get_buffer_internal(tl::type2type<T>)
	  {
		  return m_allocator.get_buffer<T>();
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
		otherT* ret = (otherT*)&this->m_buffer[0];
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


      byte_string m_buffer;

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


  protected:
	  virtual int_type overflow( int_type _Meta = traits_type::eof())
	  {
		  traits_type::char_type temp = traits_type::to_char_type(_Meta);
		  this->push_data(&temp, sizeof(temp));
		  return traits_type::not_eof(_Meta);
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

      size_t m_current_read;

      size_t m_next_push;

      allocT m_allocator;

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


      byte* m_buffer;

      static ref_counting_type g_memory_consuption;

      size_t m_size;


};







typedef backward_memory_buffer_base< backward_simple_allocator  > back_buffer;


// Parameterized Class rx::memory::memory_buffer_base 

template <class allocT>
memory_buffer_base<allocT>::memory_buffer_base()
      : m_current_read(0),
        m_next_push(0)
{
	m_allocator.allocate(0);// default allocation
}

template <class allocT>
memory_buffer_base<allocT>::memory_buffer_base (const void* ptr, size_t size)
      : m_current_read(0),
        m_next_push(0)
{
	m_next_push = (int)size;
	m_allocator.allocate(size);
	memcpy(m_allocator.get_buffer<byte>(), ptr, size);
}


template <class allocT>
memory_buffer_base<allocT>::~memory_buffer_base()
{
}



template <class allocT>
void memory_buffer_base<allocT>::push_data (const void* ptr, size_t size)
{
	while (m_next_push + size>m_allocator.get_buffer_size())
		m_allocator.reallocate(m_next_push + size);
	memcpy(&m_allocator.get_buffer<byte>()[m_next_push], ptr, size);
	m_next_push += size;
}

template <class allocT>
void memory_buffer_base<allocT>::read_data (void* ptr, size_t size)
{
	if (m_current_read + size>m_next_push)
		throw std::exception("buffer end of file!");
	memcpy(ptr, &m_allocator.get_buffer<byte>()[m_current_read], size);
	m_current_read += size;
}

template <class allocT>
bool memory_buffer_base<allocT>::eof () const
{
	RX_ASSERT(m_current_read <= m_next_push);// check first
	return m_current_read >= m_next_push;// just in case
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
	char* buff = m_allocator.get_buffer<char>();
	char* start = buff;
	while (*buff!='\r' && *buff!='\n' && !eof())
		buff++;
	if (buff - start > 0)
	{
		m_current_read += (buff - start);
		if (*buff == '\r' || *buff == '\n')
			m_current_read++;
		line.assign(start, (buff - start));
	}
}

template <class allocT>
void memory_buffer_base<allocT>::reinit (bool clear_memory)
{
	if (clear_memory)
		m_allocator.deallocate();
	m_current_read = 0;
	m_next_push = 0;
}

template <class allocT>
size_t memory_buffer_base<allocT>::get_size () const
{
	return m_next_push;
}

template <class allocT>
bool memory_buffer_base<allocT>::empty () const
{
	return m_next_push == 0;
}

template <class allocT>
void* memory_buffer_base<allocT>::get_data () const
{
  return m_allocator.get_char_buffer();
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
	return this->m_allocator.get_char_buffer();
}

template <class allocT>
char* std_strbuff<allocT>::pptr () const
{
	char* ret= &this->m_allocator.get_char_buffer()[this->m_next_push];
	return ret;
}

template <class allocT>
char* std_strbuff<allocT>::epptr () const
{
	return &this->m_allocator.get_char_buffer()[this->m_allocator.get_size()];
}


// Parameterized Class rx::memory::backward_memory_buffer_base 

template <class allocT>
backward_memory_buffer_base<allocT>::backward_memory_buffer_base (size_t size)
      : m_current_read(0),
        m_next_push(0)
	, m_allocator(size)
{
	m_next_push = (int)size;
	m_allocator.allocate(size);
}


template <class allocT>
backward_memory_buffer_base<allocT>::~backward_memory_buffer_base()
{
}



template <class allocT>
void backward_memory_buffer_base<allocT>::push_data (const void* ptr, size_t size)
{
	while (m_next_push + size>m_allocator.get_buffer_size())
		m_allocator.reallocate(m_next_push+size);
	memcpy(&m_allocator.get_buffer<byte>()[m_allocator.get_buffer_size() - m_next_push - size], ptr, size);
	m_next_push += ((int)size);
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
		m_allocator.deallocate();
	m_current_read = 0;
	m_next_push = 0;
}

template <class allocT>
size_t backward_memory_buffer_base<allocT>::get_size () const
{
	return m_next_push;
}

template <class allocT>
bool backward_memory_buffer_base<allocT>::empty () const
{
	return m_next_push == 0;
}

template <class allocT>
void* backward_memory_buffer_base<allocT>::get_data () const
{
	return &m_allocator.get_buffer<byte>()[m_allocator.size() - m_next_push];
}


} // namespace memory
} // namespace rx



#endif
