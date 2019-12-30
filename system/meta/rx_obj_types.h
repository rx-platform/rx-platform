

/****************************************************************************
*
*  system\meta\rx_obj_types.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef rx_obj_types_h
#define rx_obj_types_h 1


// rx_logic
#include "system/logic/rx_logic.h"

// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_meta_algorithm
#include "system/meta/rx_meta_algorithm.h"
// rx_meta_support
#include "system/meta/rx_meta_support.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "system/runtime/rx_relations.h"
using rx_platform::meta::construct_context;


namespace rx_platform {

namespace meta {

namespace object_types {





class relation_attribute 
{
  public:
	relation_attribute(const relation_attribute& right) = default;
	relation_attribute(relation_attribute&& right) = default;
	relation_attribute() = default;
	~relation_attribute() = default;

  public:
      relation_attribute (const string_type& name, const rx_node_id& id);

      relation_attribute (const string_type& name, const string_type& target_name);


      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      rx_result check (type_check_context& ctx);

      rx_result_with<runtime::relation_runtime_ptr> construct (construct_context& ctx) const;


      const string_type& get_name () const
      {
        return name_;
      }


      rx_item_reference get_relation_type () const
      {
        return relation_type_;
      }


      rx_item_reference get_target () const
      {
        return target_;
      }



  protected:

  private:


      string_type name_;

      rx_item_reference relation_type_;

      rx_item_reference target_;


    friend class meta_algorithm::relation_blocks_algorithm;
};


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
	typedef std::vector<program_runtime_ptr> programs_type;
	typedef std::vector<relation_attribute> relations_type;
	//typedef std::vector<int> programs_type;

  public:
      object_data_type();

      object_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false, bool sealed = false, bool abstract = false);


      rx_result serialize_object_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_object_definition (base_meta_reader& stream, uint8_t type);

      rx_result construct (runtime::blocks::runtime_holder& what, construct_context& ctx) const;

      bool check_type (type_check_context& ctx);

      rx_result resolve (rx_directory_ptr dir);


      const bool is_constructable () const
      {
        return constructable_;
      }



  protected:

  private:


      programs_type programs_;

      relations_type relations_;


      bool constructable_;


};







class application_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(application_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of application type");
public:
    static constexpr bool has_default_constructor = true;
	typedef runtime::objects::application_runtime RType;
	typedef runtime::objects::application_runtime::smart_ptr RTypePtr;
	typedef runtime::objects::application_instance_data instance_data_t;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      application_type();

      application_type (const object_type_creation_data& data);

      ~application_type();


      rx_result construct (rx_application_ptr& what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr () const;

      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      meta_data& meta_info ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      rx_result check_type (type_check_context& ctx);

      static void set_runtime_data (runtime_data_prototype& prototype, RTypePtr where);

      static void set_instance_data (instance_data_t&& data, RTypePtr where);


      const object_data_type& object_data () const;

      const meta_data& meta_info () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


      object_data_type object_data_;

      meta_data meta_info_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};







class domain_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(domain_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of domain type");
public:
    static constexpr bool has_default_constructor = true;
	typedef runtime::objects::domain_runtime RType;
	typedef typename runtime::objects::domain_runtime::smart_ptr RTypePtr;
	typedef runtime::objects::domain_instance_data instance_data_t;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      domain_type();

      domain_type (const object_type_creation_data& data);

      ~domain_type();


      rx_result construct (rx_domain_ptr what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr () const;

      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      meta_data& meta_info ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);

      static void set_runtime_data (runtime_data_prototype& prototype, RTypePtr where);

      static void set_instance_data (instance_data_t&& data, RTypePtr where);


      const object_data_type& object_data () const;

      const meta_data& meta_info () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


      object_data_type object_data_;

      meta_data meta_info_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};







class object_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(object_type);
	DECLARE_CODE_INFO("rx", 0, 5, 2, "\
implementation of object type");
public:
    static constexpr bool has_default_constructor = true;
	typedef runtime::objects::object_runtime RType;
	typedef runtime::object_runtime_ptr RTypePtr;
	typedef runtime::objects::object_instance_data instance_data_t;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      object_type();

      object_type (const object_type_creation_data& data);

      ~object_type();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      rx_result construct (runtime::object_runtime_ptr what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr () const;

      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      meta_data& meta_info ();

      def_blocks::complex_data_type& complex_data ();

      static void set_runtime_data (runtime_data_prototype& prototype, RTypePtr where);

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);

      static void set_instance_data (instance_data_t&& data, RTypePtr where);


      const object_data_type& object_data () const;

      const meta_data& meta_info () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


      object_data_type object_data_;

      meta_data meta_info_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};







class port_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(port_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of port type");
public:
    static constexpr bool has_default_constructor = false;
	typedef runtime::objects::port_runtime RType;
	typedef runtime::objects::port_runtime::smart_ptr RTypePtr;
	typedef runtime::objects::port_instance_data instance_data_t;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;

  public:
      port_type();

      port_type (const object_type_creation_data& data);

      ~port_type();


      rx_result construct (rx_port_ptr what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr () const;

      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      meta_data& meta_info ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);

      static void set_runtime_data (runtime_data_prototype& prototype, RTypePtr where);

      static void set_instance_data (instance_data_t&& data, RTypePtr where);


      const object_data_type& object_data () const;

      const meta_data& meta_info () const;

      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


      object_data_type object_data_;

      meta_data meta_info_;

      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;


};






class relation_data_type 
{

  public:

      rx::data::runtime_values_data& get_overrides () const;


  protected:

  private:


};






class relation_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(relation_type);
	DECLARE_CODE_INFO("rx", 0, 2, 0, "\
relation is now full blown object!");
public:
	typedef runtime::relation_runtime_ptr RDataType;
	typedef runtime::relations::relation_runtime RType;
	typedef runtime::relations::relation_instance_data instance_data_t;
	typedef runtime::objects::domain_instance_data domain_instance_data_t;
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;
	typedef runtime::relation_runtime_ptr RTypePtr;

  public:
      relation_type();

      relation_type (const object_type_creation_data& data);


      platform_item_ptr get_item_ptr () const;

      rx_result serialize_definition (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize_definition (base_meta_reader& stream, uint8_t type);

      meta_data& meta_info ();

      rx_result_with<runtime::relation_runtime_ptr> construct (runtime::relation_runtime_ptr what, construct_context& ctx) const;

      bool check_type (type_check_context& ctx);

      static void set_runtime_data (runtime_data_prototype& prototype, RTypePtr where);

      static void set_instance_data (instance_data_t&& data, RTypePtr where);


      const meta_data& meta_info () const;

      const relation_data_type& complex_data () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }


      rx_item_reference get_inverse_reference () const
      {
        return inverse_reference_;
      }


      bool get_hierarchical () const
      {
        return hierarchical_;
      }



      static rx_item_type type_id;


  protected:

  private:


      meta_data meta_info_;

      relation_data_type complex_data_;


      rx_item_reference inverse_reference_;

      bool hierarchical_;


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
typedef pointers::reference<object_types::relation_type> relation_type_ptr;

}

typedef pointers::reference<meta::object_types::object_type> rx_object_type_ptr;
typedef pointers::reference<meta::object_types::domain_type> rx_domain_type_ptr;
typedef pointers::reference<meta::object_types::application_type> rx_application_type_ptr;
typedef pointers::reference<meta::object_types::port_type> rx_port_type_ptr;
typedef pointers::reference<meta::object_types::relation_type> relation_type_ptr;
}


#endif
