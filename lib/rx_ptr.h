

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
typedef uint32_t ref_counting_type;


namespace pointers {

class _create_new_type
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

  public:

      const ptrT* operator -> () const
      {
		  return _ptr;
      }

      ptrT* operator -> ()
      {
		  return _ptr;
      }

      ptrT& operator * ()
      {
		  return *_ptr;
      }

      const ptrT& operator * () const
      {
		  return *_ptr;
      }



	  basic_smart_ptr()
	  {

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
		  return _ptr < right._ptr;
	  }
	  template<class otherT, class otherFullT>
	  bool operator>(const basic_smart_ptr<otherT>& right) const
	  {
		  return _ptr > right._ptr;
	  }
	  template<class otherT, class otherFullT>
	  bool operator==(const basic_smart_ptr<otherT>& right) const
	  {
		  return _ptr == right._ptr;
	  }
	  template<class otherT, class otherFullT>
	  bool operator!=(const basic_smart_ptr<otherT>& right) const
	  {
		  return _ptr != right._ptr;
	  }

	  ptrT* unsafe_ptr()
	  {
		  return _ptr;
	  }
  protected:

  private:


      ptrT* _ptr;


};






template <class ptrT>
class reference : public basic_smart_ptr<ptrT>  
{
public:


	static reference<ptrT> null_ptr;

	static reference create_from_pointer(ptrT* ptr)
	{
		reference ret;
		ret._ptr = ptr;
		if (ret._ptr)
			ret._ptr->bind();
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
		this->_ptr = new ptrT(args...);
	}
	void __constructor_workaround()
	{
		this->_ptr = nullptr;
	}
	void __constructor_workaround(_create_new_type)
	{
		this->_ptr = new ptrT;
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
	reference(const reference<ptrT>& right)
	{
		if (this != &right)
		{
			this->_ptr = right._ptr;
			if (this->_ptr)
				this->_ptr->bind();
		}
	}
	template<class derT>
	reference(const basic_smart_ptr<derT>& right)
	{
		this->_ptr = right._ptr;
		if (this->_ptr)
			this->_ptr->bind();
	}
	//move constructor
	reference(reference<ptrT>&& right)
	{
		if (this != &right)
		{
			this->_ptr = right._ptr;
			right._ptr = nullptr;
		}
	}
	template<class derT>
	reference(basic_smart_ptr<derT>&& right)
	{
		this->_ptr = right._ptr;
		right._ptr = nullptr;
	}
	// assigment operator
	reference<ptrT>& operator=(const reference<ptrT>& right)
	{
		if (this != &right)
		{
			if (this->_ptr)
				this->_ptr->release();
			this->_ptr = right._ptr;
			if (this->_ptr)
				this->_ptr->bind();
		}
		return *this;
	}
	template<class derT>
	reference<ptrT>& operator=(const basic_smart_ptr<derT>& right)
	{
		if (this->_ptr)
			this->_ptr->release();
		this->_ptr = right._ptr;
		if (this->_ptr)
			this->_ptr->bind();
		return *this;
	}
	// bool conversion
	operator bool() const
	{
		return (this->_ptr != nullptr);
	}

	~reference()
	{
		if (this->_ptr)
			this->_ptr->release();
	}

	// cast operations
	// dangerous stuff but had to do
private:
	template<class otherT>
	otherT _internal_cast_to(tl::type2type<otherT>)
	{
		otherT ret;
		return otherT::create_from_pointer(static_cast<typename otherT::pointer_type>(this->_ptr));
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


  protected:

      void bind ();

      void release ();


  private:
      reference_object(const reference_object &right);

      reference_object & operator=(const reference_object &right);



      std::atomic<ref_counting_type> _ref_count;

      static std::atomic<ref_counting_type> g_objects_count;


};






class virtual_reference_object 
{

  public:
      virtual_reference_object();

      virtual ~virtual_reference_object();


  protected:

      virtual void virtual_bind () = 0;

      virtual void virtual_release () = 0;


  private:
      virtual_reference_object(const virtual_reference_object &right);

      virtual_reference_object & operator=(const virtual_reference_object &right);



};






template <class ptrT>
class virtual_reference : public basic_smart_ptr<ptrT>  
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// class that implements virtual_bind/virtual_release smart ptr, this class uses second level abstract funcion calls that eventualy maps to pin/unpin

	typedef ptrT* pointer_type;
	typedef ptrT pointee_type;
public:
	static virtual_reference<ptrT> null_ptr;

	static virtual_reference create_from_pointer(ptrT* ptr)
	{
		virtual_reference ret;
		ret._ptr = ptr;
		if (ret._ptr)
			ret._ptr->virtual_bind();
		return ret;
	}

	explicit virtual_reference()
	{
		this->_ptr = nullptr;
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
			this->_ptr = right._ptr;
			if (this->_ptr)
				this->_ptr->virtual_bind();
		}
	}
	template<class derT>
	virtual_reference(const basic_smart_ptr<derT>& right)
	{
		this->_ptr = right._ptr;
		if (this->_ptr)
			this->_ptr->virtual_bind();
	}
	//move constructor
	virtual_reference(virtual_reference<ptrT>&& right)
	{
		if (this != &right)
		{
			this->_ptr = right._ptr;
			right._ptr = nullptr;
		}
	}
	template<class derT>
	virtual_reference(basic_smart_ptr<derT>&& right)
	{
		this->_ptr = right._ptr;
		right._ptr = nullptr;
	}
	// assigment operator
	virtual_reference<ptrT>& operator=(const virtual_reference<ptrT>& right)
	{
		if (this != &right)
		{
			if (this->_ptr)
				this->_ptr->virtual_release();
			this->_ptr = right._ptr;
			if (this->_ptr)
				this->_ptr->virtual_bind();
		}
		return *this;
	}
	template<class derT>
	virtual_reference<ptrT>& operator=(const basic_smart_ptr<derT>& right)
	{
		if (this->_ptr)
			this->_ptr->virtual_release();
		this->_ptr = right._ptr;
		if (this->_ptr)
			this->_ptr->virtual_bind();
		return *this;
	}
	// bool conversion
	operator bool() const
	{
		return (this->_ptr != nullptr);
	}
	// operator access overridnig

	~virtual_reference()
	{
		if (this->_ptr)
			this->_ptr->virtual_release();
	}


	// cast operations
	// dangerous stuff but had to do
private:
	template<class otherT>
	otherT _internal_cast_to(tl::type2type<otherT>)
	{
		otherT ret;
		return otherT::create_from_pointer(static_cast<typename otherT::pointer_type>(this->_ptr));
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





class slim_reference 
{
	DECLARE_REFERENCE_PTR(slim_reference);

  public:
      slim_reference();

      ~slim_reference();


  protected:

      void bind ();

      void release ();


  private:


      std::atomic<ref_counting_type> _ref_count;


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
		ret._ptr = ptr;
		if (ret._ptr)
			ret._ptr->interface_bind();
		return ret;
	}

	explicit interface_reference()
	{
		this->_ptr = nullptr;
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
			this->_ptr = right._ptr;
			if (this->_ptr)
				this->_ptr->interface_bind();
		}
	}
	template<class derT>
	interface_reference(const basic_smart_ptr<derT>& right)
	{
		this->_ptr = right._ptr;
		if (this->_ptr)
			this->_ptr->interface_bind();
	}
	//move constructor
	interface_reference(interface_reference<ptrT>&& right)
	{
		if (this != &right)
		{
			this->_ptr = right._ptr;
			right._ptr = nullptr;
		}
	}
	template<class derT>
	interface_reference(basic_smart_ptr<derT>&& right)
	{
		this->_ptr = right._ptr;
		right._ptr = nullptr;
	}
	// assigment operator
	interface_reference<ptrT>& operator=(const interface_reference<ptrT>& right)
	{
		if (this != &right)
		{
			if (this->_ptr)
				this->_ptr->interface_release();
			this->_ptr = right._ptr;
			if (this->_ptr)
				this->_ptr->interface_bind();
		}
		return *this;
	}
	template<class derT>
	interface_reference<ptrT>& operator=(const basic_smart_ptr<derT>& right)
	{
		if (this->_ptr)
			this->_ptr->interface_release();
		this->_ptr = right._ptr;
		if (this->_ptr)
			this->_ptr->interface_bind();
		return *this;
	}
	// bool conversion
	operator bool() const
	{
		return (this->_ptr != nullptr);
	}
	// operator access overridnig

	~interface_reference()
	{
		if (this->_ptr)
			this->_ptr->interface_release();
	}


	// cast operations
	// dangerous stuff but had to do
private:
	template<class otherT>
	otherT _internal_cast_to(tl::type2type<otherT>)
	{
		otherT ret;
		return otherT::create_from_pointer(static_cast<typename otherT::pointer_type>(this->_ptr));
	}
public:
	template<class otherT>
	otherT cast_to()
	{
		return _internal_cast_to<otherT>(tl::type2type<otherT>());
	}

  public:

      void pera ();


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


  protected:

      virtual void interface_bind () = 0;

      virtual void interface_release () = 0;


  private:
      interface_object(const interface_object &right);

      interface_object & operator=(const interface_object &right);



};


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


// Parameterized Class rx::pointers::basic_smart_ptr 


} // namespace pointers
} // namespace rx



#endif
