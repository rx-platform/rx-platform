

/****************************************************************************
*
*  system\meta\rx_obj_types.h
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


#ifndef rx_obj_types_h
#define rx_obj_types_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_logic
#include "system/logic/rx_logic.h"
// rx_meta_algorithm
#include "system/meta/rx_meta_algorithm.h"
// rx_meta_support
#include "system/meta/rx_meta_support.h"
// rx_checkable
#include "system/meta/rx_checkable.h"
// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_ptr
#include "lib/rx_ptr.h"

using rx_platform::meta::construct_context;


namespace rx_platform {

namespace meta {

namespace object_types {

template<class T>
bool generate_json(T whose, std::ostream& def, std::ostream& err)
{
	rx_platform::serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_OBJECT, 0);

	bool out = whose->serialize_definition(writer, STREAMING_TYPE_OBJECT);

    string_type result;
	if (out)
	{
		writer.write_footer();

		out = writer.get_string(result, true);
	}

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return out;
}





class object_data_type 
{
	typedef std::vector<logic::program_runtime_ptr> programs_type;
	//typedef std::vector<int> programs_type;

  public:
      object_data_type();

      object_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);


      bool serialize_object_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_object_definition (base_meta_reader& stream, uint8_t type);

      void construct (runtime::object_runtime_ptr what, construct_context& ctx) const;

      bool check_type (type_check_context& ctx);


      const bool is_constructible () const
      {
        return constructible_;
      }



  protected:

  private:


      programs_type programs_;


      bool constructible_;


};







class application_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(application_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of application type");
public:
	typedef runtime::objects::application_runtime RType;
	typedef runtime::rx_application_ptr RTypePtr;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      application_type (const object_type_creation_data& data);

      virtual ~application_type();


      void construct (runtime::rx_application_ptr& what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);


      const object_data_type& object_data () const;

      const checkable_data& meta_data () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      application_type();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};







class domain_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(domain_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of domain type");
public:
	typedef runtime::objects::domain_runtime RType;
	typedef typename runtime::objects::domain_runtime::smart_ptr RTypePtr;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      domain_type (const object_type_creation_data& data);

      virtual ~domain_type();


      void construct (runtime::rx_domain_ptr what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);


      const object_data_type& object_data () const;

      const checkable_data& meta_data () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      domain_type();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};







class object_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(object_type);
	DECLARE_CODE_INFO("rx", 0, 5, 2, "\
implementation of object type");
public:
	typedef runtime::objects::object_runtime RType;
	typedef runtime::object_runtime_ptr RTypePtr;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      object_type (const object_type_creation_data& data);

      virtual ~object_type();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      void construct (runtime::object_runtime_ptr what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      static void set_object_runtime_data (runtime_data_prototype& prototype, RTypePtr where);

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);


      const object_data_type& object_data () const;

      const checkable_data& meta_data () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      object_type();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};







class port_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(port_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of port type");
public:
	typedef runtime::objects::port_runtime RType;
	typedef runtime::objects::port_runtime::smart_ptr RTypePtr;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      port_type (const object_type_creation_data& data);

      virtual ~port_type();


      void construct (runtime::rx_port_ptr what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);


      const object_data_type& object_data () const;

      const checkable_data& meta_data () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      port_type();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};


} // namespace object_types
} // namespace meta
} // namespace rx_platform

namespace rx_platform
{
namespace meta
{

typedef pointers::reference<object_types::object_type> object_type_ptr;
typedef pointers::reference<object_types::domain_type> domain_type_ptr;
typedef pointers::reference<object_types::application_type> application_type_ptr;
typedef pointers::reference<object_types::port_type> port_type_ptr;

}
}


#endif
