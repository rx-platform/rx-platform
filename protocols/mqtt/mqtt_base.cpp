

/****************************************************************************
*
*  protocols\mqtt\mqtt_base.cpp
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

#include "system/server/rx_server.h"

// mqtt_base
#include "protocols/mqtt/mqtt_base.h"

#include "lib/rx_io_buffers.h"
#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"


namespace protocols {

namespace mqtt {

namespace mqtt_common {
uint8_t create_qos0_control_byte(mqtt_message_type msg_type)
{
    return ((((uint8_t)msg_type) << 4));
}
uint8_t create_qos1_control_byte(mqtt_message_type msg_type)
{
    return ((((uint8_t)msg_type) << 4) | 0x02);
}
uint8_t create_publish_control_byte(mqtt_qos_level qos_level, bool dup, bool ret)
{
    return ((((uint8_t)mqtt_message_type::PUBLISH) << 4)
        | (dup ? 0x08 : 0x00)
        | (((uint8_t)qos_level) << 1)
        | (ret ? 0x01 : 0x00));
}
int32_t read_multibyte_size(io::rx_const_io_buffer& buffer)
{
    int32_t len = 0;
    uint8_t temp_byte;
    auto result = buffer.read_from_buffer(temp_byte);
    if (!result)
        return -1;
    if ((temp_byte & 0x80) == 0)
    {
        len = temp_byte;
    }
    else
    {
        len = ((uint32_t)temp_byte & 0x7f);
        result = buffer.read_from_buffer(temp_byte);
        if (!result)
            return -1;
        if ((temp_byte & 0x80) == 0)
        {
            len |= (((uint32_t)temp_byte & 0x7f)<<7);
        }
        else
        {
            len |= (((uint32_t)temp_byte & 0x7f) << 7);
            result = buffer.read_from_buffer(temp_byte);
            if (!result)
                return -1;
            if ((temp_byte & 0x80) == 0)
            {
                len |= (((uint32_t)temp_byte & 0x7f) << 14);
            }
            else
            {
                return -1; // no more then 3 bytes!!!
            }
        }
    }
    return len;
}
bool write_multibyte_size(int32_t size, io::rx_io_buffer& buffer)
{
    if (size < 0x7f)
    {
        buffer.write_to_buffer((uint8_t)size);
    }
    else if (size < 0x7fff)
    {
        buffer.write_to_buffer((uint8_t)(0x80 | (size&0x7f)));
        buffer.write_to_buffer((uint8_t)((size>>7) & 0x7f));
    }
    else if (size < 0x7fffff)
    {
        buffer.write_to_buffer((uint8_t)(0x80 | (size & 0x7f)));
        buffer.write_to_buffer((uint8_t)(0x80 | ((size >> 7) & 0x7f)));
        buffer.write_to_buffer((uint8_t)((size >> 14) & 0x7f));
    }
    else
    {
        return false;// invalid length
    }
    return true;
}

// Class protocols::mqtt::mqtt_common::mqtt_transaction

mqtt_transaction::mqtt_transaction()
    : timeout_(0)
    , trans_id_(0)
{
}

mqtt_transaction::mqtt_transaction (uint16_t id, uint32_t timeout)
    : timeout_(((rx_timer_ticks_t)timeout) * 10000)
    , trans_id_(id)
{
}


mqtt_transaction::~mqtt_transaction()
{
}



uint16_t mqtt_transaction::get_trans_id () const
{
  return trans_id_;
}

void mqtt_transaction::set_trans_id (uint16_t value)
{
  trans_id_ = value;
}


// Class protocols::mqtt::mqtt_common::mqtt_subscribe_transaction

mqtt_subscribe_transaction::mqtt_subscribe_transaction()
      : done_(false)
{
}

mqtt_subscribe_transaction::mqtt_subscribe_transaction (const string_type& subs_id, topics_type topics, uint16_t id, uint32_t timeout)
      : done_(false)
    , mqtt_transaction(id, timeout)
    , topics_(std::move(topics))
    , subscription_id_(subs_id)
{
}

mqtt_subscribe_transaction::mqtt_subscribe_transaction (const string_type& subs_id, mqtt_subscribe_data topic, uint16_t id, uint32_t timeout)
      : done_(false)
    , mqtt_transaction(id, timeout)
    , topics_({ std::move(topic) })
    , subscription_id_(subs_id)
{
}



rx_protocol_result_t mqtt_subscribe_transaction::start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    // message id
    buffer.write_to_buffer(htons(get_trans_id()));

    if (version >= mqtt_protocol_version::version50)
    {
        // property length
        uint32_t props_size = 0;
        if (!subscription_id_.empty())
            props_size += (3 + (uint32_t)subscription_id_.size());


        if (!user_properties_.empty())
        {
            for (const auto& one : user_properties_)
            {
                props_size += (int32_t)(5 + one.first.size() + one.second.size());
            }
        }

        if (!write_multibyte_size(props_size, buffer))
            return RX_PROTOCOL_OUT_OF_MEMORY;

        if (!subscription_id_.empty())
        {
            buffer.write_to_buffer((uint8_t)0x26);
            buffer.write_to_buffer((uint16_t)subscription_id_.size());
            buffer.write_chars(subscription_id_);
        }

        if (!user_properties_.empty())
        {
            for (const auto& one : user_properties_)
            {
                auto ret = buffer.write_to_buffer((uint8_t)0x26);
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                ret = buffer.write_to_buffer(htons((uint16_t)one.first.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.first.empty())
                {
                    ret = buffer.write_chars(one.first);
                    if (!RX_PROTOCOL_OUT_OF_MEMORY)
                        return ret;
                }
                ret = buffer.write_to_buffer(htons((uint16_t)one.second.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.second.empty())
                {
                    ret = buffer.write_chars(one.second);
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                }
            }
        }
    }

    // now do the topics
    for (const auto& one : topics_)
    {
        uint8_t opt_byte = (uint8_t)one.qos;
        if (version >= mqtt_protocol_version::version50)
        {
            opt_byte |= (((uint8_t)one.retain_handling) << 4);
            if (one.no_local)
                opt_byte |= (0x04);
            if (one.retain_as_published)
                opt_byte |= (0x08);
        }

        buffer.write_to_buffer(htons((uint16_t)one.topic.size()));
        buffer.write_chars(one.topic);
        buffer.write_to_buffer(opt_byte);
    }
    control_byte = create_qos1_control_byte(mqtt_message_type::SUBSCRIBE);

    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_subscribe_transaction::process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    done_ = true;
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_subscribe_transaction::start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{

    if (version >= mqtt_protocol_version::version50)
    {// read properties
        int32_t props_len = mqtt_common::read_multibyte_size(buffer);
        if (props_len < 0)
            return RX_PROTOCOL_INVALID_SEQUENCE;

        while (props_len > 0)
        {
            uint8_t prop_id = 0;
            auto ret = buffer.read_from_buffer(prop_id);
            if (!ret)
                return RX_PROTOCOL_INVALID_SEQUENCE;

            switch (prop_id)
            {
            case 0x0b:// Subscription Id
                {
                    uint32_t temp = read_multibyte_size(buffer);
                    if (temp == 0)
                        return RX_PROTOCOL_INVALID_ARG;
                    subscription_id_ = std::move(temp);
                }
                break;
            case 0x26:// User Property
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str1;
                    str1.assign(str_len, ' ');
                    ret = buffer.read_data(&str1[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    str_len = ntohs(temp);
                    string_type str2;
                    str2.assign(str_len, ' ');
                    ret = buffer.read_data(&str2[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (5 + (int32_t)str1.size() + (int32_t)str2.size());

                    user_properties_.emplace(std::move(str1), std::move(str2));

                }
                break;

            default:
                RX_ASSERT(false);
                std::ostringstream ss;
                ss << "Error reading property. Unknown or unexpected Id "
                    << prop_id
                    << " from SUBSCRIBE.";
                MQTT_LOG_ERROR("mqtt_simple_client_endpoint", 200, ss.str());
                return RX_PROTOCOL_INVALID_ARG;
            }
        }
    }

    while (!buffer.eof())
    {
        mqtt_subscribe_data data;
        string_type temp;
        uint16_t str_len = 0;
        auto ret = buffer.read_from_buffer(str_len);
        if (!ret)
            return RX_PROTOCOL_INVALID_SEQUENCE;
        str_len = ntohs(str_len);
        if(str_len==0)
            return RX_PROTOCOL_INVALID_ARG;

        temp.assign(str_len, ' ');
        ret = buffer.read_data(&temp[0], str_len);
        if (!ret)
            return RX_PROTOCOL_INVALID_SEQUENCE;

        data.topic = std::move(temp);
        uint8_t opt_byte = 0;
        auto res = buffer.read_from_buffer(opt_byte);
        if (!res)
            return RX_PROTOCOL_INVALID_SEQUENCE;

        if (version == mqtt_protocol_version::version311)
        {
            if((opt_byte & 0xfc)!=0)
                return RX_PROTOCOL_INVALID_ARG;
            data.qos = (mqtt_qos_level)(opt_byte & 0x03);
        }
        else if (version == mqtt_protocol_version::version50)
        {

            if ((opt_byte & 0xc0) != 0)
                return RX_PROTOCOL_INVALID_ARG;

            data.retain_handling = (mqtt_retain_handling)(opt_byte >> 4);
            data.no_local = ((opt_byte & 0x04) != 0);
            data.retain_as_published = ((opt_byte & 0x08) != 0);
        }
        topics_.push_back(std::move(data));
    }
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_subscribe_transaction::send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    control_byte = create_qos0_control_byte(mqtt_message_type::SUBACK);
    auto ret = buffer.write_to_buffer(get_trans_id());
    if (!ret)
        return RX_PROTOCOL_OUT_OF_MEMORY;

    if (version >= mqtt_protocol_version::version50)
    {
        uint32_t props_size = 0;
        if (!result_string_.empty())
            props_size += (uint32_t)(3 + result_string_.size());

        if (!user_properties_.empty())
        {
            for (const auto& one : user_properties_)
            {
                props_size += (int32_t)(5 + one.first.size() + one.second.size());
            }
        }

        if (!write_multibyte_size(props_size, buffer))
            return RX_PROTOCOL_OUT_OF_MEMORY;

        if (!result_string_.empty())
        {
            if (!buffer.write_to_buffer((uint8_t)0x1a))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_to_buffer(htons((uint16_t)result_string_.size())))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_chars(result_string_))
                return RX_PROTOCOL_OUT_OF_MEMORY;
        }
        if (!user_properties_.empty())
        {
            for (const auto& one : user_properties_)
            {
                auto ret = buffer.write_to_buffer((uint8_t)0x26);
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                ret = buffer.write_to_buffer(htons((uint16_t)one.first.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.first.empty())
                {
                    ret = buffer.write_chars(one.first);
                    if (!RX_PROTOCOL_OUT_OF_MEMORY)
                        return ret;
                }
                ret = buffer.write_to_buffer(htons((uint16_t)one.second.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.second.empty())
                {
                    ret = buffer.write_chars(one.second);
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                }
            }
        }
    }
    for (const auto& one : topics_)
    {
        uint8_t res_code = 0;
        if (one.result_code == 0)
        {
            res_code = ((uint8_t)one.qos);
        }
        else
        {
            if (version == mqtt_protocol_version::version311)
                res_code = 0x80;
            else if (version == mqtt_protocol_version::version311)
                res_code = one.result_code;
        }
        ret = buffer.write_to_buffer(res_code);
        if (!ret)
            return RX_PROTOCOL_OUT_OF_MEMORY;
    }
    return RX_PROTOCOL_OK;
}

bool mqtt_subscribe_transaction::is_done () const
{
    return done_;
}


const string_type& mqtt_subscribe_transaction::get_result_string () const
{
  return result_string_;
}

void mqtt_subscribe_transaction::set_result_string (const string_type& value)
{
  result_string_ = value;
}


// Class protocols::mqtt::mqtt_common::mqtt_publish_transaction

mqtt_publish_transaction::mqtt_publish_transaction (mqtt_publish_data data, uint16_t id, uint32_t timeout)
    : step_(0)
    , data_(data)
    , mqtt_transaction(id, timeout)
{
    data_.packet_id = id;
}



rx_protocol_result_t mqtt_publish_transaction::start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    switch (step_)
    {
    case 0:
        {
            control_byte = create_publish_control_byte(data_.qos, false, false);
            auto result = data_.serialize(control_byte, buffer, version);
            if (result)
            {
                return RX_PROTOCOL_OK;
            }
            else
            {
                return RX_PROTOCOL_PARSING_ERROR;
            }
        }
    case 1:
        {
            switch (data_.qos)
            {
            case mqtt_qos_level::Level2:
                {
                    auto ret = buffer.write_to_buffer(htons(get_trans_id()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    control_byte = create_qos1_control_byte(mqtt_message_type::PUBREL);

                    return RX_PROTOCOL_OK;

                }
                break;
            default:
                ;
            }
        }
    }

    return RX_PROTOCOL_INVALID_SEQUENCE;
}

rx_protocol_result_t mqtt_publish_transaction::process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    switch (step_)
    {
    case 0:
        {
            switch (data_.qos)
            {
            case mqtt_qos_level::Level1:
                {
                    if (ctrl == create_qos0_control_byte(mqtt_message_type::PUBACK))
                    {
                        step_++;
                        return RX_PROTOCOL_OK;
                    }
                }
                break;
            case mqtt_qos_level::Level2:
                {
                    if (ctrl == create_qos0_control_byte(mqtt_message_type::PUBREC))
                    {
                        step_++;
                        return RX_PROTOCOL_OK;
                    }
                }
            default:
                ;
            }
        }
        break;
    case 1:
        {
            switch (data_.qos)
            {
            case mqtt_qos_level::Level2:
                {
                    if (ctrl == create_qos0_control_byte(mqtt_message_type::PUBCOMP))
                    {
                        step_++;
                        return RX_PROTOCOL_OK;
                    }
                }
            default:
                ;
            }
        }
    }
    return RX_PROTOCOL_INVALID_SEQUENCE;
}

rx_protocol_result_t mqtt_publish_transaction::start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    switch (step_)
    {
    case 1:
        {
            switch (data_.qos)
            {
            case mqtt_qos_level::Level2:
                {
                    if (ctrl == create_qos1_control_byte(mqtt_message_type::PUBREL))
                    {
                        return RX_PROTOCOL_OK;
                    }
                }
            default:
                ;
            }
        }
    }
    return RX_PROTOCOL_INVALID_SEQUENCE;
}

rx_protocol_result_t mqtt_publish_transaction::send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    switch (data_.qos)
    {
    case mqtt_qos_level::Level1:
        {
            switch (step_)
            {
            case 0:
                {
                    auto ret = buffer.write_to_buffer(htons(get_trans_id()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    control_byte = create_qos0_control_byte(mqtt_message_type::PUBACK);
                    step_++;
                    return RX_PROTOCOL_OK;
                }
            default:
                RX_ASSERT(false);
                return RX_PROTOCOL_INVALID_SEQUENCE;
            }
        }
        break;
    case mqtt_qos_level::Level2:
        {
            switch (step_)
            {
            case 0:
                {
                    auto ret = buffer.write_to_buffer(htons(get_trans_id()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    control_byte = create_qos0_control_byte(mqtt_message_type::PUBREC);
                    step_++;
                    return RX_PROTOCOL_OK;
                }
            case 1:
                {
                    auto ret = buffer.write_to_buffer(htons(get_trans_id()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    control_byte = create_qos0_control_byte(mqtt_message_type::PUBCOMP);
                    step_++;
                    return RX_PROTOCOL_OK;
                }
            default:
                RX_ASSERT(false);
                return RX_PROTOCOL_INVALID_SEQUENCE;
            }
        }
        break;
    default:
        {

        }
        break;

    }
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

bool mqtt_publish_transaction::is_done () const
{
    bool ret = ((int)data_.qos <= step_);
    return ret;
}


// Class protocols::mqtt::mqtt_common::mqtt_publish_data

mqtt_publish_data::mqtt_publish_data()
      : qos(mqtt_qos_level::Level0),
        dup(false),
        retain(true),
        utf_string(false),
        message_expiry_interval(0),
        packet_id(0),
        topic_alias(0)
{
}



rx_result mqtt_publish_data::deserialize (uint8_t ctrl, io::rx_const_io_buffer& buff, mqtt_protocol_version version)
{
    retain = (ctrl & 0x01) != 0;
    dup = (ctrl & 0x08) != 0;
    qos = (mqtt_qos_level)((ctrl & 0x06) >> 1);
    uint16_t topic_len = 0;
    auto ret = buff.read_from_buffer(topic_len);
    if (!ret)
        return ret;
    topic_len = ntohs(topic_len);

    if (topic_len == 0)
        return RX_INVALID_ARGUMENT;
    topic.resize(topic_len);
    ret = buff.read_data(&topic[0], topic_len);
    if (!ret)
        return ret;

    if (qos != mqtt_qos_level::Level0)
    {// this is not level 0 so read msg id
        buff.read_from_buffer(packet_id);
        packet_id = ntohs(packet_id);
    }

    int32_t props_len = mqtt_common::read_multibyte_size(buff);
    if (props_len < 0)
        return "Error reading Properties Length";

    while (props_len > 0)
    {
        uint8_t prop_id = 0;
        ret = buff.read_from_buffer(prop_id);
        if (!ret)
            return ret;

        switch (prop_id)
        {
        case 0x01://Payload Format Indicator
            {
                uint8_t temp;
                ret = buff.read_from_buffer(temp);
                if (!ret)
                    return ret;
                if (temp == 0)
                    utf_string = false;
                else if (temp == 1)
                    utf_string = true;
                else
                    return RX_INVALID_ARGUMENT;
                props_len -= 2;
            }
            break;
        case 0x02:// Message Expiry Interval
            {
                uint32_t temp;
                ret = buff.read_from_buffer(temp);
                if (!ret)
                    return ret;
                message_expiry_interval = ntohl(temp);
                props_len -= 5;
            }
            break;
        case 0x08:// Response Topic
            {
                uint16_t temp;
                ret = buff.read_from_buffer(temp);
                if (!ret)
                    return ret;
                size_t str_len = ntohs(temp);
                string_type str;
                if (str_len > 0)
                {
                    str.assign(str_len, ' ');
                    ret = buff.read_data(&str[0], str_len);
                    if (!ret)
                        return ret;
                }
                props_len -= (3 + (int32_t)str.size());
                response_topic = std::move(str);
            }
            break;
        case 0x09://  Correlation Data
            {
                uint16_t temp;
                ret = buff.read_from_buffer(temp);
                if (!ret)
                    return ret;
                size_t str_len = ntohs(temp);
                byte_string str;
                if (str_len > 0)
                {
                    str.assign(str_len, std::byte(0));
                    ret = buff.read_data(&str[0], str_len);
                    if (!ret)
                        return ret;
                }
                props_len -= (3 + (int32_t)str.size());
                corelation_data = std::move(str);
            }
            break;
        case 0x26:// User Property
            {
                uint16_t temp;
                ret = buff.read_from_buffer(temp);
                if (!ret)
                    return ret;
                size_t str_len = ntohs(temp);
                string_type str1;

                if (str_len > 0)
                {
                    str1.assign(str_len, ' ');
                    ret = buff.read_data(&str1[0], str_len);
                    if (!ret)
                        return ret;
                }
                ret = buff.read_from_buffer(temp);
                if (!ret)
                    return ret;
                str_len = ntohs(temp);
                string_type str2;

                if (str_len > 0)
                {
                    str2.assign(str_len, ' ');
                    ret = buff.read_data(&str2[0], str_len);
                    if (!ret)
                        return ret;
                }
                props_len -= (5 + (int32_t)str1.size() + (int32_t)str2.size());

                user_properties.emplace(std::move(str1), std::move(str2));

            }
            break;
        case 0x03:// Content Type
            {
                uint16_t temp;
                ret = buff.read_from_buffer(temp);
                if (!ret)
                    return ret;
                size_t str_len = ntohs(temp);
                string_type str;
                if (str_len > 0)
                {
                    str.assign(str_len, ' ');
                    ret = buff.read_data(&str[0], str_len);
                    if (!ret)
                        return ret;
                }
                props_len -= (3 + (int32_t)str.size());
                content_type = std::move(str);
            }
            break;
        case 0x0b:// Subscription Id
            {
                uint32_t temp = read_multibyte_size(buff);
                if (temp == 0)
                    return RX_INVALID_ARGUMENT;
                subs_ids.emplace(temp);
            }
            break;

        default:
            RX_ASSERT(false);
            std::ostringstream ss;
            ss << "Error reading property. Unknown or unexpected Id "
                << prop_id
                << " from PUBLISH.";
            MQTT_LOG_ERROR("mqtt_simple_client_endpoint", 200, ss.str());
            return ss.str();
        }
    }

    ret = buff.read_to_end(data);
    if (!ret)
        return ret;

    return true;


    {
        RX_ASSERT(false);
        return RX_NOT_IMPLEMENTED;
    }
}

uint32_t mqtt_publish_data::get_prop_size (uint8_t ctrl, mqtt_protocol_version version) const
{
    uint32_t ret = 0;
    if (utf_string)
        ret += 2;
    if (message_expiry_interval != 0)
        ret += 5;
    if (topic_alias != 0)
        ret += 3;
    if (!response_topic.empty())
        ret += (3 + (int32_t)response_topic.size());
    if (!corelation_data.empty())
        ret += (3 + (int32_t)corelation_data.size());
    if (!user_properties.empty())
    {
        for (const auto& one : user_properties)
        {
            ret += (int32_t)(5 + one.first.size() + one.second.size());
        }
    }
    if (!content_type.empty())
        ret += (3 + (int32_t)content_type.size());
    if (!subs_ids.empty())
    {
        RX_ASSERT(false);
    }
    return ret;
}

rx_result mqtt_publish_data::serialize (uint8_t& ctrl, io::rx_io_buffer& buff, mqtt_protocol_version version)
{
    rx_result ret;

    if (topic.empty() || data.empty())
        return RX_INVALID_ARGUMENT;

    if (retain)
        ctrl |= 0x01;

    ctrl |= (((uint8_t)qos) << 1);

    ret = buff.write_to_buffer(htons((uint16_t)topic.size()));
    if (!ret)
        return ret;
    ret = buff.write_chars(topic);
    if (!ret)
        return ret;

    if ((ctrl & 0x06) > 0)
    {
        buff.write_to_buffer(htons(packet_id));
    }

    if (version >= mqtt_protocol_version::version50)
    {
        uint32_t props_size = get_prop_size(ctrl, version);
        write_multibyte_size(props_size, buff);

        if (utf_string)
        {
            ret = buff.write_to_buffer((uint8_t)0x01);
            if (!ret)
                return ret;
            ret = buff.write_to_buffer((uint8_t)1);
            if (!ret)
                return ret;
        }
        if (message_expiry_interval != 0)
        {
            ret = buff.write_to_buffer((uint8_t)0x02);
            if (!ret)
                return ret;
            ret = buff.write_to_buffer(htonl(message_expiry_interval));
            if (!ret)
                return ret;
        }
        if (topic_alias != 0)
        {
            ret = buff.write_to_buffer((uint8_t)0x23);
            if (!ret)
                return ret;
            ret = buff.write_to_buffer(htons(topic_alias));
            if (!ret)
                return ret;
        }
        if (!response_topic.empty())
        {
            ret = buff.write_to_buffer((uint8_t)0x08);
            if (!ret)
                return ret;
            ret = buff.write_to_buffer(htons((uint16_t)response_topic.size()));
            if (!ret)
                return ret;
            ret = buff.write_chars(response_topic);
            if (!ret)
                return ret;
        }

        if (!corelation_data.empty())
        {
            ret = buff.write_to_buffer((uint8_t)0x09);
            if (!ret)
                return ret;
            ret = buff.write_to_buffer(htons((uint16_t)corelation_data.size()));
            if (!ret)
                return ret;
            ret = buff.write(&corelation_data[0], corelation_data.size());
            if (!ret)
                return ret;
        }

        if (!content_type.empty())
        {
            ret = buff.write_to_buffer((uint8_t)0x03);
            if (!ret)
                return ret;
            ret = buff.write_to_buffer(htons((uint16_t)content_type.size()));
            if (!ret)
                return ret;
            ret = buff.write_chars(content_type);
            if (!ret)
                return ret;
        }
        if (!user_properties.empty())
        {
            for (const auto& one : user_properties)
            {
                ret = buff.write_to_buffer((uint8_t)0x26);
                if (!ret)
                    return ret;
                ret = buff.write_to_buffer(htons((uint16_t)one.first.size()));
                if (!ret)
                    return ret;
                if (!one.first.empty())
                {
                    ret = buff.write_chars(one.first);
                    if (!ret)
                        return ret;
                }
                ret = buff.write_to_buffer(htons((uint16_t)one.second.size()));
                if (!ret)
                    return ret;
                if (!one.second.empty())
                {
                    ret = buff.write_chars(one.second);
                    if (!ret)
                        return ret;
                }
            }
        }
        if (!subs_ids.empty())
        {
            RX_ASSERT(false);
        }
    }
    ret = buff.write(&data[0], data.size());
    if (!ret)
        return ret;
    return ret;
}


// Class protocols::mqtt::mqtt_common::mqtt_connect_transaction

mqtt_connect_transaction::mqtt_connect_transaction()
    : result_received_(false)
    , result_code_(0)
{
}

mqtt_connect_transaction::mqtt_connect_transaction (mqtt_connection_data data, const mqtt_publish_data& will_data, const string_type& user, const string_type& pass, uint32_t timeout)
    : will_data_(will_data)
    , result_received_(false)
    , result_code_(0)
    , mqtt_transaction(0, timeout)
    , data_(std::move(data))
{
}



rx_protocol_result_t mqtt_connect_transaction::start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    connect_var_header* a = buffer.alloc_from_buffer<connect_var_header>();

    /*a->control = 0x10;
    a->length = 17;*/
    a->protocol_name_length = htons(4);
    a->protocol_name_0 = 0x4d; // M
    a->protocol_name_1 = 0x51; // Q
    a->protocol_name_2 = 0x54; // T
    a->protocol_name_3 = 0x54; // T
    a->protocol_version = (uint8_t)data_.protocol_version;
    a->connect_flags =
        (data_.new_session ? 0x02 : 0x00);
    a->keep_alive = htons(data_.keep_alive);


    if (version >= mqtt_protocol_version::version50)
    {


        uint32_t props_size = 0;
        if (data_.session_expiry)
            props_size += 5;
        if (data_.receive_maximum)
            props_size += 3;
        if (data_.maximum_packet_size)
            props_size += 5;
        if (data_.topic_alias_max)
            props_size += 3;
        if (!data_.user_properties.empty())
        {
            for (const auto& one : data_.user_properties)
            {
                props_size += (int32_t)(5 + one.first.size() + one.second.size());
            }
        }
        if (!data_.auth_method.empty())
            props_size += (uint32_t)(3 + data_.auth_method.size());
        if (!data_.auth_data.empty())
            props_size += (uint32_t)(3 + data_.auth_data.size());

        if (!write_multibyte_size(props_size, buffer))
            return RX_PROTOCOL_OUT_OF_MEMORY;

        if (data_.session_expiry)
        {
            if (!buffer.write_to_buffer((uint8_t)0x11))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_to_buffer(htonl(data_.session_expiry)))
                return RX_PROTOCOL_OUT_OF_MEMORY;
        }
        if (data_.receive_maximum)
        {
            if (!buffer.write_to_buffer((uint8_t)0x21))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_to_buffer(htons(data_.receive_maximum)))
                return RX_PROTOCOL_OUT_OF_MEMORY;
        }
        if (data_.maximum_packet_size)
        {
            if (!buffer.write_to_buffer((uint8_t)0x27))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_to_buffer(htonl(data_.maximum_packet_size)))
                return RX_PROTOCOL_OUT_OF_MEMORY;
        }

        if (data_.topic_alias_max)
        {
            if (!buffer.write_to_buffer((uint8_t)0x22))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_to_buffer(htons(data_.topic_alias_max)))
                return RX_PROTOCOL_OUT_OF_MEMORY;
        }
        if (!data_.user_properties.empty())
        {
            for (const auto& one : data_.user_properties)
            {
                auto ret = buffer.write_to_buffer((uint8_t)0x26);
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                ret = buffer.write_to_buffer(htons((uint16_t)one.first.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.first.empty())
                {
                    ret = buffer.write_chars(one.first);
                    if (!RX_PROTOCOL_OUT_OF_MEMORY)
                        return ret;
                }
                ret = buffer.write_to_buffer(htons((uint16_t)one.second.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.second.empty())
                {
                    ret = buffer.write_chars(one.second);
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                }
            }
        }
        if (!data_.auth_method.empty())
        {
            if (!buffer.write_to_buffer((uint8_t)0x15))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_to_buffer(htons((uint16_t)data_.auth_method.size())))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_chars(data_.auth_method))
                return RX_PROTOCOL_OUT_OF_MEMORY;
        }
        if (!data_.auth_data.empty())
        {
            if (!buffer.write_to_buffer((uint8_t)0x15))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write_to_buffer(htons((uint16_t)data_.auth_data.size())))
                return RX_PROTOCOL_OUT_OF_MEMORY;
            if (!buffer.write(&data_.auth_data[0], data_.auth_data.size()))
                return RX_PROTOCOL_OUT_OF_MEMORY;
        }

    }


    buffer.write_to_buffer<uint16_t>(htons((uint16_t)data_.client_identifier.size())); // id length
    buffer.write_chars(data_.client_identifier);


    control_byte = create_qos0_control_byte(mqtt_message_type::CONNECT);

    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_connect_transaction::process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    result_received_ = true;

    connect_ack_var_header var_head;
    auto ret = buffer.read_from_buffer(var_head);
    if (ret)
    {
        data_.new_session = (var_head.ack_flags & 0x1) == 0;
        result_code_ = var_head.reason_code;

        int32_t props_len = mqtt_common::read_multibyte_size(buffer);
        if (props_len < 0)
            return RX_PROTOCOL_INVALID_SEQUENCE;

        while (props_len > 0)
        {
            uint8_t prop_id = 0;
            ret = buffer.read_from_buffer(prop_id);
            if (!ret)
                return RX_PROTOCOL_INVALID_SEQUENCE;

            switch (prop_id)
            {
            case 0x11://Session Expiry Interval
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.session_expiry = ntohs(temp);
                    props_len -= 3;
                }
                break;
            case 0x21://Receive Maximum
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.receive_maximum = ntohs(temp);
                    props_len -= 3;
                }
                break;
            case 0x24://Maximum QoS
                {
                    uint8_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    if (temp > 1)
                        return RX_PROTOCOL_INVALID_ARG;
                    data_.max_qos_level = (mqtt_qos_level)temp;
                    props_len -= 2;
                }
                break;
            case 0x25:// Retain Available
                {
                    uint8_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    if (temp > 1)
                        return RX_PROTOCOL_INVALID_ARG;
                    data_.retain_available = (temp == 1);
                    props_len -= 2;
                }
                break;
            case 0x27:// Maximum Packet Size
                {
                    uint32_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.maximum_packet_size = ntohl(temp);
                    props_len -= 5;
                }
                break;
            case 0x12:// Assigned Client Identifier
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str;
                    str.assign(str_len, ' ');
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    data_.client_identifier = std::move(str);
                }
                break;
            case 0x22:// Topic Alias Maximum
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.topic_alias_max = ntohs(temp);
                    props_len -= 3;
                }
                break;
            case 0x1f:// Reason String
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str;
                    str.assign(str_len, ' ');
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    result_string_ = std::move(str);
                }
                break;
            case 0x26:// User Property
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str1;
                    str1.assign(str_len, ' ');
                    ret = buffer.read_data(&str1[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    str_len = ntohs(temp);
                    string_type str2;
                    str2.assign(str_len, ' ');
                    ret = buffer.read_data(&str2[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (5 + (int32_t)str1.size() + (int32_t)str2.size());

                    data_.user_properties.emplace(std::move(str1), std::move(str2));

                }
                break;
            case 0x28:// Wildcard Subscription Available
                {
                    uint8_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    if (temp > 1)
                        return RX_PROTOCOL_INVALID_ARG;
                    data_.wildcard_available = (temp == 1);
                    props_len -= 2;
                }
                break;
            case 0x29:// Subscription Identifiers Available
                {
                    uint8_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    if (temp > 1)
                        return RX_PROTOCOL_INVALID_ARG;
                    data_.subscription_id_available = (temp == 1);
                    props_len -= 2;
                }
                break;
            case 0x2a:// Shared Subscription Available
                {
                    uint8_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    if (temp > 1)
                        return RX_PROTOCOL_INVALID_ARG;
                    data_.shared_subscription = (temp == 1);
                    props_len -= 2;
                }
                break;
            case 0x13:// Server Keep Alive
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.keep_alive = ntohs(temp);
                    props_len -= 3;
                }
                break;
            case 0x1a:// Response Information
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str;
                    str.assign(str_len, ' ');
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    data_.response_info = std::move(str);
                }
                break;
            case 0x1c:// Server Reference
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str;
                    str.assign(str_len, ' ');
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    data_.server_reference = std::move(str);
                }
                break;
            case 0x15:// Authentication Method
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str;
                    str.assign(str_len, ' ');
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    data_.auth_method = std::move(str);
                }
                break;
            case 0x16:// Authentication Data
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    byte_string str;
                    str.assign(str_len, std::byte(0));
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    data_.auth_data = std::move(str);
                }
                break;

            default:
                RX_ASSERT(false);
                std::ostringstream ss;
                ss << "Error reading property. Unknown or unexpected Id "
                    << prop_id
                    << " from CONNECTREQ.";
                MQTT_LOG_ERROR("mqtt_simple_client_endpoint", 200, ss.str());
                return RX_PROTOCOL_INVALID_ARG;
            }
        }

        return RX_PROTOCOL_OK;
    }

    return RX_PROTOCOL_INVALID_SEQUENCE;
}

rx_protocol_result_t mqtt_connect_transaction::start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    const connect_var_header* phead = buffer.get_from_buffer<connect_var_header>();
    data_.keep_alive = ntohs(phead->keep_alive);
    switch (phead->protocol_version)
    {
    case 4:
        data_.protocol_version = mqtt_protocol_version::version311;
        break;
    case 5:
        data_.protocol_version = mqtt_protocol_version::version50;
        break;
    default:
        return RX_PROTOCOL_INVALID_ARG;
    }
    data_.new_session = (phead->connect_flags & 0x02) != 0;

    if (data_.protocol_version >= mqtt_protocol_version::version50)
    {// read properties
        int32_t props_len = mqtt_common::read_multibyte_size(buffer);
        if (props_len < 0)
            return RX_PROTOCOL_INVALID_SEQUENCE;

        while (props_len > 0)
        {
            uint8_t prop_id = 0;
            auto ret = buffer.read_from_buffer(prop_id);
            if (!ret)
                return RX_PROTOCOL_INVALID_SEQUENCE;

            switch (prop_id)
            {
            case 0x11://Session Expiry Interval
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.session_expiry = ntohs(temp);
                    props_len -= 3;
                }
                break;
            case 0x21://Receive Maximum
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.receive_maximum = ntohs(temp);
                    props_len -= 3;
                }
                break;
            case 0x27:// Maximum Packet Size
                {
                    uint32_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.maximum_packet_size = ntohl(temp);
                    props_len -= 5;
                }
                break;
            case 0x22:// Topic Alias Maximum
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    data_.topic_alias_max = ntohs(temp);
                    props_len -= 3;
                }
                break;
            case 0x19:// Request Response Information
                {
                    uint8_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                }
                break;
            case 0x17:// Request Problem Information
                {
                    uint8_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                }
                break;
            case 0x26:// User Property
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str1;
                    str1.assign(str_len, ' ');
                    ret = buffer.read_data(&str1[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    str_len = ntohs(temp);
                    string_type str2;
                    str2.assign(str_len, ' ');
                    ret = buffer.read_data(&str2[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (5 + (int32_t)str1.size() + (int32_t)str2.size());

                    data_.user_properties.emplace(std::move(str1), std::move(str2));

                }
                break;
            case 0x15:// Authentication Method
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    string_type str;
                    str.assign(str_len, ' ');
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    data_.auth_method = std::move(str);
                }
                break;
            case 0x16:// Authentication Data
                {
                    uint16_t temp;
                    ret = buffer.read_from_buffer(temp);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    size_t str_len = ntohs(temp);
                    byte_string str;
                    str.assign(str_len, std::byte(0));
                    ret = buffer.read_data(&str[0], str_len);
                    if (!ret)
                        return RX_PROTOCOL_INVALID_SEQUENCE;
                    props_len -= (3 + (int32_t)str.size());

                    data_.auth_data = std::move(str);
                }
                break;

            default:
                RX_ASSERT(false);
                std::ostringstream ss;
                ss << "Error reading property. Unknown or unexpected Id "
                    << prop_id
                    << " from CONNECT.";
                MQTT_LOG_ERROR("mqtt_simple_client_endpoint", 200, ss.str());
                return RX_PROTOCOL_INVALID_ARG;
            }
        }
    }
    // client id
    uint16_t temp_len = 0;
    auto ret = buffer.read_from_buffer(temp_len);
    if (!ret)
        return RX_PROTOCOL_INVALID_SEQUENCE;
    if (temp_len == 0)
        data_.client_identifier.clear();
    else
    {
        temp_len = ntohs(temp_len);
        data_.client_identifier.assign(temp_len, ' ');
        ret = buffer.read_data(&data_.client_identifier[0], temp_len);
        if (!ret)
            return RX_PROTOCOL_INVALID_SEQUENCE;
    }
    if ((phead->connect_flags & 0x04) != 0)
    {//will flag present

        will_data_.qos = (mqtt_qos_level)((phead->connect_flags >> 3) & 0x03);
        will_data_.retain = (phead->connect_flags & 0x20) != 0;

        if (data_.protocol_version >= mqtt_protocol_version::version50)
        {

            int32_t props_len = mqtt_common::read_multibyte_size(buffer);
            if (props_len < 0)
                return RX_PROTOCOL_INVALID_SEQUENCE;

            while (props_len > 0)
            {
                uint8_t prop_id = 0;
                ret = buffer.read_from_buffer(prop_id);
                if (!ret)
                    return RX_PROTOCOL_INVALID_SEQUENCE;

                switch (prop_id)
                {
                case 0x18://Will Delay Interval
                    {
                        uint32_t temp;
                        ret = buffer.read_from_buffer(temp);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        data_.will_delay_interval = ntohl(temp);
                        props_len -= 5;
                    }
                    break;
                case 0x01://Payload Format Indicator
                    {
                        uint8_t temp;
                        ret = buffer.read_from_buffer(temp);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        if (temp == 0)
                            will_data_.utf_string = false;
                        else if(temp==1)
                            will_data_.utf_string = true;
                        else
                            return RX_PROTOCOL_INVALID_ARG;
                        props_len -= 2;
                    }
                    break;
                case 0x02:// Message Expiry Interval
                    {
                        uint32_t temp;
                        ret = buffer.read_from_buffer(temp);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        will_data_.message_expiry_interval = ntohl(temp);
                        props_len -= 5;
                    }
                    break;
                case 0x08:// Response Topic
                    {
                        uint16_t temp;
                        ret = buffer.read_from_buffer(temp);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        size_t str_len = ntohs(temp);
                        string_type str;
                        str.assign(str_len, ' ');
                        ret = buffer.read_data(&str[0], str_len);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        props_len -= (3 + (int32_t)str.size());

                        will_data_.response_topic = std::move(str);
                    }
                    break;
                case 0x09://  Correlation Data
                    {
                        uint16_t temp;
                        ret = buffer.read_from_buffer(temp);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        size_t str_len = ntohs(temp);
                        byte_string str;
                        str.assign(str_len, std::byte(0));
                        ret = buffer.read_data(&str[0], str_len);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        props_len -= (3 + (int32_t)str.size());

                        will_data_.corelation_data = std::move(str);
                    }
                    break;
                case 0x26:// User Property
                    {
                        uint16_t temp;
                        ret = buffer.read_from_buffer(temp);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        size_t str_len = ntohs(temp);
                        string_type str1;
                        str1.assign(str_len, ' ');
                        ret = buffer.read_data(&str1[0], str_len);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        ret = buffer.read_from_buffer(temp);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        str_len = ntohs(temp);
                        string_type str2;
                        str2.assign(str_len, ' ');
                        ret = buffer.read_data(&str2[0], str_len);
                        if (!ret)
                            return RX_PROTOCOL_INVALID_SEQUENCE;
                        props_len -= (5 + (int32_t)str1.size() + (int32_t)str2.size());

                        will_data_.user_properties.emplace(std::move(str1), std::move(str2));

                    }
                    break;

                default:
                    RX_ASSERT(false);
                    std::ostringstream ss;
                    ss << "Error reading property. Unknown or unexpected Id "
                        << prop_id
                        << " from CONNECT WILL PROPERTIES.";
                    MQTT_LOG_ERROR("mqtt_simple_client_endpoint", 200, ss.str());
                    return RX_PROTOCOL_INVALID_ARG;
                }
            }
        }

        temp_len = 0;
        ret = buffer.read_from_buffer(temp_len);
        if (!ret)
            return RX_PROTOCOL_INVALID_SEQUENCE;
        if (temp_len == 0)
            will_data_.topic.clear();
        else
        {
            temp_len = ntohs(temp_len);
            will_data_.topic.assign(temp_len, ' ');
            ret = buffer.read_data(&will_data_.topic[0], temp_len);
            if (!ret)
                return RX_PROTOCOL_INVALID_SEQUENCE;
        }
        temp_len = 0;
        ret = buffer.read_from_buffer(temp_len);
        if (!ret)
            return RX_PROTOCOL_INVALID_SEQUENCE;
        if (temp_len == 0)
            will_data_.data.clear();
        else
        {
            temp_len = ntohs(temp_len);
            will_data_.data.assign(temp_len, std::byte(0));
            ret = buffer.read_data(&will_data_.data[0], temp_len);
            if (!ret)
                return RX_PROTOCOL_INVALID_SEQUENCE;
        }
    }
    if ((phead->connect_flags & 0x80) != 0)
    {// user name
        temp_len = 0;
        ret = buffer.read_from_buffer(temp_len);
        if (!ret)
            return RX_PROTOCOL_INVALID_SEQUENCE;
        if (temp_len == 0)
            data_.user_name.clear();
        else
        {
            temp_len = ntohs(temp_len);
            data_.user_name.assign(temp_len, ' ');
            ret = buffer.read_data(&data_.user_name[0], temp_len);
            if (!ret)
                return RX_PROTOCOL_INVALID_SEQUENCE;
        }
    }
    if ((phead->connect_flags & 0x40) != 0)
    {// password
        temp_len = 0;
        ret = buffer.read_from_buffer(temp_len);
        if (!ret)
            return RX_PROTOCOL_INVALID_SEQUENCE;
        if (temp_len == 0)
            data_.password.clear();
        else
        {
            temp_len = ntohs(temp_len);
            data_.password.assign(temp_len, ' ');
            ret = buffer.read_data(&data_.password[0], temp_len);
            if (!ret)
                return RX_PROTOCOL_INVALID_SEQUENCE;
        }
    }

    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_connect_transaction::send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    connect_ack_var_header* a = buffer.alloc_from_buffer<connect_ack_var_header>();

    a->ack_flags =
        (data_.new_session ? 0x02 : 0x00);
    a->reason_code = result_code_;

    if ((result_code_ & 0x80) != 0)
    {
        if (version >= mqtt_protocol_version::version50)
        {
            uint32_t props_size = 0;
            if (!result_string_.empty())
                props_size += (uint32_t)(3 + result_string_.size());
            if (!data_.user_properties.empty())
            {
                for (const auto& one : data_.user_properties)
                {
                    props_size += (int32_t)(5 + one.first.size() + one.second.size());
                }
            }

            if (!write_multibyte_size(props_size, buffer))
                return RX_PROTOCOL_OUT_OF_MEMORY;

            if (!result_string_.empty())
            {
                if (!buffer.write_to_buffer((uint8_t)0x1a))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons((uint16_t)result_string_.size())))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_chars(result_string_))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.user_properties.empty())
            {
                for (const auto& one : data_.user_properties)
                {
                    auto ret = buffer.write_to_buffer((uint8_t)0x26);
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    ret = buffer.write_to_buffer(htons((uint16_t)one.first.size()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    if (!one.first.empty())
                    {
                        ret = buffer.write_chars(one.first);
                        if (!RX_PROTOCOL_OUT_OF_MEMORY)
                            return ret;
                    }
                    ret = buffer.write_to_buffer(htons((uint16_t)one.second.size()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    if (!one.second.empty())
                    {
                        ret = buffer.write_chars(one.second);
                        if (!ret)
                            return RX_PROTOCOL_OUT_OF_MEMORY;
                    }
                }
            }
        }
    }
    else
    {
        if (version >= mqtt_protocol_version::version50)
        {

            uint32_t props_size = 0;
            if (data_.session_expiry)
                props_size += 5;
            if (data_.receive_maximum)
                props_size += 3;
            if (data_.max_qos_level != mqtt_qos_level::Level2)
                props_size += 2;
            if (!data_.retain_available)
                props_size += 2;
            if (data_.maximum_packet_size)
                props_size += 5;
            if (!data_.client_identifier.empty())
                props_size += (uint32_t)(3 + data_.client_identifier.size());
            if (data_.topic_alias_max)
                props_size += 3;
            if (!result_string_.empty())
                props_size += (uint32_t)(3 + result_string_.size());
            if (!data_.user_properties.empty())
            {
                for (const auto& one : data_.user_properties)
                {
                    props_size += (int32_t)(5 + one.first.size() + one.second.size());
                }
            }
            if (!data_.wildcard_available)
                props_size += 2;
            if (!data_.subscription_id_available)
                props_size += 2;
            if (!data_.shared_subscription)
                props_size += 2;
            if (data_.keep_alive)
                props_size += 3;
            if (!data_.response_info.empty())
                props_size += (uint32_t)(3 + data_.response_info.size());
            if (!data_.server_reference.empty())
                props_size += (uint32_t)(3 + data_.server_reference.size());
            if (!data_.auth_method.empty())
                props_size += (uint32_t)(3 + data_.auth_method.size());
            if (!data_.auth_data.empty())
                props_size += (uint32_t)(3 + data_.auth_data.size());

            if (!write_multibyte_size(props_size, buffer))
                return RX_PROTOCOL_OUT_OF_MEMORY;

            if (data_.session_expiry)
            {
                if (!buffer.write_to_buffer((uint8_t)0x11))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htonl(data_.session_expiry)))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (data_.receive_maximum)
            {
                if (!buffer.write_to_buffer((uint8_t)0x21))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons(data_.receive_maximum)))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (data_.max_qos_level != mqtt_qos_level::Level2)
            {
                if (!buffer.write_to_buffer((uint8_t)0x24))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer((uint8_t)data_.max_qos_level))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.retain_available)
            {
                if (!buffer.write_to_buffer((uint8_t)0x25))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer((uint8_t)0))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (data_.maximum_packet_size)
            {
                if (!buffer.write_to_buffer((uint8_t)0x27))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htonl(data_.maximum_packet_size)))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.client_identifier.empty())
                props_size += (uint32_t)(3 + data_.client_identifier.size());
            if (data_.topic_alias_max)
            {
                if (!buffer.write_to_buffer((uint8_t)0x22))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons(data_.topic_alias_max)))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!result_string_.empty())
            {
                if (!buffer.write_to_buffer((uint8_t)0x1a))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons((uint16_t)result_string_.size())))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_chars(result_string_))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.user_properties.empty())
            {
                for (const auto& one : data_.user_properties)
                {
                    auto ret = buffer.write_to_buffer((uint8_t)0x26);
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    ret = buffer.write_to_buffer(htons((uint16_t)one.first.size()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    if (!one.first.empty())
                    {
                        ret = buffer.write_chars(one.first);
                        if (!RX_PROTOCOL_OUT_OF_MEMORY)
                            return ret;
                    }
                    ret = buffer.write_to_buffer(htons((uint16_t)one.second.size()));
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                    if (!one.second.empty())
                    {
                        ret = buffer.write_chars(one.second);
                        if (!ret)
                            return RX_PROTOCOL_OUT_OF_MEMORY;
                    }
                }
            }
            if (!data_.wildcard_available)
            {
                if (!buffer.write_to_buffer((uint8_t)0x28))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer((uint8_t)0))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.subscription_id_available)
            {
                if (!buffer.write_to_buffer((uint8_t)0x29))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer((uint8_t)0))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.shared_subscription)
            {
                if (!buffer.write_to_buffer((uint8_t)0x2a))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer((uint8_t)0))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (data_.keep_alive)
            {
                if (!buffer.write_to_buffer((uint8_t)0x13))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons(data_.keep_alive)))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.response_info.empty())
            {
                if (!buffer.write_to_buffer((uint8_t)0x1a))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons((uint16_t)data_.response_info.size())))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_chars(data_.response_info))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.server_reference.empty())
            {
                if (!buffer.write_to_buffer((uint8_t)0x1c))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons((uint16_t)data_.server_reference.size())))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_chars(data_.server_reference))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.auth_method.empty())
            {
                if (!buffer.write_to_buffer((uint8_t)0x15))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons((uint16_t)data_.auth_method.size())))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_chars(data_.auth_method))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
            if (!data_.auth_data.empty())
            {
                if (!buffer.write_to_buffer((uint8_t)0x15))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write_to_buffer(htons((uint16_t)data_.auth_data.size())))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!buffer.write(&data_.auth_data[0], data_.auth_data.size()))
                    return RX_PROTOCOL_OUT_OF_MEMORY;
            }
        }
    }

    control_byte = create_qos0_control_byte(mqtt_message_type::CONNACK);

    return RX_PROTOCOL_OK;
}

bool mqtt_connect_transaction::is_done () const
{
    return result_received_;
}


const mqtt_connection_data& mqtt_connect_transaction::get_data () const
{
  return data_;
}

const mqtt_publish_data& mqtt_connect_transaction::get_will_data () const
{
  return will_data_;
}


const uint8_t mqtt_connect_transaction::get_result_code () const
{
  return result_code_;
}

void mqtt_connect_transaction::set_result_code (uint8_t value)
{
  result_code_ = value;
}

const string_type& mqtt_connect_transaction::get_result_string () const
{
  return result_string_;
}

void mqtt_connect_transaction::set_result_string (const string_type& value)
{
  result_string_ = value;
}


// Class protocols::mqtt::mqtt_common::mqtt_connection_data

mqtt_connection_data::mqtt_connection_data()
      : new_session(false),
        session_expiry(0),
        receive_maximum(0xffff),
        max_qos_level(mqtt_qos_level::Level2),
        topic_alias_max(0),
        keep_alive(0),
        retain_available(true),
        maximum_packet_size(0),
        wildcard_available(true),
        subscription_id_available(true),
        shared_subscription(true),
        protocol_version(mqtt_protocol_version::version50),
        will_delay_interval(0)
{
}



// Class protocols::mqtt::mqtt_common::mqtt_ping_transaction

mqtt_ping_transaction::mqtt_ping_transaction (uint32_t timeout)
      : done_(false)
    , mqtt_transaction(0, timeout)
{
}



rx_protocol_result_t mqtt_ping_transaction::start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    control_byte = create_qos0_control_byte(mqtt_message_type::PINGREQ);
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_ping_transaction::process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    done_ = true;
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_ping_transaction::start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t mqtt_ping_transaction::send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

bool mqtt_ping_transaction::is_done () const
{
    return done_;
}


// Class protocols::mqtt::mqtt_common::mqtt_subscribe_data

mqtt_subscribe_data::mqtt_subscribe_data()
      : qos(mqtt_qos_level::Level0),
        retain_handling(mqtt_retain_handling::at_subscribe),
        no_local(false),
        retain_as_published(false),
        result_code(0x00)
{
}



// Class protocols::mqtt::mqtt_common::mqtt_packet_decoder

mqtt_packet_decoder::mqtt_packet_decoder()
      : expected_(0),
        collected_(0),
        collected_header_(0),
        collected_size_(0),
        header_(nullptr),
        header_size_(2)
{
}



rx_protocol_result_t mqtt_packet_decoder::received_function (recv_protocol_packet packet)
{

    rx_const_packet_buffer* data = packet.buffer;
    size_t temp_size;
    rx_protocol_result_t result = RX_PROTOCOL_OK;
    size_t count = data->size - data->next_read;
    const void* buffer = NULL;
    if (header_ == NULL)
    {
        if (collected_header_ == 0)
        {
            // reinitialize collect buffer
            receiving_buffer_.reinit();
        }
        size_t head_size = collected_header_ + count < header_size_ ? count : header_size_ - collected_header_;
        size_t one_head_size = head_size;
        do
        {
            buffer = rx_get_from_packet(data, head_size, &result);
            if (result != RX_PROTOCOL_OK)
                return result;

            memcpy(&header_buffer_[collected_header_], buffer, head_size);
            collected_header_ += head_size;
            if (collected_header_ < header_size_)
                return result;
            if (((header_buffer_[collected_header_ - 1] & 0x80) != 0))
            {
                header_size_++;
                head_size = 1;
                one_head_size++;
                if (header_size_ > 5)
                    return RX_PROTOCOL_INVALID_SEQUENCE;
            }
           // head_size = collected_header_ + count < header_size_ ? count : header_size_ - collected_header_;

        } while (collected_header_ < header_size_);

        collected_header_ = 0;
        header_ = &header_buffer_[0];

        temp_size = header_size_;
        size_t multiplier = 1;
        for (size_t i = 1; i < header_size_; i++)
        {
            temp_size += (multiplier * (header_buffer_[i] & 0x7f));
            multiplier <<= 7;
        }
        //temp_size = header_[1] + 2;
        //result = decoder->check_header_func(header_, &temp_size);
        //if (result != RX_PROTOCOL_OK)
        //	return result;

        expected_ = temp_size - header_size_;
        receiving_buffer_.reinit();

        result = rx_push_to_packet(&receiving_buffer_, &header_buffer_[0], header_size_);
        if (result != RX_PROTOCOL_OK)
            return result;
        count = count - one_head_size;
        collected_header_ = 0;
    }

    if (count + collected_ >= expected_)
    {

        size_t usefull = expected_ - collected_;
        buffer = rx_get_from_packet(data, usefull, &result);
        if (result != RX_PROTOCOL_OK)
            return result;
        result = rx_push_to_packet(&receiving_buffer_, buffer, usefull);
        if (result != RX_PROTOCOL_OK)
            return result;
        // fill receive buffer
        rx_const_packet_buffer_type recv_buffer{};
        rx_init_const_from_packet_buffer(&recv_buffer, &receiving_buffer_);
        // set everything to 0
        collected_ = 0;
        expected_ = 0;
        header_ = NULL;
        header_size_ = 2;
        auto up_packet = rx_create_recv_packet(0, &recv_buffer, 0, 0);
        if (callback_)
            return callback_(up_packet);
    }
    else if (count > 0)
    {
        // just move from input buffer
        size_t usefull = count;
        buffer = rx_get_from_packet(data, usefull, &result);
        if (result != RX_PROTOCOL_OK)
            return result;
        result = rx_push_to_packet(&receiving_buffer_, buffer, usefull);
        if (result != RX_PROTOCOL_OK)
            return result;
        collected_ += count;
    }
    return RX_PROTOCOL_OK;
}

void mqtt_packet_decoder::init_decoder ()
{
    expected_ = 0;
    collected_ = 0;
    collected_header_ = 0;
    collected_size_ = 0;
    header_ = nullptr;
    header_size_ = 2;
    receiving_buffer_.reinit();
}

void mqtt_packet_decoder::init_decoder (callback_type callback)
{
    init_decoder();
    callback_ = callback;
}


// Class protocols::mqtt::mqtt_common::mqtt_unsubscribe_transaction

mqtt_unsubscribe_transaction::mqtt_unsubscribe_transaction()
      : done_(false)
{
}

mqtt_unsubscribe_transaction::mqtt_unsubscribe_transaction (string_array topics, uint16_t id, uint32_t timeout)
      : done_(false)
    , mqtt_transaction(id, timeout)
    , topics_(std::move(topics))
{
}

mqtt_unsubscribe_transaction::mqtt_unsubscribe_transaction (string_type topic, uint16_t id, uint32_t timeout)
      : done_(false)
    , mqtt_transaction(id, timeout)
    , topics_({ std::move(topic) })
{
}



rx_protocol_result_t mqtt_unsubscribe_transaction::start_initiator_step (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    // message id
    buffer.write_to_buffer(htons(get_trans_id()));

    if (version >= mqtt_protocol_version::version50)
    {
        // property length
        uint32_t props_size = 0;

        if (!user_properties_.empty())
        {
            for (const auto& one : user_properties_)
            {
                props_size += (int32_t)(5 + one.first.size() + one.second.size());
            }
        }

        if (!write_multibyte_size(props_size, buffer))
            return RX_PROTOCOL_OUT_OF_MEMORY;


        if (!user_properties_.empty())
        {
            for (const auto& one : user_properties_)
            {
                auto ret = buffer.write_to_buffer((uint8_t)0x26);
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                ret = buffer.write_to_buffer(htons((uint16_t)one.first.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.first.empty())
                {
                    ret = buffer.write_chars(one.first);
                    if (!RX_PROTOCOL_OUT_OF_MEMORY)
                        return ret;
                }
                ret = buffer.write_to_buffer(htons((uint16_t)one.second.size()));
                if (!ret)
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                if (!one.second.empty())
                {
                    ret = buffer.write_chars(one.second);
                    if (!ret)
                        return RX_PROTOCOL_OUT_OF_MEMORY;
                }
            }
        }
    }

    // now do the topics
    for (const auto& one : topics_)
    {

        buffer.write_to_buffer(htons((uint16_t)one.size()));
        buffer.write_chars(one);
    }
    control_byte = create_qos1_control_byte(mqtt_message_type::UNSUBSCRIBE);

    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_unsubscribe_transaction::process_initiator_result (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    done_ = true;
    return RX_PROTOCOL_OK;
}

rx_protocol_result_t mqtt_unsubscribe_transaction::start_listener_step (uint8_t ctrl, io::rx_const_io_buffer& buffer, mqtt_protocol_version version)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t mqtt_unsubscribe_transaction::send_listener_result (io::rx_io_buffer& buffer, uint8_t& control_byte, mqtt_protocol_version version)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

bool mqtt_unsubscribe_transaction::is_done () const
{
    return done_;
}


const string_type& mqtt_unsubscribe_transaction::get_result_string () const
{
  return result_string_;
}

void mqtt_unsubscribe_transaction::set_result_string (const string_type& value)
{
  result_string_ = value;
}


} // namespace mqtt_common
} // namespace mqtt
} // namespace protocols



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0

#endif
