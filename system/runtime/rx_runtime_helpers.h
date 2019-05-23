

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.h
*
*  Copyright (c) 2018-2019 Dusan Ciric
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_runtime_helpers_h
#define rx_runtime_helpers_h 1




namespace rx_platform {
namespace runtime {
namespace blocks {
class runtime_object;
class variable_runtime;
} // namespace blocks

namespace structure {
class runtime_item;

} // namespace structure
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {
typedef rx_reference<blocks::variable_runtime> rx_variable_ptr;
typedef std::unique_ptr<structure::runtime_item> rx_runtime_item_ptr;






class runtime_path_resolver 
{

  public:

      void push_to_path (const string_type& name);

      void pop_from_path ();

      const string_type& get_current_path () const;


  protected:

  private:


      string_type path_;


};







class variables_stack 
{
	typedef std::stack<rx_variable_ptr, std::vector<rx_variable_ptr> > variables_type;

  public:

      void push_variable (rx_variable_ptr what);

      void pop_variable ();

      rx_variable_ptr get_current_variable () const;


  protected:

  private:


      variables_type variables_;


};






class runtime_deinit_context 
{

  public:

      variables_stack variables;


  protected:

  private:


};






class runtime_stop_context 
{

  public:

      variables_stack variables;


  protected:

  private:


};







class runtime_structure_resolver 
{
	typedef std::stack<std::reference_wrapper<structure::runtime_item>, std::vector<std::reference_wrapper<structure::runtime_item> > > runtime_items_type;

  public:
      runtime_structure_resolver();


      void push_item (structure::runtime_item& item);

      void pop_item ();

      structure::runtime_item& get_current_item ();

      blocks::runtime_object* get_root ();

      void set_root (blocks::runtime_object* item);


  protected:

  private:


      runtime_items_type items_;

      blocks::runtime_object* root_;


};






struct runtime_start_context 
{


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

  public:

  protected:

  private:


};






struct runtime_init_context 
{


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

  public:

  protected:

  private:


};


} // namespace runtime
} // namespace rx_platform



#endif
