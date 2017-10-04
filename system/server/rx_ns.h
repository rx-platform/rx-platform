

/****************************************************************************
*
*  system\server\rx_ns.h
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


#ifndef rx_ns_h
#define rx_ns_h 1


#include "lib/rx_lock.h"

// rx_ptr
#include "lib/rx_ptr.h"
// rx_values
#include "lib/rx_values.h"
// cpp_lib
#include "system/libraries/cpp_lib.h"

namespace server {
namespace ns {
class rx_server_item;

} // namespace ns
} // namespace server



using namespace rx;
using namespace rx::values;
using namespace server;


namespace server {

namespace ns
{
class rx_server_directory;
class rx_server_item;
}
namespace prog
{
class server_command_base;
}
typedef rx::pointers::virtual_reference<prog::server_command_base> server_command_base_ptr;
typedef rx::pointers::virtual_reference<ns::rx_server_item> server_item_ptr;
typedef rx::pointers::reference<ns::rx_server_directory> server_directory_ptr;
typedef std::vector<server_item_ptr> server_items_type;
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
	namespace_item_read_access = 1,
	namespace_item_write_access = 2,
	namespace_item_delete_access = 4,
	namespace_item_execute = 8,
	namespace_item_system = 0x10,
	namespace_item_command = 0x20,
	namespace_item_script = 0x40,
	namespace_item_class = 0x80,
	namespace_item_object = 0x100,
	namespace_item_variable = 0x200,
	namespace_item_application = 0x400,
	namespace_item_domain = 0x800,
	namespace_item_port = 0x1000,

	namespace_item_system_const_value = 0xfffff211,
	namespace_item_system_value = 0xfffff111,

	namespace_item_browsable = (namespace_item_object | namespace_item_application | namespace_item_domain| namespace_item_port)
};

void fill_namepsace_string(namespace_item_attributes attr, string_type& str);

void fill_quality_string(values::rx_value val, string_type& q);







class rx_server_directory : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_server_directory);
	typedef std::map<string_type, rx_server_directory::smart_ptr>  sub_directories_type;
	typedef std::map<string_type, server_item_ptr> sub_items_type;


  public:
      rx_server_directory();

      rx_server_directory (const string_type& name);

      rx_server_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items);

      virtual ~rx_server_directory();


      virtual void get_content (server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern) const;

      void structure_lock ();

      void structure_unlock ();

      server_directory_ptr get_parent () const;

      virtual server_directory_ptr get_sub_directory (const string_type& path) const;

      string_type get_path () const;

      string_type get_name (bool plain = false) const;

      void fill_path (string_type& path) const;

      void set_parent (server_directory_ptr parent);

      virtual namespace_item_attributes get_attributes () const = 0;

      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual const string_type& get_type_name () const;

      virtual server_item_ptr get_sub_item (const string_type& path) const;

      void structure_lock () const;

      void structure_unlock () const;

      virtual void get_value (rx_value& value);

      void fill_code_info (std::ostream& info);

      virtual void get_value (const string_type& name, rx_value& value);

      bool add_sub_directory (server_directory_ptr who);

      virtual void fill_code_info (std::ostream& info, const string_type& name) = 0;

      virtual bool generate_json (std::ostream& def, std::ostream& err) = 0;


      const rx_time get_created () const
      {
        return m_created;
      }



  protected:

  private:


      rx_server_directory::smart_ptr m_parent;

      sub_directories_type m_sub_directories;

      sub_items_type m_sub_items;


      string_type m_name;

      rx_time m_created;

      rx::locks::slim_lock m_structure_lock;


};







class rx_server_item : public rx::pointers::virtual_reference_object  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(rx_server_item);
		

  public:
      rx_server_item();

      virtual ~rx_server_item();


      virtual void code_info_to_string (string_type& info);

      virtual void fill_code_info (std::ostream& info, const string_type& name) = 0;

      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info) = 0;

      virtual string_type get_type_name () const = 0;

      void item_lock ();

      void item_unlock ();

      void item_lock () const;

      void item_unlock () const;

      virtual void get_value (values::rx_value& val) const = 0;

      virtual namespace_item_attributes get_attributes () const = 0;

      virtual const string_type& get_item_name () const = 0;

      server_directory_ptr get_parent () const;

      void set_parent (server_directory_ptr parent);

      string_type get_path () const;

      virtual bool generate_json (std::ostream& def, std::ostream& err) const = 0;

      virtual bool serialize (base_meta_writter& stream) const;

      virtual bool deserialize (base_meta_reader& stream);


  protected:

  private:


      rx_server_directory::smart_ptr m_parent;


      locks::lockable m_item_lock;


};


} // namespace ns
} // namespace server



#endif
