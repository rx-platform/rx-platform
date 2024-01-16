

/****************************************************************************
*
*  interfaces\rx_transaction_limiter.cpp
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


// rx_transaction_limiter
#include "interfaces/rx_transaction_limiter.h"

#include "interfaces/rx_endpoints.h"


namespace rx_internal {

namespace interfaces {

namespace ports_lib {

// Class rx_internal::interfaces::ports_lib::limiter_endpoint 

limiter_endpoint::limiter_endpoint (transaction_limiter_port* port)
      : port_(port),
        next_timer_tick_(0),
        timeouts_({ 0, 0 })
{
    ITF_LOG_DEBUG("limiter_endpoint", 200, "Limiter endpoint created.");
    rx_init_stack_entry(&stack_endpoint_, this);
    /*stack_endpoint_.connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
    {
        limiter_endpoint* self = reinterpret_cast<limiter_endpoint*>(reference->user_data);
        return self->connected(session);
    };*/
    stack_endpoint_.send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
    {
        limiter_endpoint* self = reinterpret_cast<limiter_endpoint*>(reference->user_data);
        return self->send_packet(packet);
    };
    stack_endpoint_.received_function = [](rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
    {
        limiter_endpoint* self = reinterpret_cast<limiter_endpoint*>(reference->user_data);
        return self->received_packet(packet);
    };
    stack_endpoint_.disconnected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
    {
        limiter_endpoint* self = reinterpret_cast<limiter_endpoint*>(reference->user_data);
        return self->disconnected(session, reason);
    };
}


limiter_endpoint::~limiter_endpoint()
{
    ITF_LOG_DEBUG("limiter_endpoint", 200, "Limiter endpoint destroyed.");
    locks::auto_lock_t _(&lock_);
    if (timer_)
        timer_->cancel();
}



rx_protocol_stack_endpoint* limiter_endpoint::bind ()
{
    timer_ = port_->create_timer_function([this]()
        {
            timer_fired();
        });
    return &stack_endpoint_;
}

runtime::items::port_runtime* limiter_endpoint::get_port ()
{
    return port_;
}

rx_protocol_result_t limiter_endpoint::connected (rx_session* session)
{
    auto result =  rx_notify_connected(&stack_endpoint_, session);
    if (result == RX_PROTOCOL_OK)
        port_->status.set_online();
    return result;
}

rx_protocol_result_t limiter_endpoint::send_packet (send_protocol_packet packet)
{
    auto options = port_->get_limit_options();
    if (options.limit > 0 && (!options.ignore_zeros || packet.id != 0)) //!!!! important for the conversion bellow
    {
        locks::auto_lock_t _(&lock_);
        if (next_timer_tick_ == 0)
        {
            while (handle_timer())
                send_next();
        }
        if (pending_count() >= (uint32_t)options.limit)// notify up about the conversion
        {
            auto trans = create_transaction(packet);
            transactions_queue_[packet.priority].emplace(std::move(trans));
            port_->set_queue_size(get_queue_size());

            return RX_PROTOCOL_OK;
        }
        else
        {
            // try send
            auto result = rx_move_packet_down(&stack_endpoint_, packet);
            if (result == RX_PROTOCOL_OK)
            {
                // we do not need the buffer, unless retransmit is needed
                packet.buffer = nullptr;// just ignore it for now
                auto trans = create_transaction(packet);
                trans->transaction_sent();
                place_pending(std::move(trans), (size_t)options.limit);// notify up about the conversion
                while (handle_timer())
                {
                    send_next();
                    port_->status.sent_packet();
                }
            }
            return result;
        }
    }
    else
    {// everything is off!!!
        auto result = rx_move_packet_down(&stack_endpoint_, packet);
        if (result == RX_PROTOCOL_OK)
            port_->status.sent_packet();
        return result;
    }
}

rx_protocol_result_t limiter_endpoint::received_packet (recv_protocol_packet packet)
{
    auto options = port_->get_limit_options();
    removed_transaction_data removed{};
    if (options.limit > 0) //!!!! important for the conversion bellow
    {
        locks::auto_lock_t _(&lock_);
        if (options.use_packet_id)
        {
            if (!options.ignore_zeros || packet.id != 0)
            {
                removed = remove_pending_packet_id(packet.id);
            }
        }
        else
        {
            removed = remove_pending();
        }
    }
    if (!options.use_packet_id)
        packet.id = removed.id;
    auto result = rx_move_packet_up(&stack_endpoint_, packet);
    if (result == RX_PROTOCOL_OK)
    {
        port_->status.received_packet();
        if (removed.found)
        {
            if(removed.write)
                port_->status.write_done(removed.ticks, true);
            else
                port_->status.read_done(removed.ticks, true);

        }
    }
    if (removed.found)
    {
        locks::auto_lock_t _(&lock_);
        do
        {
            send_next();
        } while (handle_timer());
    }
    return result;
}

rx_protocol_result_t limiter_endpoint::disconnected (rx_session* session, rx_protocol_result_t reason)
{
    auto result = rx_notify_disconnected(&stack_endpoint_, session, reason);
    if (result == RX_PROTOCOL_OK)
        port_->status.set_offline();
    return result;
}

void limiter_endpoint::timer_fired ()
{
    auto options = port_->get_limit_options();
    if (options.limit > 0)
    {
        locks::auto_lock_t _(&lock_);
        while (handle_timer())
        {
            send_next();
        }
    }
}

size_t limiter_endpoint::pending_count () const
{
    size_t ret = 0;
    if (!pending_transactions_.empty())
    {
        for (const auto& one : pending_transactions_)
        {
            if (one)
                ret++;// skip empty ones
        }
    }
    return ret;
}

void limiter_endpoint::place_pending (transaction_ptr_t data, size_t new_limit)
{
    RX_ASSERT(new_limit > 0);
    size_t count = 0;
    if (!pending_transactions_.empty())
    {
        for (auto& one : pending_transactions_)
        {
            count++;
            if (!one)
            {
                one = std::move(data);
                return;
            }
        }
    }
    if (count < new_limit)
        pending_transactions_.emplace_back(std::move(data));
    else
        RX_ASSERT(false);// this should not happen
}

limiter_endpoint::removed_transaction_data limiter_endpoint::remove_pending_packet_id (rx_packet_id_type id)
{
    for (auto& one : pending_transactions_)
    {
        if (one && one->packet.id == id)
        {
            auto ticks = rx_get_us_ticks() - one->arrived_tick;
            bool is_write = one->is_write();
            auto id = one->packet.id;
            release_transaction(std::move(one));
            return { ticks, is_write, true, id };
        }
    }
    return {};
}

limiter_endpoint::removed_transaction_data limiter_endpoint::remove_pending ()
{
    for (auto& one : pending_transactions_)
    {
        if (one)
        {
            auto ticks = rx_get_us_ticks() - one->arrived_tick;
            bool is_write = one->is_write();
            auto id = one->packet.id;
            release_transaction(std::move(one));
            return { ticks, is_write, true, id };
        }
    }
    return {};
}

void limiter_endpoint::send_next ()
{
    uint64_t ticks = rx_get_us_ticks();
    read_timeouts();
    auto options = port_->get_limit_options();
    RX_ASSERT(options.limit > 0);
    auto pending = pending_count();
    for (auto it = transactions_queue_.rbegin(); it!=transactions_queue_.rend(); it++)
    {
        if (pending >= (uint32_t)options.limit)
            break;
        while (!it->second.empty())
        {
            if (pending >= (uint32_t)options.limit)
                break;
            transaction_ptr_t& data = it->second.front();
            uint64_t next_data_tick = ticks + (data->is_write() ? timeouts_.write : timeouts_.read);
            if (next_data_tick > ticks)// not timeouted?!?
            {
                std::ostringstream ss;
                rx_protocol_result_t result = rx_move_packet_down(&stack_endpoint_, data->packet);
                if (result == RX_PROTOCOL_OK
                    && options.limit > 0)
                {
                    data->transaction_sent();
                    place_pending(std::move(it->second.front()), options.limit);
                    pending++;
                    // see about the timeouts now
                    if (next_timer_tick_ == 0 || next_timer_tick_ > next_data_tick)
                    {
                        next_timer_tick_ = next_data_tick;
                        uint32_t period = (uint32_t)((next_timer_tick_ - ticks) / 1000 + 1);
                        timer_->start(period);
                    }
                }
            }
            else
            {
                if (data->is_write())
                    port_->status.write_done(ticks - data->arrived_tick, false);
                else
                    port_->status.read_done(ticks - data->arrived_tick, false);
                release_transaction(std::move(data));
            }
            it->second.pop();
            port_->set_queue_size(get_queue_size());
        }
    }
}

int limiter_endpoint::handle_timer ()
{
    const uint64_t initial_tick = (uint64_t)(-1);
    int removed = 0;
    int active = 0;
    uint64_t now_tick = rx_get_us_ticks();
    read_timeouts();
    uint64_t next_tick = initial_tick;

    for (auto& one : pending_transactions_)
    {
        if (one)
        {
            active++;
            RX_ASSERT(one->sent_tick != 0);
            if (one->sent_tick)
            {
                uint64_t one_timeout_tick = one->sent_tick + (one->is_write() ? timeouts_.write : timeouts_.read);
                if (one_timeout_tick < now_tick)
                {// transaction timeouted

                    if (one->is_write())
                        port_->status.write_done(now_tick - one->arrived_tick, false);
                    else
                        port_->status.read_done(now_tick - one->arrived_tick, false);
                    release_transaction(std::move(one));
                    removed++;
                }
                else
                {
                    if (one_timeout_tick < next_tick)
                        next_tick = one_timeout_tick;
                }
            }
        }
    }
    if (active == 0)
    {// nothing pending suspend timer
        next_timer_tick_ = 0;
        timer_->suspend();
    }
    else if (next_tick != initial_tick)
    {

        if (now_tick > next_tick)
        {
            next_timer_tick_ = 0;
            timer_->suspend();
            return removed + handle_timer();
        }
        else
        {
            if (next_timer_tick_ == 0 || next_tick < next_timer_tick_)
            {
                next_timer_tick_ = next_tick;
                uint32_t period = (uint32_t)((next_tick - now_tick) / 1000 + 1);
                timer_->start(period);
            }
        }
    }
    else
    {
        RX_ASSERT(next_timer_tick_!=0);
    }
    return removed;
}

int limiter_endpoint::get_queue_size ()
{
    int ret = 0;
    for (const auto& one_queue : transactions_queue_)
    {
        ret += ((int)one_queue.second.size());
    }
    return ret;
}

void limiter_endpoint::read_timeouts ()
{
    timeouts_.read = port_->get_read_timeout() * 1000ull;
    timeouts_.write = port_->get_write_timeout() * 1000ull;
}

limiter_endpoint::transaction_ptr_t limiter_endpoint::create_transaction (send_protocol_packet packet)
{
    locks::auto_lock_t _(&lock_);
    if (!empty_transactions_.empty())
    {
        limiter_endpoint::transaction_ptr_t ret = std::move(empty_transactions_.back());
        empty_transactions_.pop_back();
        ret->reinit(packet);
        return ret;
    }
    else
    {
        return std::make_unique<limiter_transaction_data>(packet, port_);
    }
}

void limiter_endpoint::release_transaction (transaction_ptr_t trans)
{
    trans->reinit();
    locks::auto_lock_t _(&lock_);
    empty_transactions_.emplace_back(std::move(trans));
}


// Class rx_internal::interfaces::ports_lib::limiter_transaction_data 

limiter_transaction_data::limiter_transaction_data()
      : sent_tick(0)
    , arrived_tick(0)
{
    memzero(&packet, sizeof(packet));
    packet.buffer = &buffer;
    status[has_buffer] = true;
    rx_reinit_packet_buffer(&buffer);
}

limiter_transaction_data::limiter_transaction_data (const send_protocol_packet& to_send, transaction_limiter_port* port)
      : sent_tick(0)
    , arrived_tick(rx_get_us_ticks())
    , packet(to_send)
    , from(to_send.from_addr)
    , to(to_send.to_addr)
{
    if (to_send.buffer)
    {
        auto result = port->alloc_io_buffer();
        if (result)
        {
            buffer = result.move_value();
            if (to_send.buffer->size > 0)
                buffer.write(to_send.buffer->buffer_ptr, to_send.buffer->size);
            packet.buffer = &buffer;
            status[has_buffer] = true;
        }
    }
    if (packet.from_addr)
        packet.from_addr = &from;
    if (packet.to_addr)
        packet.to_addr = &to;
    status[active_transaction] = true;
}



void limiter_transaction_data::transaction_sent ()
{
    RX_ASSERT(!status[packet_sent] && status[active_transaction]);
    status[packet_sent] = true;
    status[active_transaction] = true;
    sent_tick = rx_get_us_ticks();
}

void limiter_transaction_data::reinit (const send_protocol_packet& to_send)
{
    arrived_tick = rx_get_us_ticks();
    packet = to_send;
    from = to_send.from_addr;
    to = to_send.to_addr;
    status[packet_sent] = false;
    status[has_buffer] = false;
    if (to_send.buffer)
    {
        if(to_send.buffer->size > 0)
            buffer.write(to_send.buffer->buffer_ptr, to_send.buffer->size);
        packet.buffer = &buffer;
        status[has_buffer] = true;
    }
    if (packet.from_addr)
        packet.from_addr = &from;
    if (packet.to_addr)
        packet.to_addr = &to;
    status[active_transaction] = true;
}

void limiter_transaction_data::reinit ()
{
    status[active_transaction] = false;
    status[packet_sent] = false;
    memzero(&packet, sizeof(packet));
    rx_reinit_packet_buffer(&buffer);
}

bool limiter_transaction_data::is_write () const
{
    return (packet.priority & 0x80) != 0;
}


// Class rx_internal::interfaces::ports_lib::transaction_limiter_port 

transaction_limiter_port::transaction_limiter_port()
      : limit_(1),
        ignore_zeros_(false),
        use_packet_id_(true),
        read_timeout_(200),
        write_timeout_(200),
        queue_size_(0)
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<limiter_endpoint>(this);
        auto entry = rt->bind();
        return construct_func_type::result_type{ entry, std::move(rt) };
    };
}



rx_result transaction_limiter_port::initialize_runtime (runtime_init_context& ctx)
{
    rx_result result = status.initialize(ctx);

    auto one_result = limit_.bind("Options.Limit", ctx);
    if (!one_result)
        result.register_error("Error connecting Options.Limit:"s + one_result.errors_line());
    one_result = ignore_zeros_.bind("Options.IgnoreZeros", ctx);
    if (!one_result)
        result.register_error("Error connecting Options.IgnoreZeros:"s + one_result.errors_line());
    one_result = use_packet_id_.bind("Options.UsePacketId", ctx);
    if (!one_result)
        result.register_error("Error connecting Options.UsePacketId:"s + one_result.errors_line());
    one_result = queue_size_.bind("Options.QueueSize", ctx);
    if (!one_result)
        result.register_error("Error connecting Options.QueueSize:"s + one_result.errors_line());

    one_result = read_timeout_.bind("Timeouts.ReadTimeout", ctx);
    if (!one_result)
        result.register_error("Error connecting Timeouts.ReadTimeout:"s + one_result.errors_line());
    one_result = write_timeout_.bind("Timeouts.WriteTimeout", ctx);
    if (!one_result)
        result.register_error("Error connecting Timeouts.WriteTimeout:"s + one_result.errors_line());

    if (!result)
    {
        ITF_LOG_WARNING("transaction_limiter_port", 200, "Error initializing "s
            + ctx.context->meta_info.get_full_path() + " status:"s + result.errors_line());
    }

    return true;
}

void transaction_limiter_port::set_queue_size (int size)
{
    queue_size_ = size;
}

limit_options_t transaction_limiter_port::get_limit_options () const
{
    limit_options_t ret;
    ret.ignore_zeros = ignore_zeros_;
    ret.use_packet_id = use_packet_id_;
    ret.limit = limit_;
    if (ret.limit > 1 && !ret.use_packet_id)
        ret.limit = 1;
    return ret;
}

uint32_t transaction_limiter_port::get_read_timeout () const
{
    return read_timeout_;
}

uint32_t transaction_limiter_port::get_write_timeout () const
{
    return write_timeout_;
}


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal

