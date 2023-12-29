

/****************************************************************************
*
*  system\python\py_support.h
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


#ifndef py_support_h
#define py_support_h 1


#ifdef PYTHON_SUPPORT

// rx_cmds
#include "system/server/rx_cmds.h"



namespace rx_platform {

namespace python {


//	Basic interface to underlying python language



class py_script : public prog::server_script_host  
{

  public:
      virtual ~py_script();


      static py_script& instance ();

      void deinitialize ();

      bool initialize ();

      bool dump_script_information (std::ostream& out);

      void do_testing_stuff ();

      bool init_thread ();

      bool deinit_thread ();


  protected:

  private:
      py_script();

      py_script(const py_script &right);

      py_script & operator=(const py_script &right);



      bool initialized_;


};






class py_item 
{

  public:
      py_item();

      py_item(const py_item &right);

      py_item (py_item &&right);

      py_item (PyObject* obj);

      py_item (float val);

      py_item (long val);

      py_item (size_t val);

      py_item (bool val);

      py_item (const string_type& val);

      virtual ~py_item();

      py_item & operator=(const py_item &right);

      bool operator==(const py_item &right) const;

      bool operator!=(const py_item &right) const;

      bool operator<(const py_item &right) const;

      bool operator>(const py_item &right) const;

      bool operator<=(const py_item &right) const;

      bool operator>=(const py_item &right) const;


      const PyObject* operator -> () const;

      PyObject* operator -> ();

      operator bool () const;

      bool is_float () const;

      double as_float () const;

      bool is_int () const;

      long as_int () const;

      bool is_size () const;

      size_t as_size () const;

      bool is_bool () const;

      bool as_bool () const;

      bool is_string () const;

      string_type as_string () const;


  protected:

  private:


      PyObject* obj_;


};


} // namespace python
} // namespace rx_platform

#endif


#endif
