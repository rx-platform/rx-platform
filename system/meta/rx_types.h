

/****************************************************************************
*
*  system\meta\rx_types.h
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


#ifndef rx_types_h
#define rx_types_h 1



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
// had to do forward declaration because of back template pattern

struct meta_data_t
{
	meta_data_t()
	{
		memzero(this, sizeof(meta_data_t));
		wd_timer_period = 1000;
	}
	uint32_t wd_timer_period;
};



namespace basic_types {






class event_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(event_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of event type");
public:
	typedef runtime::structure::event_data RDataType;
	typedef runtime::blocks::event_runtime RType;
	typedef runtime::event_runtime_ptr RTypePtr;
	template<class typeT>
	friend class meta_algorithm::basic_types_algorithm;

  public:
      event_type (const type_creation_data& data);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      void construct (RTypePtr& what, construct_context& ctx) const;

      bool check_type (type_check_context& ctx);


      const def_blocks::complex_data_type& complex_data () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      event_type();



      def_blocks::complex_data_type complex_data_;

      checkable_data meta_data_;


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
	template<class typeT>
	friend class meta_algorithm::basic_types_algorithm;

  public:
      filter_type (const type_creation_data& data);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      void construct (RTypePtr& what, construct_context& ctx) const;

      bool check_type (type_check_context& ctx);


      const def_blocks::complex_data_type& complex_data () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      filter_type();



      def_blocks::complex_data_type complex_data_;

      checkable_data meta_data_;


};







class mapper_type : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(mapper_type);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of mapper type");
public:
	typedef runtime::structure::mapper_data RDataType;
	typedef runtime::blocks::mapper_runtime RType;
	typedef runtime::mapper_runtime_ptr RTypePtr;
	template<class typeT>
	friend class meta_algorithm::basic_types_algorithm;

  public:
      mapper_type();

      mapper_type (const type_creation_data& data);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      void construct (RTypePtr& what, construct_context& ctx) const;

      bool check_type (type_check_context& ctx);


      const checkable_data& meta_data () const;

      const def_blocks::complex_data_type& complex_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:


      checkable_data meta_data_;

      def_blocks::complex_data_type complex_data_;


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
	template<class typeT>
	friend class meta_algorithm::basic_types_algorithm;

  public:
      source_type (const type_creation_data& data);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      void construct (RTypePtr& what, construct_context& ctx) const;

      bool check_type (type_check_context& ctx);


      const def_blocks::complex_data_type& complex_data () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      source_type();



      def_blocks::complex_data_type complex_data_;

      checkable_data meta_data_;


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
	template<class typeT>
	friend class meta_algorithm::basic_types_algorithm;

  public:
      struct_type (const type_creation_data& data);


      void construct (RTypePtr& what, construct_context& ctx) const;

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      bool check_type (type_check_context& ctx);


      const def_blocks::complex_data_type& complex_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      struct_type();



      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;

      checkable_data meta_data_;


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
	template<class typeT>
	friend class meta_algorithm::basic_types_algorithm;

  public:
      variable_type (const type_creation_data& data);


      void construct (RTypePtr& what, construct_context& ctx) const;

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      platform_item_ptr get_item_ptr ();

      rx_value get_value () const;

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      def_blocks::variable_data_type& variable_data ();

      bool check_type (type_check_context& ctx);


      const checkable_data& meta_data () const;

      const def_blocks::variable_data_type& variable_data () const;

      const def_blocks::mapped_data_type& mapping_data () const;

      const def_blocks::complex_data_type& complex_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      variable_type();



      checkable_data meta_data_;

      def_blocks::variable_data_type variable_data_;

      def_blocks::mapped_data_type mapping_data_;

      def_blocks::complex_data_type complex_data_;


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

} // namespace meta
} // namespace server rx_platform


#endif
