

/****************************************************************************
*
*  sys_internal\rx_internal_ns.h
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


#ifndef rx_internal_ns_h
#define rx_internal_ns_h 1



// rx_internal_objects
#include "sys_internal/rx_internal_objects.h"
// rx_host
#include "system/hosting/rx_host.h"
// rx_ns
#include "system/server/rx_ns.h"

#include "system/serialization/rx_serialization_defs.h"
#include "system/meta/rx_classes.h"
using namespace rx_platform::ns;


namespace sys_internal {

namespace internal_ns {






class platform_root : public rx_platform::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(platform_root);

	DECLARE_CODE_INFO("rx", 0,6,0, "\
root server directory:\r\n\
contains root server folders\
");

  public:
      platform_root();

      virtual ~platform_root();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class namespace_directory : public rx_platform::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(namespace_directory);


	DECLARE_CODE_INFO("rx", 0,5,0, "\
server directory:\r\n\
used to create system defined folders...\
");

  public:
      namespace_directory (const string_type& name);

      virtual ~namespace_directory();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class unassigned_directory : public rx_platform::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(unassigned_directory);


	DECLARE_CODE_INFO("rx", 0,5,0, "\
storing unassigned domains and applications\r\n\
All objects here are with unassigned state and hawing a bad quality\
");

  public:
      unassigned_directory();

      virtual ~unassigned_directory();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class world_directory : public rx_platform::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(world_directory);


	DECLARE_CODE_INFO("rx", 0,5,0, "\
user directory:\r\n\
used to create user defined folders...\
");

  public:
      world_directory();

      virtual ~world_directory();


      namespace_item_attributes get_attributes () const;


  protected:

  private:


};






class simple_platform_item : public rx_platform::ns::rx_platform_item  
{
	DECLARE_REFERENCE_PTR(simple_platform_item);

  public:
      simple_platform_item (const string_type& name, const rx_value& value, namespace_item_attributes attributes, const string_type& type_name, rx_time created_time);

      virtual ~simple_platform_item();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      values::rx_value get_value () const;

      namespace_item_attributes get_attributes () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool is_browsable () const;

      string_type get_name () const;

      size_t get_size () const;


      rx_time get_created_time () const
      {
        return created_time_;
      }



  protected:

  private:


      rx_value value_;

      namespace_item_attributes attributes_;

      string_type type_name_;

      rx_time created_time_;

      string_type name_;


};






template <class T, int class_name_idx>
class runtime_simple_platform_item : public rx_platform::ns::rx_platform_item  
{
	DECLARE_REFERENCE_PTR(runtime_simple_platform_item);

  public:
      runtime_simple_platform_item();

      runtime_simple_platform_item (const string_type& name, const rx_value& value, namespace_item_attributes attributes, const string_type& type_name);

      virtual ~runtime_simple_platform_item();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name ();

      values::rx_value get_value ();

      namespace_item_attributes get_attributes ();

      bool generate_json (std::ostream& def, std::ostream& err);

      bool is_browsable ();

      size_t get_size () const;


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

      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool is_browsable () const;

      rx_time get_created_time () const;

      string_type get_name () const;

      size_t get_size () const;

	  ~rx_item_implementation() = default;
  protected:

  private:


      TImpl impl_;


};






class storage_directory : public rx_platform::ns::rx_server_directory  
{
	DECLARE_REFERENCE_PTR(storage_directory);

	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
storage directory:\r\n\
used to interface storage objects...\
");

  public:
      storage_directory();

      virtual ~storage_directory();


      namespace_item_attributes get_attributes () const;

      void get_content (server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern) const;


  protected:

  private:


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

      bool generate_json (std::ostream& def, std::ostream& err) const;

      bool is_browsable () const;

      rx_time get_created_time () const;

      string_type get_name () const;

      size_t get_size () const;


  protected:

  private:


      TImpl impl_;


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
	return impl_->get_attributes();
}

template <class TImpl>
bool rx_item_implementation<TImpl>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_CLASS, 0);

	writer.start_object(RX_SER_ITEM_HEADER_NAME);
	{
		writer.write_string(RX_SER_ITEM_TYPE_NAME, impl_->get_type_name().c_str());
		writer.write_id(RX_SER_ITEM_ID_NAME, impl_->meta_data().get_id());
		writer.write_id(RX_SER_ITEM_ORIGIN_NAME, impl_->meta_data().get_parent());
		writer.write_string(RX_SER_ITEM_NAME_NAME, impl_->get_name().c_str());
		writer.start_object(impl_->get_type_name().c_str());
		{
			impl_->serialize_definition(writer, STREAMING_TYPE_CLASS);
		}
	}
	writer.end_object();

	writer.write_footer();

	string_type result;
	bool out = writer.get_string(result, true);

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return true;
}

template <class TImpl>
bool rx_item_implementation<TImpl>::is_browsable () const
{
	return impl_->is_browsable();
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
bool rx_meta_item_implementation<TImpl>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_CLASS, 0);

	bool out = false;

	writer.start_object(impl_->get_type_name().c_str());
	{
		out = impl_->serialize_definition(writer, STREAMING_TYPE_CLASS);
	}
	writer.end_object();

	writer.write_footer();

	string_type result;
	if (out)
	{
		out = writer.get_string(result, true);
	}

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return out;
}

template <class TImpl>
bool rx_meta_item_implementation<TImpl>::is_browsable () const
{
	return false;
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


} // namespace internal_ns
} // namespace sys_internal



#endif
