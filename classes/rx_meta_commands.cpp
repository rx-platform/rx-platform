

/****************************************************************************
*
*  classes\rx_meta_commands.cpp
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


#include "pch.h"


// rx_meta_commands
#include "classes/rx_meta_commands.h"



namespace model {

namespace meta_commands {

// Class model::meta_commands::create_command 

create_command::create_command()
	: server_command("create")
{
}


create_command::~create_command()
{
}



bool create_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	if (!in.eof())
	{
		string_type what;
		in >> what;
		if (!what.empty())
		{
			if (what == "object")
			{
				return create_object(in, out, err, ctx);
			}
			if (what == "type")
			{
				return create_type(in, out, err, ctx);
			}
			else
			{
				err << "Unknown type:" << what << "\r\n";
			}
		}
		else
			err << "Create type is unknown!\r\n";
	}
	else
		err << "Create type is unknown!\r\n";
	return false;
}

bool create_command::create_object (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type def;
	std::getline(in, def, '\0');
	out << "Hello from create object!!!\r\n";
	return true;
}

bool create_command::create_type (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type def;
	std::getline(in, def, '\0');
	out << "Hello from create object!!!\r\n";
	return true;
}


} // namespace meta_commands
} // namespace model

