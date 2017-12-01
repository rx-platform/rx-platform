

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
      : _initialized(false)
	, prog::server_script_host({"pera","zika","mika"})
{
}

py_script::py_script(const py_script &right)
      : _initialized(false)
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
	_initialized = Py_IsInitialized()!=0;
	PyEval_InitThreads();

	return _initialized;
}

bool py_script::dump_script_information (std::ostream& out)
{

	if (_initialized)
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
	: _obj(nullptr)
{
}

py_item::py_item(const py_item &right)
{
	if (this != &right)
	{
		this->_obj = right._obj;
		Py_XINCREF(this->_obj);
	}
}

py_item::py_item (py_item &&right)
{
	if (this != &right)
	{
		this->_obj = right._obj;
		right._obj = nullptr;
	}
}

py_item::py_item (PyObject* obj)
{
	_obj = obj;
	Py_XINCREF(_obj);
}

py_item::py_item (float val)
{
	_obj = PyFloat_FromDouble(val);
	Py_XINCREF(_obj);
}

py_item::py_item (long val)
{
	_obj = PyLong_FromLong(val);
	Py_XINCREF(_obj);
}

py_item::py_item (size_t val)
{
	_obj = PyLong_FromSize_t(val);
	Py_XINCREF(_obj);
}

py_item::py_item (bool val)
{
	_obj = PyBool_FromLong(val ? 1 : 0);
	Py_XINCREF(_obj);
}

py_item::py_item (const string_type& val)
{
#if PY_MAJOR_VERSION>=3 // version 3 or latter
	_obj = PyUnicode_FromString(val.c_str());
#else
	_obj = PyString_FromString(val.c_str());
#endif
	Py_XINCREF(_obj);
}


py_item::~py_item()
{
	Py_CLEAR(this->_obj);
}


py_item & py_item::operator=(const py_item &right)
{
	if (this != &right)
	{
		Py_CLEAR(this->_obj);
		this->_obj = right._obj;
		Py_XINCREF(this->_obj);
	}
	return *this;
}


bool py_item::operator==(const py_item &right) const
{
	return this->_obj==right._obj;
}

bool py_item::operator!=(const py_item &right) const
{
	return this->_obj == right._obj;
}


bool py_item::operator<(const py_item &right) const
{
	return this->_obj < right._obj;
}

bool py_item::operator>(const py_item &right) const
{
	return this->_obj > right._obj;
}

bool py_item::operator<=(const py_item &right) const
{
	return this->_obj <= right._obj;
}

bool py_item::operator>=(const py_item &right) const
{
	return this->_obj >= right._obj;
}



const PyObject* py_item::operator -> () const
{
	return _obj;
}

PyObject* py_item::operator -> ()
{
	return _obj;
}

py_item::operator bool () const
{
	return _obj != nullptr;
}

bool py_item::is_float () const
{
	return (_obj && PyFloat_CheckExact(_obj));
}

double py_item::as_float () const
{
	RX_ASSERT(is_float());
	if (is_float())
		return PyFloat_AsDouble(_obj);
	else
		return std::numeric_limits<double>::signaling_NaN();
}

bool py_item::is_int () const
{
	return (_obj && PyLong_CheckExact(_obj));
}

long py_item::as_int () const
{
	RX_ASSERT(is_int());
	if (is_int())
		return PyLong_AsLong(_obj);
	else
		return 0;
}

bool py_item::is_size () const
{
	return (_obj && PyLong_CheckExact(_obj));
}

size_t py_item::as_size () const
{
	RX_ASSERT(is_size());
	if (is_size())
		return PyLong_AsSsize_t(_obj);
	else
		return 0;
}

bool py_item::is_bool () const
{
	return (_obj && PyBool_Check(_obj));
}

bool py_item::as_bool () const
{
	RX_ASSERT(is_bool());
	if (is_bool())
		return _obj == Py_True;
	else
		return false;
}

bool py_item::is_string () const
{

#if PY_MAJOR_VERSION>=3 // version 3 or latter
	return (_obj && PyUnicode_Check(_obj));
#else
	return (_obj && PyString_Check(_obj));
#endif
}

string_type py_item::as_string () const
{
	RX_ASSERT(is_bool());
	if (is_string())
	{
#if PY_MAJOR_VERSION>=3 // version 3 or latter
		const char* str = PyUnicode_AsUTF8(_obj);
#else
		const char* str = PyString_AsString(_obj);
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

