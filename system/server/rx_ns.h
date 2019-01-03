

/****************************************************************************
*
*  system\server\rx_ns.h
*
*  Copyright (c) 2018 Dusan Ciric
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


#ifndef rx_ns_h
#define rx_ns_h 1


#include "lib/rx_lock.h"

// cpp_lib
#include "system/libraries/cpp_lib.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_values
#include "lib/rx_values.h"

namespace rx_platform {
namespace ns {
class rx_platform_item;

} // namespace ns
} // namespace rx_platform



using namespace rx;
using namespace rx::values;
using namespace rx_platform;


namespace rx_platform {

namespace ns
{
class rx_server_directory;
class rx_platform_item;
}
namespace prog
{
class server_command_base;
}
typedef rx::pointers::reference<prog::server_command_base> server_command_base_ptr;
typedef rx::pointers::reference<ns::rx_platform_item> platform_item_ptr;
typedef rx::pointers::reference<ns::rx_server_directory> server_directory_ptr;
typedef std::vector<platform_item_ptr> server_items_type;
typedef std::vector<server_directory_ptr> server_directories_type;


namespace ns {

struct namespace_data_t
{
	namespace_data_t()
	{
	}
};

enum namespace_item_attributes
{
	namespace_item_null = 0,
	namespace_item_read_access = 1,
	namespace_item_write_access = 2,
	namespace_item_delete_access = 4,
	namespace_item_execute_access = 8,
	// type of item
	namespace_item_system = 0x10,
	namespace_item_internal = 0x20,
	// combinations
	namespace_item_full_access = 7,
	namespace_item_system_access = 0x11
};
void fill_attributes_string(namespace_item_attributes attr, string_type& str);

void fill_quality_string(values::rx_value val, string_type& q);







class rx_server_directory : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_server_directory);
	typedef std::map<string_type, rx_server_directory::smart_ptr>  sub_directories_type;
	typedef std::map<string_type, platform_item_ptr> sub_items_type;


  public:
      rx_server_directory();

      rx_server_directory (const string_type& name);

      virtual ~rx_server_directory();


      virtual void get_content (server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern) const;

      void structure_lock ();

      void structure_unlock ();

      server_directory_ptr get_parent () const;

      virtual server_directory_ptr get_sub_directory (const string_type& path) const;

      string_type get_path () const;

      string_type get_name () const;

      void fill_path (string_type& path) const;

      void set_parent (server_directory_ptr parent);

      virtual namespace_item_attributes get_attributes () const = 0;

      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual const string_type& get_type_name () const;

      virtual platform_item_ptr get_sub_item (const string_type& path) const;

      void structure_lock () const;

      void structure_unlock () const;

      virtual void get_value (rx_value& value);

      void fill_code_info (std::ostream& info);

      virtual void get_value (const string_type& name, rx_value& value);

      bool add_sub_directory (server_directory_ptr who);

      virtual void fill_code_info (std::ostream& info, const string_type& name) = 0;

      bool add_item (platform_item_ptr who);


      const rx_time get_created () const
      {
        return created_;
      }


	  template<class TImpl>
	  void add_item(TImpl who);
  protected:

  private:


      rx_reference<rx_server_directory> parent_;

      sub_directories_type sub_directories_;

      sub_items_type sub_items_;


      string_type name_;

      rx_time created_;

      rx::locks::slim_lock structure_lock_;


};







class rx_platform_item : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_item);
	
public:
	typedef std::map<string_type, platform_item_ptr> sub_items_type;

  public:
      rx_platform_item();

      virtual ~rx_platform_item();


      virtual void code_info_to_string (string_type& info);

      virtual void fill_code_info (std::ostream& info, const string_type& name);

      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual string_type get_type_name () const = 0;

      void lock ();

      void unlock ();

      virtual values::rx_value get_value () const = 0;

      virtual namespace_item_attributes get_attributes () const = 0;

      server_directory_ptr get_parent () const;

      void set_parent (server_directory_ptr parent);

      string_type get_path () const;

      virtual bool generate_json (std::ostream& def, std::ostream& err) const = 0;

      virtual bool serialize (base_meta_writer& stream) const;

      virtual bool deserialize (base_meta_reader& stream);

      platform_item_ptr get_sub_item (const string_type& path) const;

      virtual bool is_browsable () const = 0;

      virtual void get_content (server_items_type& sub_items, const string_type& pattern) const;

      virtual rx_time get_created_time () const = 0;

      virtual string_type get_name () const = 0;

      virtual size_t get_size () const = 0;

      virtual rx_node_id get_node_id () const = 0;


      const rx_reference<rx_platform_item> get_runtime_parent () const
      {
        return runtime_parent_;
      }



  protected:

  private:


      rx_reference<rx_server_directory> parent_;

      sub_items_type sub_items_;

      rx_reference<rx_platform_item> runtime_parent_;


      locks::lockable item_lock_;


};


} // namespace ns
} // namespace rx_platform



#endif
