

/****************************************************************************
*
*  runtime_internal\rx_data_source.h
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


#ifndef rx_data_source_h
#define rx_data_source_h 1



// rx_thread
#include "lib/rx_thread.h"
// rx_value_point
#include "runtime_internal/rx_value_point.h"

namespace rx_internal {
namespace sys_runtime {
namespace data_source {
class data_controler;

} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal




namespace rx_internal {

namespace sys_runtime {

namespace data_source {





class data_source 
{

  public:
      virtual ~data_source();


      virtual void add_item (const string_type& path, uint32_t rate, value_handle_extended& handle) = 0;

      virtual void write_item (const value_handle_extended& handle, rx_simple_value val, runtime_transaction_id_t id) = 0;

      virtual void execute_item (const value_handle_extended& handle, data::runtime_values_data data, runtime_transaction_id_t id) = 0;

      virtual void remove_item (const value_handle_extended& handle) = 0;

      virtual bool is_empty () const = 0;


      data_controler *my_controler;


  protected:

  private:


};







class data_source_factory 
{
	typedef std::map<string_type, std::function<std::unique_ptr<data_source>(const string_type&)> > creators_type;

  public:

      static data_source_factory& instance ();

      void register_data_source (const string_type& type, std::function<std::unique_ptr<data_source>(const string_type&)> creator);

      rx_result register_internal_sources ();

      rx_result_with<std::unique_ptr<data_source> > create_data_source (const string_type& source);

      void deinitialize ();


  protected:

  private:
      data_source_factory();



      creators_type creators_;

      std::function<std::unique_ptr<data_source>(const string_type&)> default_;


};







class data_controler 
{
	typedef std::unordered_set<value_point_impl*> registered_objects_type;
	typedef std::unordered_map<value_handle_type, registered_objects_type> registered_values_type;

	struct source_data
	{
		std::unique_ptr<data_source> source;
		string_type name;
	};
	typedef std::unordered_map<uint16_t, source_data > sources_type;
	struct named_source_data
	{
		std::reference_wrapper<data_source> source;
		uint16_t handle;
	};
	typedef std::unordered_map<string_type, named_source_data> named_sources_type;

  public:
      data_controler (rx::threads::physical_job_thread* worker);

      ~data_controler();


      void register_value (value_handle_type handle, value_point_impl* whose);

      void unregister_value (value_handle_type handle, value_point_impl* whose);

      value_handle_type add_item (const string_type& path, uint32_t rate);

      void write_item (value_handle_type handle, rx_simple_value val, runtime_transaction_id_t id);

      void execute_item (value_handle_type handle, data::runtime_values_data data, runtime_transaction_id_t id);

      void remove_item (value_handle_type handle);

      static data_controler* get_controler ();

      void items_changed (const std::vector<std::pair<value_handle_type, rx_value> >& values);

      void result_received (value_handle_type handle, rx_result&& result, runtime_transaction_id_t id);

      void execute_result_received (value_handle_type handle, rx_result&& result, data::runtime_values_data data, runtime_transaction_id_t id);


  protected:

  private:


      rx::threads::physical_job_thread *my_worker_;

      sources_type sources_;

      registered_values_type registered_values_;


      locks::slim_lock data_lock_;

      named_sources_type named_sources_;

      uint16_t next_source_id_;

      std::vector<value_point_impl*> changed_points_;

      char token_buffer_[0x40];


};


} // namespace data_source
} // namespace sys_runtime
} // namespace rx_internal



#endif
