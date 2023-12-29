

/****************************************************************************
*
*  lib\rx_mem.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_mem_h
#define rx_mem_h 1



// dummy
#include "dummy.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include <type_traits>


namespace rx {

namespace memory {
//	Verry important
enum byte_order_type
{
	same_byte_order = 0,
	reversed_byte_order = 1,
	little_edian = 2,
	intel_byte_order = 2,
	big_endian = 3,
	network_byte_order = 3
};

byte_order_type get_platform_byte_order();


template<typename T>
T rx_byte_swap(T val);


template<>
int8_t rx_byte_swap(int8_t val);
template<>
uint8_t rx_byte_swap(uint8_t val);
template<>
int16_t rx_byte_swap(int16_t val);
template<>
uint16_t rx_byte_swap(uint16_t val);
template<>
int32_t rx_byte_swap(int32_t val);
template<>
uint32_t rx_byte_swap(uint32_t val);
template<>
int64_t rx_byte_swap(int64_t val);
template<>
uint64_t rx_byte_swap(uint64_t val);
template<>
float rx_byte_swap(float val);
template<>
double rx_byte_swap(double val);






template <class allocT, bool swap_bytes = false>
class memory_buffer_base : public pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(memory_buffer_base<allocT>);

  public:
      memory_buffer_base();

      memory_buffer_base (const void* ptr, size_t size);

      memory_buffer_base (memory_buffer_base&& right, size_t size);

      ~memory_buffer_base();


      bool push_data (const void* ptr, size_t size);

      bool read_data (void* ptr, size_t size);

      bool eof () const;

      bool push_string (const string_type& str);

      bool push_line (const string_type& str);

      bool read_line (string_type& line);

      void reinit (bool clear_memory = false);

      size_t get_size () const;

      bool empty () const;

      void* get_data () const;

      void dump_to_stream (std::ostream& out);

	  template<typename T>
	  bool read_data(T& val)
	  {
		  return from_buffer(val, std::is_fundamental<T>(), std::is_trivially_copyable<T>());
	  }
      bool read_data(bool& val)
	  {
		  uint8_t temp;
          if (!read_data(temp))
              return false;
		  val = temp != 0;
          return true;
	  }
      bool read_data(string_type& val)
	  {
          int size;
          if (!read_data(size))
              return false;
          if (size > 0)
          {
              val.assign(size, '\0');
              if (!read_data(&val[0], size))
                  return false;
          }
          else
          {
              val.clear();
          }
          return true;
	  }
	  bool read_data(byte_string& val)
	  {
          int size;
          if (!read_data(size))
              return false;
          if (size > 0)
          {
              val = byte_string(size);
              if (!read_data(&val[0], size))
                  return false;
          }
          else
          {
              val.clear();
          }
          return true;
	  }
	  bool read_data(rx_uuid_t& val)
	  {
		  return read_data(&val, sizeof(rx_uuid_t));
	  }
	  template<typename T>
      bool push_data(const T& val)
	  {
		  return to_buffer(val, std::is_fundamental<T>(),  std::is_trivially_copyable<T>());
	  }
      bool push_data(const bool& val)
	  {
		  uint8_t temp = val ? 0xff : 0x00;
		  return push_data(temp);
	  }
      bool push_data(const string_type& val)
	  {
          if (!push_data((uint32_t)val.size()))
              return false;
          if (!val.empty())
              return push_data(val.c_str(), val.size());
          else
              return true;
	  }
      bool push_data(const string_view_type val)
      {
          if (!push_data((uint32_t)val.size()))
              return false;
          if (!val.empty())
              return push_data(&val[0], val.size());
          else
              return true;
      }
      bool push_data(const char* val)
      {
          uint32_t len = 0;
          if (val)
              len = (uint32_t)strlen(val);
          if (!push_data(len))
              return false;
          if (len)
              return push_data(val, len);
          else
              return true;
      }
      bool push_data(const byte_string& val)
      {
          if (!push_data((uint32_t)val.size()))
              return false;
          if (!val.empty())
              return push_data(&val[0], val.size());
          else
              return true;
      }
      bool push_data(const rx_uuid_t& val)
      {
          return push_data(&val, sizeof(rx_uuid_t));
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
          T* ret = (T*)allocator_.get_char_buffer();
		  return ret;
	  }
	  template<typename T>
      bool to_buffer(const T& val, std::true_type, std::true_type)
	  {
		  if constexpr (swap_bytes)
		  {
			  T temp = rx_byte_swap<T>(val);
			  return push_data(&temp, sizeof(T));
		  }
		  else
			  return push_data(&val, sizeof(T));
	  }
	  template<typename T>
	  bool to_buffer(const T& val, std::false_type, std::true_type)
	  {
		  if constexpr (swap_bytes)
		  {
			  T temp = val;
			  temp.swap_bytes();
			  return push_data(&temp, sizeof(T));
		  }
		  else
			  return push_data(&val, sizeof(T));
	  }
	  template<typename T>
	  bool from_buffer(T& val, std::true_type, std::true_type)
	  {
          if (!read_data(&val, sizeof(T)))
              return false;
		  if constexpr (swap_bytes)
		  {
			  val = rx_byte_swap<T>(val);
		  }
          return true;
	  }
	  template<typename T>
	  bool constexpr from_buffer(T& val, std::false_type, std::true_type)
	  {
          if (!read_data(&val, sizeof(T)))
              return false;
		  if constexpr (swap_bytes)
		  {
			  val.swap_bytes();
		  }
          return true;
	  }

};






class std_vector_allocator 
{

  public:
      std_vector_allocator();

      ~std_vector_allocator();


      bool allocate (size_t size);

      void deallocate ();

      bool reallocate (size_t new_size);

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






template <class allocT, bool swap_bytes = false>
class std_strbuff : public memory_buffer_base<allocT, swap_bytes>, 
                    	public std::streambuf  
{
	DECLARE_REFERENCE_PTR(std_strbuff<allocT>);

  public:
      std_strbuff();

      std_strbuff (const void* ptr, size_t size);

      ~std_strbuff();


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
typedef std_strbuff<memory::std_vector_allocator> std_buffer_type;





template <class allocT>
class backward_memory_buffer_base : public pointers::reference_object  
{

  public:
      backward_memory_buffer_base (size_t size);

      ~backward_memory_buffer_base();


      bool push_data (const void* ptr, size_t size);

      bool push_string (const string_type& str);

      bool push_line (const string_type& str);

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

      ~backward_simple_allocator();


      bool allocate (size_t size);

      void deallocate ();

      bool reallocate (size_t new_size);

      size_t get_buffer_size () const;

      char* get_char_buffer () const;


  protected:

  private:


      uint8_t* buffer_;

      static ref_counting_type g_memory_consuption;

      size_t size_;


};







typedef backward_memory_buffer_base< backward_simple_allocator  > back_buffer;






class page_aligned_buffer 
{

  public:
      page_aligned_buffer();

      page_aligned_buffer (size_t size);

      ~page_aligned_buffer();


      bool alloc_buffer (size_t size);

      void free_buffer ();

      uint8_t* buffer ();

      const uint8_t* buffer () const;

      size_t get_size () const;


  protected:

  private:


      uint8_t* buffer_;

      size_t size_;


};






template <class bufferT>
class binary_istream 
{

  public:
      binary_istream (bufferT* buffer, uint32_t version)
            : buffer_(buffer),
              version_(version)
      {
      }


      uint32_t get_version () const
      {
        return version_;
      }

      void set_version (uint32_t value)
      {
        version_ = value;
      }



      template<typename T>
      binary_istream& operator >> (T& val)
      {
          buffer_->read_data(val);
          return *this;
      }
  protected:

  private:


      bufferT* buffer_;

      uint32_t version_;


};






template <class bufferT>
class binary_ostream 
{

  public:
      binary_ostream (bufferT* buffer, uint32_t version)
            : buffer_(buffer),
              version_(version)
      {
      }


      uint32_t get_version () const
      {
        return version_;
      }




      template<typename T>
      binary_ostream& operator << (const T& val)
      {
          buffer_->push_data(val);
          return *this;
      }
  protected:

  private:


      bufferT* buffer_;

      uint32_t version_;


};


// Parameterized Class rx::memory::memory_buffer_base 

template <class allocT, bool swap_bytes>
memory_buffer_base<allocT,swap_bytes>::memory_buffer_base()
      : current_read_(0),
        next_push_(0)
{
	allocator_.allocate(0);// default allocation
}

template <class allocT, bool swap_bytes>
memory_buffer_base<allocT,swap_bytes>::memory_buffer_base (const void* ptr, size_t size)
      : current_read_(0),
        next_push_(0)
{
	next_push_ = (int)size;
	allocator_.allocate(size);
	memcpy(allocator_.get_char_buffer(), ptr, size);
}

template <class allocT, bool swap_bytes>
memory_buffer_base<allocT,swap_bytes>::memory_buffer_base (memory_buffer_base&& right, size_t size)
      : current_read_(0),
        next_push_(0)
{
    current_read_ = right.current_read_;
    next_push_ = right.next_push_;
    right.current_read_ = 0;
    right.next_push_ = 0;
    allocator_ = std::move(right.allocator_);
}


template <class allocT, bool swap_bytes>
memory_buffer_base<allocT,swap_bytes>::~memory_buffer_base()
{
}



template <class allocT, bool swap_bytes>
bool memory_buffer_base<allocT,swap_bytes>::push_data (const void* ptr, size_t size)
{
    while (next_push_ + size > allocator_.get_buffer_size())
    {
        if (!allocator_.reallocate(next_push_ + size))
            return false;
    }
	memcpy(&allocator_.get_char_buffer()[next_push_], ptr, size);
	next_push_ += size;
    return true;
}

template <class allocT, bool swap_bytes>
bool memory_buffer_base<allocT,swap_bytes>::read_data (void* ptr, size_t size)
{
    if (current_read_ + size > next_push_)
    {
        return false;
    }
	memcpy(ptr, &allocator_.get_char_buffer()[current_read_], size);
	current_read_ += size;
    return true;
}

template <class allocT, bool swap_bytes>
bool memory_buffer_base<allocT,swap_bytes>::eof () const
{
	RX_ASSERT(current_read_ <= next_push_);// check first
	return current_read_ >= next_push_;// just in case
}

template <class allocT, bool swap_bytes>
bool memory_buffer_base<allocT,swap_bytes>::push_string (const string_type& str)
{
	return push_data(str.c_str(), str.size());
}

template <class allocT, bool swap_bytes>
bool memory_buffer_base<allocT,swap_bytes>::push_line (const string_type& str)
{
    if (!push_data(str.c_str(), str.size()))
        return false;
    if (!push_data("\r\n", 2))
        return false;
    return true;
}

template <class allocT, bool swap_bytes>
bool memory_buffer_base<allocT,swap_bytes>::read_line (string_type& line)
{
	if (eof())
	{
		return true;// empty line!?!
    }
	char* buff = &allocator_.get_char_buffer()[current_read_];
    if (!buff)
    {
        return false;
    }
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
    return true;
}

template <class allocT, bool swap_bytes>
void memory_buffer_base<allocT,swap_bytes>::reinit (bool clear_memory)
{
	if (clear_memory)
		allocator_.deallocate();
	current_read_ = 0;
	next_push_ = 0;
}

template <class allocT, bool swap_bytes>
size_t memory_buffer_base<allocT,swap_bytes>::get_size () const
{
	return next_push_;
}

template <class allocT, bool swap_bytes>
bool memory_buffer_base<allocT,swap_bytes>::empty () const
{
	return next_push_ == 0;
}

template <class allocT, bool swap_bytes>
void* memory_buffer_base<allocT,swap_bytes>::get_data () const
{
  return allocator_.get_char_buffer();
}

template <class allocT, bool swap_bytes>
void memory_buffer_base<allocT,swap_bytes>::dump_to_stream (std::ostream& out)
{
	bool show32_addr = (next_push_ >= std::numeric_limits<uint16_t>::max());
	out << "\r\n";
	out.setf(std::ios::hex, std::ios::basefield);
	out << std::setfill('0');
	uint8_t* buffer = get_buffer<uint8_t>();
	for (size_t addr = 0; addr < get_size(); addr += 8)
	{
		out << std::setw(show32_addr ? 8 : 4) << addr << " ";
		for (size_t i = 0; i < 8;  i++)
		{
			if (addr + i > get_size())
				out << "   ";
			else
				out << std::setw(2) << (size_t)buffer[addr + i] << " ";
		}
		out << "\r\n";
	}
	out.unsetf(std::ios::hex);

	out << "\r\n";
}


// Parameterized Class rx::memory::std_strbuff 

template <class allocT, bool swap_bytes>
std_strbuff<allocT,swap_bytes>::std_strbuff()
{
}

template <class allocT, bool swap_bytes>
std_strbuff<allocT,swap_bytes>::std_strbuff (const void* ptr, size_t size)
    : memory_buffer_base<allocT, swap_bytes>(ptr, size)
{
}


template <class allocT, bool swap_bytes>
std_strbuff<allocT,swap_bytes>::~std_strbuff()
{
}



template <class allocT, bool swap_bytes>
char* std_strbuff<allocT,swap_bytes>::pbase () const
{
	return this->allocator_.get_char_buffer();
}

template <class allocT, bool swap_bytes>
char* std_strbuff<allocT,swap_bytes>::pptr () const
{
	char* ret= &this->allocator_.get_char_buffer()[this->next_push_];
	return ret;
}

template <class allocT, bool swap_bytes>
char* std_strbuff<allocT,swap_bytes>::epptr () const
{
	return &this->allocator_.get_char_buffer()[this->allocator_.get_buffer_size()];
}

template <class allocT, bool swap_bytes>
char* std_strbuff<allocT,swap_bytes>::eback () const
{
  return this->allocator_.get_char_buffer();
}

template <class allocT, bool swap_bytes>
char* std_strbuff<allocT,swap_bytes>::gptr () const
{
  char* ret= &this->allocator_.get_char_buffer()[this->current_read_];
  return ret;
}

template <class allocT, bool swap_bytes>
char* std_strbuff<allocT,swap_bytes>::egptr () const
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
bool backward_memory_buffer_base<allocT>::push_data (const void* ptr, size_t size)
{
    while (next_push_ + size > allocator_.get_buffer_size())
    {
        if (!allocator_.reallocate(next_push_ + size))
            return false;
    }
	memcpy(&allocator_.get_char_buffer()[allocator_.get_buffer_size() - next_push_ - size], ptr, size);
	next_push_ += ((int)size);
    return true;
}

template <class allocT>
bool backward_memory_buffer_base<allocT>::push_string (const string_type& str)
{
	return push_data(str.c_str(), str.size());
}

template <class allocT>
bool backward_memory_buffer_base<allocT>::push_line (const string_type& str)
{
    if (!push_data(str.c_str(), str.size()))
    {
        return false;
    }
	return push_data("\r\n", 2);
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
	return &allocator_.get_char_buffer()[allocator_.get_buffer_size() - next_push_];
}


// Parameterized Class rx::memory::binary_istream 


// Parameterized Class rx::memory::binary_ostream 


} // namespace memory
} // namespace rx



#endif
