

/****************************************************************************
*
*  system\python\py_support.cpp
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


#include "stdafx.h"


#ifndef NO_PYTHON_SUPPORT

// py_support
#include "system/python/py_support.h"



namespace rx_platform {

namespace python {

// Class rx_platform::python::py_script 

py_script::py_script()
      : initialized_(false)
	, prog::server_script_host({"pera","zika","mika"})
{
}

py_script::py_script(const py_script &right)
      : initialized_(false)
	, prog::server_script_host({ "","","" })
{
	RX_ASSERT(false);
}


py_script::~py_script()
{
}


py_script & py_script::operator=(const py_script &right)
{
	RX_ASSERT(false);
	return *this;
}



py_script& py_script::instance ()
{
	static py_script g_object;
	return g_object;
}

void py_script::deinitialize ()
{
	Py_Finalize();
}

bool py_script::initialize ()
{

	Py_Initialize();
	initialized_ = Py_IsInitialized()!=0;
	PyEval_InitThreads();

	return initialized_;
}

bool py_script::dump_script_information (std::ostream& out)
{

	if (initialized_)
	{
		out << Py_GetVersion() << "\r\n";
		//out << "\r\n" << Py_GetCopyright() << "\r\n\r\n";
	}
	else
	{
		out << "Not initialized!\r\n";
	}
	return true;

//	out << "Not supported!\r\n";
//	return false;
}

void py_script::do_testing_stuff ()
{
}

bool py_script::init_thread ()
{
	return false;
}

bool py_script::deinit_thread ()
{
	return false;
}


// Class rx_platform::python::py_item 

py_item::py_item()
	: obj_(nullptr)
{
}

py_item::py_item(const py_item &right)
{
	if (this != &right)
	{
		this->obj_ = right.obj_;
		Py_XINCREF(this->obj_);
	}
}

py_item::py_item (py_item &&right)
{
	if (this != &right)
	{
		this->obj_ = right.obj_;
		right.obj_ = nullptr;
	}
}

py_item::py_item (PyObject* obj)
{
	obj_ = obj;
	Py_XINCREF(obj_);
}

py_item::py_item (float val)
{
	obj_ = PyFloat_FromDouble(val);
	Py_XINCREF(obj_);
}

py_item::py_item (long val)
{
	obj_ = PyLong_FromLong(val);
	Py_XINCREF(obj_);
}

py_item::py_item (size_t val)
{
	obj_ = PyLong_FromSize_t(val);
	Py_XINCREF(obj_);
}

py_item::py_item (bool val)
{
	obj_ = PyBool_FromLong(val ? 1 : 0);
	Py_XINCREF(obj_);
}

py_item::py_item (const string_type& val)
{
#if PY_MAJOR_VERSION>=3 // version 3 or latter
	obj_ = PyUnicode_FromString(val.c_str());
#else
	obj_ = PyString_FromString(val.c_str());
#endif
	Py_XINCREF(obj_);
}


py_item::~py_item()
{
	Py_CLEAR(this->obj_);
}


py_item & py_item::operator=(const py_item &right)
{
	if (this != &right)
	{
		Py_CLEAR(this->obj_);
		this->obj_ = right.obj_;
		Py_XINCREF(this->obj_);
	}
	return *this;
}


bool py_item::operator==(const py_item &right) const
{
	return this->obj_==right.obj_;
}

bool py_item::operator!=(const py_item &right) const
{
	return this->obj_ == right.obj_;
}


bool py_item::operator<(const py_item &right) const
{
	return this->obj_ < right.obj_;
}

bool py_item::operator>(const py_item &right) const
{
	return this->obj_ > right.obj_;
}

bool py_item::operator<=(const py_item &right) const
{
	return this->obj_ <= right.obj_;
}

bool py_item::operator>=(const py_item &right) const
{
	return this->obj_ >= right.obj_;
}



const PyObject* py_item::operator -> () const
{
	return obj_;
}

PyObject* py_item::operator -> ()
{
	return obj_;
}

py_item::operator bool () const
{
	return obj_ != nullptr;
}

bool py_item::is_float () const
{
	return (obj_ && PyFloat_CheckExact(obj_));
}

double py_item::as_float () const
{
	RX_ASSERT(is_float());
	if (is_float())
		return PyFloat_AsDouble(obj_);
	else
		return std::numeric_limits<double>::signaling_NaN();
}

bool py_item::is_int () const
{
	return (obj_ && PyLong_CheckExact(obj_));
}

long py_item::as_int () const
{
	RX_ASSERT(is_int());
	if (is_int())
		return PyLong_AsLong(obj_);
	else
		return 0;
}

bool py_item::is_size () const
{
	return (obj_ && PyLong_CheckExact(obj_));
}

size_t py_item::as_size () const
{
	RX_ASSERT(is_size());
	if (is_size())
		return PyLong_AsSsize_t(obj_);
	else
		return 0;
}

bool py_item::is_bool () const
{
	return (obj_ && PyBool_Check(obj_));
}

bool py_item::as_bool () const
{
	RX_ASSERT(is_bool());
	if (is_bool())
		return obj_ == Py_True;
	else
		return false;
}

bool py_item::is_string () const
{

#if PY_MAJOR_VERSION>=3 // version 3 or latter
	return (obj_ && PyUnicode_Check(obj_));
#else
	return (obj_ && PyString_Check(obj_));
#endif
}

string_type py_item::as_string () const
{
	RX_ASSERT(is_bool());
	if (is_string())
	{
#if PY_MAJOR_VERSION>=3 // version 3 or latter
		const char* str = PyUnicode_AsUTF8(obj_);
#else
		const char* str = PyString_AsString(obj_);
#endif
		if (str)
			return string_type(str);
		else
			return string_type();
	}
	else
	{
		return string_type();
	}
}


} // namespace python
} // namespace rx_platform

#else 

#endif

