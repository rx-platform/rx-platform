

/****************************************************************************
*
*  protocols\opcua\rx_opcua_basic.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_opcua_basic
#include "protocols/opcua/rx_opcua_basic.h"
// rx_opcua_subscriptions
#include "protocols/opcua/rx_opcua_subscriptions.h"

#include "protocols/opcua/rx_opcua_mapping.h"
#include "protocols/opcua/rx_opcua_binary.h"
#include "protocols/opcua/rx_opcua_identifiers.h"
#include "protocols/opcua/rx_opcua_requests.h"
#include "protocols/opcua/rx_opcua_builder.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "rx_opcua_resources.h"
using namespace protocols::opcua::opcua_addr_space;

using namespace protocols::opcua::ids;

namespace rx_platform
{
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();
}


namespace protocols {

namespace opcua {

namespace opcua_basic_server {

// Class protocols::opcua::opcua_basic_server::opcua_basic_server_port 

std::map<rx_node_id, opcua_basic_server_port::smart_ptr> opcua_basic_server_port::runtime_instances;

opcua_basic_server_port::opcua_basic_server_port()
	: subscriptions(&address_space)
{
	construct_func = [this]()
	{
		string_type endpoint_url = application_description_.ep_bind;

		auto rt = rx_create_reference<opcua_basic_server_endpoint>(
			application_description_, endpoint_url, port_path_, this);

		return construct_func_type::result_type{ &rt->stack_entry, rt };
	};
	address_space.set_parent(&std_address_space);
	resolver_user_.my_port = this;
}



void opcua_basic_server_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}

rx_result opcua_basic_server_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	string_type app_uri = ctx.get_item_static("Options.AppUri", ""s);
	string_type app_name = ctx.get_item_static("Options.AppName", ""s);
	string_type app_bind = ctx.get_item_static("Bind.Endpoint", ""s);


	if (app_uri.empty())
		app_uri = app_bind.c_str();;

	if (app_name.empty())
		app_name = app_uri;


	application_description_ = opcua_server_endpoint_base::fill_application_description(app_uri, app_name, app_bind, "BasicServer");

	auto result = opcua_addr_space::build_standard_address_space(std_address_space, application_description_.application_uri, app_uri, "BasicServer");
	if (!result)
		return result;

	return result;
}

rx_result opcua_basic_server_port::register_node (std::shared_ptr<opcua_basic_node> node)
{
	return address_space.register_node(node);
}

rx_result opcua_basic_server_port::unregister_node (std::shared_ptr<opcua_basic_node> node)
{
	return address_space.unregister_node(node);
}

bool opcua_basic_server_port::internal_port_connected (const platform_item_ptr& item)
{
	port_path_ = item->meta_info().get_full_path();
	opcua_resources_repository::instance().register_server(port_path_, application_description_);
	return true;
}

void opcua_basic_server_port::internal_port_disconnected ()
{
	opcua_resources_repository::instance().unregister_server(port_path_, application_description_.ep_bind);
}

rx_result opcua_basic_server_port::start_runtime (runtime::runtime_start_context& ctx)
{
	auto ret = ctx.register_relation_subscriber("StackTop", &resolver_user_);
	if (!ret)
	{
		RUNTIME_LOG_WARNING("opcua_basic_server_port", 900, "Error starting port registration "
			+ ctx.context->meta_info.get_full_path() + "." + ctx.path.get_current_path() + " " + ret.errors_line());
	}
	return true;
}

rx_result opcua_basic_server_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class protocols::opcua::opcua_basic_server::opcua_basic_server_endpoint 

opcua_basic_server_endpoint::opcua_basic_server_endpoint (const application_description& app_description, const string_type& endpoint_url, const string_type& port_path, opcua_basic_server_port* port)
      : executer_(-1),
        port_(port)
		, opcua_server_endpoint_base(endpoint_url, app_description, endpoint_url, &port->address_space, &port->subscriptions, port_path)
{
    OPCUA_LOG_DEBUG("opcua_basic_server_endpoint", 200, "Basic OPC UA Server endpoint created.");
    rx_init_stack_entry(&stack_entry, this);
    stack_entry.received_function = &opcua_basic_server_endpoint::received_function;
	stack_entry.connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
	{
		opcua_basic_server_endpoint* me = (opcua_basic_server_endpoint*)reference->user_data;
		return me->connected_function(session);
	};

}


opcua_basic_server_endpoint::~opcua_basic_server_endpoint()
{
	OPCUA_LOG_DEBUG("opcua_basic_server_endpoint", 200, "Basic OPC UA Server endpoint destroyed.");
}



rx_protocol_result_t opcua_basic_server_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	opcua_basic_server_endpoint* me = (opcua_basic_server_endpoint*)reference->user_data;
	io::rx_const_io_buffer received(packet.buffer);

	rx_node_id msg_id;

	opcua::binary::ua_binary_istream stream(&received);
	stream >> msg_id;

	requests::opcua_request_ptr request = requests::opcua_requests_repository::instance().get_request(msg_id);
	if (request)
	{
		auto result = request->deserialize_header_binary(stream);
		if (result)
		{
			result = request->deserialize_binary(stream);

			if (result)
			{
				request->request_id = packet.id;
				requests::opcua_response_ptr resp = request->do_job(me->smart_this());

				if (resp)
				{
					result = me->send_response(std::move(resp));
				}
			}
		}
		if(!result)
		{
			OPCUA_LOG_ERROR("opcua_basic_server_endpoint", 500, "Error while handling request:"s + result.errors_line());
		}
	}
	else
	{
		RX_ASSERT(false);
	}
	return RX_PROTOCOL_OK;
}

rx_protocol_result_t opcua_basic_server_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

void opcua_basic_server_endpoint::close_endpoint ()
{
}

rx_result opcua_basic_server_endpoint::send_response (requests::opcua_response_ptr resp)
{
	rx_result result;
	auto to_send = port_->alloc_io_buffer();
	if (to_send)
	{
		opcua::binary::ua_binary_ostream ostream(&to_send.value());
		ostream << resp->get_binary_response_id();
		result = resp->serialize_header_binary(ostream);
		if (result)
		{
			result = resp->serialize_binary(ostream);
			if (result)
			{
				auto packet = rx_create_send_packet(resp->request_id, &to_send.value(), 0, 0);
				auto protocol_result = rx_move_packet_down(&stack_entry, packet);
				if (protocol_result != RX_PROTOCOL_OK)
				{
					result = rx_protocol_error_message(protocol_result);
				}
			}
		}
		port_->release_io_buffer(to_send.move_value());
	}
	else
	{
		result = to_send.errors();
	}
	return result;
}


// Class protocols::opcua::opcua_basic_server::opcua_simple_address_space 

opcua_simple_address_space::opcua_simple_address_space()
      : parent_(nullptr),
        config_ts_(rx_time::now())
{
}



void opcua_simple_address_space::set_parent (opcua_addr_space::opcua_address_space_base* parent)
{
	RX_ASSERT(parent_ == nullptr);
	parent_ = parent;
}

rx_result opcua_simple_address_space::register_node (std::shared_ptr<opcua_basic_node> what)
{
	locks::auto_write_lock _(get_lock());
	auto current_it = variable_nodes_.find(what->get_node_id());
	if (current_it != variable_nodes_.end())
	{
		return "Duplicated node id";
	}

	string_type path = what->get_path();
	rx_node_id parent_id = get_folder_node(path, rx_node_id::opcua_standard_id(opcid_ObjectsFolder));

	what->references.inverse_references.emplace_back(rx_node_id::opcua_standard_id(opcid_Organizes), parent_id);

	for (auto& one : what->get_reference_data().references)
	{
		auto target_ptr = connect_node_reference(what, one, false);
		if (target_ptr)
			one.resolved_node = target_ptr;
	}
	for (auto& one : what->get_reference_data().inverse_references)
	{
		auto target_ptr = connect_node_reference(what, one, true);
		if (target_ptr)
			one.resolved_node = target_ptr;
	}
	variable_nodes_.emplace(what->get_node_id(), what);
	return true;
}

rx_result opcua_simple_address_space::unregister_node (std::shared_ptr<opcua_basic_node> what)
{
	locks::auto_write_lock _(get_lock());
	auto current_it = variable_nodes_.find(what->get_node_id());
	if (current_it == variable_nodes_.end())
	{
		return "Unknown node id";
	}
	variable_nodes_.erase(current_it);
	return true;
}

void opcua_simple_address_space::read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const
{
	if (parent_ != nullptr)
	{
		parent_->read_attributes(to_read, values);
	}
	else
	{
		for (const auto& one : to_read)
		{
			data_value temp_val;
			temp_val.status_code = opcid_Bad_NodeIdUnknown;
			values.push_back(std::move(temp_val));
		}
	}
	size_t count = values.size();
	RX_ASSERT(count == to_read.size());
	for (size_t i = 0; i < count; i++)
	{
		if (values[i].status_code == opcid_Bad_NodeIdUnknown)
		{
			locks::const_auto_read_lock _(get_lock());
			auto it_var = variable_nodes_.find(to_read[i].node_id);
			if (it_var != variable_nodes_.end())
			{
				it_var->second->read_attribute(to_read[i].attr_id, to_read[i].range, to_read[i].data_encoding.name, values[i], config_ts_);
			}
			else
			{
				auto it_folder = folder_nodes_.find(to_read[i].node_id);
				if (it_folder != folder_nodes_.end())
				{
					it_folder->second->read_attribute(to_read[i].attr_id, to_read[i].range, to_read[i].data_encoding.name, values[i], config_ts_);
				}
			}
		}
	}
}

std::pair<opcua_result_t, runtime_transaction_id_t> opcua_simple_address_space::write_attribute (const rx_node_id& node_id, attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep)
{
	std::pair<opcua_result_t, runtime_transaction_id_t> ret = { opcid_Bad_NodeIdUnknown, 0 };
	if (parent_ != nullptr)
	{
		ret = parent_->write_attribute(node_id, id, range, value, ep);
	}
	if (ret.first == opcid_Bad_NodeIdUnknown)
	{
		locks::const_auto_read_lock _(get_lock());
		auto it_var = variable_nodes_.find(node_id);
		if (it_var != variable_nodes_.end())
		{
			ret = it_var->second->write_attribute(id, range, value, ep);
		}
		else
		{
			auto it_folder = folder_nodes_.find(node_id);
			if (it_folder != folder_nodes_.end())
			{
				ret = { opcid_Bad_NotSupported, 0 };
			}
		}
	}
	return ret;
}

void opcua_simple_address_space::browse (const opcua_view_description& view, const std::vector<opcua_browse_description>& to_browse, std::vector<browse_result_internal>& results) const
{
	if (parent_ != nullptr)
	{
		parent_->browse(view, to_browse, results);
	}
	else
	{
		for (const auto& one : to_browse)
		{
			browse_result_internal temp_val;
			temp_val.status_code = opcid_Bad_NodeIdUnknown;
			results.push_back(std::move(temp_val));
		}
	}
	size_t count = results.size();
	RX_ASSERT(count == to_browse.size());
	for (size_t i = 0; i < count; i++)
	{
		if (results[i].status_code == opcid_Bad_NodeIdUnknown)
		{
			locks::const_auto_read_lock _(get_lock());

			opcua_addr_space::opcua_browse_context ctx(this);

			auto it_var = variable_nodes_.find(to_browse[i].node_id);
			if (it_var != variable_nodes_.end())
			{
				it_var->second->browse(to_browse[i], results[i], &ctx);
			}
			else
			{
				auto it_folder = folder_nodes_.find(to_browse[i].node_id);
				if (it_folder != folder_nodes_.end())
				{
					it_folder->second->browse(to_browse[i], results[i], &ctx);
				}
			}
		}
	}
}

void opcua_simple_address_space::translate (const std::vector<browse_path>& browse_paths, std::vector<browse_path_result>& results, opcua_address_space_base* root) const
{
	if (parent_ != nullptr)
	{
		parent_->translate(browse_paths, results, root);
	}
	else
	{
		for (const auto& one : browse_paths)
		{
			browse_path_result temp_val;
			temp_val.status_code = opcid_Bad_NodeIdUnknown;
			results.push_back(std::move(temp_val));
		}
	}
	size_t count = results.size();
	RX_ASSERT(count == browse_paths.size());
	for (size_t i = 0; i < count; i++)
	{
		if (results[i].status_code == opcid_Bad_NodeIdUnknown)
		{
			locks::const_auto_read_lock _(get_lock());

			opcua_addr_space::opcua_browse_context ctx(this);

			auto it_var = variable_nodes_.find(browse_paths[i].starting_node);
			if (it_var != variable_nodes_.end())
			{
				it_var->second->translate(browse_paths[i].path, results[i], &ctx);
			}
			else
			{
				auto it_folder = folder_nodes_.find(browse_paths[i].starting_node);
				if (it_folder != folder_nodes_.end())
				{
					it_folder->second->translate(browse_paths[i].path, results[i], &ctx);
				}
			}
		}
	}
}

rx_result opcua_simple_address_space::fill_relation_types (const rx_node_id& base_id, bool include_subtypes, std::set<rx_node_id>& buffer) const
{
	if (parent_ != nullptr)
		return parent_->fill_relation_types(base_id, include_subtypes, buffer);
	else
		return RX_NOT_IMPLEMENTED;
}

rx_result opcua_simple_address_space::set_node_value (const rx_node_id& id, values::rx_value&& val)
{
	if (parent_ != nullptr)
	{
		auto result = parent_->set_node_value(id, std::move(val));
		if (result)
			return result;
	}
	locks::auto_write_lock _(get_lock());
	auto it_var = variable_nodes_.find(id);
	if (it_var != variable_nodes_.end())
	{
		auto result = it_var->second->set_node_value(std::move(val));
		if (result)
		{// notify changes
			auto it_mon = value_monitors_.find(id);
			if (it_mon != value_monitors_.end() && !it_mon->second.empty())
			{
				data_value val = it_var->second->value;
				for (auto& one_mon : it_mon->second)
				{
					one_mon->monitored_value_changed(val);
				}
			}
		}
		return result;
	}
	return RX_INVALID_ARGUMENT;
}

locks::rw_slim_lock* opcua_simple_address_space::get_lock ()
{
	return parent_->get_lock();
}

const locks::rw_slim_lock* opcua_simple_address_space::get_lock () const
{
	return parent_->get_lock();
}

std::shared_ptr<opcua_node_base> opcua_simple_address_space::connect_node_reference (std::shared_ptr<opcua_node_base> node, const opcua_addr_space::reference_data& ref_data, bool inverse)
{
	if (ref_data.target_id.is_null())
		return nullptr;

	auto target_ptr = parent_->connect_node_reference(node, ref_data, inverse);
	if (target_ptr)
		return target_ptr;

	auto fold_it = folder_nodes_.find(ref_data.target_id);
	if (fold_it != folder_nodes_.end())
	{
		if (fold_it->second->references.connect_node_reference(node, ref_data, inverse))
			return fold_it->second;
		else
			return nullptr;
	}
	auto var_it = variable_nodes_.find(ref_data.target_id);
	if (var_it != variable_nodes_.end())
	{
		if (var_it->second->references.connect_node_reference(node, ref_data, inverse))
			return var_it->second;
		else
			return nullptr;
	}
	return nullptr;
}

opcua_result_t opcua_simple_address_space::register_value_monitor (opcua_subscriptions::opcua_monitored_value* who, data_value& val)
{
	locks::auto_write_lock _(get_lock());
	auto var_it = variable_nodes_.find(who->get_node_id());
	if (var_it != variable_nodes_.end())
	{
		auto mon_it = value_monitors_.find(who->get_node_id());
		if (mon_it != value_monitors_.end())
		{
			mon_it->second.insert(who);
		}
		else
		{
			auto ret_val = value_monitors_.emplace(who->get_node_id(), value_monitors_type::mapped_type());
			ret_val.first->second.insert(who);
		}
		val = var_it->second->value;
		return opcid_OK;
	}
	else
	{
		return opcid_Bad_NodeIdUnknown;
	}
}

opcua_result_t opcua_simple_address_space::unregister_value_monitor (opcua_subscriptions::opcua_monitored_value* who)
{
	locks::auto_write_lock _(get_lock());
	auto mon_it = value_monitors_.find(who->get_node_id());
	if (mon_it != value_monitors_.end())
	{
		mon_it->second.erase(who);
		return opcid_OK;
	}
	else
	{
		return opcid_Bad_InternalError;
	}
}

rx_node_id opcua_simple_address_space::get_folder_node (const string_type& folder_path, const rx_node_id& parent_id)
{
	if (!folder_path.empty())
	{
		string_type path = folder_path;
		rx_node_id node_id = rx_node_id(path.c_str(), default_basic_namespace);
		auto it_folder = folder_nodes_.find(node_id);
		if (it_folder != folder_nodes_.end())
			return it_folder->second->get_node_id();
		// we don't have this folder, try to create one
		string_type folder_name;
		string_type rest_path;
		auto idx = path.rfind('.');
		if (idx != string_type::npos)
		{
			folder_name = path.substr(idx + 1);
			rest_path = path.substr(0, idx);
		}
		else
		{
			folder_name = path;
			rest_path = "";
		}
		rx_node_id new_id = get_folder_node(rest_path, parent_id);
		std::shared_ptr<opcua_basic_folder_node> node = std::make_shared<opcua_basic_folder_node>();
		node->node_id = rx_node_id(path.c_str(), default_basic_namespace);
		node->browse_name = qualified_name{ default_basic_namespace, folder_name };
		node->display_name = folder_name;


		node->references.references.emplace_back(rx_node_id::opcua_standard_id(opcid_HasTypeDefinition), rx_node_id::opcua_standard_id(opcid_FolderType));
		node->references.inverse_references.emplace_back(rx_node_id::opcua_standard_id(opcid_Organizes), new_id);

		for (auto& one : node->references.references)
		{
			auto target_ptr = connect_node_reference(node, one, false);
			if (target_ptr)
				one.resolved_node = target_ptr;
		}
		for (auto& one : node->references.inverse_references)
		{
			auto target_ptr = connect_node_reference(node, one, true);
			if (target_ptr)
				one.resolved_node = target_ptr;
		}
		new_id = node->node_id;// store node id before move!
		auto emplace_result = folder_nodes_.emplace(new_id, std::move(node));

		return new_id;

	}
	return parent_id;
}


// Class protocols::opcua::opcua_basic_server::opcua_basic_mapper 

opcua_basic_mapper::opcua_basic_mapper()
{
	size_t sz = sizeof(opcua_basic_mapper);
	node_ = std::make_shared<opcua_basic_node>();
	node_->mapper_ = smart_this();
}


opcua_basic_mapper::~opcua_basic_mapper()
{
}



rx_result opcua_basic_mapper::initialize_mapper (runtime::runtime_init_context& ctx)
{
	string_type full_path = ctx.get_item_static(".SimplePath", ""s);
	uint32_t numeric_id = ctx.get_item_static(".NumericId", 0);
	if (full_path.empty())
		full_path = ctx.meta.name + RX_OBJECT_DELIMETER + ctx.path.get_parent_path(1);

	string_type path;
	string_type name;
	auto idx = full_path.rfind('.');
	if (idx != string_type::npos)
	{
		name = full_path.substr(idx + 1);
		path = full_path.substr(0, idx);
	}
	else
	{
		name = full_path;
		path = full_path;
	}

	if (numeric_id)
		node_->node_id = rx_node_id(numeric_id, default_basic_namespace);
	else
		node_->node_id = rx_node_id(full_path.c_str(), default_basic_namespace);
	node_->browse_name = qualified_name{ default_basic_namespace, name };
	node_->display_name = name;
	node_->path_ = path;
	node_->type_id = rx_node_id::opcua_standard_id(opcid_BaseDataVariableType);
	node_->data_type = rx_node_id::opcua_standard_id(variant_type::get_opc_type_from_rx_type(get_value_type()));
	node_->references.references.emplace_back(rx_node_id::opcua_standard_id(opcid_HasTypeDefinition), node_->type_id);

	return true;
}

void opcua_basic_mapper::port_connected (port_ptr_t port)
{
	if (port)
	{
		auto result = port->register_node(node_);
	}
}

void opcua_basic_mapper::port_disconnected (port_ptr_t port)
{
	if (port)
	{
		auto result = port->unregister_node(node_);
	}
}

void opcua_basic_mapper::mapped_value_changed (rx_value&& val, runtime::runtime_process_context* ctx)
{
	if (this->my_port_ && node_->node_id)
		this->my_port_->address_space.set_node_value(node_->node_id, std::move(val));
}

void opcua_basic_mapper::mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime::runtime_process_context* ctx)
{
	locks::auto_lock_t _(&transactions_lock_);
	auto it = write_transactions_.find(id);
	if (it != write_transactions_.end())
	{
		it->second->write_response(result ? opcid_OK : opcid_Bad_NoCommunication, id);
		write_transactions_.erase(it);
	}
}

std::pair<opcua_result_t, runtime_transaction_id_t> opcua_basic_mapper::write_value (const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep)
{
	if(!range.empty())
		return { opcid_Bad_IndexRangeInvalid, 0 };
	auto trans_id = rx_internal::sys_runtime::platform_runtime_manager::get_new_transaction_id();
	rx_value val;
	auto result = value.fill_rx_value(val);
	if (result)
	{
		if (val.convert_to(get_value_type()))
		{
			{
				locks::auto_lock_t _(&transactions_lock_);
				write_transactions_.emplace(trans_id, ep);
			}
			write_data data;
			data.internal = false;
			data.test = false;
			data.identity = rx_security_context();
			data.transaction_id = trans_id;
			data.value = val.to_simple();
			mapper_write_pending(std::move(data));

			return { opcid_RxPending, trans_id };
		}
	}
	return { opcid_Bad_TypeMismatch, 0 };
}


// Class protocols::opcua::opcua_basic_server::opcua_basic_node 

opcua_basic_node::opcua_basic_node()
{
	size_t sz1 = sizeof(data_value);
	size_t sz2 = sizeof(variant_type);
	size_t sz3 = sizeof(*this);
	size_t sz4 = sizeof(opcua_basic_mapper);
}


opcua_basic_node::~opcua_basic_node()
{
}



std::pair<opcua_result_t, runtime_transaction_id_t> opcua_basic_node::write_attribute (attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep)
{
	if (id == attribute_id::value)
	{
		return mapper_->write_value(range, value, ep);
	}
	else
	{
		return { opcid_Bad_NotWritable, 0 };
	}
}


// Class protocols::opcua::opcua_basic_server::opcua_basic_folder_node 

opcua_basic_folder_node::opcua_basic_folder_node()
{
	type_id = rx_node_id::opcua_standard_id(opcid_FolderType);
}


opcua_basic_folder_node::~opcua_basic_folder_node()
{
}



} // namespace opcua_basic_server
} // namespace opcua
} // namespace protocols

