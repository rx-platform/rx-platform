

/****************************************************************************
*
*  protocols\mqtt\mqtt_simple_server.cpp
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


// mqtt_simple_server
#include "protocols/mqtt/mqtt_simple_server.h"

#include "sys_internal/rx_async_functions.h"
#include "mqtt_protocol.h"
#include "lib/rx_ser_json.h"

using namespace protocols::mqtt::mqtt_common;


namespace protocols {

namespace mqtt {

namespace mqtt_simple {

// Class protocols::mqtt::mqtt_simple::mqtt_simple_server_endpoint 

mqtt_simple_server_endpoint::mqtt_simple_server_endpoint (mqtt_simple_server_port_ptr port, rx_timer_ptr timer)
      : port_(port),
        protocol_version_(0)
{
	MQTT_LOG_DEBUG("mqtt_simple_server_endpoint", 200, "MQTT simple Broker endpoint created.");
	timer_ = timer;

	rx_init_stack_entry(&stack_, this);

	stack_.received_function = &mqtt_simple_server_endpoint::received_function;
	stack_.connected_function = &mqtt_simple_server_endpoint::connected_function;
	stack_.disconnected_function = &mqtt_simple_server_endpoint::disconnected_function;
}


mqtt_simple_server_endpoint::~mqtt_simple_server_endpoint()
{
	MQTT_LOG_DEBUG("mqtt_simple_server_endpoint", 200, "MQTT simple Broker endpoint destroyed.");
}



rx_protocol_stack_endpoint* mqtt_simple_server_endpoint::bind_endpoint ()
{
	return &stack_;
}

void mqtt_simple_server_endpoint::close_endpoint ()
{
}

rx_port_impl_ptr mqtt_simple_server_endpoint::get_port ()
{
	return port_;
}

rx_protocol_result_t mqtt_simple_server_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	mqtt_simple_server_endpoint* me = (mqtt_simple_server_endpoint*)reference->user_data;

	me->decoder_.init_decoder([me](recv_protocol_packet packet)
		{
			return me->received_packet(&me->stack_, packet);
		});

	return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_simple_server_endpoint::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t mqtt_simple_server_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	mqtt_simple_server_endpoint* me = (mqtt_simple_server_endpoint*)reference->user_data;

	auto result = me->decoder_.received_function(packet);
	if (result != RX_PROTOCOL_OK)
	{
		return result;
	}

	return result;
}

void mqtt_simple_server_endpoint::timer_fired ()
{
}

rx_protocol_result_t mqtt_simple_server_endpoint::received_packet (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{

	io::rx_const_io_buffer buff(packet.buffer);


	uint8_t ctrl;
	auto ret = buff.read_from_buffer(ctrl);
	if (ret)
	{
		uint32_t multiplier = 1;
		uint32_t value = 0;
		uint8_t enc_byte;

		do
		{
			ret = buff.read_from_buffer(enc_byte);

			value += (enc_byte & 0x80) * multiplier;

			if (multiplier > 0x80 * 0x80 * 0x80)
				return RX_PROTOCOL_PARSING_ERROR;

			multiplier <<= 7;

		} while ((enc_byte & 0x80) != 0);


		mqtt_message_type msg_type = (mqtt_message_type)(ctrl >> 4);

		if (!session_)
		{
			if (msg_type != mqtt_message_type::CONNECT)
				return RX_PROTOCOL_INVALID_SEQUENCE;

			auto trans = std::make_unique<mqtt_connect_transaction>();

			auto result = trans->start_listener_step(ctrl, buff, connection_data_.protocol_version);

			if (result != RX_PROTOCOL_OK)
				return result;

			connection_data_ = trans->get_data();


			auto alloc_result = port_->alloc_io_buffer();
			if (alloc_result)
			{
				auto session_result = port_->connected(connection_data_);

				if (!session_result.succeeded())
				{
					trans->set_result_code(session_result.result_code);
					trans->set_result_string(session_result.result_reason);
				}

				auto out_buff = alloc_result.move_value();

				uint8_t send_ctrl = 0;

				result = trans->send_listener_result(out_buff, send_ctrl, connection_data_.protocol_version);

				if (result == RX_PROTOCOL_OK)
				{

					size_t len = out_buff.size;
					do
					{
						uint8_t digit = len & 0x7f;
						len = len >> 7;
						// if there are more digits to encode, set the top bit of this digit
						if (len > 0)
							digit = digit | 0x80;

						out_buff.write_front(&digit, 1);
					} while (len > 0);

					out_buff.write_front(&send_ctrl, 1);

					auto send_packet = rx_create_send_packet(0, &out_buff, 0, 0);
					result = rx_move_packet_down(&stack_, send_packet);
					if (result == RX_PROTOCOL_OK && session_result.succeeded())
					{
						session_ = session_result.session_ptr;
					}
				}
				port_->release_io_buffer(std::move(out_buff));

				return result;
			}
			else
			{
				return RX_PROTOCOL_OUT_OF_MEMORY;
			}
		}
		else
		{

			switch (msg_type)
			{
			case mqtt_message_type::SUBSCRIBE:
				{
					uint16_t id = 0;
					ret = buff.read_from_buffer(id);
					if (!ret)
						return RX_PROTOCOL_PARSING_ERROR;
					auto trans = std::make_unique<mqtt_subscribe_transaction>();
					trans->set_trans_id(id);

					return do_transaction(ctrl, buff, std::move(trans));

				}
				break;
			case mqtt_message_type::SUBACK:
			case mqtt_message_type::PUBACK:
			case mqtt_message_type::PUBREL:
			case mqtt_message_type::PUBCOMP:
				{
					uint16_t msg_id;
					ret = buff.read_from_buffer(msg_id);
					if (!ret)
						return RX_PROTOCOL_PARSING_ERROR;

					return RX_PROTOCOL_OK;
				}
			case mqtt_message_type::PUBLISH:
				{

					uint16_t msg_id = 0;
					RX_ASSERT(false);
					/*ret = buff.read_from_buffer(msg_id);
					if (!ret)
						return RX_PROTOCOL_PARSING_ERROR;*/

				//	auto trans = std::make_unique<mqtt_publish_transaction>();
				//	trans->set_trans_id(msg_id);
				//	return do_transaction(ctrl, buff, std::move(trans));
				}
				break;
			case mqtt_message_type::PINGREQ:
				{
					auto trans = std::make_unique<mqtt_ping_transaction>();
					return do_transaction(ctrl, buff, std::move(trans));
				}
				break;
			default:
				break;
				;
			}
		}
	}
	return RX_PROTOCOL_PARSING_ERROR;
}

rx_protocol_result_t mqtt_simple_server_endpoint::do_transaction (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_transaction_ptr trans)
{
	auto result = trans->start_listener_step(ctrl, buffer, connection_data_.protocol_version);
	if (result != RX_PROTOCOL_OK)
		return result;

	auto alloc_result = port_->alloc_io_buffer();
	if (alloc_result)
	{
		auto out_buff = alloc_result.move_value();

		uint8_t send_ctrl = 0;

		result = trans->send_listener_result(out_buff, send_ctrl, connection_data_.protocol_version);

		if (result == RX_PROTOCOL_OK)
		{

			size_t len = out_buff.size;
			do
			{
				uint8_t digit = len & 0x7f;
				len = len >> 7;
				// if there are more digits to encode, set the top bit of this digit
				if (len > 0)
					digit = digit | 0x80;

				out_buff.write_front(&digit, 1);
			} while (len > 0);

			out_buff.write_front(&send_ctrl, 1);

			auto send_packet = rx_create_send_packet(0, &out_buff, 0, 0);
			result = rx_move_packet_down(&stack_, send_packet);
			
		}
		port_->release_io_buffer(std::move(out_buff));

		return result;
	}
	else
	{
		return RX_PROTOCOL_OUT_OF_MEMORY;
	}

	return RX_PROTOCOL_OK;
}


// Class protocols::mqtt::mqtt_simple::mqtt_simple_server_port 

std::map<rx_node_id, mqtt_simple_server_port::smart_ptr> mqtt_simple_server_port::runtime_instances;

mqtt_simple_server_port::mqtt_simple_server_port()
      : message_id_(1)
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<mqtt_simple_server_endpoint>(smart_this(), timer_);
		auto entry = rt->bind_endpoint();
		return construct_func_type::result_type{ entry, rt };
	};
}


mqtt_simple_server_port::~mqtt_simple_server_port()
{
	if (timer_)//just in case
	{
		RX_ASSERT(false);
		timer_->cancel();
		timer_ = rx_timer_ptr::null_ptr;
	}
}



void mqtt_simple_server_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}

rx_result mqtt_simple_server_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	sessions_.max_session_expiry = ctx.get_item_static("MaxSessExpiry", (uint16_t)120);
	sessions_.min_keep_alive = ctx.get_item_static("MinKeepAlive", (uint16_t)60);
	return true;
}

rx_result mqtt_simple_server_port::start_runtime (runtime::runtime_start_context& ctx)
{
	timer_ = create_timer_function([this]()
		{
			this->timer_fired();
		});
	return true;
}

rx_result mqtt_simple_server_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	if (timer_)
	{
		timer_->cancel();
		timer_ = rx_timer_ptr::null_ptr;
	}
	return true;
}

void mqtt_simple_server_port::register_source (const string_type& topic, mqtt_broker_source_ptr what)
{
	std::scoped_lock _(lock_);
	auto it = sources_.find(topic);
	if (it == sources_.end())
	{
		// create subscribe request!!!
		// !!!!!

		it = sources_.emplace(topic, mqtt_source_def()).first;

		it->second.subscribed = false;
		it->second.subs_msg_id = 0;

		/*auto ep = this->active_endpoint();
		if (ep && ep->is_connected_)
		{
			uint16_t msg_id = get_next_message_id();
			mqtt_subscribe_data data;
			data.topic = topic;
			mqtt_transaction_ptr trans = std::make_unique<mqtt_subscribe_transaction>("", std::move(data), msg_id);
			auto result = it->second.subs_msg_id = ep->start_transaction(trans.get());
			if (result == RX_PROTOCOL_OK && !trans->is_done())
			{
				transactions_.emplace(msg_id, std::move(trans));
			}
		}*/

	}
	it->second.sources.emplace(what);
}

void mqtt_simple_server_port::unregister_source (const string_type& topic, mqtt_broker_source_ptr what)
{
	std::scoped_lock _(lock_);
	auto it = sources_.find(topic);
	if (it != sources_.end())
	{
		it->second.sources.erase(what);
	}
}

mqtt_topics::session_request_result mqtt_simple_server_port::connected (mqtt_common::mqtt_connection_data& data)
{
	return sessions_.session_request(data);
}

void mqtt_simple_server_port::disconnected ()
{
	std::scoped_lock _(lock_);
	if (!sources_.empty())
	{

		byte_string data;
		rx_time now(rx_time::now());
		for (auto& one : sources_)
		{
			one.second.subs_msg_id = 0;
			for (auto src : one.second.sources)
			{
				src->topic_changed(data, now);
			}
		}
	}
}

void mqtt_simple_server_port::publish (mqtt_common::mqtt_publish_data data)
{
	topics_.publish_received(std::move(data), &sessions_);
}

void mqtt_simple_server_port::publish_received (mqtt_common::mqtt_publish_data data)
{
	topics_.publish_received(std::move(data), &sessions_);
}

void mqtt_simple_server_port::timer_fired ()
{
}

uint16_t mqtt_simple_server_port::get_next_message_id ()
{
	uint16_t ret = message_id_++;
	if (ret == 0)
		return get_next_message_id();
	return ret;
}


// Class protocols::mqtt::mqtt_simple::mqtt_simple_broker_source 

mqtt_simple_broker_source::mqtt_simple_broker_source()
{
}


mqtt_simple_broker_source::~mqtt_simple_broker_source()
{
}



rx_result mqtt_simple_broker_source::initialize_source (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_broker_source_base::initialize_source(ctx);
	if (!result)
		return result;

	topic_ = ctx.get_item_static(".Topic", ""s);

	return true;
}

void mqtt_simple_broker_source::port_connected (port_ptr_t port)
{
	port->register_source(topic_, smart_this());
}

void mqtt_simple_broker_source::port_disconnected (port_ptr_t port)
{
	port->unregister_source(topic_, smart_this());

	rx_value val;
	val.set_quality(RX_BAD_QUALITY_CONFIG_ERROR);
	val.set_time(rx_time::now());
	source_value_changed(std::move(val));
}


// Class protocols::mqtt::mqtt_simple::mqtt_json_broker_source 

mqtt_json_broker_source::mqtt_json_broker_source()
{
}


mqtt_json_broker_source::~mqtt_json_broker_source()
{
}



void mqtt_json_broker_source::topic_changed (const byte_string& val, rx_time now)
{
	if (!val.empty())
	{// we have data
		string_type str_val;
		str_val.resize(val.size());
		memcpy(&str_val[0], &val[0], val.size());

		serialization::json_reader reader;
		if (reader.parse_data(str_val))
		{
			data::runtime_values_data vals;
			if (reader.read_init_values(nullptr, vals))
			{
				rx_simple_value val = vals.get_value(value_path_);

				rx_value new_val;
				val.get_value(new_val, now, rx_mode_type());
				new_val.increment_signal_level();
				this->source_value_changed(std::move(new_val));
			}
		}
		else
		{

		}
	}
	else
	{// this is not connected
		rx_value val;
		val.set_quality(RX_BAD_QUALITY_NOT_CONNECTED);
		val.set_time(now);
		source_value_changed(std::move(val));
	}
}

rx_result mqtt_json_broker_source::initialize_source (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_broker_source::initialize_source(ctx);
	if (!result)
		return result;

	value_path_ = ctx.get_item_static(".ValuePath", ""s);

	return true;
}


// Class protocols::mqtt::mqtt_simple::mqtt_simple_broker_mapper 

mqtt_simple_broker_mapper::mqtt_simple_broker_mapper()
{
}


mqtt_simple_broker_mapper::~mqtt_simple_broker_mapper()
{
}



rx_result mqtt_simple_broker_mapper::initialize_mapper (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_broker_mapper_base::initialize_mapper(ctx);
	if (!result)
		return result;

	topic_ = ctx.get_item_static(".Topic", ""s);

	return true;
}

void mqtt_simple_broker_mapper::port_connected (port_ptr_t port)
{
}

void mqtt_simple_broker_mapper::port_disconnected (port_ptr_t port)
{
}

void mqtt_simple_broker_mapper::mapped_value_changed (rx_value&& val, runtime::runtime_process_context* ctx)
{
	mqtt_publish_data data;
	update_publish_from_value(std::move(val), data);
	if (!data.data.empty())
	{
		data.topic = topic_;
		if (my_port_)
			my_port_->publish(std::move(data));
	}
}

void mqtt_simple_broker_mapper::mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime::runtime_process_context* ctx)
{
}


// Class protocols::mqtt::mqtt_simple::mqtt_json_broker_mapper 

mqtt_json_broker_mapper::mqtt_json_broker_mapper()
{
}


mqtt_json_broker_mapper::~mqtt_json_broker_mapper()
{
}



rx_result mqtt_json_broker_mapper::initialize_mapper (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_broker_mapper::initialize_mapper(ctx);
	if (!result)
		return result;

	return true;
}

void mqtt_json_broker_mapper::update_publish_from_value (rx_value&& val, mqtt_common::mqtt_publish_data& data)
{
	data::runtime_values_data rt;
	rt.add_value("val", val.to_simple());

	data.content_type = "application/json";
	data.utf_string = true;

	serialization::json_writer writer;

	writer.write_header(STREAMING_TYPE_MESSAGE, 0);
	auto result = writer.write_init_values(nullptr, rt);
	if (result)
	{
		result = writer.write_footer();
		if (result)
		{
			string_type expr = writer.get_string();
			if (!expr.empty())
			{
				data.data.resize(expr.size());
				memcpy(&data.data[0], &expr[0], expr.size());
			}

		}
	}
}


} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols

