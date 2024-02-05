

/****************************************************************************
*
*  discovery\rx_discovery_comm.cpp
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

#include "rx_discovery_configuration.h"
#include "rx_discovery_main.h"

// rx_discovery_comm
#include "discovery/rx_discovery_comm.h"

#include "lib/rx_ser_bin.h"
#include "sys_internal/rx_inf.h"
#include "sys_internal/rx_async_functions.h"
#include "rx_discovery_algorithm.h"
#include "rx_discovery_protocol.h"

#define RX_FALLBACK_TIMEOUT_MIN 2000
#define RX_FALLBACK_TIMEOUT_MAX 10000

#define RX_ACTIVE_TIMEOUT_MIN 2000
#define RX_ACTIVE_TIMEOUT_MAX 5000


#define RX_KEEP_ALIVE_MIN 10000
#define RX_KEEP_ALIVE_MAX 20000

#define RX_KEEP_ALIVE_TIMEOUT (RX_KEEP_ALIVE_MAX*2)


#define DISC_LOCAL_SOCKET_ADDR ""
#define DISC_SOCKETS_PORT 0




namespace rx_internal {

namespace discovery {

void discovery_point::socket_holder_t::release_buffer(buffer_ptr what)
{
    if (whose)
        return whose->release_buffer(what);
}
bool discovery_point::socket_holder_t::readed(const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity)
{
    security::secured_scope _(identity);
    if (whose)
        whose->readed(data, count, bind_address.get_address(), addr, identity);

    return true;
}
void discovery_point::socket_holder_t::on_shutdown(rx_security_handle_t identity)
{
    if (whose)
    {
    }
}
discovery_point::socket_holder_t::socket_holder_t(discovery_point* whose, const io::ip4_address& bind_addr)
    : whose(whose)
    , bind_address(bind_addr)
{
}
discovery_point::socket_holder_t::socket_holder_t(socket_holder_t&& right) noexcept
{
    whose = right.whose;
    bind_address = right.bind_address;
    right.whose = nullptr;
}
void discovery_point::socket_holder_t::timer_tick(rx_timer_ticks_t ticks)
{
    if (whose && bind_address.is_empty_ip4())
        whose->timer_tick(ticks);
}
void discovery_point::socket_holder_t::disconnect()
{
    whose = nullptr;
    initiate_shutdown();
}

// Class rx_internal::discovery::discovery_point 

discovery_point::discovery_point()
      : my_register_(nullptr),
        executer_(-1),
        multicast_arg_(nullptr),
        last_id_(1),
        state_(discovery_state::idle),
        current_port_(0),
        system_port_(0),
        default_port_(0),
        next_tick_(0)
{
}


discovery_point::~discovery_point()
{
}



bool discovery_point::on_startup (rx_thread_handle_t destination)
{
    return true;
}

void discovery_point::on_shutdown (rx_security_handle_t identity)
{
}

bool discovery_point::readed (const void* data, size_t count, const struct sockaddr* bind, const struct sockaddr* addr, rx_security_handle_t identity)
{
    if (count >= sizeof(rx_discovery_header_t))
    {
        buffer_ptr out_buffer;
        io::ip4_address to_addr;
        io::ip4_address ip_addr(addr);
        rx_discovery_header_t* phead = (rx_discovery_header_t*)data;
        //printf("Discovery Point readed:%d msg type:%d from %s\r\n", (int)count, (int)phead->type, ip_addr.to_string().c_str());
        if (phead->known == RX_DISCOVERY_KNOWN_UINT
            && phead->size == count
            && !my_register_->is_this_you(phead->idenity)
            && (phead->type >= DISCOVERY_MESSAGE_MIN && phead->type <= DISCOVERY_MESSAGE_MAX))
        {

            buffer_ptr buffer = get_buffer();
            buffer->push_data(((uint8_t*)data) + sizeof(rx_discovery_header_t), count - sizeof(rx_discovery_header_t));
            serialization::std_buffer_reader reader(*buffer);
            if (phead->version <= RX_CURRENT_DISCOVERY_VERSION)
            {
                switch (phead->type)
                {
                case DISCOVERY_NEW_MASTER:
                    {
                        DISCOVERY_LOG_DEBUG("discovery_point", 200, "Discovery NEW_MASTER received from "s + ip_addr.to_string());
                        std::scoped_lock _(state_lock_);
                        switch (state_)
                        {
                        case discovery_state::idle:
                            {
                                RX_ASSERT(false);
                                // this isn't possible but doesn't harm
                            }
                            break;
                        case discovery_state::opening_default:
                            {
                                // this isn't possible but doesn't harm
                                // this might be only us!!!
                            }
                            break;
                        case discovery_state::default_active:
                            {
                                out_buffer = get_buffer();

                                rx_discovery_header_t head;
                                memzero(&head, sizeof(head));
                                head.version = std::min(phead->version, (uint32_t)RX_CURRENT_DISCOVERY_VERSION);
                                head.status = (uint32_t)state_;
                                head.size = 0;
                                head.type = DISCOVERY_MASTERLIST;
                                head.known = RX_DISCOVERY_KNOWN_UINT;
                                head.idenity = my_register_->get_identity();
                                out_buffer->push_data(&head, sizeof(head));

                                serialization::std_buffer_writer writer(*out_buffer);
                                bool ret = peer_discovery_algorithms::parse_new_master_when_active(*my_register_
                                    , writer, reader, phead->idenity, ip_addr, phead->version);
                                if (!ret)
                                {
                                    release_buffer(out_buffer);
                                    out_buffer = buffer_ptr::null_ptr;
                                }
                                else
                                {
                                    rx_discovery_header_t* phead = out_buffer->get_buffer<rx_discovery_header_t>();
                                    phead->size = (uint16_t)(out_buffer->get_size());

                                    to_addr = ip_addr;// send it back
                                }
                            }
                            break;
                        case discovery_state::opening_fallback:
                            {

                            }
                            break;
                        case discovery_state::fallback_active:
                            {

                            }
                            break;
                        case discovery_state::stopping:
                            {

                            }
                            break;
                        }
                    }
                    break;
                case DISCOVERY_FALLBACK:
                    {
                        std::scoped_lock _(state_lock_);
                        switch (state_)
                        {
                        case discovery_state::idle:
                            {
                            }
                            break;
                        case discovery_state::opening_default:
                            {

                            }
                            break;
                        case discovery_state::default_active:
                            {
                                out_buffer = get_buffer();

                                rx_discovery_header_t head;
                                memzero(&head, sizeof(head));
                                head.version = RX_CURRENT_DISCOVERY_VERSION;
                                head.status = (uint32_t)state_;
                                head.size = 0;
                                head.type = DISCOVERY_MASTERLIST;
                                head.known = RX_DISCOVERY_KNOWN_UINT;
                                head.idenity = my_register_->get_identity();
                                out_buffer->push_data(&head, sizeof(head));
                                bool broadcast = false;
                                serialization::std_buffer_writer writer(*out_buffer);
                                bool ret = peer_discovery_algorithms::parse_new_fallback_when_active(*my_register_
                                    , writer, reader, phead->idenity, ip_addr, phead->version, broadcast);
                                if (!ret)
                                {
                                    release_buffer(out_buffer);
                                    out_buffer = buffer_ptr::null_ptr;
                                }
                                else
                                {
                                    rx_discovery_header_t* phead = out_buffer->get_buffer<rx_discovery_header_t>();
                                    phead->size = (uint16_t)(out_buffer->get_size());

                                    to_addr = ip_addr;// send it back
                                }
                                if (broadcast)
                                {
                                    auto multi_buff = get_buffer();
                                    memzero(&head, sizeof(head));
                                    head.version = RX_CURRENT_DISCOVERY_VERSION;
                                    head.status = (uint32_t)state_;
                                    head.size = 0;
                                    head.type = DISCOVERY_MASTERLIST;
                                    head.known = RX_DISCOVERY_KNOWN_UINT;
                                    head.idenity = my_register_->get_identity();
                                    multi_buff->push_data(&head, sizeof(head));
                                    serialization::std_buffer_writer multi_writer(*multi_buff);
                                    bool ret = peer_discovery_algorithms::create_master_list_mine(*my_register_
                                        , multi_writer, RX_CURRENT_DISCOVERY_VERSION);
                                    if (!ret)
                                    {
                                        release_buffer(multi_buff);
                                    }
                                    else
                                    {
                                        send_broadcast(multi_buff);
                                    }
                                    next_tick_ = rx_get_tick_count() + rx_border_rand(RX_KEEP_ALIVE_MIN, RX_KEEP_ALIVE_MAX);
                                }
                            }
                            break;
                        case discovery_state::opening_fallback:
                            {

                            }
                            break;
                        case discovery_state::fallback_active:
                            {

                            }
                            break;
                        case discovery_state::stopping:
                            {

                            }
                            break;
                        }
                    }
                    break;
                case DISCOVERY_QUERY:
                    {
                        DISCOVERY_LOG_DEBUG("discovery_point", 200, "Discovery QUERY received from "s + ip_addr.to_string());
                        std::scoped_lock _(state_lock_);
                        switch (state_)
                        {
                        case discovery_state::idle:
                            {
                            }
                            break;
                        case discovery_state::opening_default:
                            {

                            }
                            break;
                        case discovery_state::default_active:
                            {
                                out_buffer = get_buffer();

                                rx_discovery_header_t head;
                                memzero(&head, sizeof(head));
                                head.version = RX_CURRENT_DISCOVERY_VERSION;
                                head.status = (uint32_t)state_;
                                head.size = 0;
                                head.known = RX_DISCOVERY_KNOWN_UINT;
                                head.type = DISCOVERY_MASTERLIST;
                                head.idenity = my_register_->get_identity();
                                out_buffer->push_data(&head, sizeof(head));

                                serialization::std_buffer_writer writer(*out_buffer);
                                bool ret = peer_discovery_algorithms::create_master_list_full(*my_register_
                                    , writer, phead->version);
                                if (!ret)
                                {
                                    release_buffer(out_buffer);
                                    out_buffer = buffer_ptr::null_ptr;
                                }
                                else
                                {
                                    rx_discovery_header_t* phead = out_buffer->get_buffer<rx_discovery_header_t>();
                                    phead->size = (uint16_t)(out_buffer->get_size());

                                    to_addr = ip_addr;// send it back
                                }
                            }
                            break;
                        case discovery_state::opening_fallback:
                            {
                            }
                            break;
                        case discovery_state::fallback_active:
                            {

                            }
                            break;
                        case discovery_state::stopping:
                            {

                            }
                            break;
                        }
                    }
                    break;
                case DISCOVERY_MASTERLIST:
                    {
                        std::scoped_lock _(state_lock_);
                        switch (state_)
                        {
                        case discovery_state::idle:
                            {
                            }
                            break;
                        case discovery_state::opening_default:
                        case discovery_state::default_active:
                            {
                                bool ret = peer_discovery_algorithms::parse_master_list_when_active(*my_register_
                                    , reader, phead->idenity, ip_addr, phead->version);
                            }
                            break;
                        case discovery_state::opening_fallback:
                        case discovery_state::fallback_active:
                            {
                                bool ret = peer_discovery_algorithms::parse_master_list_when_inactive(*my_register_
                                    , reader, phead->idenity, ip_addr, phead->version);
                            }
                            break;
                        case discovery_state::stopping:
                            {

                            }
                            break;
                        }
                    }
                    break;
                default:
                    RX_ASSERT(false);// this shouldn't happened (look on the if condition above)
                }
            }
            else
            {
                // don't know what?!?
            }
        }


        if (out_buffer)
        {
            rx_discovery_header_t* head = (rx_discovery_header_t*)data;
            head->size = (uint16_t)out_buffer->get_size();

            if (bind)
            {
                io::ip4_address bind_addr(bind);
                auto it = sockets_.find(bind_addr);
                if (it != sockets_.end())
                {
                    bool ret = it->second.socket->write(out_buffer, to_addr.get_address(), sizeof(sockaddr_in));
                    if (!ret)
                    {
                        release_buffer(out_buffer);
                    }
                }
            }
            else
            {
                bool ret = local_socket_ptr_->write(out_buffer, to_addr.get_address(), sizeof(sockaddr_in));
                if (!ret)
                {
                    release_buffer(out_buffer);
                }
            }
        }

    }
    return true;
}

void discovery_point::release_buffer (buffer_ptr what)
{
    std::scoped_lock _(free_buffers_lock_);
    what->reinit();
    free_buffers_.push(what);
}

void discovery_point::activate ()
{
    if (!multicast_address_.is_null() && !multicast_address_.is_empty_ip4())
        multicast_arg_ = multicast_address_.get_ip4_address();
    local_socket_ptr_ = rx_create_reference<socket_holder_t>(this, io::ip4_address());
}

void discovery_point::deactivate ()
{
    std::scoped_lock _(state_lock_);
    switch (state_)
    {
    case discovery_state::default_active:
        {
            buffer_ptr buffer = get_buffer();

            rx_discovery_header_t head;
            memzero(&head, sizeof(head));
            head.version = RX_CURRENT_DISCOVERY_VERSION;
            head.status = (uint32_t)state_;
            head.known = RX_DISCOVERY_KNOWN_UINT;
            head.size = 0;
            head.type = DISCOVERY_NEW_MASTER;
            head.idenity = my_register_->get_identity();
            buffer->push_data(&head, sizeof(head));
            rx::serialization::std_buffer_writer writer(*buffer);

            bool ret = peer_discovery_algorithms::create_unregister(*my_register_, writer, RX_CURRENT_DISCOVERY_VERSION);
            if (ret)
            {
                rx_discovery_header_t* phead = buffer->get_buffer<rx_discovery_header_t>();
                phead->size = (uint16_t)(buffer->get_size());

                ret = send_broadcast(buffer);
                if (ret)
                    send_broadcast(buffer_ptr());
            }
        }
        break;

    case discovery_state::fallback_active:
        {

            buffer_ptr buffer = get_buffer();

            rx_discovery_header_t head;
            memzero(&head, sizeof(head));
            head.version = RX_CURRENT_DISCOVERY_VERSION;
            head.status = (uint32_t)state_;
            head.known = RX_DISCOVERY_KNOWN_UINT;
            head.size = 0;
            head.type = DISCOVERY_FALLBACK;
            head.idenity = my_register_->get_identity();
            buffer->push_data(&head, sizeof(head));
            rx::serialization::std_buffer_writer writer(*buffer);

            bool ret = peer_discovery_algorithms::create_unregister(*my_register_, writer, RX_CURRENT_DISCOVERY_VERSION);
            if (ret)
            {
                rx_discovery_header_t* phead = buffer->get_buffer<rx_discovery_header_t>();
                phead->size = (uint16_t)(buffer->get_size());

                ret = send_broadcast(buffer);
                if (ret)
                    send_broadcast(buffer_ptr());
            }
        }
        break;
    }
    state_ = discovery_state::stopping;
}

void discovery_point::timer_tick (rx_timer_ticks_t ticks)
{
    if (ticks > next_tick_)
    {
        next_tick_ = (rx_timer_ticks_t)(-1ll);
        std::scoped_lock _(state_lock_);
        switch (state_)
        {
        case discovery_state::idle:
            {
                DISCOVERY_LOG_INFO("discovery_point", 200, "Discovery processing idle state.");

                rx_result result;
                if (current_port_ == 0)
                {
                    io::ip4_address local_addr(DISC_LOCAL_SOCKET_ADDR, default_port_);
                    auto result = local_socket_ptr_->bind_socket_udpip_4(
                        local_addr.get_ip4_address(), multicast_arg_
                        , infrastructure::server_runtime::instance().get_io_pool()->get_pool());
                    if (result)
                    {
                        DISCOVERY_LOG_INFO("discovery_point", 200, "UDP port opened at "s
                            + local_addr.to_string() + ", sending query...");

                        open_sockets(DISC_SOCKETS_PORT);

                        state_ = discovery_state::opening_default;
                        current_port_ = default_port_;
                        system_port_ = current_port_;
                        port_changed();
                        my_register_->set_system_port(system_port_);
                        next_tick_ = ticks + rx_border_rand(RX_ACTIVE_TIMEOUT_MIN, RX_ACTIVE_TIMEOUT_MAX);

                        buffer_ptr buffer = get_buffer();

                        rx_discovery_header_t head;
                        memzero(&head, sizeof(head));
                        head.version = RX_CURRENT_DISCOVERY_VERSION;
                        head.status = (uint32_t)state_;
                        head.known = RX_DISCOVERY_KNOWN_UINT;
                        head.size = 0;
                        head.type = DISCOVERY_NEW_MASTER;
                        head.idenity = my_register_->get_identity();
                        buffer->push_data(&head, sizeof(head));
                        rx::serialization::std_buffer_writer writer(*buffer);

                        bool ret = peer_discovery_algorithms::create_dafult_query(*my_register_, writer);
                        if (ret)
                        {
                            rx_discovery_header_t* phead = buffer->get_buffer<rx_discovery_header_t>();
                            phead->size = (uint16_t)(buffer->get_size());

                            ret = send_broadcast(buffer);

                        }
                    }
                    else
                    {
                        DISCOVERY_LOG_ERROR("discovery_point", 200, "Error opening UDP port at "s
                            + local_addr.to_string() + ", " + result.errors_line());
                        next_tick_ = ticks;// this is next scan
                        current_port_ = default_port_;
                        current_port_ += 3;
                        local_socket_ptr_->close();
                        local_socket_ptr_ = rx_create_reference<socket_holder_t>(this, io::ip4_address());
                        close_sockets();
                    }
                }
                else
                {
                    io::ip4_address next_addr("127.0.0.1", current_port_);
                    result = local_socket_ptr_->bind_socket_udpip_4(next_addr.get_ip4_address()
                        , nullptr
                        , infrastructure::server_runtime::instance().get_io_pool()->get_pool());
                    if (result)
                    {
                        DISCOVERY_LOG_INFO("discovery_point", 200, "UDP port opened at "s
                            + next_addr.to_string() + ", sending query...");

                        state_ = discovery_state::opening_fallback;
                        next_tick_ = ticks + rx_border_rand(RX_FALLBACK_TIMEOUT_MIN, RX_FALLBACK_TIMEOUT_MAX);
                        system_port_ = current_port_;
                        port_changed();
                        my_register_->set_system_port(system_port_);


                        buffer_ptr buffer = get_buffer();

                        rx_discovery_header_t head;
                        memzero(&head, sizeof(head));
                        head.version = RX_CURRENT_DISCOVERY_VERSION;
                        head.status = (uint32_t)state_;
                        head.size = 0;
                        head.known = RX_DISCOVERY_KNOWN_UINT;
                        head.type = DISCOVERY_FALLBACK;
                        head.idenity = my_register_->get_identity();
                        buffer->push_data(&head, sizeof(head));
                        rx::serialization::std_buffer_writer writer(*buffer);

                        bool ret = peer_discovery_algorithms::create_fallback_register(*my_register_, writer);
                        if (ret)
                        {
                            rx_discovery_header_t* phead = buffer->get_buffer<rx_discovery_header_t>();
                            phead->size = (uint16_t)(buffer->get_size());

                            io::ip4_address addr("127.0.0.1", default_port_);
                            bool ret = local_socket_ptr_->write(buffer, addr.get_address(), sizeof(sockaddr_in));
                        }
                        if (!ret)
                            release_buffer(buffer);
                    }
                    else
                    {

                        DISCOVERY_LOG_ERROR("discovery_point", 200, "Error opening UDP port at "s
                            + local_socket_ptr_->bind_address.to_string() + ", " + result.errors_line());

                        next_tick_ = ticks;// this is next scan
                        current_port_ += 3;
                        local_socket_ptr_->close();
                        local_socket_ptr_ = rx_create_reference<socket_holder_t>(this, io::ip4_address());
                        close_sockets();
                    }
                }
            }
            break;
        case discovery_state::opening_default:
            {
                //timeout expired so, maybe no other masters?
                // do nothing for now
                DISCOVERY_LOG_INFO("discovery_point", 200, "Discovery processing opening_default state.");
                state_ = discovery_state::default_active;
                next_tick_ = ticks + rx_border_rand(RX_KEEP_ALIVE_MIN, RX_KEEP_ALIVE_MIN);
            }
            break;
        case discovery_state::default_active:
            {
                if (peer_discovery_algorithms::do_time_checks_master(*my_register_, ticks, RX_KEEP_ALIVE_TIMEOUT))
                {
                    buffer_ptr buffer = get_buffer();

                    rx_discovery_header_t head;
                    memzero(&head, sizeof(head));
                    head.version = RX_CURRENT_DISCOVERY_VERSION;
                    head.status = (uint32_t)state_;
                    head.known = RX_DISCOVERY_KNOWN_UINT;
                    head.size = 0;
                    head.type = DISCOVERY_MASTERLIST;
                    head.idenity = my_register_->get_identity();
                    buffer->push_data(&head, sizeof(head));
                    rx::serialization::std_buffer_writer writer(*buffer);

                    bool ret = peer_discovery_algorithms::create_master_list_mine(*my_register_, writer, RX_CURRENT_DISCOVERY_VERSION);
                    if (ret)
                    {
                        rx_discovery_header_t* phead = buffer->get_buffer<rx_discovery_header_t>();
                        phead->size = (uint16_t)(buffer->get_size());

                        ret = send_broadcast(buffer);
                    }
                }
                next_tick_ = ticks + rx_border_rand(RX_KEEP_ALIVE_MIN, RX_KEEP_ALIVE_MAX);
            }
            break;
        case discovery_state::opening_fallback:
            {
                DISCOVERY_LOG_INFO("discovery_point", 200, "Discovery processing opening_fallback state.");
                //timeout expired so, maybe no master try everything again?
                state_ = discovery_state::idle;
                next_tick_ = ticks;// this is next scan
                current_port_ = 0;
                local_socket_ptr_->close();
                local_socket_ptr_ = rx_create_reference<socket_holder_t>(this, io::ip4_address());
                close_sockets();

            }
            break;
        case discovery_state::fallback_active:
            {
                if (peer_discovery_algorithms::do_time_checks_follower(*my_register_, ticks, RX_KEEP_ALIVE_TIMEOUT))
                {
                    buffer_ptr buffer = get_buffer();

                    rx_discovery_header_t head;
                    memzero(&head, sizeof(head));
                    head.version = RX_CURRENT_DISCOVERY_VERSION;
                    head.status = (uint32_t)state_;
                    head.known = RX_DISCOVERY_KNOWN_UINT;
                    head.size = 0;
                    head.type = DISCOVERY_FALLBACK;
                    head.idenity = my_register_->get_identity();
                    buffer->push_data(&head, sizeof(head));
                    rx::serialization::std_buffer_writer writer(*buffer);

                    bool ret = peer_discovery_algorithms::create_fallback_register(*my_register_, writer);
                    if (ret)
                    {
                        rx_discovery_header_t* phead = buffer->get_buffer<rx_discovery_header_t>();
                        phead->size = (uint16_t)(buffer->get_size());

                        io::ip4_address addr("127.0.0.1", default_port_);
                        bool ret = local_socket_ptr_->write(buffer, addr.get_address(), sizeof(sockaddr_in));
                    }
                    if (!ret)
                        release_buffer(buffer);

                    next_tick_ = ticks + rx_border_rand(RX_KEEP_ALIVE_MIN, RX_KEEP_ALIVE_MIN);
                }
                else
                {
                    // we lost master so try to become one
                    state_ = discovery_state::idle;
                    current_port_ = 0;
                    local_socket_ptr_->close();
                    local_socket_ptr_ = rx_create_reference<socket_holder_t>(this, io::ip4_address());
                    close_sockets();
                    next_tick_ = ticks;
                }
            }
            break;
        case discovery_state::stopping:
            {
                DISCOVERY_LOG_INFO("discovery_point", 200, "Discovery processing stopping state.");

            }
            break;
        }
    }
}

buffer_ptr discovery_point::get_buffer ()
{
    {
        std::scoped_lock _(free_buffers_lock_);
        if (!free_buffers_.empty())
        {
            buffer_ptr ret = free_buffers_.top();
            free_buffers_.pop();
            return ret;
        }
    }
    return rx_create_reference<buffer_ptr::pointee_type>();
}

uint32_t discovery_point::subscribe_to_port (std::function<void(uint16_t)> callback, rx_reference_ptr anchor)
{
    auto thr = rx_thread_context();
    std::scoped_lock _(state_lock_);
    uint32_t new_id = last_id_++;
    auto it_thr = port_subscribers_.find(thr);
    if (it_thr != port_subscribers_.end())
    {
        it_thr->second.emplace(new_id, port_subscriber_data{ anchor, callback });
    }
    else
    {
        port_subscribers_type::mapped_type temp;
        temp.emplace(new_id, port_subscriber_data{ anchor, callback });
        port_subscribers_.emplace(thr, std::move(temp));
    }
    rx_post_function_to(thr, anchor, std::move(callback), system_port_);
    return new_id;
}

void discovery_point::unsubscribe_from_port (uint32_t id)
{
    {
        std::scoped_lock _(state_lock_);
        for (auto& one : port_subscribers_)
        {
            if (one.second.erase(id))
                return;
        }
    }
    RX_ASSERT(false);
}

void discovery_point::port_changed ()
{
    for (auto& one : port_subscribers_)
    {
        for (auto& subs : one.second)
        {
            rx_post_function_to(one.first, subs.second.anchor, subs.second.callback, system_port_);
        }
    }
}

bool discovery_point::send_broadcast (buffer_ptr buffer)
{
    if (buffer)
    {
        buffer_ptr next_buffer;
        for (const auto& one : registered_followers_)
        {
            buffer_ptr next_buffer = get_buffer();
            next_buffer->push_data(buffer->get_buffer<void>(), buffer->get_size());
            auto ret = local_socket_ptr_->write(buffer, one.second.binded.get_address(), sizeof(sockaddr_in));
            if (!ret)
            {
                release_buffer(buffer);
                release_buffer(next_buffer);
                return false;
            }
            buffer = next_buffer;
        }
        for (auto& one : sockets_)
        {
            buffer_ptr next_buffer = get_buffer();
            next_buffer->push_data(buffer->get_buffer<void>(), buffer->get_size());
            auto ret = one.second.socket->write(buffer, multicast_address_.get_address(), sizeof(sockaddr_in));
            if (!ret)
            {
                release_buffer(buffer);
                release_buffer(next_buffer);
                return false;
            }
            buffer = next_buffer;
        }
        return true;
    }
    else
    {
        for (const auto& one : registered_followers_)
        {
            auto ret = local_socket_ptr_->write(buffer, one.second.binded.get_address(), sizeof(sockaddr_in));
            if (!ret)
            {
                return false;
            }
        }
        for (auto& one : sockets_)
        {
            auto ret = one.second.socket->write(buffer, multicast_address_.get_address(), sizeof(sockaddr_in));
            if (!ret)
            {
                return false;
            }
        }
        return true;
    }
}

void discovery_point::close_sockets ()
{
    for (auto& one : sockets_)
    {
        one.second.socket->close();
        one.second.socket = socket_ptr_type::null_ptr;
    }
    sockets_.clear();
}

rx_result discovery_point::open_sockets (uint16_t port)
{
    close_sockets();
    auto addrs = my_register_->get_own_addresses(port);
    for (const auto& addr : addrs)
    {
        socket_struct_t temp;
        temp.bind_address = addr;
        temp.socket = rx_create_reference<socket_holder_t>(this, temp.bind_address);
        auto result = temp.socket->bind_socket_udpip_4(temp.bind_address.get_ip4_address(), multicast_arg_
            , infrastructure::server_runtime::instance().get_io_pool()->get_pool());
        if (result)
        {
            sockets_.emplace(temp.bind_address, temp);
            DISCOVERY_LOG_INFO("discovery_point", 200, "UDP port opened at "s
                + temp.bind_address.to_string() + ", sending query...");
        }
        else
        {
            temp.socket->close();
            DISCOVERY_LOG_ERROR("discovery_point", 200, "Error opening UDP port at "s
                + temp.bind_address.to_string() + ", " + result.errors_line());
        }
    }
    return true;
}


// Class rx_internal::discovery::discovery_register 

discovery_register::discovery_register()
      : identity_(rx_uuid::create_new()),
        system_port_(0)
{
}


discovery_register::~discovery_register()
{
}



void discovery_register::set_system_port (uint16_t port)
{
    system_port_ = port;
    peer_discovery_algorithms::recreate_own_data(*this);
}

bool discovery_register::is_this_you (const rx_uuid_t& id)
{
    return identity_ == rx_uuid(id);
}

const rx_uuid_t& discovery_register::get_identity ()
{
    return identity_;
}

std::vector<io::ip4_address> discovery_register::get_own_addresses (uint16_t port) const
{
    std::vector<io::ip4_address> ret;
    ret.reserve(system_addresses_.size());
    for(const auto& one : system_addresses_)
    {
        ret.push_back(io::ip4_address(one.address, port));
    }
    return ret;
}


// Class rx_internal::discovery::discovered_peer_data 

bool discovered_peer_data::operator==(const discovered_peer_data &right) const
{
    if (id != right.id)
        return false;
    if (instance != right.instance)
        return false;
    if (node != right.node)
        return false;
    return addresses != right.addresses;
}

bool discovered_peer_data::operator!=(const discovered_peer_data &right) const
{
    return !operator==(right);
}



bool discovered_peer_data::serialize (base_meta_writer& writer, const string_type& name, uint32_t version) const
{
    if (!writer.start_object(name.c_str()))
        return false;
    if (!writer.write_uuid("id", id))
        return false;
    if (!writer.write_string("instance", instance.c_str()))
        return false;
    if (!writer.write_string("node", node.c_str()))
        return false;
    if (!writer.start_array("addresses", addresses.size()))
        return false;
    for (const auto& one : addresses)
    {
        if (!writer.start_object("addr"))
            return false;
        if (!writer.write_string("address", one.address.c_str()))
            return false;
        if (!writer.write_string("net", one.network.c_str()))
            return false;
        if (!writer.end_object())
            return false;
    }
    if (!writer.end_array())
        return false;
    if (!writer.write_uint("ver", version))
        return false;
    if (!writer.end_object())
        return false;
    return true;
}

bool discovered_peer_data::deserialize (base_meta_reader& reader, const string_type& name, uint32_t version)
{
    if (!reader.start_object(name.c_str()))
        return false;
    if (!reader.read_uuid("id", id))
        return false;
    if (!reader.read_string("instance", instance))
        return false;
    if (!reader.read_string("node", node))
        return false;
    if (!reader.start_array("addresses"))
        return false;
    addresses.clear();
    while (!reader.array_end())
    {
        discovered_network_point temp;
        if (!reader.start_object("addr"))
            return false;
        if (!reader.read_string("address", temp.address))
            return false;
        if (!reader.read_string("net", temp.network))
            return false;
        if (!reader.end_object())
            return false;
        addresses.push_back(std::move(temp));
    }
    if (!reader.read_uint("ver", version))
        return false;

    if (!reader.end_object())
        return false;

    return true;
}


} // namespace discovery
} // namespace rx_internal

