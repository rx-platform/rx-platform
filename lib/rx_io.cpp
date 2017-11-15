

/****************************************************************************
*
*  lib\rx_io.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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


#include "stdafx.h"


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

time_aware_subscribers_type dispatcher_subscriber::_time_aware_subscribers;

locks::lockable dispatcher_subscriber::_time_aware_subscribers_lock;

dispatcher_subscriber::dispatcher_subscriber (rx_thread_handle_t destination)
      : _dispatcher_handle(0),
        _destination_context(destination)
{
	memzero(&_dispatcher_data, sizeof(_dispatcher_data));

	_dispatcher_data.read_callback = dispatcher_read_callback;
	_dispatcher_data.write_callback = dispatcher_write_callback;
	_dispatcher_data.connect_callback = dispatcher_connect_callback;
	_dispatcher_data.accept_callback = dispatcher_accept_callback;
	_dispatcher_data.shutdown_callback = dispatcher_shutdown_callback;
}


dispatcher_subscriber::~dispatcher_subscriber()
{
}



bool dispatcher_subscriber::connect_dispatcher (threads::dispatcher_pool::smart_ptr& dispatcher)
{
	uint32_t ret = rx_dispatcher_register(dispatcher->_dispatcher, &_dispatcher_data);
	if (ret)
	{
		_disptacher = dispatcher;
		_dispatcher_handle = dispatcher->_dispatcher;
	}
	return ret != 0;
}

bool dispatcher_subscriber::disconnect_dispatcher ()
{
	int ret = rx_dispatcher_unregister(_dispatcher_handle, &_dispatcher_data);
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
	locks::auto_lock dummy(&_time_aware_subscribers_lock);
	_time_aware_subscribers.emplace(smart_this());
}

void dispatcher_subscriber::unregister_timed ()
{
	locks::auto_lock dummy(&_time_aware_subscribers_lock);
	_time_aware_subscribers.erase(smart_this());
}

void dispatcher_subscriber::propagate_timer ()
{
	uint32_t tick = rx_get_tick_count();
	_time_aware_subscribers_lock.lock();
	std::vector<dispatcher_subscriber::smart_ptr> helper;
	helper.reserve(_time_aware_subscribers.size());
	for (auto one : _time_aware_subscribers)
	{
		helper.emplace_back(one);
	}
	_time_aware_subscribers_lock.unlock();
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

