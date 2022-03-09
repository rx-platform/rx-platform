

/****************************************************************************
*
*  model\rx_meta_commands.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_meta_commands_h
#define rx_meta_commands_h 1



// rx_commands
#include "terminal/rx_commands.h"

#include "api/rx_meta_api.h"
using rx_internal::terminal::console_context_ptr;


namespace rx_internal {

namespace model {

namespace meta_commands {





class create_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(create_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for creating various objects and types in platform\r\n\
\
");

  public:
      create_command();

      ~create_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:
	  template<class T>
	  bool create_object(typename T::instance_data_t instance_data, std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);
	  template<class T>
	  bool create_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);
	  template<class T>
	  bool create_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);

};






class dump_types_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(dump_types_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for dumping types data\r\n\
\
");

  public:
      dump_types_command();

      ~dump_types_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:
	  template<typename T>
	  bool dump_types_to_console(tl::type2type<T>, std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);
	  
	  template<typename T>
	  bool dump_types_recursive(tl::type2type<T>, rx_node_id start, int indent, std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);

};






class delete_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(delete_command);

  public:
      delete_command (const string_type& console_name);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:
	  template<class T>
	  bool delete_object(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);
	  template<class T>
	  bool delete_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);
	  template<class T>
	  bool delete_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);

};






class rm_command : public delete_command  
{
	DECLARE_REFERENCE_PTR(rm_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for deleting objects and types\r\n\
\
");

  public:
      rm_command();


  protected:

  private:


};






class del_command : public delete_command  
{
	DECLARE_REFERENCE_PTR(del_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for deleting objects and types\r\n\
\
");

  public:
      del_command();


  protected:

  private:


};






class check_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(check_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for checking various types in platform\r\n\
\
");

  public:
      check_command();

      ~check_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:
	  template<class T>
	  bool check_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);
	  template<class T>
	  bool check_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);

};






class prototype_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(prototype_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for prototyping objects in platform\r\n\
\
");

  public:
      prototype_command();

      ~prototype_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:
	  template<class T>
	  bool create_prototype(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);

};






class save_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(save_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for saving items to storage\r\n\
\
");

  public:
      save_command();

      ~save_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class update_command : public terminal::commands::server_command  
{
    DECLARE_REFERENCE_PTR(update_command);
    DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for updating various objects and types in platform\r\n\
\
");

  public:
      update_command();

      ~update_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:
      template<class T>
      bool update_object(typename T::instance_data_t instance_data, std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);
      template<class T>
      bool update_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);
      template<class T>
      bool update_simple_type(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx, tl::type2type<T>);

};






class query_command : public terminal::commands::server_command  
{
    DECLARE_REFERENCE_PTR(query_command);
    DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for querying various runtime entities in platform\r\n\
\
");

  public:
      query_command();

      ~query_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:

      bool do_depends_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);



};






class carray_command : public terminal::commands::server_command  
{
    DECLARE_REFERENCE_PTR(carray_command);
    DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for exporting item to C array def\r\n\
\
");

  public:
      carray_command();

      ~carray_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};


} // namespace meta_commands
} // namespace model
} // namespace rx_internal



#endif
