

/****************************************************************************
*
*  system\meta\rx_obj_classes.h
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


#ifndef rx_obj_classes_h
#define rx_obj_classes_h 1



// rx_logic
#include "system/logic/rx_logic.h"
// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_classes
#include "system/meta/rx_classes.h"
// rx_objbase
#include "system/meta/rx_objbase.h"
// rx_ptr
#include "lib/rx_ptr.h"



namespace rx_platform {

namespace meta {

namespace object_defs {

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

      void construct (objects::object_runtime_ptr what);


      const bool is_creatable () const
      {
        return creatable_;
      }



  protected:

  private:


      programs_type programs_;


      bool creatable_;


};






class application_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(application_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of application type");
public:
	typedef objects::object_types::application_runtime RType;
	typedef objects::application_runtime_ptr RTypePtr;
	typedef objects::blocks::complex_runtime_item CType;

	friend class obj_meta_helpers;

  public:
      application_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);

      virtual ~application_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
      application_class();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};






class domain_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(domain_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of domain type");
public:
	typedef objects::object_types::domain_runtime RType;
	typedef objects::domain_runtime_ptr RTypePtr;
	typedef objects::blocks::complex_runtime_item CType;

	friend class obj_meta_helpers;

  public:
      domain_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);

      virtual ~domain_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
      domain_class();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};






class object_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(object_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of object type");
public:
	typedef objects::object_types::object_runtime RType;
	typedef objects::object_runtime_ptr RTypePtr;
	typedef objects::blocks::complex_runtime_item CType;

	friend class obj_meta_helpers;

  public:
      object_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);

      virtual ~object_class();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      void construct (objects::object_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      static objects::object_runtime_ptr create_runtime_ptr ();

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


      const object_data_type& object_data () const;

      const checkable_data& meta_data () const;

      const def_blocks::complex_data_type& complex_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



  protected:

  private:
      object_class();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


      static string_type type_name;


};






class port_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(port_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of port type");
public:
	typedef objects::object_types::port_runtime RType;
	typedef objects::port_runtime_ptr RTypePtr;
	typedef objects::blocks::complex_runtime_item CType;

	friend class obj_meta_helpers;

  public:
      port_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);

      virtual ~port_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
      port_class();



      object_data_type object_data_;

      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};


} // namespace object_defs
} // namespace meta
} // namespace rx_platform

namespace rx_platform
{
namespace meta
{

typedef pointers::reference<object_defs::object_class> object_class_ptr;
typedef pointers::reference<object_defs::domain_class> domain_class_ptr;
typedef pointers::reference<object_defs::application_class> application_class_ptr;
typedef pointers::reference<object_defs::port_class> port_class_ptr;

}
}


#endif
