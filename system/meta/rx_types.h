

/****************************************************************************
*
*  system\meta\rx_types.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_types_h
#define rx_types_h 1



// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_ptr
#include "lib/rx_ptr.h"

using rx_platform::meta::construct_context;


namespace rx_platform {

namespace meta {
namespace meta_algorithm
{
template<class typeT>
class basic_types_algorithm;
class data_types_algorithm;
}

namespace basic_types {





class event_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(event_type);
	DECLARE_CODE_INFO("rx", 0, 9, 1, "\
implementation of event type");
public:
	typedef runtime::structure::event_data RDataType;
	typedef runtime::blocks::event_runtime RType;
	typedef runtime::event_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<event_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      def_blocks::complex_data_type complex_data;

      meta_data meta_info;


      static rx_item_type type_id;

      rx_item_reference arguments;


  protected:

  private:


};






class filter_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(filter_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of filter type");
public:
	typedef runtime::structure::filter_data RDataType;
	typedef runtime::blocks::filter_runtime RType;
	typedef runtime::filter_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<filter_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      def_blocks::complex_data_type complex_data;

      meta_data meta_info;


      static rx_item_type type_id;


  protected:

  private:


};






class mapper_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(mapper_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of mapper type");
public:
	typedef runtime::structure::mapper_data RDataType;
	typedef runtime::blocks::mapper_runtime RType;
	typedef rx_reference<RType> RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<mapper_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      meta_data meta_info;

      def_blocks::complex_data_type complex_data;

      def_blocks::filtered_data_type filter_data;


      static rx_item_type type_id;


  protected:

  private:


};






class source_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(source_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of source type");
public:
	typedef runtime::structure::source_data RDataType;
	typedef runtime::blocks::source_runtime RType;
	typedef runtime::source_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<source_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      def_blocks::complex_data_type complex_data;

      meta_data meta_info;

      def_blocks::filtered_data_type filter_data;


      static rx_item_type type_id;


  protected:

  private:


};






class struct_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(struct_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of struct type");
public:
	typedef runtime::structure::struct_data RDataType;
	typedef runtime::blocks::struct_runtime RType;
	typedef runtime::struct_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<struct_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      def_blocks::complex_data_type complex_data;

      def_blocks::mapped_data_type mapping_data;

      meta_data meta_info;


      static rx_item_type type_id;


  protected:

  private:


};






class variable_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(variable_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of variable type");
public:
	typedef runtime::structure::variable_data RDataType;
	typedef runtime::blocks::variable_runtime RType;
	typedef runtime::variable_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<variable_type> algorithm_type;

  public:

      rx_value get_value () const;

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      meta_data meta_info;

      def_blocks::variable_data_type variable_data;

      def_blocks::mapped_data_type mapping_data;

      def_blocks::complex_data_type complex_data;


      static rx_item_type type_id;


  protected:

  private:


};






class method_type : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(method_type);
    DECLARE_CODE_INFO("rx", 0, 9, 1, "\
implementation of method type");
public:
    typedef runtime::logic_blocks::method_data RDataType;
    typedef logic::method_runtime RType;
    typedef method_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<method_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      def_blocks::complex_data_type complex_data;

      meta_data meta_info;


      static rx_item_type type_id;

      rx_item_reference inputs;

      rx_item_reference outputs;


  protected:

  private:


};






class program_type : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(program_type);
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
implementation of method type");
public:
    typedef runtime::logic_blocks::program_data RDataType;
    typedef logic::program_runtime RType;
    typedef program_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<program_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      meta_data meta_info;

      def_blocks::complex_data_type complex_data;


      static rx_item_type type_id;


  protected:

  private:


};






class display_type : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(display_type);
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
implementation of display type");
public:
    typedef runtime::display_blocks::display_data RDataType;
    typedef displays::display_runtime RType;
    typedef display_runtime_ptr RTypePtr;
    typedef meta_algorithm::basic_types_algorithm<display_type> algorithm_type;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      meta_data meta_info;

      def_blocks::complex_data_type complex_data;


      static rx_item_type type_id;


  protected:

  private:


};






class data_type : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(data_type);
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
rx-platform implementation of data type");
public:
    typedef std::vector<def_blocks::const_value_def> values_type;
    typedef std::vector<def_blocks::data_attribute> children_type;

    typedef data_blocks_prototype RDataType;
    typedef meta_algorithm::data_types_algorithm algorithm_type;

    friend class meta_algorithm::data_types_algorithm;

  public:

      platform_item_ptr get_item_ptr () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      meta_data meta_info;

      def_blocks::data_type_def complex_data;


      static rx_item_type type_id;


  protected:

  private:


};


} // namespace basic_types
} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {

typedef pointers::reference<basic_types::mapper_type> mapper_type_ptr;
typedef pointers::reference<basic_types::struct_type> struct_type_ptr;
typedef pointers::reference<basic_types::variable_type> variable_type_ptr;
typedef pointers::reference<basic_types::source_type> source_type_ptr;
typedef pointers::reference<basic_types::event_type> event_type_ptr;
typedef pointers::reference<basic_types::filter_type> filter_type_ptr;
typedef pointers::reference<basic_types::data_type> data_type_ptr;
typedef pointers::reference<basic_types::method_type> method_type_ptr;
typedef pointers::reference<basic_types::program_type> program_type_ptr;
typedef pointers::reference<basic_types::display_type> display_type_ptr;

} // namespace meta
} // namespace server rx_platform


#endif
