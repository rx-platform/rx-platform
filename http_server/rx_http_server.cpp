

/****************************************************************************
*
*  http_server\rx_http_server.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


// rx_http_server
#include "http_server/rx_http_server.h"


#define DMON_IMPL
#define DMON_MALLOC rx_heap_alloc
#define DMON_FREE rx_heap_free
#define DMON_REALLOC rx_heap_realloc

#define DMON_ASSERT(e)   RX_ASSERT(e)


#define DMON_LOG_ERROR(s) RX_LOG_ERROR("dmon","files",200,s)

#include "third-party/dmon/dmon.h"


#include "rx_http_version.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "lib/rx_ser_json.h"
#include "sys_internal/rx_async_functions.h"
#include "system/server/rx_file_helpers.h"
#include "rx_aspnet.h"
#include "jwt_token_filter.h"


namespace rx_internal {

namespace rx_http_server {
namespace {
http_server* g_inst = nullptr;

void watch_callback_internal(dmon_watch_id watch_id, dmon_action action, const char* rootdir,
	const char* filepath, const char* oldfilepath, void* user)
{
	http_files_manager* self = (http_files_manager*)user;
	self->watch_callback(watch_id.id, action, rootdir, filepath, oldfilepath);
}

struct dmon_initializer
{
	dmon_initializer()
	{
		dmon_init();
		HTTP_LOG_INFO("dmon_initializer", 100, "dmon initialized");
	}
	~dmon_initializer()
	{
		dmon_deinit();
	}
};
}

// Class rx_internal::rx_http_server::http_server

http_server::http_server()
{
	static dmon_initializer dmon_init_;
}



http_server& http_server::instance ()
{
	if (g_inst == nullptr)
		g_inst = new http_server();
	return *g_inst;
}

rx_result http_server::initialize (hosting::rx_platform_host* host, configuration_data_t& config)
{
	spa_ = config.other.http_spa;
	start_page_ = config.other.http_start_page;

	register_standard_filters();
	http_displays::rx_http_display_base::fill_globals();
	string_type master_path = config.other.http_path;
	if (!master_path.empty())
	{
		static_paths_.push_back(rx_combine_paths(master_path, "_static"));
		dynamic_paths_.push_back(rx_combine_paths(master_path, "_dynamic"));
		handlers_.initialize(host, config);
	}
	if (!config.other.http_user.empty())
	{
		string_type user_path = host->get_full_path(config.other.http_user);
		static_paths_.push_back(user_path);
		dynamic_paths_.push_back(user_path);
		auto result = files_.initialize(user_path, this);
		if (!result)
		{
			HTTP_LOG_ERROR("http_server", 100, "Error initializing HTTP files manager: "s + result.errors_line());
			return result;
		}
	}
	auto result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		RX_STATIC_HTTP_DISPLAY_TYPE_ID, [] {
			return rx_create_reference<http_displays::rx_http_static_display>();
		});
	result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		rx_node_id::from_string("g:80956ABB-34ED-4F7F-BE09-BFB9C6E0ACF2"), [] {
			return rx_create_reference<aspnet::aspnet_logon_display>();
		});
	result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		RX_STANDARD_HTTP_DISPLAY_TYPE_ID, [] {
			return rx_create_reference<http_displays::rx_http_standard_display>();
		});
	result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		RX_SIMPLE_HTTP_DISPLAY_TYPE_ID, [] {
			return rx_create_reference<http_displays::rx_http_simple_display>();
		});

	result = model::platform_types_manager::instance().get_simple_type_repository<display_type>().register_constructor(
		RX_MAIN_HTTP_DISPLAY_TYPE_ID, [] {
			return rx_create_reference<http_displays::rx_http_main_display>();
		});
	return true;
}

rx_result http_server::handle_request (http_request req)
{
	HTTP_LOG_DEBUG("http_server", 100, "HTTP request received for "s + req.path);

	http_response response;
	if (req.method == rx_http_method::get)
	{
		bool found = false;
		{
			locks::auto_lock_t _(&cache_lock_);
			auto it = cached_items_.find(req.path);
			if (it != cached_items_.end())
			{
				found = true;
				response = it->second;
			}
		}
		if (found)
		{
			HTTP_LOG_DEBUG("http_server", 100, "Cache sending HTTP response "s + response.result_string + " for "s + req.path);
			req.whose->send_response(std::move(response));
			return true;// done with the cache
		}
	}
	else if(req.method == rx_http_method::options)
	{
		response.result = 204;
		if (rx_is_debug_instance())
		{
			response.headers["Access-Control-Allow-Origin"] = "*";
			response.headers["Access-Control-Allow-Methods"] = "GET,POST,PUT,OPTIONS";
			response.headers["Access-Control-Allow-Headers"] = "Content-Type";
			response.headers["Access-Control-Allow-Credentials"] = "true";
		}
		response.headers["Content-Length"] = "0";
		send_response(req, std::move(response));
		return true;// done with options
	}

	for (auto& one : filters_)
	{
		auto result = one->handle_request_before(req, response);
		if(!result)
		{
			response.set_string_content(result.errors_line());
			response.headers["Content-Type"] = "text/plain";
			response.result = 500;
			send_response(req, std::move(response));
			return true;// done with filters
		}
	}
	auto handler = handlers_.get_handler(req.extension);
	if (handler)
	{
		rx_security_handle_t ident = 0;
		if (req.request_identity)
		{
			if (req.request_identity->login())
			{
				ident = req.request_identity->get_handle();
				req.identity = ident;
			}
		}

		security::secured_scope _(ident);

		auto result = handler->handle_request(req, response);
		if (!result)
		{
			response.set_string_content(result.errors_line());
			response.headers["Content-Type"] = "text/plain";
			response.result = 500;
		}
	}
	else
	{
		response.set_string_content("No handler registered for this type");
		response.headers["Content-Type"] = "text/plain";
		response.result = 501;
	}
	if (response.result > 0)
		send_response(req, std::move(response));
	return true;
}

void http_server::send_response (http_request& request, http_response response)
{
	for (auto& one : filters_)
	{
		auto result = one->handle_request_after(request, response);
		if(!result)
		{
			HTTP_LOG_ERROR("http_server", 100, "Error on filter for HTTP response "s + result.errors_line() + " for "s + request.path);
		}
	}
	
	if (!response.cache_path.empty() && request.method == rx_http_method::get)
	{
		locks::auto_lock_t _(&cache_lock_);
		cached_items_.insert_or_assign(request.path, response);
		cached_paths_.insert_or_assign(response.cache_path, request.path);
	}
	HTTP_LOG_DEBUG("http_server", 100, "Sending HTTP response "s + response.result_string + " for "s + request.path);
	auto ret = request.whose->send_response(std::move(response));
	if (!ret)
	{
		HTTP_LOG_ERROR("http_server", 100, "Error Sending HTTP response "s + ret.errors_line() + " for "s + request.path);
	}
	if (request.request_identity)
	{
		request.request_identity->logout();
		request.request_identity = security::security_context_ptr::null_ptr;
	}
}

string_type http_server::get_server_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_HTTP_NAME, RX_HTTP_MAJOR_VERSION, RX_HTTP_MINOR_VERSION, RX_HTTP_BUILD_NUMBER);
	}
	return ret;
}

string_type http_server::get_server_header_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		std::ostringstream ss;
		ss << "{rx-platform}/"
			<< RX_HTTP_MAJOR_VERSION << '.'
			<< RX_HTTP_MINOR_VERSION;
		ret = ss.str();
	}
	return ret;
}

void http_server::deinitialize ()
{
	auto result = files_.deinitialize();
	if(!result)
	{
		HTTP_LOG_ERROR("http_server", 800, "Error deinitializing HTTP files manager: "s + result.errors_line());
	}
	delete this;
}

string_type http_server::get_content (const string_type& path)
{
	string_type ret;
	{
		std::scoped_lock _(cache_lock_);
		auto it = cached_content_.find(path);
		if (it != cached_content_.end())
		{
			ret = it->second;
			HTTP_LOG_DEBUG("http_server", 100, "Dynamic content from cache "s + path);
		}
	}
	if (!ret.empty())
		return ret;

	for (const auto& one_path : dynamic_paths_)
	{
		auto file_path = std::filesystem::path(rx_combine_paths(one_path, path)).lexically_normal().string();
		rx::rx_source_file src;
		auto result = src.open(file_path.c_str());
		if (result)
		{
			string_type buffer;
			result = src.read_string(buffer);
			if (result)
			{
				HTTP_LOG_TRACE("http_server", 100, "Dynamic content loaded from "s + file_path);
				{
					std::scoped_lock _(cache_lock_);
					cached_content_.insert_or_assign(path, buffer);
					cached_content_paths_.insert_or_assign(file_path, path);
				}
				ret = std::move(buffer);
			}
			return ret;
		}
	}
	return ret;
}

rx_result http_server::get_parsed_html_content (const string_type& path, const string_type& root, byte_string& buffer)
{
	return html_loader_.get_parsed_html_content(path, root, buffer);
}

void http_server::register_standard_filters ()
{
	filters_.emplace_back(std::make_unique<http_path_filter>());
	filters_.emplace_back(std::make_unique<standard_request_filter>(spa_, start_page_));
	filters_.emplace_back(std::make_unique<aspnet::aspnet_authorizer>());
	filters_.emplace_back(std::make_unique<jwt_authorize::jwt_authorizer>());
}

void http_server::dump_http_references (std::ostream& out)
{
	out << "Static paths:\r\n";
	for (auto& one : static_paths_)
	{
		out << "  - " << one << "\r\n";
	}
	out << "\r\n";
	out << "Dynamic paths:\r\n";
	for(auto& one : dynamic_paths_)
	{
		out << "  - " << one << "\r\n";
	}
	out << "\r\n";
}

void http_server::process_file_changes (string_array paths)
{
	{
		std::scoped_lock _(cache_lock_);
		for (const auto& file_path : paths)
		{
			auto it = cached_paths_.find(file_path);
			if (it != cached_paths_.end())
			{
				HTTP_LOG_DEBUG("http_server", 100, "Cache deleted response for "s + it->second);
				cached_items_.erase(it->second);
				cached_paths_.erase(it);
			}
		}
	}
	html_loader_.process_file_changes(paths);
}


// Class rx_internal::rx_http_server::standard_request_filter

standard_request_filter::standard_request_filter (const string_type& spa, const string_type& start_page)
      : spa_(spa.empty() ? ""s : "/"s + spa),
        start_page_(start_page.empty() ? ""s : "/"s + start_page)
{
}



rx_result standard_request_filter::handle_request_after (http_request& req, http_response& resp)
{
	////////////////////////////////////////////////////////////////////////////////////
	if (rx_is_debug_instance())
	{
		resp.headers["Access-Control-Allow-Origin"] = "*";
		resp.headers["Access-Control-Allow-Methods"] = "GET,POST,PUT,DELETE";
	}
	//resp.headers.emplace("Cross-Origin-Opener-Policy", "same-origin");
//	resp.headers.emplace("Cross-Origin-Embedder-Policy", "require-corp");
	////////////////////////////////////////////////////////////////////////////////////
	resp.headers.emplace("Connection", "Keep-Alive");
	resp.headers.emplace("Keep-Alive", "timeout=32");
	resp.headers.emplace("Content-Language", "en");
	resp.headers.emplace("Server", http_server::get_server_header_info());
	char buff[0x20];
	auto[ptr, ep] = std::to_chars(buff, buff + 0x1f, resp.content.size());
	*ptr = '\0';
	resp.headers.emplace("Content-Length", buff);
	switch (resp.result)
	{
	case 100:
		resp.result_string = "100 Continue";
		break;
	case 200:
		resp.result_string = "200 OK";
		break;
	case 204:
		resp.result_string = "204 No Content";
		break;
	case  403:
		resp.result_string = "403 Forbidden";
		break;
	case  404:
		resp.result_string = "404 Not Found";
		break;
	case 405:
		resp.result_string = "405 Method Not Allowed";
		break;
	case 500:
		resp.result_string = "500 Internal Server Error";
		break;
	case 501:
		resp.result_string = "501 Not Implemented";
		break;
	default:
		{
			char buff[0x20];
			auto [ptr, ec] = std::to_chars(buff, buff + 0x1f, resp.result);
			*ptr = '\0';
			resp.result_string = buff;
			resp.result_string += " Unknown";
		}
	}
	return true;
}

rx_result standard_request_filter::handle_request_before (http_request& req, http_response& resp)
{
	if (spa_.empty())
	{
		if (req.path.empty() || req.path == "/")
		{
			if (!start_page_.empty())
				req.path = start_page_;
			else
				req.path = "/System.index.disp";
		}
	}

	int sub_count = 0;
	string_view_type path = req.path;
	auto in_path = path.rfind('/');
	while (in_path != 0 && in_path != string_view_type::npos)
	{
		sub_count++;
		in_path = path.rfind('/', in_path - 1);
	}
	string_type to_replace = "./";
	while (sub_count > 0)
	{
		to_replace += "../";
		sub_count--;
	}

	req.rel_root = to_replace;

	size_t idx = req.path.rfind('.');
	if (idx != string_type::npos)
	{
		req.extension = req.path.substr(idx + 1);
		if(!spa_.empty() && req.extension == "html" && req.path.size()>10 && req.path.substr(req.path.size() - 11, 11) == spa_)
			req.path = spa_;
	}
	else
	{
		if (!spa_.empty())
		{
			req.path = spa_;
			req.extension = "html";
		}
		else
		{
			req.extension = "json";
		}
	}
	return true;
}


// Class rx_internal::rx_http_server::http_display_handler


rx_result http_display_handler::handle_request (http_request& req, http_response& resp)
{
	resp.result = 0;

	auto idx = req.path.rfind('.');
	if (idx != string_type::npos)
	{
		req.path = req.path.substr(0, idx);
	}

	rx_thread_handle_t executer;

	auto disp_res = http_server::instance().get_displays().get_display(req.path, executer);
	if (disp_res)
	{

		disp_res.value()->req_ += 1;
		auto ticks1 = rx_get_us_ticks();

		rx_platform::rx_post_function_to(executer, req.whose
			, [ticks1](http_displays::rx_http_display_base::smart_ptr disp, http_request&& req, http_response&& resp) mutable
			{
				auto result = disp->handle_request(req, resp);


				float time_val = (float)(rx_get_us_ticks() - ticks1) / 1000.0f;
				disp->last_req_time_ = time_val;
				if (disp->max_req_time_ <= time_val)
					disp->max_req_time_ = time_val;

				if (!result)
				{
					disp->failed_ += 1;

					resp.headers["Content-Type"] = "text/plain";
					if (resp.result == 0 || resp.result == 200)
					{
						resp.result = 501;
						resp.set_string_content(result.errors_line());
					}

				}
				if (req.whose)
					http_server::instance().send_response(req, resp);

			}, disp_res.move_value(), std::move(req), std::move(resp));
		return true;
	}
	else
	{
		return "Invalid path";
	}

}

const char* http_display_handler::get_extension ()
{
	return "disp";
}

size_t process_next_find(const string_view_type& view, size_t from, size_t& to_skip, char& ending)
{
	size_t idx = 0;
	auto idx1 = view.find("href", from);
	auto idx2= view.find("src", from);
	if(idx1==string_view_type::npos && idx2==string_view_type::npos)
		return string_view_type::npos;

	if (idx1 < idx2)
		idx = idx1 + 4;
	else
		idx = idx2 + 3;

	while (idx < view.size() && isspace(view[idx]))
		idx++;

	if (idx < view.size() && view[idx] == '=')
		idx++;
	else if (idx >= view.size())
		return string_view_type::npos;
	else
		return process_next_find(view, idx, to_skip, ending);

	while (idx < view.size() && isspace(view[idx]))
	{
		idx++;
	}

	if(idx<view.size() && (view[idx] == '\"' || view[idx] == '\'') )
	{
		ending = view[idx];
		idx++;
		if(idx >= view.size())
			return string_view_type::npos;
		if (view[idx] == '~')
		{
			if (idx + 1 >= view.size())
				return string_view_type::npos;
			to_skip = 1;
			if (view[idx + 1] == '/')
				to_skip = 2;

			return idx;
		}
		else
		{
			idx = view.find(ending, idx);
			if (idx != string_view_type::npos)
				return process_next_find(view, idx + 1, to_skip, ending);
			else
				return string_view_type::npos;
		}
	}
	else if(idx < view.size())
	{
		return process_next_find(view, idx, to_skip, ending);
	}
	else
	{
		return string_view_type::npos;
	}

}
// Class rx_internal::rx_http_server::http_path_filter


rx_result http_path_filter::handle_request_after (http_request& req, http_response& resp)
{
	//auto it = resp.headers.find("Content-Type");
	//if (it != resp.headers.end()
	//	&& it->second == "text/html"
	//	&& resp.content.size() > 0x20 /*just about right*/)
	//{
	//	string_view_type view((const char*)&resp.content[0], resp.content.size());

	//	char ending = '\0';
	//	size_t to_skip = 0;
	//	size_t idx = process_next_find(view, 0, to_skip, ending);
	//	if (idx == string_view_type::npos)
	//		return true; // nothing to do
	//	size_t idx2 = 0;
	//	string_type new_buffer;
	//	while (idx != string_view_type::npos)
	//	{
	//		new_buffer+=(view.substr(idx2, idx - idx2));
	//		idx2 = view.find(ending, idx + to_skip);
	//		if (idx2 != string_view_type::npos)
	//		{
	//			new_buffer += req.rel_root;
	//			new_buffer += view.substr(idx + to_skip, idx2 - (idx + to_skip));
	//			idx = process_next_find(view, idx2, to_skip, ending);
	//		}
	//	}
	//	if (idx2 != string_view_type::npos)
	//		new_buffer += (view.substr(idx2));
	//	resp.set_string_content(new_buffer);
	//}
	return true;
}

rx_result http_path_filter::handle_request_before (http_request& req, http_response& resp)
{
	return true;
}


// Class rx_internal::rx_http_server::html_file_loader


rx_result html_file_loader::get_parsed_html_content (const string_type& path, const string_type& root, byte_string& buffer)
{
	{
		// check the cache first
		std::scoped_lock _(cache_lock_);
		auto it = cached_items_.find(path);
		if (it != cached_items_.end())
		{
			HTTP_LOG_DEBUG("html_file_loader", 100, "HTML Files Cache processing cached data for "s + path);
			return fill_html_data(it->second, root, buffer);
		}
	}
	// no cache, load from file
	string_type file_data;
	string_type cache_file_path;
	auto paths = rx_internal::rx_http_server::http_server::instance().get_static_paths();
	for (const auto& one_dir : paths)
	{
		auto file_path = rx_combine_paths(one_dir, path);
		auto file = rx_file(file_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		if (file)
		{
			uint64_t size = 0;
			auto result = rx_file_get_size(file, &size);
			if (result == RX_OK)
			{
				file_data.resize((size_t)size);
				uint32_t readed = 0;
				result = rx_file_read(file, &file_data[0], (uint32_t)size, &readed);
				if (result == RX_OK)
				{
					cache_file_path = std::filesystem::path(file_path).lexically_normal().string();
					HTTP_LOG_TRACE("html_file_loader", 100, "HTML file "s + cache_file_path + " loaded from disk!");
				}
				else
				{
					file_data.clear();
				}
			}
			rx_file_close(file);
			break;
		}
	}
	if (file_data.empty())
		return RX_INVALID_PATH;

	auto indexes = process_html_file(file_data);
	{
		std::scoped_lock _(cache_lock_);
		cached_paths_.insert_or_assign(cache_file_path, path);
		auto result = cached_items_.insert_or_assign(path, html_file_data_t{ std::move(file_data), std::move(indexes) });
		return fill_html_data(result.first->second, root, buffer);

	}

}

std::vector<html_file_poss_t> html_file_loader::process_html_file (string_view_type content)
{
	std::vector<html_file_poss_t> indexes;
	char ending = '\0';
	size_t to_skip = 0;
	size_t idx = process_next_find(content, 0, to_skip, ending);
	if (idx == string_view_type::npos)
		return indexes; // nothing to do
	size_t idx2 = 0;
	while (idx != string_view_type::npos)
	{
		idx2 = content.find(ending, idx + to_skip);
		if (idx2 != string_view_type::npos)
		{
			html_file_poss_t one;
			one.start = idx - 1;
			one.length = to_skip;
			idx = process_next_find(content, idx2, to_skip, ending);
			indexes.push_back(one);
		}
	}
	return indexes;
}

rx_result html_file_loader::fill_html_data (const html_file_data_t& item, const string_type& root, byte_string& buffer)
{
	size_t start_idx = 0;
	const std::byte* root_start = nullptr;
	const std::byte* root_end= nullptr;
	if(!root.empty())
	{
        root_start=reinterpret_cast<const std::byte*>(root.c_str());
        root_end=reinterpret_cast<const std::byte*>(root.c_str() + root.size());
	}
	buffer.reserve(item.content.size() + 0x20);// this should do
	for (const auto& one : item.path_indexes)
	{
		RX_ASSERT(one.start + one.length <= item.content.size());
		if (one.start + one.length <= item.content.size())
		{
			buffer.insert(buffer.end()
                , reinterpret_cast<const std::byte*>(item.content.c_str() + start_idx)
                , reinterpret_cast<const std::byte*>(item.content.c_str() + one.start + 1));
            if(root_start)
                buffer.insert(buffer.end(), root_start, root_end);
			start_idx = one.start + one.length + 1;
		}
	}
	if (start_idx < item.content.size())
		buffer.insert(buffer.end()
            , reinterpret_cast<const std::byte*>(item.content.c_str() + start_idx)
            , reinterpret_cast<const std::byte*>(item.content.c_str() + item.content.size()));

	return true;
}

void html_file_loader::process_file_changes (string_array paths)
{
	std::scoped_lock _(cache_lock_);
	for(const auto& file_path : paths)
	{
		auto it = cached_paths_.find(file_path);
		if (it != cached_paths_.end())
		{
			HTTP_LOG_DEBUG("html_file_loader", 100, "HTML Files Cache deleted response for "s + it->second);
			cached_items_.erase(it->second);
			cached_paths_.erase(it);
		}
	}
}


// Class rx_internal::rx_http_server::http_files_manager

http_files_manager::http_files_manager()
      : server_(nullptr),
        watch_id_(0)
{
}


http_files_manager::~http_files_manager()
{
}



rx_result http_files_manager::initialize (const string_type& path, http_server* srv)
{
	path_ = path;
	server_ = srv;
	if (watch_id_)
		return RX_ALREADY_CONNECTED;

	auto result = dmon_watch(path.c_str(), watch_callback_internal, DMON_WATCHFLAGS_RECURSIVE, this);
	if (result.id == 0)
	{
		char buff[0x100];
		rx_last_os_error("Error connecting directory.", buff, sizeof(buff));
		HTTP_LOG_WARNING("http_files_manager", 100, "Error connecting file monitor on "s + path + ": "s + buff);

	}
	else
	{
		HTTP_LOG_INFO("http_files_manager", 100, "File monitor on "s + path + " started.");
		watch_id_ = result.id;
		return true;
	}
	return true;
}

rx_result http_files_manager::deinitialize ()
{
	if (watch_id_ != 0)
	{
		dmon_unwatch({ watch_id_ });
		watch_id_ = { 0 };
	}
	return true;
}

void http_files_manager::watch_callback (uint32_t watch_id, int action, const char* rootdir, const char* filepath, const char* oldfilepath)
{
	if (watch_id == watch_id_)
	{
		string_array changes;
		std::filesystem::path path1 = rx_combine_paths(rootdir, filepath);
		string_type path1_str = path1.lexically_normal().string();
		string_type path2_str;
		if (oldfilepath && oldfilepath[0])
		{
			std::filesystem::path path2 = rx_combine_paths(rootdir, oldfilepath);
			path2_str = path2.lexically_normal().string();
		}
		switch ((dmon_action_t)action)
		{
			case DMON_ACTION_MOVE:
				{
					if (!path2_str.empty())
					{
						changes.push_back(std::move(path2_str));
					}

				}
				[[fallthrough]];
			case DMON_ACTION_MODIFY:
			case DMON_ACTION_CREATE:
			case DMON_ACTION_DELETE:
				{
					if (!path1_str.empty())
					{
						changes.push_back(std::move(path1_str));
					}
				}
				break;
			default:
				RX_ASSERT(false);
		}
		if (!changes.empty() && server_)
		{
			rx_post_function_to(RX_JOB_SLOW, rx_reference_ptr(), [changes = std::move(changes), server = server_]()
				{
					for(const auto& one : changes)
					{
						HTTP_LOG_DEBUG("http_files_manager", 100, "File change detected: "s + one);
					}
					server->process_file_changes(std::move(changes));
				});
		}
	}
	else
	{
		RX_ASSERT(false);
	}

}


} // namespace rx_http_server
} // namespace rx_internal

