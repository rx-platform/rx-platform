

/****************************************************************************
*
*  system\libraries\cpp_lib.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef cpp_lib_h
#define cpp_lib_h 1






namespace server {

namespace library {


	/*
struct code_behind_definition_type
{
	const char* class_name;
	const char* plugin;
	const char* description;
	const char* version;

	bool serialize_to_stream(common::base::base_out_stream& stream) const
	{
	if (!stream.start_object("CPP"))
	return false;

	if (!stream.write_string("ClsName", class_name))
	return false;
	if (!stream.write_string("Addin", plugin))
	return false;
	if (!stream.write_string("Desc", description))
	return false;
	if (!stream.write_string("Ver", version))
	return false;

	if (!stream.end_object())
	return false;
	return true;
	}
	static bool deserialize_from_stream(common::base::base_in_stream& stream)
	{
	// shouldn't happend on server
	if (!stream.start_object("CPP"))
	return false;
	itf_string temp;
	if (!stream.read_string("ClsName", temp))
	return false;
	temp = "";
	if (!stream.read_string("Addin", temp))
	return false;
	temp = "";
	if (!stream.read_string("Desc", temp))
	return false;
	temp = "";
	if (!stream.read_string("Ver", temp))
	return false;

	if (!stream.end_object())
	return false;
	return true;
	}

	void to_string(string_type& str) const
	{
	str = "Addin:";
	str += plugin;
	str += "\r\nC++ class name:";
	str += class_name;
	str += "\r\nCode version:";
	str += version;
	str += "\r\nCode description:";
	str += description;
	}

};
*/




class cpp_classes_manager 
{
	typedef std::set<rx::pointers::code_behind_definition_t*> definitions_type;

  public:
      virtual ~cpp_classes_manager();


      static cpp_classes_manager& instance ();

      bool check_class (rx::pointers::code_behind_definition_t* cd);

	  template<class clsT>
	  rx::pointers::code_behind_definition_t* _internal_read_class(tl::type2type<clsT>)
	  {
		  return clsT::get_code_behind();
	  }
	  template<class clsT>
	  rx::pointers::code_behind_definition_t* register_class_for_usage()
	  {
		  rx::pointers::code_behind_definition_t* cd = _internal_read_class<clsT>(tl::type2type<clsT>());
		  auto& it = this->m_definitions.find(cd);
		  if (it ==  this->m_definitions.end())
		  {
			   this->m_definitions.insert(cd);
		  }
		  return cd;

	  }
  protected:

  private:
      cpp_classes_manager();

      cpp_classes_manager(const cpp_classes_manager &right);

      cpp_classes_manager & operator=(const cpp_classes_manager &right);



      definitions_type _definitions;


};


} // namespace library
} // namespace server



#endif
