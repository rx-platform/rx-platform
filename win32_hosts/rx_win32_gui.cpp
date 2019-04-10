

/****************************************************************************
*
*  win32_hosts\rx_win32_gui.cpp
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


#include "stdafx.h"

#include "rx_win32_gui_version.h"
#include "rx_win32_common.h"

// rx_win32_gui
#include "win32_hosts/rx_win32_gui.h"



namespace win32 {

// Class win32::win32_gui_host 

win32_gui_host::win32_gui_host (hosting::rx_host_storages& storage)
	: host::gui::gui_platform_host(storage)
{
}


win32_gui_host::~win32_gui_host()
{
}



string_type win32_gui_host::get_config_path () const
{
	string_type ret;
	get_full_path("config", ret);
	return ret;
}

string_type win32_gui_host::get_default_name () const
{
	string_type ret;
	get_host_name(ret);
	return ret;
}

void win32_gui_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_win32_gui_info());
	host::gui::gui_platform_host::get_host_info(hosts);
}

string_type win32_gui_host::get_win32_gui_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_WIN32_PIPE_HOST_NAME, RX_WIN32_PIPE_HOST_MAJOR_VERSION, RX_WIN32_PIPE_HOST_MINOR_VERSION, RX_WIN32_PIPE_HOST_BUILD_NUMBER);
	}
	return ret;
}


} // namespace win32

