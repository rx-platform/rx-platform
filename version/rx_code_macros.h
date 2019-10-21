

/****************************************************************************
*
*  version\rx_code_macros.h
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/


#ifndef rx_code_macros_h
#define rx_code_macros_h 1




#define DECLARE_CODE_INFO(subsytem,major, minor, build,comment) \
public:\
	string_type get_class_name () const\
	{\
		return string_type(get_code_behind()->class_name);\
	}\
	static rx::pointers::code_behind_definition_t* get_code_behind()\
		{\
			char buff[0x100];\
			sprintf(buff,"%d.%d.%d",major,minor,build);\
			static rx::pointers::code_behind_definition_t temp = {\
				smart_ptr::get_pointee_class_name() , subsytem , comment , buff \
								}; \
				return &temp;\
		}\
	void fill_code_info(std::ostream& info,const string_type& name)\
	{\
		static string_type compile_time;\
		if(compile_time.empty())\
		{\
			char compile_buffer[0x100];\
			create_module_compile_time_string(__DATE__, __TIME__,compile_buffer);\
			compile_time=compile_buffer;\
		}\
		info << "CODE INFO" << "\r\n";\
		info << "--------------------------------------------------------------------------------" << "\r\n";\
		info << "name       : "<< name << "\r\n";\
		info << "subsystem  : " << subsytem << "\r\n";\
		info << "--------------------------------------------------------------------------------" << "\r\n";\
		info << "file       : " << get_code_module(__FILE__) << "\r\n";\
		info << "class name : ";\
		info << smart_ptr::get_pointee_class_name(); \
		info << "\r\n";\
		info << "version    : "<< major << "." << minor << "." << build << "\r\n";\
		info << "compiled   : " << compile_time << "\r\n";\
		info << "comment\r\n";\
		info << "/*\r\n";\
		info << comment << "\r\n";\
		info << "*/\r\n";\
	}\
private:\

#define DECLARE_TEST_CODE_INFO(maj,min,build,comment) \
const char* get_help() const { return comment; }\
DECLARE_CODE_INFO("test",maj,min,build,"class intendend for testing puprposes\r\n" comment)\

#define DECLARE_CONSOLE_CODE_INFO(maj,min,build,comment) \
DECLARE_CODE_INFO("console",maj,min,build,"class intendend for console usage\r\n" comment)\






#endif
