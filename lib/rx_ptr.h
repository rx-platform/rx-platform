

/****************************************************************************
*
*  lib\rx_ptr.h
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


#ifndef rx_ptr_h
#define rx_ptr_h 1



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
	template<class T>\
	friend class rx::pointers::reference;\
private:\
	smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }\


#define DECLARE_DERIVED_FROM_INTERFACE \
protected:\
void interface_bind() { bind(); }\
void interface_release() { release(); }\
private:\


#define DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE \
protected:\
	void virtual_bind() { this->bind(); }\
	void virtual_release() { this->release(); }\
private:\



#define DECLARE_VIRTUAL_REFERENCE_PTR(class_name) \
public:\
	typedef rx::pointers::virtual_reference<class_name > smart_ptr; \
	template<class T>\
	friend class rx::pointers::virtual_reference;\
private:\
	smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }\



#define DECLARE_INTERFACE_PTR(class_name) \
public:\
	typedef rx::pointers::interface_reference<class_name > smart_ptr; \
	template<class T>\
	friend class rx::pointers::interface_reference;\
private:\
	smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }\




struct code_behind_definition_t
{
	const char* class_name;
	const char* plugin;
	const char* description;
	const char* version;
};



//	basic functionality for smart pointers. storage for
//	pointer, call operators and compare for containers



template <class ptrT>
class basic_smart_ptr 
{

	static code_behind_definition_t* rx_code_behind;


	// firend declaration here
	template<class otherT>
	friend class reference;
	template<class otherT>
	friend class virtual_reference;
	template<class otherT>
	friend class interface_reference;
public:

	typedef ptrT* pointer_type;
	typedef ptrT pointee_type;


	// nothing special here
	basic_smart_ptr() = default;
	// it's not virtual!!!
	~basic_smart_ptr() = default;
	// reference based objet no copying
	basic_smart_ptr(const basic_smart_ptr &right) = delete;
	basic_smart_ptr & operator=(const basic_smart_ptr &right) = delete;
	// use 'memmove' to move object around
	basic_smart_ptr(basic_smart_ptr &&right) = default;
	basic_smart_ptr & operator=(basic_smart_ptr &&right) = default;

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
		  static char* ret = nullptr;
		  if (ret == nullptr)
		  {
			  string_type temp(_rx_func_);
			  if (temp.size()>1 && *(temp.rbegin()) == ']')
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
			  ret = new char[temp.size() + 1];
			  strcpy(ret, temp.c_str());
		  }
		  return ret;
	  }
	  template<class otherT>
	  bool operator<(const basic_smart_ptr<otherT>& right) const
	  {
		  return ptr_ < right.ptr_;
	  }
	  template<class otherT, class otherFullT>
	  bool operator>(const basic_smart_ptr<otherT>& right) const
	  {
		  return ptr_ > right.ptr_;
	  }
	  template<class otherT, class otherFullT>
	  bool operator==(const basic_smart_ptr<otherT>& right) const
	  {
		  return ptr_ == right.ptr_;
	  }
	  template<class otherT, class otherFullT>
	  bool operator!=(const basic_smart_ptr<otherT>& right) const
	  {
		  return ptr_ != right.ptr_;
	  }

	  ptrT* unsafe_ptr()
	  {
		  return ptr_;
	  }
  protected:

  private:


      ptrT* ptr_;


};






template <class ptrT>
class reference : public basic_smart_ptr<ptrT>  
{
public:


	static reference<ptrT> null_ptr;

	static reference create_from_pointer(ptrT* ptr)
	{
		reference ret;
		ret.ptr_ = ptr;
		if (ret.ptr_)
			ret.ptr_->bind();
		return ret;
	}
private:

	////////////////////////////////////////////////////////////////////////////////////
	// this code bellow is because this can't be done with constructors VS c++ stuff
	// so constructor specialization is actualy this function specialization
	// vs detects two default constructors !?!
	template<typename... Args>
	void __constructor_workaround(Args... args)
	{
		this->ptr_ = new ptrT(args...);
	}
	void __constructor_workaround()
	{
		this->ptr_ = nullptr;
	}
	void __constructor_workaround(_create_new_type)
	{
		this->ptr_ = new ptrT;
	}
	void __constructor_workaround(ptrT* pt)
	{
		STATIC_CHECK(false);
	}
	////////////////////////////////////////////////////////////////////////////////////
public:

	template<typename... Args>
	explicit reference(Args... args)
	{
		// call dummy constructor function
		__constructor_workaround(args...);
	}
	// firend declaration here
	template<class otherT>
	friend class reference;
	template<class otherT>
	friend class virtual_reference;
	template<class otherT>
	friend class interface_reference;
	// copy constructor
	explicit reference(const reference<ptrT>& right)
	{
		if (this != &right)
		{
			this->ptr_ = right.ptr_;
			if (this->ptr_)
				this->ptr_->bind();
		}
	}
	template<class derT>
	reference(const basic_smart_ptr<derT>& right)
	{
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->bind();
	}
	//move constructor
	reference(reference<ptrT>&& right)
	{
		if (this != &right)
		{
			this->ptr_ = right.ptr_;
			right.ptr_ = nullptr;
		}
	}
	template<class derT>
	reference(basic_smart_ptr<derT>&& right)
	{
		this->ptr_ = right.ptr_;
		right.ptr_ = nullptr;
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
	reference<ptrT>& operator=(const basic_smart_ptr<derT>& right)
	{
		if (this->ptr_)
			this->ptr_->release();
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->bind();
		return *this;
	}

	// move assignment  operator
	reference<ptrT>& operator=(reference<ptrT>&& right)
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
	reference<ptrT>& operator=(basic_smart_ptr<derT>&& right)
	{
		if (this->ptr_)
			this->ptr_->release();
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->bind();
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
		otherT ret;
		return otherT::create_from_pointer(static_cast<typename otherT::pointer_type>(this->ptr_));
	}
public:
	template<class otherT>
	otherT cast_to()
	{
		return _internal_cast_to<otherT>(tl::type2type<otherT>());
	}

  public:

  protected:

  private:


};

template<class ptrT>
reference<ptrT> reference<ptrT>::null_ptr;

// this is an expreiment!!!
template<typename T>
class reference_pointer
{
public:
	typedef rx::pointers::reference<T> smart_ptr;
	template<class Tother>
	friend class rx::pointers::reference;
private:
	smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }
	const smart_ptr smart_this() const { return smart_ptr::create_from_pointer(this); }
};






class reference_object 
{

	DECLARE_REFERENCE_PTR(reference_object);


	/*
	where to put this code from macros?!?
	public:
	typedef basic_smart_ptr<ptrT> smart_ptr;
	template<class Tother>
	friend class rx::pointers::reference;
	private:
	smart_ptr smart_this() { return smart_ptr::create_from_pointer(this); }
	*/

  public:
      reference_object();

      virtual ~reference_object();


      static size_t get_objects_count ();

      virtual string_type get_class_name () const
      {
        return "*unknown*";

      }


  protected:

      void bind ();

      void release ();


  private:
      reference_object(const reference_object &right);

      reference_object & operator=(const reference_object &right);



      std::atomic<ref_counting_type> ref_count_;

      static std::atomic<ref_counting_type> g_objects_count;


};






template <class ptrT>
class virtual_reference : public basic_smart_ptr<ptrT>  
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// class that implements virtual_bind/virtual_release smart ptr, this class uses second level abstract funcion calls that eventualy maps to pin/unpin

public:
	typedef ptrT* pointer_type;
	typedef ptrT pointee_type;
	static virtual_reference<ptrT> null_ptr;

	static virtual_reference create_from_pointer(ptrT* ptr)
	{
		virtual_reference ret;
		ret.ptr_ = ptr;
		if (ret.ptr_)
			ret.ptr_->virtual_bind();
		return ret;
	}

	explicit virtual_reference()
	{
		this->ptr_ = nullptr;
	}
	explicit virtual_reference(ptrT* pt)
		: virtual_reference<ptrT>(pt)
	{
		STATIC_CHECK(false);
	}
	// firend declaration here
	template<class otherT>
	friend class virtual_reference;
	// copy constructor
	virtual_reference(const virtual_reference<ptrT>& right)
	{
		if (this != &right)
		{
			this->ptr_ = right.ptr_;
			if (this->ptr_)
				this->ptr_->virtual_bind();
		}
	}
	template<class derT>
	virtual_reference(const basic_smart_ptr<derT>& right)
	{
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->virtual_bind();
	}
	//move constructor
	virtual_reference(virtual_reference<ptrT>&& right)
	{
		if (this != &right)
		{
			this->ptr_ = right.ptr_;
			right.ptr_ = nullptr;
		}
	}
	template<class derT>
	virtual_reference(basic_smart_ptr<derT>&& right)
	{
		this->ptr_ = right.ptr_;
		right.ptr_ = nullptr;
	}
	// assignment operator
	virtual_reference<ptrT>& operator=(const virtual_reference<ptrT>& right)
	{
		if (this != &right)
		{
			if (this->ptr_)
				this->ptr_->virtual_release();
			this->ptr_ = right.ptr_;
			if (this->ptr_)
				this->ptr_->virtual_bind();
		}
		return *this;
	}
	template<class derT>
	virtual_reference<ptrT>& operator=(const basic_smart_ptr<derT>& right)
	{
		if (this->ptr_)
			this->ptr_->virtual_release();
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->virtual_bind();
		return *this;
	}
	// bool conversion
	operator bool() const
	{
		return (this->ptr_ != nullptr);
	}
	// operator access overridnig

	~virtual_reference()
	{
		if (this->ptr_)
			this->ptr_->virtual_release();
	}


	// cast operations
	// dangerous stuff but had to do
private:
	template<class otherT>
	otherT _internal_cast_to(tl::type2type<otherT>)
	{
		otherT ret;
		return otherT::create_from_pointer(static_cast<typename otherT::pointer_type>(this->ptr_));
	}
public:
	template<class otherT>
	otherT cast_to()
	{
		return _internal_cast_to<otherT>(tl::type2type<otherT>());
	}

  public:

  protected:

  private:


};


template<class ptrT>
virtual_reference<ptrT> virtual_reference<ptrT>::null_ptr;






class virtual_reference_object 
{

  public:
      virtual_reference_object();

      virtual ~virtual_reference_object();


      virtual string_type get_class_name () const
      {
        return "*unknown*";

      }


  protected:

      virtual void virtual_bind () = 0;

      virtual void virtual_release () = 0;


  private:
      virtual_reference_object(const virtual_reference_object &right);

      virtual_reference_object & operator=(const virtual_reference_object &right);



};







struct struct_reference 
{
	DECLARE_REFERENCE_PTR(struct_reference);

public:
	// nothing special here
	struct_reference() 
		: ref_count_(1)
	{
	}
	// it's not virtual!!!
	~struct_reference()
	{
	}
	// reference based objet no copying
	struct_reference(const struct_reference &right) = delete;
	reference_object & operator=(const struct_reference &right) = delete;
	// dont use '&&' to move object around
	struct_reference(struct_reference &&right) = delete;
	struct_reference & operator=(struct_reference &&right) = delete;

  public:

  protected:

      void bind ();

      void release ();


  private:


      std::atomic<ref_counting_type> ref_count_;


};






template <class ptrT>
class interface_reference : public basic_smart_ptr<ptrT>  
{

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// class that implements virtual_bind/virtual_release smart ptr, this class uses second level abstract funcion calls that eventualy maps to pin/unpin

public:
	static interface_reference<ptrT> null_ptr;

	static interface_reference create_from_pointer(ptrT* ptr)
	{
		interface_reference ret;
		ret.ptr_ = ptr;
		if (ret.ptr_)
			ret.ptr_->interface_bind();
		return ret;
	}

	explicit interface_reference()
	{
		this->ptr_ = nullptr;
	}
	explicit interface_reference(ptrT* pt)
		: interface_reference<ptrT>(pt)
	{
		STATIC_CHECK(false);
	}
	// firend declaration here
	template<class otherT>
	friend class interface_reference;
	// copy constructor
	interface_reference(const interface_reference<ptrT>& right)
	{
		if (this != &right)
		{
			this->ptr_ = right.ptr_;
			if (this->ptr_)
				this->ptr_->interface_bind();
		}
	}
	template<class derT>
	interface_reference(const basic_smart_ptr<derT>& right)
	{
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->interface_bind();
	}
	//move constructor
	interface_reference(interface_reference<ptrT>&& right)
	{
		if (this != &right)
		{
			this->ptr_ = right.ptr_;
			right.ptr_ = nullptr;
		}
	}
	template<class derT>
	interface_reference(basic_smart_ptr<derT>&& right)
	{
		this->ptr_ = right.ptr_;
		right.ptr_ = nullptr;
	}
	// assignment operator
	interface_reference<ptrT>& operator=(const interface_reference<ptrT>& right)
	{
		if (this != &right)
		{
			if (this->ptr_)
				this->ptr_->interface_release();
			this->ptr_ = right.ptr_;
			if (this->ptr_)
				this->ptr_->interface_bind();
		}
		return *this;
	}
	template<class derT>
	interface_reference<ptrT>& operator=(const basic_smart_ptr<derT>& right)
	{
		if (this->ptr_)
			this->ptr_->interface_release();
		this->ptr_ = right.ptr_;
		if (this->ptr_)
			this->ptr_->interface_bind();
		return *this;
	}
	// bool conversion
	operator bool() const
	{
		return (this->ptr_ != nullptr);
	}
	// operator access overridnig

	~interface_reference()
	{
		if (this->ptr_)
			this->ptr_->interface_release();
	}


	// cast operations
	// dangerous stuff but had to do
private:
	template<class otherT>
	otherT _internal_cast_to(tl::type2type<otherT>)
	{
		otherT ret;
		return otherT::create_from_pointer(static_cast<typename otherT::pointer_type>(this->ptr_));
	}
public:
	template<class otherT>
	otherT cast_to()
	{
		return _internal_cast_to<otherT>(tl::type2type<otherT>());
	}

  public:

  protected:

  private:


};


template<class ptrT>
interface_reference<ptrT> interface_reference<ptrT>::null_ptr;






class interface_object 
{

  public:
      interface_object();

      virtual ~interface_object();


      virtual string_type get_class_name () const
      {
        return "*unknown*";

      }


  protected:

      virtual void interface_bind () = 0;

      virtual void interface_release () = 0;


  private:
      interface_object(const interface_object &right);

      interface_object & operator=(const interface_object &right);



};


// Parameterized Class rx::pointers::basic_smart_ptr 

//helper functions for creating concrete classes
// for standard references
template<class T, typename... Args>
reference<T> create_reference(Args... args)
{
	return reference<T>(args...);
}
template<class T>
reference<T> create_reference()
{
	return reference<T>(_create_new);
}
} // namespace pointers
} // namespace rx

namespace rx
{

//convinient alias templates
template<class Tptr>
using rx_reference = pointers::reference<Tptr>;

//convinient alias
typedef pointers::struct_reference::smart_ptr rx_struct_ptr;

//convinient alias templates
template<class Tptr>
using rx_interface = pointers::interface_reference<Tptr>;

//convinient alias templates
template<class Tptr>
using rx_virtual = pointers::virtual_reference<Tptr>;

// for standard references
template<class T, typename... Args>
pointers::reference<T> rx_create_reference(Args... args)
{
	return pointers::reference<T>(args...);
}
template<class T>
pointers::reference<T> rx_create_reference()
{
	return pointers::reference<T>(pointers::_create_new);
}


}


#endif
