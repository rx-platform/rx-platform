

/****************************************************************************
*
*  win32_hosts\rx_win32_service.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#include "pch.h"

#include "rx_win32_service_version.h"
#include "rx_win32_common.h"

// rx_win32_service
#include "win32_hosts/rx_win32_service.h"



namespace win32 {
namespace
{
SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
}

// Class win32::win32_service_host 

rx::locks::event win32_service_host::finished_ = false;

rx::locks::event win32_service_host::stop_ = false;

win32_service_host::win32_service_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : supports_ansi_(false),
        is_service_(false)
	, win32_headless_host(storages)
{
	g_host = this;
}


win32_service_host::~win32_service_host()
{
}



void win32_service_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_win32_service_info());
	host::headless::headless_platform_host::get_host_info(hosts);
}

string_type win32_service_host::get_win32_service_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_WIN32_SERVICE_HOST_NAME, RX_WIN32_SERVICE_HOST_MAJOR_VERSION, RX_WIN32_SERVICE_HOST_MINOR_VERSION, RX_WIN32_SERVICE_HOST_BUILD_NUMBER);
	}
	return ret;
    
}

rx_result win32_service_host::start_service (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins)
{
	rx_result ret;
	printf("Opening SCM on localhost...\r\n");
	SC_HANDLE scm = OpenSCManager(NULL, NULL, GENERIC_READ);
	if (scm)
	{
		printf("Opening service %s...\r\n", RX_WIN32_SERVICE_HOST);
		SC_HANDLE svc = OpenServiceA(scm, RX_WIN32_SERVICE_HOST, GENERIC_READ);
		if (svc)
		{
			is_service_ = true;
			CloseServiceHandle(svc);
		}
	}

	if (is_service_)
	{
		plugins_ = plugins;
		char* name_buff = new char[strlen(RX_WIN32_SERVICE_HOST) + 1];
		strcpy(name_buff, RX_WIN32_SERVICE_HOST);
		OutputDebugStringA(RX_WIN32_SERVICE_HOST " : Registering SCD...");
		SERVICE_TABLE_ENTRYA ServiceTable[] =
		{
			{ name_buff, (LPSERVICE_MAIN_FUNCTIONA)ServiceMain },
			{ NULL, NULL }
		};

		if (!StartServiceCtrlDispatcherA(ServiceTable))
		{
			char buff[0x100];
			_snprintf(buff, sizeof(buff) / sizeof(buff[0]), RX_WIN32_SERVICE_HOST " : Unable to register SCD! Error code:0x%08x", GetLastError());
			OutputDebugStringA(buff);
		}
		delete[] name_buff;
	}
	else
	{
		printf("Running as normal process...\r\n");
		initialize_platform(argc, argv, "rx_win32_service"
			, log::log_subscriber::smart_ptr::null_ptr
			, host::headless::synchronize_callback_t(), plugins);
		start_platform();
		finished_.wait_handle();
	}
	return ret;
}

rx_result win32_service_host::stop_service ()
{
	rx_result ret;
	if (is_service_)
	{
		OutputDebugStringA(RX_WIN32_SERVICE_HOST " : Stopping service...\r\n");
		if (g_ServiceStatus.dwCurrentState == SERVICE_RUNNING)
		{
			g_ServiceStatus.dwControlsAccepted = 0;
			g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			g_ServiceStatus.dwWin32ExitCode = 0;
			g_ServiceStatus.dwCheckPoint = 4;

			if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
			{
				OutputDebugStringA(RX_WIN32_SERVICE_HOST " : ServiceCtrlHandler: SetServiceStatus returned error");
			}
			stop_.set();
		}

	}
	else
	{
		printf("Stopping process...\r\n");
		stop_platform();
		deinitialize_platform();
		finished_.set();
	}
	return ret;
}

rx_result win32_service_host::install_service ()
{
	rx_result ret;
	printf("Opening SCM on localhost...\r\n");
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scm)
	{
		printf("Creating service...\r\n");
		char path[MAX_PATH * 4];
		if (GetModuleFileNameA(NULL, path, sizeof(path) / sizeof(path[0])))
		{
			string_type full_path("\"");
			full_path += path;
			full_path += "\"";
			SC_HANDLE svc = CreateServiceA(scm
				, RX_WIN32_SERVICE_HOST
				, RX_WIN32_SERVICE_NAME
				, SC_MANAGER_ALL_ACCESS
				, SERVICE_WIN32_OWN_PROCESS
				, SERVICE_AUTO_START
				, SERVICE_ERROR_NORMAL
				, full_path.c_str()
				, nullptr
				, nullptr
				, "RPCSS\0"
				, nullptr
				, nullptr);
			if (svc)
			{
				printf("Created service %s.\r\n", RX_WIN32_SERVICE_HOST);
				CloseServiceHandle(svc);
			}
			else
			{
				printf("Unable to create service! Error code:0x%08x.\r\n", GetLastError());
			}
		}
		CloseServiceHandle(scm);
	}
	else
	{
		printf("Unable to open SCM on localhost! Error code:0x%08x.\r\n", GetLastError());
	}
	return ret;
}

rx_result win32_service_host::uninstall_service ()
{
	rx_result ret;
	printf("Opening SCM on localhost...\r\n");
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scm)
	{
		printf("Opening service...\r\n");
		SC_HANDLE svc = OpenServiceA(scm, RX_WIN32_SERVICE_HOST, SC_MANAGER_ALL_ACCESS);
		if (svc)
		{
			printf("Deleting service...\r\n");
			if (DeleteService(svc))
			{
				printf("Deleted service %s.\r\n", RX_WIN32_SERVICE_HOST);
			}
			else
			{
				printf("Unable to delete service! Error code:0x%08x.\r\n", GetLastError());
			}
			CloseServiceHandle(svc);
		}
		else
		{
			printf("Unable to open service! Error code:0x%08x.\r\n", GetLastError());
		}
		CloseServiceHandle(scm);
	}
	else
	{
		printf("Unable to open SCM on localhost! Error code:0x%08x.\r\n", GetLastError());
	}
	return ret;
}

VOID WINAPI win32_service_host::ServiceCtrlHandler (DWORD ctrlCode, DWORD eventType, LPVOID data, LPVOID reserved)
{
	switch (ctrlCode)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:

		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		/*
		* Perform tasks necessary to stop the service here
		*/

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugStringA(RX_WIN32_SERVICE_HOST " : ServiceCtrlHandler: SetServiceStatus returned error");
		}

		// This will signal the worker thread to start shutting down
		stop_.set();

		break;
	case SERVICE_CONTROL_POWEREVENT:
		{
			if (eventType == PBT_APMSUSPEND)
			{
				/**((int*)13) = 0;
				PPOWERBROADCAST_SETTING pdata = (PPOWERBROADCAST_SETTING)data;*/
				//if (memcpy(&pdata->PowerSetting, &GUID_SYSTEM_AWAYMODE,sizeof(GUID))==0 && pdata->Data)
				{
					g_ServiceStatus.dwControlsAccepted = 0;
					g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
					g_ServiceStatus.dwWin32ExitCode = 0;
					g_ServiceStatus.dwCheckPoint = 4;

					if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
					{
						OutputDebugStringA(RX_WIN32_SERVICE_HOST " : ServiceCtrlHandler: SetServiceStatus returned error");
					}

					// This will signal the worker thread to start shutting down
					stop_.set();
				}
			}
		}
		break;
	default:
		break;
	}
}

VOID WINAPI win32_service_host::ServiceMain (DWORD argc, LPSTR* argv)
{
	DWORD Status = E_FAIL;

	// Register our service control handler with the SCM
	g_StatusHandle = RegisterServiceCtrlHandlerExA(win32_service_host::instance().service_name_.c_str()
		, (LPHANDLER_FUNCTION_EX)ServiceCtrlHandler, nullptr);

	if (g_StatusHandle == NULL)
	{
		OutputDebugStringA(RX_WIN32_SERVICE_HOST " : ServiceMain: RegisterServiceCtrlHandler returned error");
		return;
	}

	// Tell the service controller we are starting
	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugStringA(RX_WIN32_SERVICE_HOST " : ServiceMain: SetServiceStatus returned error");
	}

	/*
	* Perform tasks necessary to start the service here
	*/

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_POWEREVENT;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugStringA(RX_WIN32_SERVICE_HOST " : ServiceMain: SetServiceStatus returned error");
	}

	OutputDebugStringA(RX_WIN32_SERVICE_HOST " : Initializing application");

	win32_service_host::instance().initialize_platform(argc, argv, "rx_win32_service"
		, log::log_subscriber::smart_ptr::null_ptr
		, host::headless::synchronize_callback_t(), win32_service_host::instance().plugins_);

	OutputDebugStringA(RX_WIN32_SERVICE_HOST " : Starting application");

	win32_service_host::instance().start_platform();


	OutputDebugStringA(RX_WIN32_SERVICE_HOST " : Application started");

	stop_.wait_handle();

	OutputDebugStringA(RX_WIN32_SERVICE_HOST " : Stopping application");
	win32_service_host::instance().stop_platform();
	win32_service_host::instance().deinitialize_platform();

	// Tell the service controller we are stopped
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugStringA(RX_WIN32_SERVICE_HOST " : ServiceMain: SetServiceStatus returned error");
	}

	return;
}

win32_service_host& win32_service_host::instance ()
{
	return *g_host;
}

string_type win32_service_host::get_host_name ()
{
	return RX_WIN32_SERVICE_HOST;
}

string_type win32_service_host::get_default_user_storage () const
{
	hosting::rx_host_directories data;
	auto result = const_cast<win32_service_host*>(this)->fill_host_directories(data);
	if (result && is_service_)
	{
		string_type temp = data.system_storage;
		auto idx = temp.rfind('/');
		if (idx != string_type::npos)
			temp = temp.substr(0, idx + 1) + RX_WIN32_SERVICE_HOST;
		return temp;
	}
	return "";
}


} // namespace win32


