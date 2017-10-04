

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


// py_support
#include "system/python/py_support.h"



namespace server {

namespace python {

// Class server::python::py_script 

py_script::py_script()
      : m_initialized(false)
	, prog::server_script_host({"pera","zika","mika"})
{
}

py_script::py_script(const py_script &right)
      : m_initialized(false)
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
#ifndef NO_PYTHON_SUPPORT
	Py_Finalize();
#endif
}

bool py_script::initialize ()
{

#ifndef NO_PYTHON_SUPPORT
	Py_Initialize();
	m_initialized = Py_IsInitialized()!=0;
	PyEval_InitThreads();
#else
	m_initialized = true;
#endif
	return m_initialized;
}

bool py_script::dump_script_information (std::ostream& out)
{

#ifndef NO_PYTHON_SUPPORT
	if (m_initialized)
	{
		out << Py_GetVersion() << "\r\n";
		//out << "\r\n" << Py_GetCopyright() << "\r\n\r\n";
	}
	else
	{
		out << "Not initialized!\r\n";
	}
	return true;
#else
	out << "Not supported!\r\n";
	return false;
#endif
}

void py_script::do_testing_stuff ()
{
}

bool py_script::init_thread ()
{
}

bool py_script::deinit_thread ()
{
}


// Class server::python::py_item 

py_item::py_item()
	: m_obj(nullptr)
{
}

py_item::py_item(const py_item &right)
{
	if (this != &right)
	{
		this->m_obj = right.m_obj;
		Py_XINCREF(this->m_obj);
	}
}

py_item::py_item (py_item &&right)
{
	if (this != &right)
	{
		this->m_obj = right.m_obj;
		right.m_obj = nullptr;
	}
}

py_item::py_item (PyObject* obj)
{
	m_obj = obj;
	Py_XINCREF(m_obj);
}

py_item::py_item (float val)
{
	m_obj = PyFloat_FromDouble(val);
	Py_XINCREF(m_obj);
}

py_item::py_item (long val)
{
	m_obj = PyLong_FromLong(val);
	Py_XINCREF(m_obj);
}

py_item::py_item (size_t val)
{
	m_obj = PyLong_FromSize_t(val);
	Py_XINCREF(m_obj);
}

py_item::py_item (bool val)
{
	m_obj = PyBool_FromLong(val ? 1 : 0);
	Py_XINCREF(m_obj);
}

py_item::py_item (const string_type& val)
{
#if PY_MAJOR_VERSION>=3 // version 3 or latter
	m_obj = PyUnicode_FromString(val.c_str());
#else
	m_obj = PyString_FromString(val.c_str());
#endif
	Py_XINCREF(m_obj);
}


py_item::~py_item()
{
	Py_CLEAR(this->m_obj);
}


py_item & py_item::operator=(const py_item &right)
{
	if (this != &right)
	{
		Py_CLEAR(this->m_obj);
		this->m_obj = right.m_obj;
		Py_XINCREF(this->m_obj);
	}
	return *this;
}


bool py_item::operator==(const py_item &right) const
{
	return this->m_obj==right.m_obj;
}

bool py_item::operator!=(const py_item &right) const
{
	return this->m_obj == right.m_obj;
}


bool py_item::operator<(const py_item &right) const
{
	return this->m_obj < right.m_obj;
}

bool py_item::operator>(const py_item &right) const
{
	return this->m_obj > right.m_obj;
}

bool py_item::operator<=(const py_item &right) const
{
	return this->m_obj <= right.m_obj;
}

bool py_item::operator>=(const py_item &right) const
{
	return this->m_obj >= right.m_obj;
}



const PyObject* py_item::operator -> () const
{
	return m_obj;
}

PyObject* py_item::operator -> ()
{
	return m_obj;
}

py_item::operator bool () const
{
	return m_obj != nullptr;
}

bool py_item::is_float () const
{
	return (m_obj && PyFloat_CheckExact(m_obj));
}

double py_item::as_float () const
{
	RX_ASSERT(is_float());
	if (is_float())
		return PyFloat_AsDouble(m_obj);
	else
		return std::numeric_limits<double>::signaling_NaN();
}

bool py_item::is_int () const
{
	return (m_obj && PyLong_CheckExact(m_obj));
}

long py_item::as_int () const
{
	RX_ASSERT(is_int());
	if (is_int())
		return PyLong_AsLong(m_obj);
	else
		return 0;
}

bool py_item::is_size () const
{
	return (m_obj && PyLong_CheckExact(m_obj));
}

size_t py_item::as_size () const
{
	RX_ASSERT(is_size());
	if (is_size())
		return PyLong_AsSsize_t(m_obj);
	else
		return 0;
}

bool py_item::is_bool () const
{
	return (m_obj && PyBool_Check(m_obj));
}

bool py_item::as_bool () const
{
	RX_ASSERT(is_bool());
	if (is_bool())
		return m_obj == Py_True;
	else
		return false;
}

bool py_item::is_string () const
{

#if PY_MAJOR_VERSION>=3 // version 3 or latter
	return (m_obj && PyUnicode_Check(m_obj));
#else
	return (m_obj && PyString_Check(m_obj));
#endif
}

string_type py_item::as_string () const
{
	RX_ASSERT(is_bool());
	if (is_string())
	{
#if PY_MAJOR_VERSION>=3 // version 3 or latter
		const char* str = PyUnicode_AsUTF8(m_obj);
#else
		const char* str = PyString_AsString(m_obj);
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
} // namespace server

