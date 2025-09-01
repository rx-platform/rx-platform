

/****************************************************************************
*
*  protocols\mqtt\mqtt_display.cpp
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


// mqtt_display
#include "protocols/mqtt/mqtt_display.h"

#include "system/server/rx_server.h"
#include "mqtt_base.h"


namespace protocols {

namespace mqtt {

namespace mqtt_simple {

// Class protocols::mqtt::mqtt_simple::mqtt_http_display 

mqtt_http_display::mqtt_http_display()
      : changed_(true)
{
	resolver_user_.my_display = this;
}


mqtt_http_display::~mqtt_http_display()
{
}



rx_result mqtt_http_display::initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path)
{

	topic_ = ctx.get_item_static(".Mqtt.Topic", ""s);
	if (topic_.empty() && !disp_path.empty())
	{
		topic_ = disp_path.substr(1);
	}
	broker_url_ = ctx.get_item_static(".Mqtt.BrokerUrl", ""s);
	auto idx = broker_url_.find('~');
	if (idx != string_type::npos)
	{
		broker_url_ = rx_create_string(
			broker_url_.substr(0, idx)
			, "' + window.location.host + '"
			, broker_url_.substr(idx + 1));
	}

	auto result = rx_http_static_display::initialize_display(ctx, disp_path);
	if (!result)
	{
		return result;
	}
	return result;
}

rx_result mqtt_http_display::deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path)
{
	auto result = true;
	if (!result)
	{
		return result;
	}
	return rx_http_static_display::deinitialize_display(ctx, disp_path);
}

rx_result mqtt_http_display::start_display (runtime::runtime_start_context& ctx, const string_type& disp_path)
{
	auto result = rx_http_static_display::start_display(ctx, disp_path);
	if (!result)
	{
		return result;
	}
	string_type port_reference = ctx.get_item_static<string_type>(".Mqtt.Port", "");
	if (!port_reference.empty())
	{
		auto rel_result = ctx.register_relation_subscriber(port_reference, &resolver_user_);
		if (!rel_result)
		{
			MQTT_LOG_WARNING("mqtt_http_display", 900, "Error starting mapper "
				+ ctx.context->meta_info.get_full_path() + "." + ctx.path.get_current_path() + " " + rel_result.errors_line());
		}

		timer_ = ctx.create_timer_function(rx_reference_ptr(), [this]()
			{
				timer_tick();
			});
		timer_->start(ctx.get_item_static<uint32_t>(".Mqtt.HoldTime", 200));
	}

	return result;
}

rx_result mqtt_http_display::stop_display (runtime::runtime_stop_context& ctx, const string_type& disp_path)
{
	auto result = true;

	if (timer_)
		timer_->cancel();

	timer_ = rx_timer_ptr::null_ptr;

	if (!result)
	{
		return result;
	}
	return rx_http_static_display::stop_display(ctx, disp_path);
}

void mqtt_http_display::point_changed ()
{
	changed_ = true;
}

void mqtt_http_display::timer_tick ()
{
	if (changed_)
	{
		string_type json = collect_json_data();
		if (my_port_ && !json.empty() && !topic_.empty())
		{
			changed_ = false;
			mqtt_common::mqtt_publish_data data;
			data.topic = topic_;
			data.content_type = "application/json";
			data.utf_string = true;
			data.data.resize(json.size());
			memcpy(&data.data[0], &json[0], json.size());
			my_port_->publish(std::move(data));
		}
	}
}

void mqtt_http_display::port_connected (mqtt_simple_client_port::smart_ptr port)
{
}

void mqtt_http_display::port_disconnected (mqtt_simple_client_port::smart_ptr port)
{
}

bool mqtt_http_display::internal_port_connected (const platform_item_ptr& item)
{
	if (!my_port_)
	{
		auto result = rx_platform::get_runtime_instance<mqtt_simple_client_port>(item->meta_info().id);
		if (result)
		{
			my_port_ = result.value();
			this->port_connected(my_port_);
			return true;
		}
		else
		{
			RX_ASSERT(false);
		}
	}
	return false;
}

void mqtt_http_display::internal_port_disconnected ()
{
	if (my_port_)
	{
		this->port_disconnected(my_port_);
		my_port_ = mqtt_simple_client_port::smart_ptr::null_ptr;
	}
}

void mqtt_http_display::fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	rx_http_static_display::fill_contents(content, ctx, disp_path);
	content.mapped_content["broker-url"] = broker_url_;
	content.mapped_content["mqtt-topic"] = topic_;
}

const string_array& mqtt_http_display::get_point_replace () const
{
	static string_array ret = { "#" };
	return ret;
}


} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols

