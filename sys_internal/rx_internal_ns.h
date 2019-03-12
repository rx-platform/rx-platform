

/****************************************************************************
*
*  sys_internal\rx_internal_ns.h
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_internal_ns_h
#define rx_internal_ns_h 1



// rx_host
#include "system/hosting/rx_host.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_internal_objects
#include "sys_internal/rx_internal_objects.h"

#include "terminal/rx_terminal_style.h"
#include "system/server/rx_server.h"
#include "system/serialization/rx_serialization_defs.h"
#include "system/meta/rx_types.h"
using namespace rx_platform::ns;


namespace sys_internal {

namespace internal_ns {






class platform_root : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(platform_root);

	DECLARE_CODE_INFO("rx", 0,6,0, "\
root server directory:\r\n\
contains root server folders\
");

  public:
      platform_root();

      ~platform_root();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class user_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(user_directory);
	DECLARE_CODE_INFO("rx", 0,5,0, "\
server directory:\r\n\
used to create user defined folders...\
");

  public:
      user_directory (const string_type& name);

      ~user_directory();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class unassigned_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(unassigned_directory);
	DECLARE_CODE_INFO("rx", 0,5,0, "\
storing unassigned domains and applications\r\n\
All objects here are with unassigned state and hawing a bad quality\
");

  public:
      unassigned_directory();

      ~unassigned_directory();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class world_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(world_directory);
	DECLARE_CODE_INFO("rx", 0,5,0, "\
user directory:\r\n\
used to create user defined folders...\
");

  public:
      world_directory();

      ~world_directory();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






template <class TImpl>
class rx_item_implementation : public rx_platform::ns::rx_platform_item  
{
	DECLARE_REFERENCE_PTR(rx_item_implementation);

  public:
      rx_item_implementation (TImpl impl);


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      values::rx_value get_value () const;

      namespace_item_attributes get_attributes () const;

      rx_result generate_json (std::ostream& def, std::ostream& err) const;

      rx_time get_created_time () const;

      string_type get_name () const;

      size_t get_size () const;

      rx_node_id get_node_id () const;

	  ~rx_item_implementation() = default;
  protected:

  private:


      TImpl impl_;


};






template <class TImpl>
class rx_meta_item_implementation : public rx_platform::ns::rx_platform_item  
{

  public:
      rx_meta_item_implementation (TImpl impl);


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      values::rx_value get_value () const;

      namespace_item_attributes get_attributes () const;

      rx_result generate_json (std::ostream& def, std::ostream& err) const;

      rx_time get_created_time () const;

      string_type get_name () const;

      size_t get_size () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      rx_node_id get_node_id () const;


  protected:

  private:


      TImpl impl_;


};






class internal_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(internal_directory);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
server directory:\r\n\
used to create system defined folders...\
");

  public:
      internal_directory (const string_type& name);

      ~internal_directory();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};


// Parameterized Class sys_internal::internal_ns::rx_item_implementation 

template <class TImpl>
rx_item_implementation<TImpl>::rx_item_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
void rx_item_implementation<TImpl>::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

template <class TImpl>
string_type rx_item_implementation<TImpl>::get_type_name () const
{
	return impl_->get_type_name();
}

template <class TImpl>
values::rx_value rx_item_implementation<TImpl>::get_value () const
{
	return impl_->get_value();
}

template <class TImpl>
namespace_item_attributes rx_item_implementation<TImpl>::get_attributes () const
{
	return impl_->meta_data().get_attributes();
}

template <class TImpl>
rx_result rx_item_implementation<TImpl>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	impl_->serialize_definition(writer, STREAMING_TYPE_OBJECT);

	writer.write_footer();

	string_type result;
	bool out = writer.get_string(result, true);

	if (out)
		def << result;
	else
		def << "\r\n" ANSI_COLOR_RED "Error in JSON deserialization." ANSI_COLOR_RESET "\r\n";

	return true;
}

template <class TImpl>
rx_time rx_item_implementation<TImpl>::get_created_time () const
{
	return impl_->get_created_time();
}

template <class TImpl>
string_type rx_item_implementation<TImpl>::get_name () const
{
	return impl_->get_name();
}

template <class TImpl>
size_t rx_item_implementation<TImpl>::get_size () const
{
	return impl_->get_size();
}

template <class TImpl>
rx_node_id rx_item_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_data().get_id();
}


// Parameterized Class sys_internal::internal_ns::rx_meta_item_implementation 

template <class TImpl>
rx_meta_item_implementation<TImpl>::rx_meta_item_implementation (TImpl impl)
      : impl_(impl)
{
}



template <class TImpl>
void rx_meta_item_implementation<TImpl>::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

template <class TImpl>
string_type rx_meta_item_implementation<TImpl>::get_type_name () const
{
	return impl_->get_type_name();
}

template <class TImpl>
values::rx_value rx_meta_item_implementation<TImpl>::get_value () const
{
	return impl_->meta_data().get_value();
}

template <class TImpl>
namespace_item_attributes rx_meta_item_implementation<TImpl>::get_attributes () const
{
	return impl_->meta_data().get_attributes();
}

template <class TImpl>
rx_result rx_meta_item_implementation<TImpl>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_TYPE, 0);

	bool out = false;

	out = impl_->serialize_definition(writer, STREAMING_TYPE_TYPE);

	writer.write_footer();

	string_type result;
	if (out)
	{
		out = writer.get_string(result, true);
	}

	if (out)
		def << result;
	else
		def << "\r\n" ANSI_COLOR_RED "Error in JSON deserialization." ANSI_COLOR_RESET "\r\n";

	return out;
}

template <class TImpl>
rx_time rx_meta_item_implementation<TImpl>::get_created_time () const
{
	return impl_->meta_data().get_created_time();
}

template <class TImpl>
string_type rx_meta_item_implementation<TImpl>::get_name () const
{
	return impl_->meta_data().get_name();
}

template <class TImpl>
size_t rx_meta_item_implementation<TImpl>::get_size () const
{
	return sizeof(*this);
}

template <class TImpl>
void rx_meta_item_implementation<TImpl>::fill_code_info (std::ostream& info, const string_type& name)
{
	impl_->fill_code_info(info, name);
}

template <class TImpl>
rx_node_id rx_meta_item_implementation<TImpl>::get_node_id () const
{
	return impl_->meta_data().get_id();
}


} // namespace internal_ns
} // namespace sys_internal



#endif

