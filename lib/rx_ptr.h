

/****************************************************************************
*
*  lib\rx_ptr.h
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


#ifndef rx_ptr_h
#define rx_ptr_h 1


#include "common/rx_common.h"
#include "rx_lib.h"




namespace rx {
// reference counting type defined here
// important stuff regarding resources
typedef uint_fast16_t ref_counting_type;


namespace pointers {

struct _create_new_type
{
};

extern _create_new_type _create_new;

#define DECLARE_REFERENCE_PTR(class_name) \
public:\
	typedef rx::pointers::reference<class_name > smart_ptr; \
	template<class Tptr>\
	friend class rx::pointers::reference;\
private:\
	smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }\
	const smart_ptr smart_this() const { return smart_ptr::create_from_pointer(const_cast<class_name*>(this)); }\


struct code_behind_definition_t
{
	const char* class_name;
	const char* plugin;
	const char* description;
	const char* version;
};






template <class ptrT>
class reference 
{
	
	static code_behind_definition_t* rx_code_behind;
	// friend declaration here
	template<class otherT>
	friend class reference;

public:
	typedef ptrT* pointer_type;
	typedef ptrT pointee_type;
	static reference<ptrT> null_ptr;
	static reference create_from_pointer(ptrT* ptr)
	{
		reference ret;
		ret.ptr_ = ptr;
		if (ret.ptr_)
			ret.ptr_->bind();
		return ret;
	}
	static reference create_from_pointer_without_bind(ptrT* ptr)
	{
		reference ret;
		ret.ptr_ = ptr;		
		return ret;
	}
private:

	template<class T, class Tbase>
	int32_t id_for()
	{
		static int result(5);
		return result;
	}
public:
	explicit reference() noexcept
	{
		this->ptr_ = nullptr;
	}
	explicit reference(_create_new_type)
	{
		this->ptr_ = new ptrT;
	}
	explicit reference(ptrT* pt)
	{
		RX_ASSERT(false);
	}
	template<class otherT>
	friend class reference;
	// copy constructor
	explicit reference(const reference<ptrT>& right)
	{
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->bind();
	}
	template<class derT>
	reference(const reference<derT>& right)
	{
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->bind();
	}
	//move constructor
	reference(reference<ptrT>&& right) noexcept
	{
		this->ptr_ = right.ptr_;
		right.ptr_ = nullptr;
	}
	template<class derT>
	reference(reference<derT>&& right) noexcept
	{
		this->ptr_ = right.ptr_;
		right.ptr_ = nullptr;
	}
	template<typename... Args>
	explicit reference(Args... args)
	{
		this->ptr_ = new ptrT(std::forward<Args>(args)...);
	}
	// assignment  operator
	reference<ptrT>& operator=(const reference<ptrT>& right)
	{
		if (this != &right)
		{
			if (this->ptr_)
				this->ptr_->release();
			this->ptr_ = right.ptr_;
			if (this->ptr_)
				this->ptr_->bind();
		}
		return *this;
	}
	template<class derT>
	reference<ptrT>& operator=(const reference<derT>& right)
	{
		if (this->ptr_)
			this->ptr_->release();
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->bind();
		return *this;
	}

	// move assignment  operator
	reference<ptrT>& operator=(reference<ptrT>&& right) noexcept
	{
		if (this != &right)
		{
			if (this->ptr_)
				this->ptr_->release();
			this->ptr_ = right.ptr_;
			right.ptr_ = nullptr;
		}
		return *this;
	}
	template<class derT>
	reference<ptrT>& operator=(reference<derT>&& right) noexcept
	{
		if (this->ptr_)
			this->ptr_->release();
		this->ptr_ = right.ptr_;
		right.ptr_ = nullptr;
		return *this;
	}
	// bool conversion
	operator bool() const
	{
		return (this->ptr_ != nullptr);
	}

	~reference()
	{
		if (this->ptr_)
			this->ptr_->release();
	}

	// cast operations
	// dangerous stuff but had to do
private:
	template<class otherT>
	otherT _internal_cast_to(tl::type2type<otherT>)
	{
		//typename otherT::pointer_type ret(this->ptr_);
		return otherT::create_from_pointer(static_cast<typename otherT::pointer_type>(this->ptr_));
	}
public:
	template<class otherT>
	otherT cast_to()
	{
		return _internal_cast_to<otherT>(tl::type2type<otherT>());
	}

  public:

      const ptrT* operator -> () const
      {
		  return ptr_;
      }

      ptrT* operator -> ()
      {
		  return ptr_;
      }

      ptrT& operator * ()
      {
		  return *ptr_;
      }

      const ptrT& operator * () const
      {
		  return *ptr_;
      }



	  static const char* get_pointee_class_name()
	  {
#ifdef _DEBUG
		  static char ret[0x100] = { '\0' };
		  if (ret[0] == '\0')
		  {
#else
		  static char* ret = nullptr;
		  if (ret == nullptr)
		  {
#endif
			  string_type temp(_rx_func_);
			  if (temp.size() > 1 && *(temp.rbegin()) == ']')
			  {// gcc stuff
				  temp = temp.substr(0, temp.size() - 1);
				  size_t idx = temp.rfind(' ');
				  if (idx != string_type::npos)
				  {
					  temp = temp.substr(idx + 1);
				  }
			  }
			  else
			  {
				  size_t idx = temp.find(">::get_pointee_class_name");
				  if (idx != string_type::npos)
				  {
					  temp = temp.substr(0, idx);
					  idx = temp.rfind(' ');
					  if (idx != string_type::npos)
					  {
						  temp = temp.substr(idx + 1);
					  }
				  }
			  }
#ifndef _DEBUG
			  ret = new char[temp.size() + 1];
#endif
			  strcpy(ret, temp.c_str());
		  }
		  return ret;
	  }
	  template<class otherT>
	  bool operator<(const reference<otherT>& right) const
	  {
		  return this->ptr_ < right.ptr_;
	  }
	  template<class otherT>
	  bool operator>(const reference<otherT>& right) const
	  {
		  return this->ptr_ > right.ptr_;
	  }
	  template<class otherT>
	  bool operator==(const reference<otherT>& right) const
	  {
		  return this->ptr_ == right.ptr_;
	  }
	  template<class otherT>
	  bool operator!=(const reference<otherT>& right) const
	  {
		  return this->ptr_ != right.ptr_;
	  }

	  ptrT* unsafe_ptr()
	  {
		  return this->ptr_;
	  }
	  const ptrT* unsafe_ptr() const
	  {
		  return this->ptr_;
	  }
  protected:

  private:


      ptrT* ptr_;


};


//helper functions for creating concrete classes
// for standard references
template<class T, typename... Args>
reference<T> create_reference(Args&&... args)
{
	return reference<T>(std::forward<Args>(args)...);
}
template<class T>
reference<T> create_reference()
{
	return reference<T>(_create_new);
}

template<class ptrT>
reference<ptrT> reference<ptrT>::null_ptr;

// this is an experiment!!!
template<typename T>
class reference_pointer
{
public:
	typedef rx::pointers::reference<T> smart_ptr;
	template<class Tother>
	friend class rx::pointers::reference;
private:
	//smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }
	//smart_ptr smart_this() const { return smart_ptr::create_from_pointer(this); }
};






class reference_object 
{

	DECLARE_REFERENCE_PTR(reference_object);
	/*
	where to put this code from macros?!?
	public:
	typedef reference<ptrT> smart_ptr;
	template<class Tother>
	friend class rx::pointers::reference;
	private:
	smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }
	*/

  public:
      reference_object();

      reference_object (lock_reference_struct* shared_data);

      virtual ~reference_object();


      lock_reference_struct* get_shared_ref ();

      static size_t get_objects_count ();

      virtual string_type get_class_name () const
      {
        return "*unknown*";

      }

      virtual void fill_code_info (std::ostream& info, const string_type& name) const;

	  reference_object(const reference_object&) = delete;
	  reference_object(reference_object&&) = delete;
	  reference_object& operator=(const reference_object&) = delete;
	  reference_object& operator=(reference_object&&) = delete;
  protected:

      void bind_as_shared (lock_reference_struct* shared_data);

      void bind ();

      void release ();


  private:


      std::atomic<ref_counting_type> ref_count_;

      lock_reference_struct* shared_data_;

      bool shared_ref_;

      static std::atomic<ref_counting_type> g_objects_count;


};


} // namespace pointers
} // namespace rx

namespace rx
{

//convenient alias templates
template<class Tptr>
using rx_reference = pointers::reference<Tptr>;

typedef pointers::reference_object::smart_ptr rx_reference_ptr;
typedef pointers::reference_object::smart_ptr rx_struct_ptr;

#ifdef RX_USE_OWN_HEAP

// for now just starndard stuff
template<class T, typename... Args>
pointers::reference<T> rx_create_reference(Args&&... args)
{
	return pointers::reference<T>::create_from_pointer_without_bind(new T(std::forward<Args>(args)...));
}
template<class T>
pointers::reference<T> rx_create_reference()
{
	return pointers::reference<T>::create_from_pointer_without_bind(new T());
}

#else

// this strange looking stuff is placed in order to catch dangling pointers with
//  microsoft debug heap, if used enables to detect the caller line and file

#ifdef _DEBUG

#ifdef _MSC_VER 

#undef new

struct debug_reference_wrapper
{
	const char* file_;
	int line_;
	debug_reference_wrapper(const char* file, int line)
		: file_(file), line_(line)
	{
	}
	template<class T, typename... Args>
	pointers::reference<T> create_func(Args&&... args)
	{
		return pointers::reference<T>::create_from_pointer_without_bind(new(_CLIENT_BLOCK, file_, line_) T(std::forward<Args>(args)...));
	}
	template<class T>
	pointers::reference<T> create_func()
	{
		return pointers::reference<T>::create_from_pointer_without_bind(new(_CLIENT_BLOCK, file_, line_) T());
	}
};

#define DEBUG_ALLOC_WRAPPER(f, l) debug_reference_wrapper(f, l).create_func

#define rx_create_reference DEBUG_ALLOC_WRAPPER(__FILE__, __LINE__)

#define new DEBUG_CLIENTBLOCK 

#else
// for standard references
template<class T, typename... Args>
pointers::reference<T> rx_create_reference(Args&&... args)
{
	return pointers::reference<T>::create_from_pointer_without_bind(new T(std::forward<Args>(args)...));
}
template<class T>
pointers::reference<T> rx_create_reference()
{
	return pointers::reference<T>::create_from_pointer_without_bind(new T());
}

#endif

#else
// for standard references
template<class T, typename... Args>
pointers::reference<T> rx_create_reference(Args&&... args)
{
	return pointers::reference<T>::create_from_pointer_without_bind(new T(std::forward<Args>(args)...));
}
template<class T>
pointers::reference<T> rx_create_reference()
{
	return pointers::reference<T>::create_from_pointer_without_bind(new T());
}

#endif

#endif //RX_USE_OWN_HEAP

}


#endif
