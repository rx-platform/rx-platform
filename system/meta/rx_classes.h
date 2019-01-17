

/****************************************************************************
*
*  system\meta\rx_classes.h
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


#ifndef rx_classes_h
#define rx_classes_h 1



// rx_checkable
#include "system/meta/rx_checkable.h"
// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_ptr
#include "lib/rx_ptr.h"

using rx_platform::meta::def_blocks::construct_context;


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



namespace basic_defs {





struct type_creation_data 
{


      string_type name;

      rx_node_id id;

      rx_node_id base_id;

      bool system;

  public:

  protected:

  private:


};







class event_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(event_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of event type");
public:
	typedef runtime::blocks::data::event_data RType;
	typedef runtime::blocks::data::event_data::smart_ptr RTypePtr;

	friend class obj_meta_helpers;

  public:
      event_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      static event_class::RType create_runtime ();

      void construct (RType& what, construct_context& ctx);

      uint_fast8_t get_runtime_data_type ();


      const def_blocks::complex_data_type& complex_data () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      event_class();



      def_blocks::complex_data_type complex_data_;

      checkable_data meta_data_;


};







class filter_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(filter_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of filter type");
public:
	typedef runtime::blocks::data::filter_data RType;
	typedef runtime::blocks::data::filter_data::smart_ptr RTypePtr;

	friend class meta_helpers;

  public:
      filter_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      static filter_class::RType create_runtime ();

      void construct (RType& what, construct_context& ctx);

      uint_fast8_t get_runtime_data_type ();


      const def_blocks::complex_data_type& complex_data () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      filter_class();



      def_blocks::complex_data_type complex_data_;

      checkable_data meta_data_;


};







class mapper_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(mapper_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of mapper type");
public:
	typedef runtime::blocks::data::mapper_data RType;
	typedef runtime::blocks::data::mapper_data::smart_ptr RTypePtr;

	friend class meta_helpers;

  public:
      mapper_class();

      mapper_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      static mapper_class::RType create_runtime ();

      void construct (RType& what, construct_context& ctx);

      uint_fast8_t get_runtime_data_type ();


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







class source_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(source_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of source type");
public:
	typedef runtime::blocks::data::source_data RType;
	typedef runtime::blocks::data::source_data::smart_ptr RTypePtr;

	friend class meta_helpers;

  public:
      source_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      static source_class::RType create_runtime ();

      void construct (RType& what, construct_context& ctx);

      uint_fast8_t get_runtime_data_type ();


      const def_blocks::complex_data_type& complex_data () const;

      const checkable_data& meta_data () const;


      static string_type get_type_name ()
      {
        return type_name;
      }



      static string_type type_name;


  protected:

  private:
      source_class();



      def_blocks::complex_data_type complex_data_;

      checkable_data meta_data_;


};







class struct_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(struct_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of struct type");
public:
	typedef runtime::blocks::data::struct_data RType;
	typedef runtime::blocks::data::struct_data::smart_ptr RTypePtr;

	friend class meta_helpers;

  public:
      struct_class (const type_creation_data& data);


      void construct (RType& what, construct_context& ctx);

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      static struct_class::RType create_runtime ();

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      uint_fast8_t get_runtime_data_type ();


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
      struct_class();



      def_blocks::complex_data_type complex_data_;

      def_blocks::mapped_data_type mapping_data_;

      checkable_data meta_data_;


};







class variable_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(variable_class);
	DECLARE_CODE_INFO("rx", 0, 5, 1, "\
implementation of variable type");
public:
	typedef runtime::blocks::data::variable_data RType;
	typedef runtime::blocks::data::variable_data::smart_ptr RTypePtr;

	friend class meta_helpers;

  public:
      variable_class (const string_type& name, const rx_node_id& id, const rx_node_id& base_id, bool system = false);


      void construct (RType& what, construct_context& ctx);

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      platform_item_ptr get_item_ptr ();

      rx_value get_value () const;

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();

      def_blocks::mapped_data_type& mapping_data ();

      def_blocks::variable_data_type& variable_data ();

      static variable_class::RType create_runtime ();

      uint_fast8_t get_runtime_data_type ();


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
      variable_class();



      checkable_data meta_data_;

      def_blocks::variable_data_type variable_data_;

      def_blocks::mapped_data_type mapping_data_;

      def_blocks::complex_data_type complex_data_;


};


} // namespace basic_defs
} // namespace meta
} // namespace rx_platform

namespace rx_platform {
namespace meta {
	
typedef pointers::reference<basic_defs::mapper_class> mapper_class_ptr;
typedef pointers::reference<basic_defs::struct_class> struct_class_ptr;
typedef pointers::reference<basic_defs::variable_class> variable_class_ptr;
typedef pointers::reference<basic_defs::source_class> source_class_ptr;
typedef pointers::reference<basic_defs::event_class> event_class_ptr;
typedef pointers::reference<basic_defs::filter_class> filter_class_ptr;

} // namespace meta
} // namespace server rx_platform


#endif
