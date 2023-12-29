

/****************************************************************************
*
*  first_plugin\first_plugin.cpp
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

#include "first_plugin_version.h"
#include "version/rx_code_macros.h"
#include "platform_api/rx_api.h"
#include "lib/rx_values.h"
#include "first_ports.h"
#include "rx_configuration.h"
#include "ether_test.h"
#include "storage_test.h"
#include "first_logic.h"

// first_plugin
#include "first_plugin/first_plugin.h"

RX_DECLARE_PLUGIN(first_plugin);

static const uint8_t c_def_mojFilter[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00
};

static const uint8_t c_def_mojMapper[] = {
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x56, 0x61, 0x72, 0x4e, 0x61,
	0x6d, 0x65, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojSource[] = {
	0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x09, 0x00, 0x00, 0x00, 0x41, 0x6d, 0x70, 0x6c, 0x69,
	0x74, 0x75, 0x64, 0x65, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x50, 0x65, 0x72, 0x69, 0x6f, 0x64, 0x00, 0x07,
	0xd0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x53, 0x61,
	0x6d, 0x70, 0x6c, 0x65, 0x00, 0x07, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const uint8_t c_def_mojObject[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojDomen[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojaApp[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojTransportPort[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojClientPort[] = {
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x07, 0x00, 0x00, 0x00, 0x43, 0x6f, 0x6d, 0x6d, 0x61,
	0x6e, 0x64, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00,
	0x00, 0x00, 0x52, 0x65, 0x73, 0x70, 0xff, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojServerPort[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t c_def_mojaRel[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x2e, 0x2e,
	0x2f, 0x2e, 0x2e, 0x2f, 0x2e, 0x2e, 0x2f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2f, 0x62, 0x61, 0x73,
	0x65, 0x2f, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x42, 0x61, 0x73,
	0x65, 0x00, 0x00, 0x00, 0x00
};


static const uint8_t c_def_mojaStruct[] = {
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x54, 0x65, 0x68, 0x48, 0x6f,
	0x6c, 0x64, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00,
	0x00, 0x00, 0x48, 0x6f, 0x6c, 0x64, 0x00, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xff, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const uint8_t c_def_ethSubs1[] = {
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x42, 0x69, 0x6e, 0x64, 0x00,
	0x1e, 0x00, 0x00, 0x00, 0x2f, 0x73, 0x79, 0x73, 0x2f, 0x74, 0x79, 0x70, 0x65, 0x73, 0x2f, 0x73,
	0x75, 0x70, 0x70, 0x6f, 0x72, 0x74, 0x2f, 0x4d, 0x41, 0x43, 0x42, 0x69, 0x6e, 0x64, 0x44, 0x61,
	0x74, 0x61, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojSingleton[] = {
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x4d, 0x73, 0x67, 0x00, 0xff,
	0xff, 0xff, 0xff, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f,
	0x72, 0x6c, 0x64, 0x21, 0x21, 0x21, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_rtSingl[] = {
	0x00, 0x0a, 0x00, 0x00, 0x00, 0x48, 0x6f, 0x73, 0x74, 0x44, 0x6f, 0x6d, 0x61, 0x69, 0x6e, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t c_def_mojMethod[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00
};




// Class first_filter 

first_filter::first_filter()
      : timer_(0)
	, rx_filter(true, true)
{
}


first_filter::~first_filter()
{
}



rx_result first_filter::initialize_filter (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_filter", 100, _rx_func_);
	return true;
}

rx_result first_filter::start_filter (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_filter", 100, _rx_func_);
	timer_ = ctx.create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_filter::stop_filter ()
{
	RX_PLUGIN_LOG_DEBUG("first_filter", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_filter::deinitialize_filter ()
{
	RX_PLUGIN_LOG_DEBUG("first_filter", 100, _rx_func_);
	return true;
}

rx_result first_filter::filter_input (rx_value& val)
{
	auto in = val.get_float();
	if (in > 0)
	{
		in = log(in);
		val.set_float(in, val.get_type());
	}
	else
	{
		val.set_quality(RX_BAD_QUALITY_DIVISION_BY_ZERO);
	}
	return true;
}

rx_result first_filter::filter_output (rx_simple_value& val)
{
	auto in = val.get_float();

	val.set_float(in*2, val.get_type());
	return true;
}

void first_filter::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_filter", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}


// Class first_mapper 

first_mapper::first_mapper()
      : timer_(0)
	, rx_mapper(true, true)
{
}


first_mapper::~first_mapper()
{
}



rx_result first_mapper::initialize_mapper (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_mapper", 100, _rx_func_);
	return true;
}

rx_result first_mapper::start_mapper (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_mapper", 100, _rx_func_);
	timer_ = ctx.create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_mapper::stop_mapper ()
{
	RX_PLUGIN_LOG_DEBUG("first_mapper", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_mapper::deinitialize_mapper ()
{
	RX_PLUGIN_LOG_DEBUG("first_mapper", 100, _rx_func_);
	return true;
}

void first_mapper::mapped_value_changed (rx_value&& val, rx_process_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_mapper", 100, _rx_func_);
	printf("\r\nCvejo izmapirao sam ti %s\r\n", val.to_string().c_str());
}

void first_mapper::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_mapper", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}


// Class first_plugin 


rx_result first_plugin::init_plugin ()
{
	RX_PLUGIN_LOG_WARNING("first_plugin", 100, "****Initializing dynamic plugin.");
	auto result = rx_platform_api::register_source_runtime<first_source>(rx_node_id(17, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_mapper_runtime<first_mapper>(rx_node_id(19, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_filter_runtime<first_filter>(rx_node_id(18, 8));
	if (!result)
		return result;

	result = rx_platform_api::register_object_runtime<first_object>(rx_node_id(20, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_domain_runtime<first_domain>(rx_node_id(21, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_struct_runtime<first_struct>(rx_node_id(30, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_application_runtime<first_application>(rx_node_id(22, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_port_runtime<first_transport_port>(rx_node_id(23, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_monitored_port_runtime<first_server_port>(rx_node_id(24, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_port_runtime<first_client_port>(rx_node_id(25, 8));
	if (!result)
		return result;
	result = rx_platform_api::register_port_runtime<ether::ether_subscriber1_port>(rx_node_id(27, 8));
	if (!result)
		return result;


	result = rx_platform_api::register_relation_runtime<first_relation>(rx_node_id(26, 8));
	if (!result)
		return result;

	result = rx_platform_api::register_storage_type<strg::test_storage>("test");
	if (!result)
	{
		RX_PLUGIN_LOG_WARNING("first_plugin", 900, ("Unable to register storage type:"s + result.errors_line()).c_str());
		result = true;
	}

	result = rx_platform_api::register_singleton_runtime<first_singleton>(rx_node_id(30, 8));
	if (!result)
		return result;

	result = rx_platform_api::register_method_runtime<first_method>(rx_node_id(33, 8));
	if (!result)
		return result;


	return result;
}

rx_result first_plugin::deinit_plugin ()
{
	return true;
}

rx_result first_plugin::build_plugin ()
{
	RX_PLUGIN_LOG_WARNING("first_plugin", 100, "****Building dynamic plugin.");
	auto result = rx_platform_api::register_item_binary_with_code<first_source>("mojSource", "subFolder"
		, rx_node_id(17,8), RX_EXTERN_SOURCE_TYPE_ID, c_def_mojSource, sizeof(c_def_mojSource), 0x10006);
	if (!result)
		return result;
	result = rx_platform_api::register_item_binary_with_code<first_filter>("mojFilter", ""
		, rx_node_id(18, 8), RX_CLASS_FILTER_BASE_ID, c_def_mojFilter, sizeof(c_def_mojFilter), 0x10006);
	if (!result)
		return result;
	result = rx_platform_api::register_item_binary_with_code<first_mapper>("mojMapper", "subFolder"
		, rx_node_id(19, 8), RX_EXTERN_MAPPER_TYPE_ID,  c_def_mojMapper, sizeof(c_def_mojMapper), 0x10006);
	if (!result)
		return result;

	result = rx_platform_api::register_item_binary_with_code<first_object>("mojObject", "subObjekti"
		, rx_node_id(20, 8), RX_USER_OBJECT_TYPE_ID, c_def_mojObject, sizeof(c_def_mojObject), 0x10006);
	if (!result)
		return result;
	result = rx_platform_api::register_item_binary_with_code<first_domain>("mojDomen", "subObjekti"
		, rx_node_id(21, 8), RX_USER_DOMAIN_TYPE_ID, c_def_mojDomen, sizeof(c_def_mojDomen), 0x10006);
	if (!result)
		return result;
	result = rx_platform_api::register_item_binary_with_code<first_struct>("mojaStruct", "subObjekti"
		, rx_node_id(30, 8), RX_CLASS_STRUCT_BASE_ID, c_def_mojaStruct, sizeof(c_def_mojaStruct), 0x10007);
	if (!result)
		return result;
	result = rx_platform_api::register_item_binary_with_code<first_application>("mojaApp", "subObjekti"
		, rx_node_id(22, 8), RX_USER_APP_TYPE_ID, c_def_mojaApp, sizeof(c_def_mojaApp), 0x10006);
	if (!result)
		return result;

	result = rx_platform_api::register_item_binary_with_code<first_transport_port>("mojTransportPort", "subObjekti"
		, rx_node_id(23, 8), RX_TRANSPORT_PORT_TYPE_ID, c_def_mojTransportPort, sizeof(c_def_mojTransportPort), 0x10006);
	if (!result)
		return result;
	result = rx_platform_api::register_item_binary_with_code<first_server_port>("mojServerPort", "subObjekti"
		, rx_node_id(24, 8), RX_APPLICATION_PORT_TYPE_ID, c_def_mojServerPort, sizeof(c_def_mojServerPort), 0x10006);
	if (!result)
		return result;
	result = rx_platform_api::register_item_binary_with_code<first_client_port>("mojClientPort", "subObjekti"
		, rx_node_id(25, 8), RX_APPLICATION_PORT_TYPE_ID, c_def_mojClientPort, sizeof(c_def_mojClientPort), 0x10006);
	if (!result)
		return result;

	result = rx_platform_api::register_item_binary_with_code<first_client_port>("ethSubs1", "ethTest"
			, rx_node_id(27, 8), RX_APPLICATION_PORT_TYPE_ID, c_def_ethSubs1, sizeof(c_def_ethSubs1), 0x10006);
	if (!result)
		return result;

	result = rx_platform_api::register_item_binary_with_code<first_relation>("mojaRel", "subObjekti"
		, rx_node_id(26, 8), RX_NS_RELATION_BASE_ID, c_def_mojaRel, sizeof(c_def_mojaRel), 0x10007);
	if (!result)
		return result;

	result = rx_platform_api::register_item_binary_with_code<first_singleton>("mojSingleton", "subObjekti"
		, rx_node_id(31, 8), RX_USER_OBJECT_TYPE_ID, c_def_mojSingleton, sizeof(c_def_mojSingleton), 0x10008);
	if (!result)
		return result;


	result = rx_platform_api::register_runtime_binary_with_code<first_singleton>("rtSingl", ""
		, rx_node_id(32, 8), rx_node_id(31, 8), c_def_rtSingl, sizeof(c_def_rtSingl), 0x10008);
	if (!result)
		return result;


	result = rx_platform_api::register_item_binary_with_code<first_method>("mojMethod", "subFolder"
		, rx_node_id(33, 8), RX_CLASS_METHOD_BASE_ID, c_def_mojMethod, sizeof(c_def_mojMethod), 0x20003);
	if (!result)
		return result;

	return result;
}


// Class first_source 

first_source::first_source()
      : timer_(0),
        amplitude_(100.0),
        period_(10000),
        sample_(200),
        ticks_module_(0)
	, rx_source(true, true)
{
}


first_source::~first_source()
{
}



rx_result first_source::initialize_source (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_source", 100, _rx_func_);
	rx_result result = sample_.bind(".Sample", ctx, [this](const uint32_t val)
		{
			if (timer_)
				start_timer(timer_, val);
		});
	if (!result)
		return result;
	result = period_.bind(".Period", ctx, [this](const uint32_t val)
		{
			ticks_module_ = ((rx_timer_ticks_t)val) * 1000;
		});
	if (!result)
		return result;
	result = amplitude_.bind(".Amplitude", ctx);
	if (!result)
		return result;
	return true;
}

rx_result first_source::start_source (rx_platform_api::rx_start_context& ctx)
{

	RX_PLUGIN_LOG_DEBUG("first_source", 100, _rx_func_);

	values::rx_value val;
	val.assign_static<double>(113.117, rx_time::now());
	val.set_good_locally();
	source_value_changed(std::move(val));

	ticks_module_ = ((rx_timer_ticks_t)period_) * 1000;

	timer_ = ctx.create_timer([this]()
		{
			timer_tick();

		}, sample_);

	return true;
}

rx_result first_source::stop_source ()
{
	RX_PLUGIN_LOG_DEBUG("first_source", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_source::deinitialize_source ()
{
	RX_PLUGIN_LOG_DEBUG("first_source", 100, _rx_func_);
	return true;
}

rx_result first_source::source_write (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_source", 100, _rx_func_);
	return RX_NOT_SUPPORTED;
}

void first_source::timer_tick ()
{
	rx_timer_ticks_t ticks = rx_get_us_ticks() % ticks_module_;
	double angle = ((double)ticks) * 3.14 * 2 / ((double)ticks_module_);
	double current = amplitude_*cos(angle);
	values::rx_value val;
	val.assign_static<double>(current, rx_time::now());
	val.set_good_locally();
	source_value_changed(std::move(val));
}


// Class first_object 

first_object::first_object()
      : timer_(0)
{
}


first_object::~first_object()
{
}



rx_result first_object::initialize_object (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_object", 100, _rx_func_);
	return true;
}

rx_result first_object::start_object (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_object", 100, _rx_func_);
	timer_ = create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_object::stop_object ()
{
	RX_PLUGIN_LOG_DEBUG("first_object", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_object::deinitialize_object ()
{
	RX_PLUGIN_LOG_DEBUG("first_object", 100, _rx_func_);
	return true;
}

void first_object::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_object", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}


// Class first_domain 

first_domain::first_domain()
      : timer_(0)
{
}


first_domain::~first_domain()
{
}



rx_result first_domain::initialize_domain (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_domain", 100, _rx_func_);
	return true;
}

rx_result first_domain::start_domain (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_domain", 100, _rx_func_);
	timer_ = create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_domain::stop_domain ()
{
	RX_PLUGIN_LOG_DEBUG("first_domain", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_domain::deinitialize_domain ()
{
	RX_PLUGIN_LOG_DEBUG("first_domain", 100, _rx_func_);
	return true;
}

void first_domain::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_domain", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}


// Class first_application 

first_application::first_application()
      : timer_(0)
{
}


first_application::~first_application()
{
}



rx_result first_application::initialize_application (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_application", 100, _rx_func_);
	return true;
}

rx_result first_application::start_application (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_application", 100, _rx_func_);
	timer_ = create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_application::stop_application ()
{
	RX_PLUGIN_LOG_DEBUG("first_application", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_application::deinitialize_application ()
{
	RX_PLUGIN_LOG_DEBUG("first_application", 100, _rx_func_);
	return true;
}

void first_application::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_application", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}


// Class first_struct 

first_struct::first_struct()
      : timer_(0)
{
}


first_struct::~first_struct()
{
}



rx_result first_struct::initialize_struct (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_struct", 100, _rx_func_);
	return true;
}

rx_result first_struct::start_struct (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_struct", 100, _rx_func_);
	timer_ = ctx.create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_struct::stop_struct ()
{
	RX_PLUGIN_LOG_DEBUG("first_struct", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_struct::deinitialize_struct ()
{
	RX_PLUGIN_LOG_DEBUG("first_struct", 100, _rx_func_);
	return true;
}

void first_struct::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_struct", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}


// Class first_relation 

first_relation::first_relation()
{
}


first_relation::~first_relation()
{
}



rx_result first_relation::initialize_relation (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_relation", 100, _rx_func_);
	return true;
}

rx_result first_relation::start_relation (rx_platform_api::rx_start_context& ctx, bool is_target)
{
	RX_PLUGIN_LOG_DEBUG("first_relation", 100, _rx_func_);
	return true;
}

rx_result first_relation::stop_relation (bool is_target)
{
	RX_PLUGIN_LOG_DEBUG("first_relation", 100, _rx_func_);
	return true;
}

rx_result first_relation::deinitialize_relation ()
{
	RX_PLUGIN_LOG_DEBUG("first_relation", 100, _rx_func_);
	return true;
}

void first_relation::relation_connected (rx_node_id from, rx_node_id to)
{
	RX_PLUGIN_LOG_DEBUG("first_relation", 100, _rx_func_);
}

void first_relation::relation_disconnected (rx_node_id from, rx_node_id to)
{
	RX_PLUGIN_LOG_DEBUG("first_relation", 100, _rx_func_);
}

rx_relation::smart_ptr first_relation::make_target_relation ()
{
	RX_PLUGIN_LOG_DEBUG("first_relation", 100, _rx_func_);
	return rx_create_reference<first_relation>();
}


// Class first_singleton 

first_singleton::first_singleton()
      : timer_(0)
{
}


first_singleton::~first_singleton()
{
}



rx_result first_singleton::initialize_object (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_singleton", 100, _rx_func_);
	return true;
}

rx_result first_singleton::start_object (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_singleton", 100, _rx_func_);
	timer_ = create_calc_timer([this]() {timer_tick(); }, 1000);
	return true;
}

rx_result first_singleton::stop_object ()
{
	RX_PLUGIN_LOG_DEBUG("first_singleton", 100, _rx_func_);
	if (timer_)
		destroy_timer(timer_);
	return true;
}

rx_result first_singleton::deinitialize_object ()
{
	RX_PLUGIN_LOG_DEBUG("first_singleton", 100, _rx_func_);
	return true;
}

void first_singleton::timer_tick ()
{
	RX_PLUGIN_LOG_DEBUG("first_singleton", 100, _rx_func_);
	destroy_timer(timer_);
	timer_ = 0;
}

first_singleton::smart_ptr first_singleton::instance ()
{
	static first_singleton::smart_ptr g_inst;
	if (!g_inst)
		g_inst = rx_create_reference< first_singleton>();
	return g_inst;
}


