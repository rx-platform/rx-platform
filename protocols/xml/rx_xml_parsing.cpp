

/****************************************************************************
*
*  protocols\xml\rx_xml_parsing.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifdef _MSC_VER 
#ifdef _DEBUG
#undef new
#endif
#endif

#include "third-party/rapidxml/rapidxml.hpp"

#ifdef _MSC_VER 
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif
using namespace rapidxml;



// rx_xml_parsing
#include "protocols/xml/rx_xml_parsing.h"



namespace protocols {

namespace rx_xml {
namespace
{

struct xml_node_data
{
	xml_node<>* node;
	string_type addr;
};

typedef std::map<string_type, xml_node<>*> nodes_data_t;

void recursive_get_addresable_nodes(xml_node<>* parent, size_t idx, const string_array& paths, nodes_data_t& nodes)
{
	if (idx < paths.size())
	{
		for (auto node = parent->first_node(paths[idx].c_str()); node; node = node->next_sibling(paths[idx].c_str()))
		{
			recursive_get_addresable_nodes(node, idx + 1, paths, nodes);
		}
	}
	else
	{
		nodes[parent->value()] = parent->parent();
	}
}
string_type recursive_get_value(xml_node<>* parent, size_t idx, const string_array& paths)
{
	if (idx < paths.size())
	{
		for (auto node = parent->first_node(paths[idx].c_str()); node; node = node->next_sibling(paths[idx].c_str()))
		{
			string_type ret = recursive_get_value(node, idx + 1, paths);
			if (!ret.empty())
				return ret;
		}
		return ""s;
	}
	else
	{
		return parent->value();
	}
}


static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}


}

// Class protocols::rx_xml::rx_xml_endpoint 

rx_xml_endpoint::rx_xml_endpoint (rx_reference<rx_xml_port> port)
      : port_(port)
{
    XML_LOG_DEBUG("rx_xml_endpoint", 200, "XML parser endpoint created.");
    rx_init_stack_entry(&stack_entry_, this);
    stack_entry_.received_function = &rx_xml_endpoint::received_function;

}


rx_xml_endpoint::~rx_xml_endpoint()
{
    XML_LOG_DEBUG("rx_xml_endpoint", 200, "XML parser endpoint destroyed.");
}



rx_protocol_result_t rx_xml_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    rx_xml_endpoint* me = reinterpret_cast<rx_xml_endpoint*>(reference->user_data);
    uint16_t trans_id = packet.id;

    if (me->port_)
    {
        //me->port_->status_received_packet();
        uint64_t ticks = rx_get_us_ticks();
    }
    size_t packet_size = rx_get_packet_available_data(packet.buffer);
    if (packet_size > 0)
    {
        me->port_->XML_received(string_view_type((const char*)&packet.buffer->buffer_ptr[packet.buffer->next_read], packet_size));
    }
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_xml_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return RX_PROTOCOL_OK;
}

rx_protocol_stack_endpoint* rx_xml_endpoint::bind_endpoint ()
{
    return &stack_entry_;
}

void rx_xml_endpoint::close_endpoint ()
{
}

rx_reference<rx_xml_port> rx_xml_endpoint::get_port ()
{
    return port_;
}


// Class protocols::rx_xml::rx_xml_port 

std::map<rx_node_id, rx_xml_port::smart_ptr> rx_xml_port::runtime_instances;

rx_xml_port::rx_xml_port()
{
    construct_func = [this]()
        {
            auto rt = rx_create_reference<rx_xml_endpoint>(smart_this());
            auto entry = rt->bind_endpoint();
            return construct_func_type::result_type{ entry, rt };
        };
}


rx_xml_port::~rx_xml_port()
{
}



void rx_xml_port::stack_assembled ()
{
    auto result = listen(nullptr, nullptr);
    if (!result)
    {

    }
}

rx_result rx_xml_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	string_type addr = ctx.get_item_static<string_type>("Options.AddrXMLPath", "");

	rx_split_string(addr, xml_addr_path_, '>');

	if (!xml_addr_path_.empty())
	{
		auto idx = xml_addr_path_.rbegin()->find('#');
		if (idx != string_type::npos)
		{
			xml_addr_attr_ = xml_addr_path_.rbegin()->substr(idx + 1);
			*xml_addr_path_.rbegin() = xml_addr_path_.rbegin()->substr(0, idx);
		}
	}
    return true;
}

void rx_xml_port::XML_received (string_view_type data)
{
	char static_buffer[0x400];
	bool is_static_buffer = data.size() + 1 < sizeof(static_buffer) / sizeof(static_buffer[0]);
	char* buff_ptr = is_static_buffer ? static_buffer : new char[data.size() + 1];
	memcpy(buff_ptr, &data[0], data.size());
	buff_ptr[data.size()] = '\0';
	bool had_exception = false;
	rx_time now = rx_time::now();
	try
	{
		xml_document<> doc;
		doc.parse<0>(buff_ptr);    // 0 means default parse flags

		if (!xml_addr_path_.empty())
		{
			size_t addr_len = xml_addr_path_.size();
			size_t idx = 0;
			auto node = doc.first_node(xml_addr_path_[idx].c_str());
			if (node)
			{
				idx++;
				nodes_data_t nodes;
				recursive_get_addresable_nodes(node, idx, xml_addr_path_, nodes);

				std::scoped_lock _(sources_lock_);

				if (!sources_.empty())
				{
					for (auto it = nodes.begin(); it != nodes.end(); it++)
					{
						received_xmls_[it->first] = data;
						auto it_src = sources_.find(it->first);
						if (it_src != sources_.end())
						{
							for (auto one : it_src->second)
							{
								string_type val = recursive_get_value(it->second, 0, one->get_xml_path());
								one->xml_changed(val, now);
							}
						}
					}
				}
			}
		}

	}
	catch (parse_error& ex)
	{
		had_exception = true;
		std::ostringstream ss;
		ss << "Error parsing XML:"
			<< ex.what();
		XML_LOG_ERROR("SAM", 500, ss.str().c_str());
	}
	catch (std::exception& ex)
	{
		had_exception = true;
		std::ostringstream ss;
		ss << "Error occurred while parsing XML:"
			<< ex.what();
		XML_LOG_ERROR("SAM", 500, ss.str().c_str());
	}
	if (had_exception)
	{
		locks::auto_lock_t _(&sources_lock_);

		if (!sources_.empty())
		{
			for (auto& one : sources_)
			{
				for (auto one_src : one.second)
				{
					one_src->notify_error(now);
				}
			}
		}
	}
	if (!is_static_buffer)
		delete[] buff_ptr;
}

void rx_xml_port::register_source (const string_type& addr, xml_source_ptr what)
{
	locks::auto_lock_t _(&sources_lock_);
	auto it_addr = sources_.find(string_type(addr));
	if (it_addr != sources_.end())
	{
		it_addr->second.insert(what);
	}
	else
	{
		auto result = sources_.emplace(string_type(addr), sources_type::mapped_type());
		if (result.second)
			result.first->second.insert(what);
	}
	auto it_xml = received_xmls_.find(string_type(addr));
	if (it_xml != received_xmls_.end())
	{
		parse_old_XML(addr, it_xml->second, what);
	}
}

void rx_xml_port::unregister_source (const string_type& addr, xml_source_ptr what)
{
	locks::auto_lock_t _(&sources_lock_);
	auto it_addr = sources_.find(string_type(addr));
	if (it_addr != sources_.end())
	{
		it_addr->second.erase(what);
	}
}

void rx_xml_port::parse_old_XML (string_view_type addr, string_view_type data, xml_source_ptr whose)
{
	char static_buffer[0x400];
	bool is_static_buffer = data.size() + 1 < sizeof(static_buffer) / sizeof(static_buffer[0]);
	char* buff_ptr = is_static_buffer ? static_buffer : new char[data.size() + 1];
	memcpy(buff_ptr, &data[0], data.size());
	buff_ptr[data.size()] = '\0';

	try
	{
		xml_document<> doc;
		doc.parse<0>(buff_ptr);    // 0 means default parse flags

		if (!xml_addr_path_.empty())
		{
			size_t addr_len = xml_addr_path_.size();
			size_t idx = 0;
			auto node = doc.first_node(xml_addr_path_[idx].c_str());
			if (node)
			{
				idx++;
				nodes_data_t nodes;
				recursive_get_addresable_nodes(node, idx, xml_addr_path_, nodes);

				if (!nodes.empty())
				{
					for (auto it = nodes.begin(); it != nodes.end(); it++)
					{
						if (it->first == addr)
						{
							string_type val = recursive_get_value(it->second, 0, whose->get_xml_path());
							whose->xml_changed(val, rx_time::now());
						}
					}
				}
			}
		}

	}
	catch (parse_error& ex)
	{
		std::ostringstream ss;
		ss << "Error parsing XML:"
			<< ex.what();
		XML_LOG_ERROR("SAM", 500, ss.str().c_str());
	}
	catch (std::exception& ex)
	{
		std::ostringstream ss;
		ss << "Error occurred while parsing XML:"
			<< ex.what();
		XML_LOG_ERROR("SAM", 500, ss.str().c_str());
	}
	if (!is_static_buffer)
		delete[] buff_ptr;
}


// Class protocols::rx_xml::rx_xml_source 

rx_xml_source::rx_xml_source()
{
}


rx_xml_source::~rx_xml_source()
{
}



rx_result rx_xml_source::initialize_source (runtime::runtime_init_context& ctx)
{
	
	string_type addr = ctx.get_item_static<string_type>(".XMLPath", "");
	rx_split_string(addr, xml_path_, '>');

	xml_addr_ = ctx.get_item_static<string_type>(".XMLAddr", "");
	

	return true;
}

void rx_xml_source::port_connected (port_ptr_t port)
{
	if (port)
	{
		port->register_source(xml_addr_, smart_this());
	}
}

void rx_xml_source::port_disconnected (port_ptr_t port)
{
	if (port)
	{
		port->unregister_source(xml_addr_, smart_this());
	}
}

const string_array& rx_xml_source::get_xml_path () const
{
	return xml_path_;
}

void rx_xml_source::xml_changed (const string_type& val, rx_time now)
{
	rx_value rx_val;
	rx_val.set_quality(RX_GOOD_QUALITY);

	string_type str_val(val);
	trim(str_val);
	rx_val.assign_static(str_val.c_str(), now);
	if (!rx_val.convert_to(get_value_type()))
	{
		rx_val = rx_value();
		rx_val.convert_to(get_value_type());
		rx_val.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
	}
	rx_val.set_time(now);
	source_value_changed(std::move(rx_val));
}

void rx_xml_source::notify_error (rx_time now)
{
	rx_value rx_val;
	rx_val.set_quality(RX_BAD_QUALITY_SYNTAX_ERROR);
	rx_val.set_time(now);
	rx_val.convert_to(get_value_type());
	source_value_changed(std::move(rx_val));
}


} // namespace rx_xml
} // namespace protocols

