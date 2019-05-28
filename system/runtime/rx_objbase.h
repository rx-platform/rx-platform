

/****************************************************************************
*
*  system\runtime\rx_objbase.h
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


#ifndef rx_objbase_h
#define rx_objbase_h 1


#include "protocols/ansi_c/common_c/rx_protocol_base.h"

// rx_io_buffers
#include "system/runtime/rx_io_buffers.h"
// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace runtime {
namespace objects {
class domain_runtime;
class application_runtime;

} // namespace objects
} // namespace runtime
} // namespace rx_platform


#include "system/callbacks/rx_callback.h"
using namespace rx;
using namespace rx_platform::ns;
using rx::values::rx_value;
using rx::values::rx_simple_value;


namespace rx_platform {

void split_item_path(const string_type& full_path, string_type& object_path, string_type& item_path);

typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
typedef std::stack<buffer_ptr, std::vector<buffer_ptr> > buffers_type;

namespace meta
{
namespace object_types
{
	class object_type;
	class application_type;
	class domain_type;
	class port_type;
}
namespace meta_algorithm
{
template <class typeT>
class object_types_algorithm;
}
}
namespace runtime {
namespace objects {
	class port_runtime;
	class object_runtime;

} // namespace object_types
}
typedef rx_reference<pointers::reference_object> rx_reference_ptr;
typedef rx_reference<runtime::objects::domain_runtime> rx_domain_ptr;
typedef rx_reference<runtime::objects::port_runtime> rx_port_ptr;
typedef rx_reference<runtime::objects::object_runtime> rx_object_ptr;
typedef rx_reference<runtime::objects::application_runtime> rx_application_ptr;



namespace runtime {

namespace objects {





class object_instance_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      rx_node_id domain_id;


  protected:

  private:


};


struct object_initialization_data
{
	rx_node_id domain_id;
	rx_node_id application_id;
};






class object_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
object class. basic implementation of an object");
	
	DECLARE_REFERENCE_PTR(object_runtime);

	friend class meta::object_types::object_type;
	template <class typeT>
	friend class meta::meta_algorithm::object_types_algorithm;

  public:
      object_runtime();

      object_runtime (const meta::meta_data& meta, const object_instance_data& instance);

      ~object_runtime();


      values::rx_value get_value () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      bool connect_domain (rx_domain_ptr&& domain);

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      platform_item_ptr get_item_ptr () const;

      string_type get_name () const;

      size_t get_size () const;

      meta::meta_data& meta_info ();

      rx_thread_handle_t get_executer () const;

      rx_result check_validity ();

      rx_item_type get_type () const;

      rx_result read_value (const string_type& path, rx_value& val) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result do_command (rx_object_command_t command_type);


      const object_instance_data& get_instance_data () const
      {
        return instance_data_;
      }



      static rx_item_type get_type_id ()
      {
        return type_id;
      }


      blocks::runtime_holder<object_runtime>& get_runtime ()
      {
        return runtime_;
      }


      const meta::meta_data& meta_info () const
      {
        return meta_info_;
      }



      static rx_item_type type_id;


  protected:

      rx_domain_ptr my_domain_;


  private:


      object_instance_data instance_data_;


      blocks::runtime_holder<object_runtime> runtime_;

      meta::meta_data meta_info_;


};






class domain_instance_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      rx_node_id app_id;

      int processor;

      rx_node_ids objects;


  protected:

  private:


};






class domain_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system domain class. basic implementation of a domain");

	DECLARE_REFERENCE_PTR(domain_runtime);
	typedef std::map<rx_node_id, object_runtime::smart_ptr> objects_type;

	friend class meta::object_types::domain_type;
	friend class object_runtime;
	template <class typeT>
	friend class meta::meta_algorithm::object_types_algorithm;
public:

  public:
      domain_runtime();

      domain_runtime (const meta::meta_data& meta, const domain_instance_data& instance);

      ~domain_runtime();


      rx_thread_handle_t get_executer () const;

      platform_item_ptr get_item_ptr () const;

      rx_item_type get_type () const;

      meta::meta_data& meta_info ();

      values::rx_value get_value () const;

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      string_type get_name () const;

      size_t get_size () const;

      bool connect_application (rx_application_ptr&& app);

      rx_result check_validity ();

      rx_result read_value (const string_type& path, rx_value& val) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      std::vector<rx_object_ptr> get_objects () const;

      rx_result do_command (rx_object_command_t command_type);


      const domain_instance_data& get_instance_data () const
      {
        return instance_data_;
      }



      static rx_item_type get_type_id ()
      {
        return type_id;
      }


      blocks::runtime_holder<domain_runtime>& get_runtime ()
      {
        return runtime_;
      }


      const meta::meta_data& meta_info () const
      {
        return meta_info_;
      }



      static rx_item_type type_id;


  protected:

  private:


      objects_type objects_;

      rx_application_ptr my_application_;

      domain_instance_data instance_data_;


      rx_thread_handle_t executer_;

      blocks::runtime_holder<domain_runtime> runtime_;

      meta::meta_data meta_info_;

      locks::slim_lock objects_lock_;


};






class application_instance_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      int processor;


  protected:

  private:


};






class port_instance_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


      rx_node_id app_id;


  protected:

  private:


};







class port_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system port class. basic implementation of a port");

	DECLARE_REFERENCE_PTR(port_runtime);

	friend class meta::object_types::port_type;
	template <class typeT>
	friend class meta::meta_algorithm::object_types_algorithm;

  public:
      port_runtime();

      port_runtime (const meta::meta_data& meta, const port_instance_data& instance);

      ~port_runtime();


      bool write (buffer_ptr what);

      platform_item_ptr get_item_ptr () const;

      virtual rx_protocol_stack_entry* get_stack_entry ();

      rx_result_with<io_types::rx_io_buffer> allocate_io_buffer (size_t initial_capacity = 0);

      rx_item_type get_type () const;

      meta::meta_data& meta_info ();

      values::rx_value get_value () const;

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      string_type get_name () const;

      size_t get_size () const;

      rx_thread_handle_t get_executer () const;

      rx_result check_validity ();

      rx_result read_value (const string_type& path, rx_value& val) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      bool connect_application (rx_application_ptr&& app);

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result do_command (rx_object_command_t command_type);


      const port_instance_data& get_instance_data () const
      {
        return instance_data_;
      }



      static rx_item_type get_type_id ()
      {
        return type_id;
      }


      blocks::runtime_holder<port_runtime>& get_runtime ()
      {
        return runtime_;
      }


      const meta::meta_data& meta_info () const
      {
        return meta_info_;
      }



      static rx_item_type type_id;


  protected:

      virtual bool readed (buffer_ptr what, rx_thread_handle_t destination);

      void update_received_counters (size_t count);


  private:


      rx_application_ptr my_application_;

      port_instance_data instance_data_;


      blocks::runtime_holder<port_runtime> runtime_;

      meta::meta_data meta_info_;

      runtime_handle_t rx_bytes_item_;

      runtime_handle_t tx_bytes_item_;

      runtime_handle_t rx_packets_item_;

      runtime_handle_t tx_packets_item_;


};






class application_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system application class. basic implementation of a application");

	DECLARE_REFERENCE_PTR(application_runtime);
	typedef std::vector<domain_runtime::smart_ptr> domains_type;
	typedef std::vector<port_runtime::smart_ptr> ports_type;

	friend class meta::object_types::application_type;
	template <class typeT>
	friend class meta::meta_algorithm::object_types_algorithm;

  public:
      application_runtime();

      application_runtime (const meta::meta_data& meta, const application_instance_data& instance);

      ~application_runtime();


      platform_item_ptr get_item_ptr () const;

      rx_item_type get_type () const;

      meta::meta_data& meta_info ();

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      string_type get_name () const;

      size_t get_size () const;

      rx_thread_handle_t get_executer () const;

      rx_result check_validity ();

      rx_result read_value (const string_type& path, rx_value& val) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      values::rx_value get_value () const;

      void fill_data (const data::runtime_values_data& data);

      void collect_data (data::runtime_values_data& data) const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result do_command (rx_object_command_t command_type);


      const application_instance_data& get_instance_data () const
      {
        return instance_data_;
      }



      static rx_item_type get_type_id ()
      {
        return type_id;
      }


      blocks::runtime_holder<application_runtime>& get_runtime ()
      {
        return runtime_;
      }


      const meta::meta_data& meta_info () const
      {
        return meta_info_;
      }



      static rx_item_type type_id;


  protected:

  private:


      domains_type domains_;

      ports_type ports_;

      application_instance_data instance_data_;


      rx_thread_handle_t executer_;

      blocks::runtime_holder<application_runtime> runtime_;

      meta::meta_data meta_info_;


};


} // namespace objects
} // namespace runtime
} // namespace rx_platform



#endif
