

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



// rx_classes
#include "system/meta/rx_classes.h"
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

	whose->serialize_definition(writter, STREAMING_TYPE_OBJECT);

	writter.write_footer();

	string_type result;
	bool out = writter.get_string(result, true);

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return out;
}




template <class complexT>
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






typedef meta_type_adapter< rx_platform::meta::object_class , false , complex_data_type , mapped_data_type<complex_data_type> , not_implemented , object_data_type<complex_data_type>  > object_class_t;






class object_class : public rx::pointers::reference_object, 
                     	public object_class_t  
{
	DECLARE_REFERENCE_PTR(object_class);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
basic object class.\r\n\
basic implementation of object class");
public:
	typedef objects::object_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      object_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~object_class();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      namespace_item_attributes get_attributes () const;

      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();


      static string_type type_name;


  protected:

  private:
      object_class();



};

typedef pointers::reference<object_class> object_class_ptr;





typedef meta_type_adapter< rx_platform::meta::domain_class , false , complex_data_type , mapped_data_type<complex_data_type> , not_implemented , object_data_type<complex_data_type>  > domain_class_t;






class domain_class : public rx::pointers::reference_object, 
                     	public domain_class_t  
{
	DECLARE_REFERENCE_PTR(domain_class);
public:
	typedef objects::domain_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      domain_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~domain_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();


      static string_type type_name;


  protected:

  private:
      domain_class();



};

typedef domain_class::smart_ptr domain_class_ptr;





typedef meta_type_adapter< rx_platform::meta::application_class , false , complex_data_type , mapped_data_type<complex_data_type> , not_implemented , object_data_type<complex_data_type>  > application_class_t;






class application_class : public rx::pointers::reference_object, 
                          	public application_class_t  
{
	DECLARE_REFERENCE_PTR(application_class);
public:
	typedef objects::application_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      application_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~application_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();


      static string_type type_name;


  protected:

  private:
      application_class();



};

typedef application_class::smart_ptr application_class_ptr;





typedef meta_type_adapter< rx_platform::meta::port_class , false , complex_data_type , mapped_data_type<complex_data_type> , not_implemented , object_data_type<complex_data_type>  > port_class_t;






class port_class : public rx::pointers::reference_object, 
                   	public port_class_t  
{
	DECLARE_REFERENCE_PTR(port_class);
public:
	typedef objects::port_runtime RType;
	typedef objects::complex_runtime_item CType;

  public:
      port_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~port_class();


      void construct (objects::object_runtime_ptr what);

      void construct (complex_runtime_ptr what);

      platform_item_ptr get_item_ptr ();


      static string_type type_name;


  protected:

  private:
      port_class();



};


} // namespace meta
} // namespace rx_platform



#endif
