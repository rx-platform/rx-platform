

/****************************************************************************
*
*  protocols\mqtt\mqtt_simple.cpp
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


// mqtt_simple
#include "protocols/mqtt/mqtt_simple.h"
// mqtt_base
#include "protocols/mqtt/mqtt_base.h"

#include "sys_internal/rx_async_functions.h"
#include "mqtt_protocol.h"
#include "lib/rx_ser_json.h"

using namespace protocols::mqtt::mqtt_common;


namespace protocols {

namespace mqtt {

namespace mqtt_simple {


rx_protocol_result_t mqtt_check_header(uint8_t* header, size_t* size)
{
	uint32_t len = (*((uint32_t*)&header[4]));
	*size = len;
	return RX_PROTOCOL_OK;
}


// Class protocols::mqtt::mqtt_simple::mqtt_json_mapper 

mqtt_json_mapper::mqtt_json_mapper()
{
}


mqtt_json_mapper::~mqtt_json_mapper()
{
}



rx_result mqtt_json_mapper::initialize_mapper (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_mapper::initialize_mapper(ctx);
	if (!result)
		return result;

	return true;
}

void mqtt_json_mapper::update_publish_from_value (rx_value&& val, mqtt_common::mqtt_publish_data& data)
{
	data::runtime_values_data rt;
	rt.add_value("val", val.to_simple());

	rx_simple_value vlsv;
	string_type very_large_string(32000, 'F');
	vlsv.assign_static(very_large_string);
	rt.add_value("vls", std::move(vlsv));

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


// Class protocols::mqtt::mqtt_simple::mqtt_json_source 

mqtt_json_source::mqtt_json_source()
{
}


mqtt_json_source::~mqtt_json_source()
{
}



void mqtt_json_source::topic_changed (const byte_string& val, rx_time now)
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

rx_result mqtt_json_source::initialize_source (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_source::initialize_source(ctx);
	if (!result)
		return result;

	value_path_ = ctx.get_item_static(".ValuePath", ""s);

	return true;
}


// Class protocols::mqtt::mqtt_simple::mqtt_simple_client_endpoint 

mqtt_simple_client_endpoint::mqtt_simple_client_endpoint (mqtt_simple_client_port_ptr port, rx_timer_ptr timer)
      : port_(port)
	, is_connected_(false)
{
	MQTT_LOG_DEBUG("mqtt_simple_client_endpoint", 200, "MQTT simple client/publisher endpoint created.");
	timer_ = timer;

	rx_init_stack_entry(&stack_, this);

	stack_.received_function = &mqtt_simple_client_endpoint::received_function;
	stack_.connected_function = &mqtt_simple_client_endpoint::connected_function;
	stack_.disconnected_function = &mqtt_simple_client_endpoint::disconnected_function;
}


mqtt_simple_client_endpoint::~mqtt_simple_client_endpoint()
{
	MQTT_LOG_DEBUG("mqtt_simple_client_endpoint", 200, "MQTT simple client/publisher endpoint destroyed.");
}



rx_protocol_stack_endpoint* mqtt_simple_client_endpoint::bind_endpoint ()
{
	return &stack_;
}

void mqtt_simple_client_endpoint::close_endpoint ()
{
}

rx_port_impl_ptr mqtt_simple_client_endpoint::get_port ()
{
	return port_;
}

rx_protocol_result_t mqtt_simple_client_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	mqtt_simple_client_endpoint* me = (mqtt_simple_client_endpoint*)reference->user_data;

	me->decoder_.init_decoder([me](recv_protocol_packet packet)
		{
			return me->received_packet(&me->stack_, packet);
		});

	if (me->connect_transaction_)
		return RX_PROTOCOL_INVALID_SEQUENCE;
	mqtt_connection_data data;
	data.keep_alive = me->port_->keep_alive;
	data.client_identifier = me->port_->client_id;

	me->connect_transaction_ = std::make_unique<mqtt_common::mqtt_connect_transaction>(std::move(data), mqtt_publish_data());

	me->ping_transaction_ = std::make_unique<mqtt_common::mqtt_ping_transaction>();

	me->start_transaction(me->connect_transaction_.get());



	return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_simple_client_endpoint::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
	mqtt_simple_client_endpoint* me = (mqtt_simple_client_endpoint*)reference->user_data;
	if (me->port_)
		me->port_->disconnected();
	me->connect_transaction_.reset();
	me->ping_transaction_.reset();
	me->is_connected_ = false;
	return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_simple_client_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	mqtt_simple_client_endpoint* me = (mqtt_simple_client_endpoint*)reference->user_data;

	rx_protocol_result_t result = RX_PROTOCOL_OK;

	while (!rx_buffer_eof(packet.buffer))
	{
		result = me->decoder_.received_function(packet);
		if (result != RX_PROTOCOL_OK)
		{
			return result;
		}
	}

	return result;
}

uint16_t mqtt_simple_client_endpoint::start_transaction (mqtt_common::mqtt_transaction* trans)
{
	if (is_connected_ || connect_transaction_ && trans->get_trans_id() == 0)
	{
		auto alloc_result = port_->alloc_io_buffer();
		if (alloc_result)
		{
			auto buff = alloc_result.move_value();
			uint8_t ctrl = 0;
			rx_protocol_result_t result = trans->start_initiator_step(buff, ctrl, port_->get_connection_data().protocol_version);
			if (result == RX_PROTOCOL_OK)
			{
				std::array<uint8_t, 4> temp;
				int num_digits = 0;
				size_t len = buff.size;
				do
				{
					uint8_t digit = len & 0x7f;
					len = len >> 7;
					// if there are more digits to encode, set the top bit of this digit
					if (len > 0)
						digit = digit | 0x80;
					temp[num_digits++] = digit;

				} while (len > 0);
				
				buff.write_front(&temp[0], num_digits);
				buff.write_front(&ctrl, 1);

				auto send_packet = rx_create_send_packet(0, &buff, 0, 0);
				auto result = rx_move_packet_down(&stack_, send_packet);

				if (port_)
					port_->status.sent_packet();

				if (timer_)
				{
					uint32_t period = (uint32_t)port_->keep_alive * 1000u;
					timer_->start(period);
				}
			}

			port_->release_io_buffer(std::move(buff));
		}
	}
	return 0;
}

void mqtt_simple_client_endpoint::timer_fired ()
{
	if (timer_)
	{
		timer_->suspend();

		if (is_connected_)
		{
			if (ping_transaction_)
			{
				start_transaction(ping_transaction_.get());
			}
			else
			{
				RX_ASSERT(false);
			}
		}
	}
}

rx_protocol_result_t mqtt_simple_client_endpoint::received_packet (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{

	if (port_)
		port_->status.received_packet();

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

		if (connect_transaction_)
		{
			if (msg_type != mqtt_message_type::CONNACK)
				return RX_PROTOCOL_INVALID_SEQUENCE;

			auto result = connect_transaction_->process_initiator_result(ctrl, buff, port_->get_connection_data().protocol_version);

			if (result != RX_PROTOCOL_OK)
				return result;

			mqtt_connection_data conn_data = connect_transaction_->get_data();

			is_connected_ = true;
			connect_transaction_.reset();

			port_->connected(std::move(conn_data));

			return RX_PROTOCOL_OK;
		}
		else
		{

			switch (msg_type)
			{
			case mqtt_message_type::UNSUBACK:
			case mqtt_message_type::SUBACK:
			case mqtt_message_type::PUBACK:
			case mqtt_message_type::PUBREC:
			case mqtt_message_type::PUBCOMP:
				{
					uint16_t msg_id;
					ret = buff.read_from_buffer(msg_id);
					if (!ret)
						return RX_PROTOCOL_PARSING_ERROR;

					return port_->initiator_transaction_result_received(htons(msg_id), ctrl, buff);
				}
			case mqtt_message_type::PUBREL:
				{
					uint16_t msg_id;
					ret = buff.read_from_buffer(msg_id);
					if (!ret)
						return RX_PROTOCOL_PARSING_ERROR;

					return port_->listener_transaction_result_received(htons(msg_id), ctrl, buff);
				}
			case mqtt_message_type::PUBLISH:
				{
					mqtt_publish_data data;
					ret = data.deserialize(ctrl, buff, port_->get_connection_data().protocol_version);
					if (!ret)
						return RX_PROTOCOL_PARSING_ERROR;

					port_->publish_received(std::move(data));

					return RX_PROTOCOL_OK;
				}
				break;
			case mqtt_message_type::PINGRESP:
				{
					if (ping_transaction_)
					{
						auto result = ping_transaction_->process_initiator_result(ctrl, buff, port_->get_connection_data().protocol_version);

						if (result != RX_PROTOCOL_OK)
							return result;


						return RX_PROTOCOL_OK;

					}
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

uint16_t mqtt_simple_client_endpoint::send_transaction_result (mqtt_common::mqtt_transaction* trans)
{
	if (is_connected_)
	{
		auto alloc_result = port_->alloc_io_buffer();
		if (alloc_result)
		{
			auto buff = alloc_result.move_value();
			uint8_t ctrl = 0;
			rx_protocol_result_t result = trans->send_listener_result(buff, ctrl, port_->get_connection_data().protocol_version);
			if (result == RX_PROTOCOL_OK)
			{
				std::array<uint8_t, 4> temp;
				int num_digits = 0;
				size_t len = buff.size;
				do
				{
					uint8_t digit = len & 0x7f;
					len = len >> 7;
					// if there are more digits to encode, set the top bit of this digit
					if (len > 0)
						digit = digit | 0x80;
					temp[num_digits++] = digit;

				} while (len > 0);

				buff.write_front(&temp[0], num_digits);
				buff.write_front(&ctrl, 1);

				auto send_packet = rx_create_send_packet(0, &buff, 0, 0);
				auto result = rx_move_packet_down(&stack_, send_packet);

				if (port_)
					port_->status.sent_packet();

				if (timer_)
				{
					uint32_t period = (uint32_t)port_->keep_alive * 1000u;
					timer_->start(period);
				}
			}

			port_->release_io_buffer(std::move(buff));
		}
	}
	return 0;
}


// Class protocols::mqtt::mqtt_simple::mqtt_simple_client_port 

std::map<rx_node_id, mqtt_simple_client_port::smart_ptr> mqtt_simple_client_port::runtime_instances;

mqtt_simple_client_port::mqtt_simple_client_port()
      : message_id_(1),
        keep_alive(0),
        connected_(false),
        published_(0),
        received_(0)
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<mqtt_simple_client_endpoint>(smart_this(), timer_);
		auto entry = rt->bind_endpoint();
		return construct_func_type::result_type{ entry, rt };
	};
}


mqtt_simple_client_port::~mqtt_simple_client_port()
{
	if (timer_)//just in case
	{
		RX_ASSERT(false);
		timer_->cancel();
		timer_ = rx_timer_ptr::null_ptr;
	}
}



void mqtt_simple_client_port::stack_assembled ()
{
	auto result = connect(nullptr, nullptr);
}

rx_result mqtt_simple_client_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	rx_result result = status.initialize(ctx);

	connected_.bind("Status.Connected", ctx);
	published_.bind("Status.Published", ctx);
	received_.bind("Status.Received", ctx);

	client_id = ctx.get_item_static("Options.ClientID", "rx-platform"s);
	keep_alive = ctx.get_item_static<uint16_t>("Options.KeepAlive", 0);

	return true;
}

rx_result mqtt_simple_client_port::start_runtime (runtime::runtime_start_context& ctx)
{
	timer_ = create_timer_function([this]()
		{
			this->timer_fired();
		});
	return true;
}

rx_result mqtt_simple_client_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	if (timer_)
	{
		timer_->cancel();
		timer_ = rx_timer_ptr::null_ptr;
	}
	return true;
}

void mqtt_simple_client_port::register_source (const string_type& topic, mqtt_qos_level qos, mqtt_source_ptr what)
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
		it->second.qos = qos;

		auto ep = this->active_endpoint();
		if (ep && ep->is_connected_)
		{
			uint16_t msg_id = get_next_message_id();
			mqtt_subscribe_data data;
			data.topic = topic;
			data.qos = qos;
			data.retain_as_published = true;
			data.retain_handling = mqtt_retain_handling::at_new_subscribe;
			mqtt_transaction_ptr trans = std::make_unique<mqtt_subscribe_transaction>("", std::move(data), msg_id);
			auto result = it->second.subs_msg_id = ep->start_transaction(trans.get());
			if (result == RX_PROTOCOL_OK && !trans->is_done())
			{
				initiated_ransactions_.emplace(msg_id, std::move(trans));
			}
		}

	}
	it->second.sources.emplace(what);
}

void mqtt_simple_client_port::unregister_source (const string_type& topic, mqtt_source_ptr what)
{
	std::scoped_lock _(lock_);
	auto it = sources_.find(topic);
	if (it != sources_.end())
	{
		it->second.sources.erase(what);
		if (it->second.sources.empty())
		{
			sources_.erase(it);
			auto ep = this->active_endpoint();
			if (ep && ep->is_connected_)
			{
				uint16_t msg_id = get_next_message_id();
				mqtt_transaction_ptr trans = std::make_unique<mqtt_unsubscribe_transaction>(topic, msg_id);
				auto result = ep->start_transaction(trans.get());
				if (result == RX_PROTOCOL_OK && !trans->is_done())
				{
					initiated_ransactions_.emplace(msg_id, std::move(trans));
				}
			}
		}
	}
}

void mqtt_simple_client_port::connected (mqtt_common::mqtt_connection_data data)
{
	auto ep = this->active_endpoint();
	if (ep && ep->is_connected_)
	{
		connected_ = true;
		//connection_data = std::move(data);

		std::scoped_lock _(lock_);
		if (!sources_.empty())
		{
			std::vector<mqtt_subscribe_data> topics;
			topics.reserve(sources_.size());
			uint16_t msg_id = get_next_message_id();
			for (auto& one : sources_)
			{
				mqtt_subscribe_data data;
				data.topic = one.first;
				data.qos = one.second.qos;
				topics.push_back(std::move(data));
			}

			mqtt_transaction_ptr trans = std::make_unique<mqtt_subscribe_transaction>("", std::move(topics), msg_id);
			auto result = ep->start_transaction(trans.get());
			if (result == RX_PROTOCOL_OK && !trans->is_done())
			{
				initiated_ransactions_.emplace(msg_id, std::move(trans));
			}
		}
		if (!publish_cache_.empty())
		{
			for (auto& one : publish_cache_)
			{
				uint16_t msg_id = get_next_message_id();

				mqtt_transaction_ptr trans = std::make_unique<mqtt_publish_transaction>(one.second, msg_id);
				auto result = ep->start_transaction(trans.get());
				if (result == RX_PROTOCOL_OK && !trans->is_done())
				{
					initiated_ransactions_.emplace(msg_id, std::move(trans));
				}
			}
		}
	}
}

void mqtt_simple_client_port::disconnected ()
{
	connected_ = false;

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

uint16_t mqtt_simple_client_port::get_next_message_id ()
{
	uint16_t ret = message_id_++;
	if (ret == 0)
		return get_next_message_id();
	return ret;
}

rx_protocol_result_t mqtt_simple_client_port::initiator_transaction_result_received (uint16_t msg_id, uint8_t ctrl, io::rx_const_io_buffer& buff)
{
	std::scoped_lock _(lock_);

	auto it = initiated_ransactions_.find(msg_id);
	if (it != initiated_ransactions_.end())
	{
		auto result = it->second->process_initiator_result(ctrl, buff, connection_data_.protocol_version);
		if (result != RX_PROTOCOL_OK || it->second->is_done())
		{
			initiated_ransactions_.erase(it);
		}
		else
		{
			auto ep = this->active_endpoint();
			if (ep && ep->is_connected_)
			{
				auto msg_id = ep->start_transaction(it->second.get());
				if (it->second->is_done())
				{
					initiated_ransactions_.erase(it);
				}
			}

		}
		
		return result;
	}
	else
	{
		return RX_PROTOCOL_INVALID_SEQUENCE;
	}
}

rx_protocol_result_t mqtt_simple_client_port::listener_transaction_result_received (uint16_t msg_id, uint8_t ctrl, io::rx_const_io_buffer& buff)
{
	std::scoped_lock _(lock_);

	auto it = listened_transactions_.find(msg_id);
	if (it != listened_transactions_.end())
	{
		auto result = it->second->start_listener_step(ctrl, buff, connection_data_.protocol_version);
		if (it->second->is_done())
		{
			listened_transactions_.erase(it);
		}
		else
		{
			auto ep = this->active_endpoint();
			if (ep && ep->is_connected_)
			{
				auto msg_id = ep->send_transaction_result(it->second.get());
				if (it->second->is_done())
				{
					listened_transactions_.erase(it);
				}
			}

		}

		return result;
	}
	else
	{
		return RX_PROTOCOL_INVALID_SEQUENCE;
	}
}

void mqtt_simple_client_port::publish (mqtt_common::mqtt_publish_data data)
{

	std::scoped_lock _(lock_);
	publish_cache_[data.topic] = data;
	auto ep = this->active_endpoint();
	if (ep && ep->is_connected_)
	{
		published_ += 1;

		uint16_t msg_id = get_next_message_id();

		mqtt_transaction_ptr trans = std::make_unique<mqtt_publish_transaction>(std::move(data), msg_id);
		auto result = ep->start_transaction(trans.get());
		if (result == RX_PROTOCOL_OK && !trans->is_done())
		{
			initiated_ransactions_.emplace(msg_id, std::move(trans));
		}
	}
}

void mqtt_simple_client_port::publish_received (mqtt_common::mqtt_publish_data data)
{

	if (data.qos < mqtt_qos_level::Level2)
	{

		received_ += 1;

		if (data.qos == mqtt_qos_level::Level1)
		{
			mqtt_publish_transaction trans(data, data.packet_id);

			auto ep = this->active_endpoint();
			if (ep && ep->is_connected_)
			{
				ep->send_transaction_result(&trans);
			}
		}

		std::scoped_lock _(lock_);

		auto it = sources_.find(data.topic);
		if (it != sources_.end())
		{
			rx_time now(rx_time::now());
			for (auto one : it->second.sources)
			{
				one->topic_changed(data.data, now);
			}
		}
	}
	else
	{
		auto trans = std::make_unique<mqtt_publish_transaction>(data, data.packet_id);

		auto ep = this->active_endpoint();
		if (ep && ep->is_connected_)
		{
			auto result = ep->send_transaction_result(trans.get());
			if (!trans->is_done())
			{
				listened_transactions_.emplace(data.packet_id, std::move(trans));
			}
		}

		received_ += 1;

		std::scoped_lock _(lock_);

		auto it = sources_.find(data.topic);
		if (it != sources_.end())
		{
			rx_time now(rx_time::now());
			for (auto one : it->second.sources)
			{
				one->topic_changed(data.data, now);
			}
		}
	}

}

void mqtt_simple_client_port::timer_fired ()
{
	auto ep = this->active_endpoint();
	if (ep)
	{
		//std::scoped_lock _(lock_);
		ep->timer_fired();
	}
}


const mqtt_common::mqtt_connection_data& mqtt_simple_client_port::get_connection_data () const
{
  return connection_data_;
}


// Class protocols::mqtt::mqtt_simple::mqtt_simple_mapper 

mqtt_simple_mapper::mqtt_simple_mapper()
      : qos_(mqtt_qos_level::Level0),
        retain_(true)
{
}


mqtt_simple_mapper::~mqtt_simple_mapper()
{
}



rx_result mqtt_simple_mapper::initialize_mapper (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_mapper_base::initialize_mapper(ctx);
	if (!result)
		return result;

	topic_ = ctx.get_item_static(".Topic", ""s);
	uint8_t temp_byte = ctx.get_item_static<uint8_t>(".QoS", 0);
	if (temp_byte > 2)
		return "Invalid QoS parameter for MQTT mapper";
	qos_ = (mqtt_qos_level)temp_byte;
	retain_ = ctx.get_item_static(".Retain", false);

	return true;
}

void mqtt_simple_mapper::port_connected (port_ptr_t port)
{
}

void mqtt_simple_mapper::port_disconnected (port_ptr_t port)
{
}

void mqtt_simple_mapper::mapped_value_changed (rx_value&& val, runtime::runtime_process_context* ctx)
{
	mqtt_publish_data data;
	update_publish_from_value(std::move(val), data);
	if (!data.data.empty())
	{
		data.retain = retain_;
		data.qos = qos_;
		data.topic = topic_;
		if(my_port_)
			my_port_->publish(std::move(data));
	}
}

void mqtt_simple_mapper::mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime::runtime_process_context* ctx)
{
}


// Class protocols::mqtt::mqtt_simple::mqtt_simple_source 

mqtt_simple_source::mqtt_simple_source()
      : qos_(mqtt_qos_level::Level0),
        retain_(true)
{
}


mqtt_simple_source::~mqtt_simple_source()
{
}



rx_result mqtt_simple_source::initialize_source (runtime::runtime_init_context& ctx)
{
	auto result = mqtt_simple_source_base::initialize_source(ctx);
	if (!result)
		return result;

	topic_ = ctx.get_item_static(".Topic", ""s);
	uint8_t temp_byte = ctx.get_item_static<uint8_t>(".QoS", 0);
	if (temp_byte > 2)
		return "Invalid QoS parameter for MQTT source";
	qos_ = (mqtt_qos_level)temp_byte;

	return true;
}

void mqtt_simple_source::port_connected (port_ptr_t port)
{
	port->register_source(topic_, qos_, smart_this());
}

void mqtt_simple_source::port_disconnected (port_ptr_t port)
{
	port->unregister_source(topic_, smart_this());

	rx_value val;
	val.set_quality(RX_BAD_QUALITY_CONFIG_ERROR);
	val.set_time(rx_time::now());
	source_value_changed(std::move(val));
}


} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols

