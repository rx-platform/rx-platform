

/****************************************************************************
*
*  http_server\rx_http_displays.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


// rx_http_displays
#include "http_server/rx_http_displays.h"

#include "third-party/rapidjson/include/rapidjson/document.h"
#include "third-party/rapidjson/include/rapidjson/writer.h"
#include "terminal/rx_vt100.h"
#include "sys_internal/rx_inf.h"
#include "rx_http_server.h"
#include "system/server/rx_file_helpers.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "system/server/rx_directory_cache.h"
#include "sys_internal/rx_plugin_manager.h"


namespace rx_internal {

namespace rx_http_server {

namespace http_displays {

// Class rx_internal::rx_http_server::http_displays::rx_http_static_display 

rx_http_static_display::rx_http_static_display()
{
}

rx_http_static_display::rx_http_static_display (const string_type& name, const rx_node_id& id)
{
}


rx_http_static_display::~rx_http_static_display()
{
}



rx_result rx_http_static_display::initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	http_display_custom_content content;
	fill_contents(content, ctx, disp_path);
	content.body_begin_content += "\r\n"
		"<script>\r\n"
		"var rx_rt_path = '";
	content.body_begin_content += ctx.meta.get_full_path();
	content.body_begin_content += "';\r\n"
		"</script>\r\n";

	content.body_end_content += "\r\n"
		"<script>\r\n"
		"setInterval(function () {\r\n"
		"  $.getJSON('~";
	content.body_end_content += disp_path;
	content.body_end_content += ".disp?data', function( data ) {\r\n"
		"	  update_rx_content(data);\r\n"
		"  }).fail(rx_offline);\r\n"
		"}, 1000);\r\n"
		"</script>";
	auto result = parse_display_data(ctx, disp_path, html_data_, content);
	if (!result)
	{
		html_data_ = "Error while parsing display data:\r\n";
		html_data_ += result.errors_line();
	}
	return true;
}

rx_result rx_http_static_display::deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path)
{
	return true;
}

rx_result rx_http_static_display::start_display (runtime::runtime_start_context& ctx, const string_type& disp_path)
{
	auto result = connect_points(ctx, disp_path);
	if (!result)
		html_data_ = "E jebi ga puta 2!!!";
	return true;
}

rx_result rx_http_static_display::stop_display (runtime::runtime_stop_context& ctx, const string_type& disp_path)
{
	auto result = disconnect_points(ctx, disp_path);

	return true;
}

rx_result rx_http_static_display::handle_request_internal (rx_platform::http::http_request& req, rx_platform::http::http_response& resp)
{
	if (req.method == rx_http_method::get)
	{
		if (meta_)
		{
			if (req.params.find("data") != req.params.end())
			{
				resp.set_string_content(collect_json_data());
				resp.headers["Content-Type"] = "application/json";
				resp.result = 200;
			}
			else if (req.params.empty())
			{
				resp.set_string_content(get_dynamic_content(html_data_, "", req.path + "." + req.extension));
				resp.headers["Content-Type"] = "application/json";
				resp.result = 200;
			}
			else
			{
				return "Invalid URL parameters";
			}
		}
		else
		{
			auto it = req.params.find("embed");
			if (it != req.params.end())
			{
				string_type ret = get_dynamic_content(html_data_, it->second, req.path + "." + req.extension);
				size_t idx1 = 0;
				size_t html_size = 0;
				size_t idx2 = 0;
				idx1 = ret.find("<script");
				if (idx1 != string_type::npos)
				{
					html_size = idx1;
					idx1 = ret.find(">", idx1);
					if (idx1 != string_type::npos)
					{
						idx2 = ret.find("</script>", idx1);
						if (idx2 != string_type::npos)
						{
							if (req.params.find("js") != req.params.end())
							{
								resp.headers["Content-Type"] = "text/javascript";
								resp.set_string_content(ret.substr(idx1 + 1, idx2 - idx1 - 1));
								resp.result = 200;
							}
							else
							{
								resp.set_string_content(ret.substr(0, html_size));
								resp.headers["Content-Type"] = "text/html";
								resp.result = 200;
							}
						}
					}
				}
			}
			else if (req.params.find("data") != req.params.end())
			{
				resp.set_string_content(collect_json_data());
				resp.headers["Content-Type"] = "application/json";
				resp.result = 200;
			}
			else if (req.params.empty())
			{
				resp.set_string_content(get_dynamic_content(html_data_, "", req.path + "." + req.extension));
				resp.headers["Content-Type"] = "text/html";
				resp.result = 200;
			}
			else
			{
				return "Invalid URL parameters";
			}
		}
	}
	else if (req.method == rx_http_method::put)
	{
		string_type str(req.get_content_as_string());
		rapidjson::Document doc;
		doc.Parse(str.c_str());
		if (doc.IsObject())
		{
			int cnt = doc.MemberCount();
			if (cnt == 1)
			{
				string_type name = doc.MemberBegin()->name.GetString();
				string_type val = doc.MemberBegin()->value.GetString();

				if (!name.empty())
				{
					auto result = write_point(name, val);
					if (result)
					{
						resp.set_string_content("OK");
						resp.headers["Content-Type"] = "text/plain";
						resp.result = 200;

						return true;
					}
				}
			}
		}
		return "Invalid parameters";
	}
	return true;
}

void rx_http_static_display::fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	string_type id = disp_path;
	std::replace(id.begin(), id.end(), '/', '_');
	std::replace(id.begin(), id.end(), '.', '_');
	//content.mapped_content["display-id"] = "<?embedded-id?>";
}

const string_array& rx_http_static_display::get_point_replace () const
{
	static string_array ret = { "<span id=\"" , "#", "\">" , "$", "</span>" };
	return ret;
}


// Class rx_internal::rx_http_server::http_displays::http_displays_repository 

http_displays_repository::http_displays_repository()
{
}


http_displays_repository::~http_displays_repository()
{
}



rx_result http_displays_repository::register_display (const string_type& path, rx_http_display_base::smart_ptr who)
{
	locks::auto_lock_t _(&displays_lock_);
	auto it = registered_displays_.find(path);
	if (it == registered_displays_.end())
	{
		auto ctx = rx_thread_context();
		//RX_ASSERT()
		registered_displays_.emplace(path, display_data_t { who, ctx });
		return true;
	}
	else
	{
		RX_ASSERT(false);
		return RX_INVALID_PATH;
	}
}

rx_result http_displays_repository::unregister_display (const string_type& path, rx_http_display_base::smart_ptr who)
{
	locks::auto_lock_t _(&displays_lock_);
	auto it = registered_displays_.find(path);
	if (it != registered_displays_.end())
	{
		RX_ASSERT(rx_thread_context() == it->second.executer);
		RX_ASSERT(it->second.display_ptr == who);
		if (it->second.display_ptr == who && rx_thread_context() == it->second.executer)
		{
			registered_displays_.erase(it);
			return true;
		}
		return RX_INTERNAL_ERROR;
	}
	else
	{
		return RX_INVALID_PATH;
	}
}

rx_result_with<rx_http_display_base::smart_ptr> http_displays_repository::get_display (const string_type& path, rx_thread_handle_t& executer)
{
	locks::auto_lock_t _(&displays_lock_);
	auto it = registered_displays_.find(path);
	if (it != registered_displays_.end())
	{
		rx_thread_handle_t temp = it->second.executer;
		executer = temp;
		return it->second.display_ptr;
	}
	else
	{
		return RX_INVALID_PATH;
	}
}


// Class rx_internal::rx_http_server::http_displays::http_display_point 

http_display_point::http_display_point (rx_http_display_base* disp)
      : my_display_(disp),
        str_value_("-")
{
}



void http_display_point::value_changed (const rx_value& val)
{
	string_type temp_val = "-";
	if (val.is_good())
	{
		value_ = val.to_simple();
		if (val.is_float())
		{
			char buff[0x40];
			sprintf(buff, "%g", val.get_float());
			temp_val = buff;
		}
		else
		{
			temp_val = val.to_string();
		}
	}
	else
	{
		value_ = rx_simple_value();
	}
	if (temp_val != str_value_)
	{
		str_value_ = temp_val;
		my_display_->point_changed();
	}

}


const string_type& http_display_point::get_str_value () const
{
  return str_value_;
}


// Class rx_internal::rx_http_server::http_displays::rx_http_display_base 

std::map<string_type, string_type> rx_http_display_base::globals_;

rx_http_display_base::rx_http_display_base()
      : req_(0),
        failed_(0),
        max_req_time_(0),
        last_req_time_(0),
        meta_(false)
{
}

rx_http_display_base::rx_http_display_base (const string_type& name, const rx_node_id& id)
      : req_(0),
        failed_(0),
        max_req_time_(0),
        last_req_time_(0),
        meta_(false)
{
}


rx_http_display_base::~rx_http_display_base()
{
}



rx_result rx_http_display_base::parse_display_data (runtime::runtime_init_context& ctx, const string_type& disp_path, string_type& html_data, const http_display_custom_content& custom)
{
	req_.bind(".Status.Req", ctx);
	failed_.bind(".Status.Failed", ctx);
	last_req_time_.bind(".Status.LastReqTime", ctx);
	max_req_time_.bind(".Status.MaxReqTime", ctx);
	string_array paths = ctx.structure.get_current_item().get_local_as<string_array>("Resources.DisplayFiles", {});
	string_type temp_str;

	html_data.clear();// just in case

	for(const auto& path : paths)
	{
		if (path.empty())
			continue;

		temp_str = http_server::instance().get_dynamic_content(path);

		if (temp_str.empty())
			return "Error in reading file: " + path;
		if(path.size() > 5 && path.substr(path.size() - 5) == ".json")
		{
			meta_ = true;
		}
		html_data += temp_str;
	}

	// preprocess static data
	html_data = preprocess_static(html_data, custom, ctx.meta);

	// now handle dynamic data
	size_t start_idx = html_data.find("<?rx");
	while (start_idx != string_type::npos)
	{
		one_point_data one_data;
		one_data.live = true;
		one_data.start_idx = start_idx;
		start_idx += 4;
		size_t end_idx = html_data.find("?>", start_idx);
		if (end_idx != string_type::npos)
		{
			one_data.end_idx = end_idx + 2;
			one_data.point_path = html_data.substr(start_idx, end_idx - start_idx);
			if (one_data.point_path.size() >= 1 && one_data.point_path[0]!='-')
			{
				bool had = false;
				char buff[0x20];
				for (const auto& one : connected_points_)
				{
					if (one.second.point_path == one_data.point_path)
					{
						strcpy(buff, one.second.point_id.c_str());
						had = true;
						break;
					}
				}
				if (!had)
				{
					sprintf(buff, "rxPt%08x", rx_get_new_handle());
				}
				one_data.point_id = buff;
				one_data.point = std::make_unique<http_display_point>(this);
				connected_points_.emplace(one_data.start_idx, std::move(one_data));
			}

			start_idx = one_data.end_idx;

			start_idx = html_data.find("<?rx", start_idx);
		}
		else
		{
			break;
		}
	}
	return true;
}

string_type rx_http_display_base::preprocess_static (const string_type& content, const http_displays::http_display_custom_content& custom, const meta_data& meta_info)
{
	string_type ret;
	size_t str_len = content.size();
	size_t start_idx = content.find("<?rx-");
	size_t rest_idx = 0;
	int type = -1;
	char temp_buff[0x40];
	sprintf(temp_buff, "%08x", meta_info.version);
	version_check_ = temp_buff;
	while (start_idx != string_type::npos)
	{
		string_view_type test(&content[start_idx + 5], 6);
		type = -1;
		ret += content.substr(rest_idx, start_idx - rest_idx);
		if (start_idx + 12 < str_len && string_view_type(&content[start_idx + 5], 7) == "global:")
		{
			type = 0;
			start_idx += 12;
		}
		else if (start_idx + 11 < str_len && string_view_type(&content[start_idx + 5], 6) == "local:")
		{
			type = 1;
			start_idx += 11;
		}
		size_t end_idx = content.find("?>", start_idx);
		if (end_idx != string_type::npos)
		{
			string_type replacement = content.substr(start_idx, end_idx - start_idx);
			const char* replacement_ptr = replacement.c_str();
			while (*replacement_ptr == '\0'
				|| *replacement_ptr == ' '
				|| *replacement_ptr == '\t'
				|| *replacement_ptr == '\r'
				|| *replacement_ptr == '\n')
				replacement_ptr++;
			const char* start_ptr = replacement_ptr;
			while (!(*replacement_ptr == '\0'
				|| *replacement_ptr == ' '
				|| *replacement_ptr == '\t'
				|| *replacement_ptr == '\r'
				|| *replacement_ptr == '\n'))
				replacement_ptr++;
			// this will do the job!!!
			string_view_type easier(start_ptr, replacement_ptr - start_ptr);
			switch (type)
			{
			case 0:
				{// global vars
					auto it = globals_.find(string_type(easier));
					if (it != globals_.end())
						ret += it->second;
					else
						ret += ("@ERROR@ <"s + string_type(easier) + "> is unknown global variable!");

				}
				break;
			case 1:
				{// local vars
					auto it = custom.mapped_content.find(string_type(easier));
					if (it != custom.mapped_content.end())
					{
						ret += it->second;
					}
					else if (easier == "display-id")
					{
						one_point_data one_data;
						one_data.live = false;
						one_data.start_idx = ret.size();
						ret += "<?embedded-id?>";
						one_data.end_idx = ret.size();
						one_data.point_path = "#";
						connected_points_.emplace(one_data.start_idx, std::move(one_data));
					}
					else if (easier == "display-path")
					{
						one_point_data one_data;
						one_data.live = false;
						one_data.start_idx = ret.size();
						ret += "<?req-path?>";
						one_data.end_idx = ret.size();
						one_data.point_path = "$";
						connected_points_.emplace(one_data.start_idx, std::move(one_data));
					}
					else if (easier == "head")
					{
						ret += custom.head_content;
					}
					else if (easier == "body-start")
					{
						ret += custom.body_begin_content;
					}
					else if (easier == "body-end")
					{
						ret += custom.body_end_content;
					}
					else if (easier == "name")
					{
						ret += meta_info.name;
					}
					else if (easier == "name")
					{
						ret += meta_info.name;
					}
					else if (easier == "path")
					{
						ret += meta_info.path;
					}
					else if (easier == "full-path")
					{
						ret += meta_info.get_full_path();
					}
					else if (easier == "created")
					{
						ret += meta_info.created_time.get_IEC_string();
					}
					else if (easier == "modified")
					{
						ret += meta_info.modified_time.get_IEC_string();
					}
					else if (easier == "version")
					{
						int major = (meta_info.version >> 16);
						int minor = (meta_info.version & 0xffff);
						char buff[0x20];
						sprintf(buff, "%d.%d", major, minor);
						ret += buff;
					}
					else
					{
						ret += ("@ERROR@ <"s + string_type(easier) + "> is unknown local variable!");
					}
				}
				break;
			}

			start_idx = end_idx + 2;
			rest_idx = start_idx;

			start_idx = content.find("<?rx-", start_idx);
		}
		else
		{
			return "";
		}
	}
	ret += content.substr(rest_idx);
	return ret;
}

rx_result rx_http_display_base::connect_points (runtime::runtime_start_context& ctx, const string_type& disp_path)
{
	size_t idx = 0;
	for (auto& one : connected_points_)
	{
		if (one.second.live)
		{
			one.second.point->set_context(ctx.context);
			one.second.point->connect(one.second.point_path, 200, nullptr, points_buffer_);
			points_hash_.emplace(one.second.point_id, one.second.start_idx);
		}
		idx++;
	}
	return true;
}

rx_result rx_http_display_base::disconnect_points (runtime::runtime_stop_context& ctx, const string_type& disp_path)
{
	for (auto& one : connected_points_)
	{
		if (one.second.live)
		{
			one.second.point->disconnect();
		}
	}
	return true;
}

string_type rx_http_display_base::get_dynamic_content (const string_type& html_data, const string_type& embedded_id, const string_type& display_path)
{
	string_type ret_content;
	ret_content.reserve(html_data.size());
	size_t last_idx = 0;
	if (!embedded_id.empty())
	{
		auto idx = html_data.find("<body");
		if (idx != string_type::npos)
		{
			idx = html_data.find('>', idx + 5);
			if (idx != string_type::npos)
			{
				last_idx = idx + 1;
			}
		}
	}
	bool has_dynamic = false;
	for (auto& one : connected_points_)
	{
		ret_content += html_data.substr(last_idx, one.second.start_idx - last_idx);


		if (one.second.live)
		{
			string_type point_id = one.second.point_id;
			string_type value = one.second.point->get_str_value();
			has_dynamic = true;

			auto& format = get_point_replace();
			for (const auto& one_format : format)
			{
				if (one_format == "#")
				{
					ret_content += point_id;
				}
				else if (one_format == "$")
				{
					ret_content += value;
				}
				else
				{
					ret_content += one_format;
				}
			}
		}
		else if (one.second.point_path == "#")
		{
			ret_content += embedded_id.empty() ? "id"s : embedded_id;
		}
		else if (one.second.point_path == "$")
		{
			if (display_path.size() > 0 && display_path[0] == '/')
			{
				ret_content += &display_path.c_str()[1];
			}
			else
			{
				ret_content += display_path;
			}
		}
		last_idx = one.second.end_idx;
	}
	if (!embedded_id.empty())
	{
		auto idx = html_data.find("</body>", last_idx);
		if (idx != string_type::npos)
		{
			ret_content += html_data.substr(last_idx, idx - last_idx);
		}
	}
	else
	{
		ret_content += html_data.substr(last_idx);
	}

	if (has_dynamic)
	{

	}
	return ret_content;
}

string_type rx_http_display_base::collect_json_data ()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	writer.StartObject();
		writer.Key("ver");
		writer.String(version_check_.c_str());
		writer.Key("data");
		writer.StartObject();
			for (const auto& one : connected_points_)
			{
				if (one.second.live)
				{
					writer.Key(one.second.point_id.c_str());
					const auto& val = one.second.point->get_value();
					if(val.is_null())
					{
						writer.Null();
					}
					else if (val.get_type() == RX_BOOL_TYPE)
					{
						writer.Bool(val.get_bool());
					}
					else if (val.is_numeric())
					{
						writer.Double(val.get_float());
					}
					else
					{
						writer.String(val.to_string().c_str());
					}
				}
			}
		writer.EndObject();
	writer.EndObject();
	return string_type(buffer.GetString());
}

rx_result rx_http_display_base::handle_request (rx_platform::http::http_request& req, rx_platform::http::http_response& resp)
{


	auto result = handle_request_internal(req, resp);

	if (result)
	{
		return true;
	}
	else
	{
		resp.result = 500;
		return true;
	}
}

rx_result rx_http_display_base::register_display (runtime::runtime_start_context& ctx, const string_type& disp_path)
{
	string_array registered_paths = get_paths_to_register(ctx.context, disp_path);
	for (const auto& path : registered_paths)
	{
		auto ret = rx_internal::rx_http_server::http_server::instance().get_displays().register_display(path, smart_this());
		if (!ret)
		{
			HTTP_LOG_ERROR("rx_http_display_base", 200, "Error registering path "s + path + " as http display" + ret.errors_line().c_str());
		}
	}
	return true;
}

rx_result rx_http_display_base::unregister_display (runtime::runtime_stop_context& ctx, const string_type& disp_path)
{
	string_array registered_paths = get_paths_to_register(ctx.context, disp_path);
	for (const auto& path : registered_paths)
	{
		auto ret = rx_internal::rx_http_server::http_server::instance().get_displays().unregister_display(path, smart_this());
		if (!ret)
		{
			HTTP_LOG_ERROR("rx_http_display_base", 200, "Error unregistering path "s + path + " as http display" + ret.errors_line().c_str());
		}
	}
	return true;
}

string_array rx_http_display_base::get_paths_to_register (runtime::runtime_process_context* ctx, const string_type& disp_path)
{
	string_array registered_paths;
	string_type disp_name;
	bool is_index = false;
	auto idx = disp_path.rfind('.');
	if (idx != string_type::npos)
	{
		disp_name = disp_path.substr(idx + 1);
		is_index = (disp_name == "index");
	}
	registered_paths.push_back(disp_path);
	if (is_index)
		registered_paths.push_back(disp_path.substr(0, idx));
	if (disp_path.size() > 6 && string_view_type(&disp_path[0], 6) == "/world")
	{
		registered_paths.push_back(disp_path.substr(6));
		if (is_index)
			registered_paths.push_back(disp_path.substr(6, idx - 6));
	}
	if (ctx->meta_info.attributes & namespace_item_system_mask)
	{
		if (is_index)
			registered_paths.push_back("/"s + ctx->meta_info.name);
		registered_paths.push_back("/"s + ctx->meta_info.name + RX_OBJECT_DELIMETER + disp_name);
	}
	return registered_paths;
}

void rx_http_display_base::fill_globals ()
{
	globals_.emplace("instance", rx_gate::instance().get_instance_name());
	globals_.emplace("node", rx_gate::instance().get_node_name());
	globals_.emplace("version", rx_gate::instance().get_rx_version());
	globals_.emplace("lib", rx_gate::instance().get_lib_version());

	globals_.emplace("http", http_server::instance().get_server_info());
	globals_.emplace("terminal", terminal::term_ports::vt100_endpoint::get_terminal_info());

	globals_.emplace("firmware", rx_gate::instance().get_hal_version());
	globals_.emplace("system", rx_gate::instance().get_os_info());
	globals_.emplace("compiler", rx_gate::instance().get_comp_version());

	globals_.emplace("start", rx_gate::instance().get_started().get_IEC_string());

	/////////////////////////////////////////////////////////////////////////
	// Processor
	char buff[0x100];
	size_t cpu_count = 1;
	std::ostringstream out1;
	rx_collect_processor_info(buff, sizeof(buff) / sizeof(buff[0]), &cpu_count);
	out1 <<  buff
		<< (rx_big_endian ? "; Big-endian" : "; Little-endian");
	globals_.emplace("cpu", out1.str());

	/////////////////////////////////////////////////////////////////////////
	// memory
	size_t total = 0;
	size_t free = 0;
	size_t process = 0;
	std::ostringstream out2;
	rx_collect_memory_info(&total, &free, &process);
	out2 << "Total "
		<< (int)(total / 1048576ull)
		<< "MiB / Free "
		<< (int)(free / 1048576ull)
		<< "MiB / Process "
		<< (int)(process / 1024ull)
		<< "KiB \r\n";
	/////////////////////////////////////////////////////////////////////////
	out2 << "Page size: " << (int)rx_os_page_size() << " bytes";
	globals_.emplace("memory", out2.str());

	hosting::hosts_type hosts;
	rx_gate::instance().get_host()->get_host_info(hosts);
	if (hosts.size() > 0)
		globals_.emplace("os-host", hosts[0]);
	if (hosts.size() > 1)
		globals_.emplace("host", *hosts.rbegin());
}

void rx_http_display_base::point_changed ()
{
}

rx_result rx_http_display_base::write_point (const string_type& id, const string_type& val)
{
	auto it = points_hash_.find(id);
	if (it != points_hash_.end())
	{
		auto it2 = connected_points_.find(it->second);
		if (it2 != connected_points_.end())
		{
			rx_simple_value rval;
			rval.assign_static(val);
			it2->second.point->write(std::move(rval), 0);
			return true;
		}
	}
	return RX_INVALID_ARGUMENT;
}

const char* system_paths[] = {
	"On",
	"Test",
	"Blocked",
	"Simulate",
	"SimActive",
	"LastScanTime",
	"MaxScanTime",
	"LoopCount",
	"ProcessSlots"
};
// Class rx_internal::rx_http_server::http_displays::rx_http_standard_display 

rx_http_standard_display::rx_http_standard_display()
{
}

rx_http_standard_display::rx_http_standard_display (const string_type& name, const rx_node_id& id)
	: rx_http_static_display(name, id)
{
}


rx_http_standard_display::~rx_http_standard_display()
{
}



void rx_http_standard_display::fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	std::ostringstream stream;
	data::runtime_values_data data;
	ctx.structure.get_root().collect_data("", data, runtime_value_type::simple_runtime_value);
	fill_div(stream, ctx.meta.name, "", data);
	content.mapped_content["main"] = stream.str();
}

void rx_http_standard_display::fill_div (std::ostream& stream, const string_type& rt_name, const string_type& path, const data::runtime_values_data& data)
{

	using child_type = data::runtime_values_data::child_type;
	using array_child_type = data::runtime_values_data::array_child_type;

	using value_type = data::runtime_values_data::value_type;
	using array_value_type = data::runtime_values_data::array_value_type;

	if (!data.values.empty())
	{
		stream << "<div class=\"mainsm3\">\r\n";
		stream << "<h3>"
			<< (path.empty() ? rt_name : rt_name + path)
			<< "</h3><table class=\"values_table\">\r\n";

		for (const auto& one : data.values)
		{
			if (std::holds_alternative<value_type>(one.second))
			{
				stream << "  <tr class=\"values_table_row\">\r\n    <td class=\"values_name_cell\">"
					<< one.first << ":"
					<< "</td>\r\n"
					<< "    <td class=\"values_value_cell\"><b><?rx "
					<< path << RX_OBJECT_DELIMETER << one.first
					<< " ?></b></td>\r\n"
					<< "  </tr>\r\n";
			}
			else
			{
				auto& array_struct = std::get<array_value_type>(one.second);
				for (size_t i = 0; i < array_struct.size(); i++)
				{
					char index_buffer[0x20];
					sprintf(index_buffer, "[%u]", (uint32_t)i);
					stream << "  <tr class=\"values_table_row\">\r\n    <td class=\"values_name_cell\">"
						<< one.first << index_buffer << ":"
						<< "</td>\r\n"
						<< "    <td class=\"values_value_cell\"><b><?rx "
						<< path << RX_OBJECT_DELIMETER << one.first << index_buffer
						<< " ?></b></td>\r\n"
						<< "  </tr>\r\n";
				}
			}
		}
		stream << "</table>\r\n</div>";
	}
	for (const auto& one : data.children)
	{
		if (std::holds_alternative<child_type>(one.second))
		{
			fill_div(stream, rt_name, path + RX_OBJECT_DELIMETER + one.first, std::get<child_type>(one.second));
		}
		else
		{
			auto& array_struct = std::get<array_child_type>(one.second);
			for (size_t i = 0; i < array_struct.size(); i++)
			{
				char index_buffer[0x20];
				sprintf(index_buffer, "[%u]", (uint32_t)i);
				fill_div(stream, rt_name, path + RX_OBJECT_DELIMETER + one.first + index_buffer, array_struct[i]);
			}
		}
	}

	if (path.empty())
	{
		stream << "<div class=\"mainsm3\">\r\n";
		stream << "<h3>"
			<< rt_name + "._Object"
			<< "</h3><table class=\"values_table\">\r\n";

		for (size_t i = 0; i < sizeof(system_paths) / sizeof(system_paths[0]); i++)
		{
			stream << "  <tr class=\"values_table_row\">\r\n    <td class=\"values_name_cell\">"
				<< system_paths[i] << ":"
				<< "</td>\r\n"
				<< "    <td class=\"values_value_cell\"><b><?rx "
				<< RX_OBJECT_DELIMETER << "_Object" << RX_OBJECT_DELIMETER << system_paths[i]
				<< " ?></b></td>\r\n"
				<< "  </tr>\r\n";
		}
		stream << "</table>\r\n</div>";
	}
}


// Class rx_internal::rx_http_server::http_displays::http_display_custom_content 


// Class rx_internal::rx_http_server::http_displays::rx_http_simple_display 

rx_http_simple_display::rx_http_simple_display()
{
}

rx_http_simple_display::rx_http_simple_display (const string_type& name, const rx_node_id& id)
	: rx_http_static_display(name, id)
{
}


rx_http_simple_display::~rx_http_simple_display()
{
}



void rx_http_simple_display::fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	content.mapped_content["main"] = ctx.structure.get_current_item().get_local_as<string_type>("Content", "");
}


// Class rx_internal::rx_http_server::http_displays::rx_http_main_display 

rx_http_main_display::rx_http_main_display()
{
}

rx_http_main_display::rx_http_main_display (const string_type& name, const rx_node_id& id)
	: rx_http_static_display(name, id)
{
}


rx_http_main_display::~rx_http_main_display()
{
}



void rx_http_main_display::fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	std::ostringstream stream;
	data::runtime_values_data data;
	ctx.structure.get_root().collect_data("", data, runtime_value_type::simple_runtime_value);

	const auto& plugins = plugins::plugins_manager::instance().get_plugins();
	for (const auto& one : plugins)
	{
		stream << "<tr><td>"
			<< one->get_plugin_name()
			<< ":</td>"
			<< "<td><b>"
			<< one->get_plugin_info().plugin_version
			<< "</b></td></tr>";
	}
	content.mapped_content["plugins"] = stream.str();
}

void rx_http_main_display::fill_div (std::ostream& stream, const string_type& rt_name, const string_type& path, const data::runtime_values_data& data)
{
}


} // namespace http_displays
} // namespace rx_http_server
} // namespace rx_internal

