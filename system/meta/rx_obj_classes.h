

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



// rx_ptr
#include "lib/rx_ptr.h"
// rx_logic
#include "system/logic/rx_logic.h"



namespace rx_platform {
namespace objects
{
	class object_runtime;
	class struct_runtime;
}

namespace meta {

template<class T>
bool generate_json(T whose, std::ostream& def, std::ostream& err)
{
	rx_platform::serialization::json_writter writter;

	writter.write_header(STREAMING_TYPE_OBJECT);

	bool out = whose->serialize_definition(writter, STREAMING_TYPE_OBJECT);

    string_type result;
	if (out)
	{
		writter.write_footer();

		out = writter.get_string(result, true);
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


      bool serialize_object_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_object_definition (base_meta_reader& stream, uint8_t type);

      void construct (complex_runtime_ptr what);

      void construct (objects::object_runtime_ptr what);


  protected:

  private:


      programs_type programs_;


};






class object_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(object_class);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
basic object class.\r\n\
basic implementation of object class");
public:
	typedef objects::object_runtime RType;
	typedef objects::complex_runtime_item CType;

	friend class meta_helpers;

  public:
      object_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~object_class();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      namespace_item_attributes get_attributes () const;

      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }


      complex_data_type& complex_data ()
      {
        return complex_data_;
      }



  protected:

  private:
      object_class();



      static string_type type_name;

      checkable_data meta_data_;

      complex_data_type complex_data_;


};

typedef pointers::reference<object_class> object_class_ptr;





class domain_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(domain_class);
public:
	typedef objects::domain_runtime RType;
	typedef objects::complex_runtime_item CType;

	friend class meta_helpers;

  public:
      domain_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~domain_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }


      complex_data_type& complex_data ()
      {
        return complex_data_;
      }



      static string_type type_name;


  protected:

  private:
      domain_class();



      checkable_data meta_data_;

      complex_data_type complex_data_;


};

typedef domain_class::smart_ptr domain_class_ptr;





class application_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(application_class);
public:
	typedef objects::application_runtime RType;
	typedef objects::complex_runtime_item CType;

	friend class meta_helpers;

  public:
      application_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~application_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }


      complex_data_type& complex_data ()
      {
        return complex_data_;
      }



      static string_type type_name;


  protected:

  private:
      application_class();



      checkable_data meta_data_;

      complex_data_type complex_data_;


};

typedef application_class::smart_ptr application_class_ptr;





class port_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(port_class);
public:
	typedef objects::port_runtime RType;
	typedef objects::complex_runtime_item CType;

	friend class meta_helpers;

  public:
      port_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~port_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      const checkable_data& meta_data () const;

      bool serialize_definition (base_meta_writter& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      static string_type get_type_name ()
      {
        return type_name;
      }


      checkable_data& meta_data ()
      {
        return meta_data_;
      }


      complex_data_type& complex_data ()
      {
        return complex_data_;
      }



      static string_type type_name;


  protected:

  private:
      port_class();



      checkable_data meta_data_;

      complex_data_type complex_data_;


};


} // namespace meta
} // namespace rx_platform



#endif
