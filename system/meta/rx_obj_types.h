

/****************************************************************************
*
*  system\meta\rx_obj_types.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_obj_types_h
#define rx_obj_types_h 1



#include "system/server/rx_log.h"
// rx_logic
#include "system/logic/rx_logic.h"

// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_meta_data
#include "lib/rx_meta_data.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "system/runtime/rx_rt_struct.h"
#include "system/meta/rx_runtime_data.h"
using namespace rx_platform::meta::def_blocks;
namespace rx_internal
{
namespace sys_runtime
{
namespace runtime_core
{
namespace runtime_data
{
class application_instance_data;
class port_instance_data;
class domain_instance_data;
class object_instance_data;
class port_behaviors;
}
}
}
}


namespace rx_platform {

namespace meta {
class construct_context;
class type_check_context;
class runtime_data_prototype;
using ::rx_platform::meta::construct_context;
namespace meta_algorithm
{
class relation_type_algorithm;
template<class typeT>
class object_types_algorithm;
template<class typeT>
class object_data_algorithm;
}

namespace object_types {





class relation_attribute 
{
public:
    typedef rx_platform::meta::object_types::relation_type TargetType;
    friend class meta_algorithm::relation_blocks_algorithm;
    typedef meta_algorithm::relation_blocks_algorithm AlgorithmType;

    relation_attribute(const relation_attribute& right) = default;
    relation_attribute(relation_attribute&& right) = default;
    relation_attribute() = default;
    ~relation_attribute() = default;

  public:

      string_type name;

      rx_item_reference relation_type;

      rx_item_reference target;

      string_type description;

      string_type value;


  protected:

  private:


};


//template<class T>
//bool generate_json(T whose, std::ostream& def, std::ostream& err)
//{
//	rx_platform::serialization::pretty_json_writer writer;
//
//	writer.write_header(STREAMING_TYPE_OBJECT, 0);
//
//	bool out = whose->serialize_definition(writer, STREAMING_TYPE_OBJECT);
//
//    string_type result;
//	if (out)
//	{
//		writer.write_footer();
//
//		result = writer.get_string();
//	}
//
//	if (!result.empty())
//		def << result;
//	else
//		def << "Error in JSON deserialization.";
//
//	return out;
//}





class object_data_type 
{
	typedef std::vector<def_blocks::program_attribute> programs_type;
	typedef std::vector<relation_attribute> relations_type;
    typedef std::vector<def_blocks::method_attribute> methods_type;
    typedef std::vector<def_blocks::display_attribute> displays_type;

    template<class typeT>
    friend class meta_algorithm::object_types_algorithm;
    template<class typeT>
    friend class meta_algorithm::object_data_algorithm;
	//typedef std::vector<int> programs_type;

  public:
      object_data_type();

      ~object_data_type();


      rx_result resolve (rx_directory_ptr dir);

      bool check_type (type_check_context& ctx);

      rx_result register_relation (const relation_attribute& what, complex_data_type& complex_data);

      rx_result register_method (const def_blocks::method_attribute& what, complex_data_type& complex_data);

      rx_result register_program (const def_blocks::program_attribute& what, complex_data_type& complex_data);

      rx_result register_display (const def_blocks::display_attribute& what, complex_data_type& complex_data);


      const relations_type& get_relations () const
      {
        return relations_;
      }



      bool is_constructable;


  protected:

  private:


      programs_type programs_;

      relations_type relations_;

      methods_type methods_;

      displays_type displays_;


};






class application_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(application_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of application type");
public:
    static constexpr bool has_default_constructor = true;
	typedef typename runtime::algorithms::runtime_holder<application_type> RType;
	typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::application_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
	typedef typename runtime_data::application_runtime_data instance_data_t;
    typedef typename rx_internal::sys_runtime::runtime_core::runtime_data::application_instance_data runtime_data_t;
    typedef int runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<application_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      rx::meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static rx_item_type type_id;

      static rx_item_type runtime_type_id;

      security::security_guard_ptr security_guard;


  protected:

  private:


};






class domain_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(domain_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of domain type");
public:
    static constexpr bool has_default_constructor = true;
	typedef typename runtime::algorithms::runtime_holder<domain_type> RType;
	typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::domain_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
    typedef typename runtime_data::domain_runtime_data instance_data_t;
    typedef typename rx_internal::sys_runtime::runtime_core::runtime_data::domain_instance_data runtime_data_t;
    typedef int runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<domain_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      rx::meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static rx_item_type type_id;

      static rx_item_type runtime_type_id;

      security::security_guard_ptr security_guard;


  protected:

  private:


};






class object_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(object_type);
	DECLARE_CODE_INFO("rx", 0, 5, 2, "\
implementation of object type");
public:
    static constexpr bool has_default_constructor = true;
	typedef typename runtime::algorithms::runtime_holder<object_type> RType;
    typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::object_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
    typedef typename runtime_data::object_runtime_data instance_data_t;
    typedef typename rx_internal::sys_runtime::runtime_core::runtime_data::object_instance_data runtime_data_t;
    typedef int runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<object_type> algorithm_type;

  public:

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      rx::meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static rx_item_type type_id;

      static rx_item_type runtime_type_id;

      security::security_guard_ptr security_guard;


  protected:

  private:


};






class port_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(port_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of port type");
public:
    static constexpr bool has_default_constructor = false;
    typedef typename runtime::algorithms::runtime_holder<port_type> RType;
    typedef rx_reference<RType> RTypePtr;
    typedef runtime::items::port_runtime RImplType;
    typedef rx_reference<RImplType> RImplPtr;
    typedef typename runtime_data::port_runtime_data instance_data_t;
    typedef typename rx_internal::sys_runtime::runtime_core::runtime_data::port_instance_data runtime_data_t;
    typedef typename rx_internal::sys_runtime::runtime_core::runtime_data::port_behaviors runtime_behavior_t;
    typedef meta_algorithm::object_types_algorithm<port_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      object_data_type object_data;

      rx::meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;


      static const rx_item_type type_id;

      static rx_item_type runtime_type_id;

      security::security_guard_ptr security_guard;


  protected:

  private:


};






class relation_type_data 
{

  public:

      rx::data::runtime_values_data& get_overrides () const;


      bool sealed_type;

      bool abstract_type;

      string_type inverse_name;

      bool hierarchical;

      bool symmetrical;

      bool dynamic;

      rx_item_reference target;

      string_type description;


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
	template<class typeT>
	friend class meta_algorithm::object_types_algorithm;
	typedef runtime::relation_runtime_ptr RTypePtr;
    typedef runtime::relation_runtime_ptr RImplPtr;
    typedef meta_algorithm::relation_type_algorithm algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;

      static void set_runtime_data (runtime_data_prototype& prototype, RTypePtr where);


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      rx::meta_data meta_info;

      relation_type_data relation_data;


      static rx_item_type type_id;

      security::security_guard_ptr security_guard;


  protected:

  private:


};


} // namespace object_types
} // namespace meta
} // namespace rx_platform



#endif
