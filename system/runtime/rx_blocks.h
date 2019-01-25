

/****************************************************************************
*
*  system\runtime\rx_blocks.h
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


#ifndef rx_blocks_h
#define rx_blocks_h 1



// rx_ptr
#include "lib/rx_ptr.h"

#include "system/server/rx_ns.h"
#include "system/callbacks/rx_callback.h"
using namespace rx_platform::ns;
using namespace rx::values;


namespace rx_platform {
namespace meta
{
namespace def_blocks
{
	class complex_data_type;
}
namespace basic_defs
{
	class struct_class;
}
}

namespace runtime {
typedef callback::callback_functor_container<locks::lockable, rx::values::rx_value> value_callback_t;
namespace object_types
{
	class object_runtime;
}
typedef rx_reference<object_types::object_runtime> object_runtime_ptr;

namespace blocks {

extern const char* g_const_simple_class_name;
typedef uint32_t runtime_item_id_t;






class filter_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
filter runtime. basic implementation of an filter runtime");

	DECLARE_REFERENCE_PTR(filter_runtime);

	friend class meta::def_blocks::complex_data_type;

  public:
      filter_runtime();


      string_type get_type_name () const;


  protected:

  private:


      static string_type type_name;


};






class mapper_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
mapper runtime. basic implementation of an mapper runtime");

	DECLARE_REFERENCE_PTR(mapper_runtime);
	friend class meta::def_blocks::complex_data_type;

  public:
      mapper_runtime();

      virtual ~mapper_runtime();


      string_type get_type_name () const;


      static string_type type_name;


  protected:

  private:


};






class source_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
source runtime. basic implementation of an source runtime");

	DECLARE_REFERENCE_PTR(source_runtime);

	friend class meta::def_blocks::complex_data_type;

  public:
      source_runtime();

      virtual ~source_runtime();


      string_type get_type_name () const;


  protected:

  private:


      static string_type type_name;


};






class struct_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
struct runtime. basic implementation of an struct runtime");

	DECLARE_REFERENCE_PTR(struct_runtime);

	typedef std::vector<mapper_runtime::smart_ptr> mappers_type;
	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_defs::struct_class;

  public:
      struct_runtime();

      struct_runtime (const string_type& name, const rx_node_id& id, bool system = false);


      bool serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      string_type get_type_name () const;


      static string_type type_name;


  protected:

  private:


};






class variable_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
variable runtime. basic implementation of an variable runtime");

	DECLARE_REFERENCE_PTR(variable_runtime);

	friend class meta::def_blocks::complex_data_type;

  public:
      variable_runtime();

      variable_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      virtual ~variable_runtime();


      string_type get_type_name () const;


      static string_type type_name;


  protected:

  private:


};






class event_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
event runtime. basic implementation of an event runtime");

	DECLARE_REFERENCE_PTR(event_runtime);

	friend class meta::def_blocks::complex_data_type;

  public:
      event_runtime();


      string_type get_type_name () const;


  protected:

  private:


      static string_type type_name;


};


} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
