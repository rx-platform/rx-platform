

/****************************************************************************
*
*  lib\rx_io.cpp
*
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_io
#include "lib/rx_io.h"



namespace rx {

namespace io {
int dispatcher_accept_callback(void* data, uint32_t status, sys_handle_t handle, struct sockaddr* addr, struct sockaddr* local_addr, size_t size)
{
	dispatcher_subscriber* whose = (dispatcher_subscriber*)data;
	int ret = whose->_internal_accept_callback(handle, (sockaddr_in*)addr, (sockaddr_in*)local_addr, status);
	whose->release();
	return ret;
}
int dispatcher_shutdown_callback(void* data, uint32_t status)
{
	dispatcher_subscriber* whose = (dispatcher_subscriber*)data;
	int ret = whose->_internal_shutdown_callback(status);
	whose->release();
	return ret;
}

int dispatcher_read_callback(void* data, uint32_t status, size_t size)
{
	dispatcher_subscriber* whose = (dispatcher_subscriber*)data;
	int ret = whose->_internal_read_callback(size, status);
	whose->release();
	return ret;
}
int dispatcher_write_callback(void* data, uint32_t status)
{
	dispatcher_subscriber* whose = (dispatcher_subscriber*)data;
	int ret = whose->_internal_write_callback(status);
	whose->release();
	return ret;
}
int dispatcher_connect_callback(void* data, uint32_t status)
{
	dispatcher_subscriber* whose = (dispatcher_subscriber*)data;
	int ret = whose->_internal_connect_callback(status);
	whose->release();
	return ret;
}

// Class rx::io::dispatcher_subscriber 

time_aware_subscribers_type dispatcher_subscriber::time_aware_subscribers_;

locks::lockable dispatcher_subscriber::time_aware_subscribers_lock_;

dispatcher_subscriber::dispatcher_subscriber (rx_thread_handle_t destination)
      : dispatcher_handle_(0),
        destination_context_(destination)
{
	memzero(&dispatcher_data_, sizeof(dispatcher_data_));

	dispatcher_data_.read_callback = dispatcher_read_callback;
	dispatcher_data_.write_callback = dispatcher_write_callback;
	dispatcher_data_.connect_callback = dispatcher_connect_callback;
	dispatcher_data_.accept_callback = dispatcher_accept_callback;
	dispatcher_data_.shutdown_callback = dispatcher_shutdown_callback;
}


dispatcher_subscriber::~dispatcher_subscriber()
{
}



bool dispatcher_subscriber::connect_dispatcher (threads::dispatcher_pool::smart_ptr& dispatcher)
{
	uint32_t ret = rx_dispatcher_register(dispatcher->dispatcher_, &dispatcher_data_);
	if (ret)
	{
		disptacher_ = dispatcher;
		dispatcher_handle_ = dispatcher->dispatcher_;
	}
	return ret != 0;
}

bool dispatcher_subscriber::disconnect_dispatcher ()
{
	int ret = rx_dispatcher_unregister(dispatcher_handle_, &dispatcher_data_);
	if(ret>0)
	{
        for(int i=0; i<ret; i++)
        {
            release();
        }
	}
	return ret >= 0;
}

void dispatcher_subscriber::register_timed ()
{
	locks::auto_lock dummy(&time_aware_subscribers_lock_);
	time_aware_subscribers_.emplace(smart_this());
}

void dispatcher_subscriber::unregister_timed ()
{
	locks::auto_lock dummy(&time_aware_subscribers_lock_);
	time_aware_subscribers_.erase(smart_this());
}

void dispatcher_subscriber::propagate_timer ()
{
	uint32_t tick = rx_get_tick_count();
	time_aware_subscribers_lock_.lock();
	std::vector<dispatcher_subscriber::smart_ptr> helper;
	helper.reserve(time_aware_subscribers_.size());
	for (auto one : time_aware_subscribers_)
	{
		helper.emplace_back(one);
	}
	time_aware_subscribers_lock_.unlock();
	for (auto one : helper)
		one->timer_tick(tick);
}

void dispatcher_subscriber::timer_tick (uint32_t tick)
{
}

int dispatcher_subscriber::internal_read_callback (size_t count, uint32_t status)
{
	return 0;
}

int dispatcher_subscriber::internal_write_callback (uint32_t status)
{
	return 0;
}

int dispatcher_subscriber::internal_shutdown_callback (uint32_t status)
{
	return 0;
}

int dispatcher_subscriber::_internal_read_callback (size_t count, uint32_t status)
{
	return internal_read_callback(count, status);
}

int dispatcher_subscriber::_internal_write_callback (uint32_t status)
{
	return internal_write_callback(status);
}

int dispatcher_subscriber::_internal_shutdown_callback (uint32_t status)
{
	return internal_shutdown_callback(status);
}

int dispatcher_subscriber::internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status)
{
	return 0;
}

int dispatcher_subscriber::_internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status)
{
	return internal_accept_callback(handle, addr, local_addr, status);
}

int dispatcher_subscriber::internal_connect_callback (uint32_t status)
{
	return 0;
}

int dispatcher_subscriber::_internal_connect_callback (uint32_t status)
{
	return internal_connect_callback(status);
}


} // namespace io
} // namespace rx

