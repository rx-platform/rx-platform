

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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



/////////////////////////////////////////////////////////////
// logging macros for console library
#define RUNTIME_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Run",src,lvl,msg)
#define RUNTIME_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Run",src,lvl,msg)
#define RUNTIME_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Run",src,lvl,msg)
#define RUNTIME_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Run",src,lvl,msg)
#define RUNTIME_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Run",src,lvl,msg)
#define RUNTIME_LOG_TRACE(src,lvl,msg) RX_TRACE("Run",src,lvl,msg)


namespace rx_platform {
namespace runtime {
namespace blocks {
class runtime_holder;
class variable_runtime;
} // namespace blocks

namespace structure {
class runtime_item;
} // namespace structure

namespace operational {
class binded_tags;

} // namespace operational
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {
typedef uint32_t runtime_handle_t;
typedef uint32_t runtime_transaction_id_t;

enum subscription_trigger_type
{
	subscription_trigger_periodic = 0,
	subscription_trigger_critical = 1,
	
	max_trigger_type = 1
};

namespace runtime {

namespace operational
{
class rx_tags_callback;
typedef rx_reference<rx_tags_callback> tags_callback_ptr;
}
namespace structure {
class const_value_data;
class value_data;
class variable_data;
} // namespace structure
union rt_value_ref_union
{
	structure::const_value_data* const_value;
	structure::value_data* value;
	structure::variable_data* variable;
};
enum rt_value_ref_type
{
	rt_null = 0,
	rt_const_value = 1,
	rt_value = 2,
	rt_variable = 3
};
struct rt_value_ref
{
	rt_value_ref_type ref_type;
	rt_value_ref_union ref_value_ptr;
};

typedef rx_reference<blocks::variable_runtime> rx_variable_ptr;
typedef std::unique_ptr<structure::runtime_item> rx_runtime_item_ptr;






class runtime_path_resolver 
{

  public:

      void push_to_path (const string_type& name);

      void pop_from_path ();

      const string_type& get_current_path () const;

      string_type get_parent_path (size_t level) const;


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






struct runtime_deinit_context 
{


      variables_stack variables;

  public:

  protected:

  private:


};






struct runtime_stop_context 
{


      variables_stack variables;

  public:

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

      blocks::runtime_holder* get_root ();

      void set_root (blocks::runtime_holder* item);


  protected:

  private:


      runtime_items_type items_;

      blocks::runtime_holder* root_;


};


typedef std::map<string_type, runtime_handle_t> binded_tags_type;




struct runtime_init_context 
{


      runtime_handle_t get_new_handle ();


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      operational::binded_tags *tags;


      binded_tags_type binded_tags;

  public:

  protected:

  private:


      runtime_handle_t next_handle_;


};






struct runtime_start_context 
{


      runtime_path_resolver path;

      variables_stack variables;

      runtime_structure_resolver structure;

      operational::binded_tags *tags;

  public:

  protected:

  private:


};


enum runtime_process_step
{
	runtime_process_idle = 0,
	runtime_process_scheduled = 1,
	runtime_process_tag_connections = 2
};




struct runtime_process_context 
{


      bool should_repeat () const;

      void tag_updates_pending ();

      rx_result init_context ();


      runtime_process_step current_step;

      bool process_all;

      bool process_tag_connections;

      rx_time now;

  public:

  protected:

  private:


};


} // namespace runtime
} // namespace rx_platform



#endif
