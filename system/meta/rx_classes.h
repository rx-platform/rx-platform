

/****************************************************************************
*
*  system\meta\rx_classes.h
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


#ifndef rx_classes_h
#define rx_classes_h 1



// rx_def_blocks
#include "system/meta/rx_def_blocks.h"
// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace objects {
namespace blocks {
class complex_runtime_item;
} // namespace blocks

namespace object_types {
class object_runtime;

} // namespace object_types
} // namespace objects
} // namespace rx_platform




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








class checkable_data 
{
	template <class T1, bool T2>
	friend class checkable_type;

  public:
      checkable_data();

      checkable_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system = false);


      bool serialize_node (base_meta_writer& stream, uint8_t type, const rx_value_union& value) const;

      bool deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value);

      bool check_in (base_meta_reader& stream);

      bool check_out (base_meta_writer& stream) const;

      bool serialize_checkable_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_checkable_definition (base_meta_reader& stream, uint8_t type);

      values::rx_value get_value () const;

      void construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, bool system = false);


      const rx_node_id& get_parent () const
      {
        return parent_;
      }


      uint32_t get_version () const
      {
        return version_;
      }


      rx_time get_created_time () const
      {
        return created_time_;
      }


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }


      string_type get_name () const
      {
        return name_;
      }


      const rx_node_id& get_id () const
      {
        return id_;
      }


      bool get_system () const
      {
        return system_;
      }


      namespace_item_attributes get_attributes () const
      {
        return attributes_;
      }



  protected:

  private:


      rx_node_id parent_;

      uint32_t version_;

      rx_time created_time_;

      rx_time modified_time_;

      string_type name_;

      rx_node_id id_;

      bool system_;

      namespace_item_attributes attributes_;


};


namespace basic_defs {





class event_class : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(event_class);
	friend class meta_helpers;

  public:
      virtual ~event_class();


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
	friend class meta_helpers;

  public:
      virtual ~filter_class();


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
	friend class meta_helpers;

  public:
      mapper_class();


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
	friend class meta_helpers;

  public:
      virtual ~source_class();


      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
basic struct class.\r\n\
basic implementation inside object class");
public:
	typedef objects::blocks::struct_runtime RType;
	typedef RType CType;
	typedef objects::blocks::struct_runtime* RTypePtr;
	friend class meta_helpers;

  public:
      struct_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~struct_class();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      namespace_item_attributes get_attributes () const;

      void construct (struct_runtime_ptr what);

      platform_item_ptr get_item_ptr ();

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      static struct_runtime_ptr create_runtime_ptr ();

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
public:
	typedef objects::blocks::variable_runtime RType;
	typedef RType CType;
	typedef objects::blocks::variable_runtime* RTypePtr;

  public:
      variable_class (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~variable_class();


      void construct (variable_runtime_ptr what);

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      platform_item_ptr get_item_ptr ();

      rx_value get_value () const;

      checkable_data& meta_data ();

      def_blocks::complex_data_type& complex_data ();


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
