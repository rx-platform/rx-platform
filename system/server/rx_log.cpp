

/****************************************************************************
*
*  system\server\rx_log.cpp
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

#define STACK_LOG_SIZE 0x400
#define CACHE_LOG_NAME "last"

#include "system/server/rx_server.h"

// rx_log
#include "system/server/rx_log.h"



namespace rx_platform {

namespace log {
const char* event_type_to_string(log_event_type type)
{
	switch (type)
	{
	case log_event_type::info:
		return "INFO";
	case log_event_type::warning:
		return "WARNING";
	case log_event_type::error:
		return "ERROR";
	case log_event_type::critical:
		return "CRITICAL";
	case log_event_type::debug:
		return "DEBUG";
	case log_event_type::trace:
		return "TRACE";
	default:
		return "***UNKNOWN***";
	}
}


void log_event_data::dump_to_stream(std::ostream& stream) const
{
	char buff[STACK_LOG_SIZE];

	int ret = snprintf(buff, STACK_LOG_SIZE, "%s %s@%s %s:%s\r\n%s",
		when.get_string().c_str(),
		event_type_to_string(event_type),
		library.c_str(),
		source.c_str(),
		message.c_str(),
		code.c_str());
	if (ret < STACK_LOG_SIZE)
	{
		stream.write(buff, ret);
	}
	else
	{
		RX_ASSERT(false);
	}
}

const char* get_log_type_string(log::log_event_type type)
{
	switch (type)
	{
	case log_event_type::info:
		return " INFO     ";
	case log_event_type::warning:
		return " WARNING  ";
	case log_event_type::error:
		return " ERROR    ";
	case log_event_type::critical:
		return " CRITICAL ";
	case log_event_type::debug:
		return " DEBUG    ";
	case log_event_type::trace:
		return " TRACE    ";
	default:
		return "***UNKNOWN***";
	}
}

void log_event_data::dump_to_stream_simple(std::ostream& stream) const
{
	stream << when.get_string(false)
		<< get_log_type_string(event_type)
		<< message
		<< "\r\n";
}
bool log_event_data::is_included(log_query_type query) const
{
	switch (query.type & rx_log_level_mask)
	{
	case rx_log_debug_level:
		break;// pass all
	case rx_log_trace_level:
		if (event_type < log_event_type::trace)
			return false;
		break;
	case rx_log_normal_level:
		if (event_type < log_event_type::info)
			return false;
		break;
	case rx_log_warining_level:
		if (event_type < log_event_type::warning)
			return false;
		break;
	case rx_log_error_level:
		if (event_type < log_event_type::error)
			return false;
		break;
    default:;
	}
	if (!query.pattern.empty() && rx_match_pattern(message.c_str(), query.pattern.c_str(), 1) == 0)
		return false;

	if (!query.library.empty() && library != query.library)
		return false;

	return true;
}

#define LOG_SELF_INFO(msg) RX_LOG_INFO(RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, RX_LOG_SELF_PRIORITY, msg);

// Class rx_platform::log::log_object 

log_object *log_object::g_object = nullptr;

log_object::log_object()
	: worker_("Log", 0)
{
}

log_object::log_object(const log_object &right)
	: worker_("Log", 0)
{
	RX_ASSERT(false);
}


log_object::~log_object()
{
}


log_object & log_object::operator=(const log_object &right)
{
	RX_ASSERT(false);
	return *this;
}



log_object& log_object::instance ()
{
	// this one here doesn't have double lock
	// it forces you on the beginning of your process to start the log
	// :)

	if (g_object == nullptr)
		g_object = new log_object;
	return *g_object;
}

void log_object::log_event_fast (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, log_callback_func_t callback, const string_type& message)
{
	// just fire the job and let worker take care of it!
	if (message == "No specific errors!")
		RX_ASSERT(false);
	auto my_job = rx_create_reference<log_event_job>(event_type, library, source, level, code, message,callback);
	worker_.append(my_job);
}

void log_object::log_event (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, log_callback_func_t callback, const char* message, ... )
{
	char buff[STACK_LOG_SIZE];
	va_list args;
	va_start(args, message);

	int ret=vsnprintf(buff, STACK_LOG_SIZE, message, args);
	if (ret < STACK_LOG_SIZE)
		log_event_fast(event_type, library, source, level, code, callback,buff);
	else
	{
		RX_ASSERT(false);
	}
	va_end(args);
}

void log_object::sync_log_event (log_event_type event_type, const char* library, const char* source, uint16_t level, const char* user, const char* code, const char* message, log_callback_func_t callback, rx_time when)
{

	std::vector<log_subscriber::smart_ptr> temp_array;
	temp_array.reserve(0x10);
	lock();
	for (auto one : subscribers_)
		temp_array.push_back(one.second);
	unlock();
	for (auto one : temp_array)
		one->log_event(event_type, library, source, level, user, code, message,when);
	if (callback)
		callback();
}

void log_object::register_subscriber (log_subscriber::smart_ptr who)
{
	locks::auto_lock_t dummy(this);
	subscribers_.emplace(who->get_name(), who);
}

void log_object::unregister_subscriber (log_subscriber::smart_ptr who)
{
	locks::auto_lock_t dummy(this);
	subscribers_.erase(who->get_name());
}

void log_object::deinitialize ()
{
	worker_.end();
	const char* line = "Log Stopped!";
	LOG_CODE_PREFIX
	sync_log_event(log_event_type::info, RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, RX_LOG_SELF_PRIORITY, "", LOG_CODE_INFO, line, nullptr, rx_time::now());
	LOG_CODE_POSTFIX

	delete g_object;
	g_object = nullptr;
}

rx_result log_object::start (bool test, std::vector<log_subscriber::smart_ptr>& subscribers, int priority)
{
	for(auto one : subscribers)
	{
		register_subscriber(one);
	}
	const char* line = "Log started!";
	LOG_CODE_PREFIX
	sync_log_event(log_event_type::info, RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, RX_LOG_SELF_PRIORITY, "", LOG_CODE_INFO, line, nullptr, rx_time::now());
	LOG_CODE_POSTFIX

	worker_.start(priority);

	if (test)
	{
		char buffer[0x100];

		line = "Performing initial log test...";
		LOG_SELF_INFO(line);

		locks::event ev(false);

		std::array<double, 4> spans;
		log_callback_func_t callback = [&ev] {
			ev.set();
		};

		for (size_t i = 0; i < sizeof(spans) / sizeof(spans[0]); i++)
		{
			snprintf(buffer, sizeof(buffer), "Initial log test pass %d...", (int)i);
			uint64_t first_tick = rx_get_us_ticks();
			RX_LOG_TEST(buffer, callback);
			ev.wait_handle();
			uint64_t second_tick = rx_get_us_ticks();
			double ms = (double)(second_tick - first_tick) / 1000.0;
			snprintf(buffer, sizeof(buffer), "Initial log test %d passed. Delay time: %g ms...", (int)i, ms);
			LOG_SELF_INFO(buffer);
			spans[i] = ms;
			rx_ms_sleep(10);
		}

		double val = 0.0;
		size_t count = sizeof(spans) / sizeof(spans[0]);
		if (count > 1)
		{
			for (size_t i = 1; i < count; i++)
			{
				val += spans[i];
			}
			val = val / (double(count - 1));
		}
		else
			val = spans[0];
		snprintf(buffer, sizeof(buffer), "Average response time: %g ms...", val);
		line = buffer;
		LOG_SELF_INFO(line);

		line = "Initial log test completed.";
		LOG_SELF_INFO(line);
	}

	return true;
}

bool log_object::read_log (const string_type& log, const log_query_type& query, std::function<void(rx_result_with<log_events_type>&&)> callback) const
{
	rx_reference_ptr ref;
	auto my_job = jobs::rx_create_func_job(std::move(ref), [this](const string_type& log, log_query_type query, std::function<void(rx_result_with < log_events_type>&&)> callback)
		{
			log_events_type result;
			auto it = subscribers_.find(log);
			if (it != subscribers_.end())
			{
				auto read_result = it->second->read_log(std::move(query), result);
				if (read_result)
				{
					result.succeeded = true;
					callback(std::move(result));
				}
				else
				{
					callback(read_result.errors());
				}
			}
			else
			{
				callback(RX_INVALID_ARGUMENT);
			}
		}, log, log_query_type(query), std::move(callback));
	const_cast<log_object*>(this)->worker_.append(my_job);
	return true;
}


// Class rx_platform::log::log_subscriber 

log_subscriber::log_subscriber()
{
}


log_subscriber::~log_subscriber()
{
}



rx_result log_subscriber::read_log (log_query_type query, log_events_type& result) const
{
	return RX_NOT_IMPLEMENTED;
}


// Class rx_platform::log::log_event_job 

log_event_job::log_event_job (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, log_callback_func_t callback, rx_time when)
      : event_type_(event_type),
        library_(library),
        source_(source),
        code_(code),
        level_(level),
        message_(message),
        when_(when),
        callback_(callback)
{
	auto sec_ctx = security::active_security();
	if(sec_ctx)
		user_ = sec_ctx->get_full_name();
}


log_event_job::~log_event_job()
{
}



void log_event_job::process ()
{
	log_object::instance().sync_log_event(event_type_, library_.c_str(), source_.c_str(), level_, user_.c_str(), code_.c_str(), message_.c_str(), callback_, when_);
}


// Class rx_platform::log::stream_log_subscriber 

stream_log_subscriber::stream_log_subscriber (std::ostream* stream)
      : stream_(*stream)
{
}


stream_log_subscriber::~stream_log_subscriber()
{
}



void stream_log_subscriber::log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when)
{

	log_event_data one = { event_type,library,source,level,code,message,when, user };

	one.dump_to_stream(stream_);
}

string_type stream_log_subscriber::get_name () const
{
	return string_type();
}

rx_result stream_log_subscriber::read_log (const log_query_type& query, log_events_type& result)
{
	return RX_NOT_SUPPORTED;
}


// Class rx_platform::log::cache_log_subscriber 

cache_log_subscriber::cache_log_subscriber (size_t max_size)
      : max_size_(max_size),
        current_size_(0)
{
}


cache_log_subscriber::~cache_log_subscriber()
{
}



void cache_log_subscriber::log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when)
{
	log_event_data one = { event_type,library,source,level,code,message,when, user };
	locks::auto_lock_t dummy(&cache_lock_);
	events_cache_.emplace(when, one);
	if (events_cache_.size() > max_size_ + (max_size_ >> 4))
	{
		size_t border = (max_size_ >> 4);
		auto it = events_cache_.begin();
		for (size_t i = 0; i < border; i++)
			it++;
		events_cache_.erase(events_cache_.begin(), it);
	}
}

rx_result cache_log_subscriber::read_log (log_query_type query, log_events_type& result) const
{
	if (!events_cache_.empty())
	{
		bool acc = (query.type & rx_log_acceding) != 0;
		result.data.reserve(0x20);
		locks::const_auto_lock_t dummy(&cache_lock_);
		events_cache_type::const_iterator start_it = events_cache_.begin();
		events_cache_type::const_iterator end_it = events_cache_.end();
		if (!query.start_time.is_null())
		{// we have start time
			start_it = events_cache_.lower_bound(query.start_time);
			if (start_it == events_cache_.end())
				start_it = events_cache_.begin();
		}
		if (!query.stop_time.is_null())
		{// we have start time
			end_it = events_cache_.upper_bound(query.start_time);
		}
		if (acc)
			read_log(query, result, events_cache_type::const_reverse_iterator(end_it), events_cache_type::const_reverse_iterator(start_it));
		else
			read_log(query, result, start_it, end_it);

		if (!result.data.empty() && !acc)
			std::reverse(result.data.begin(), result.data.end());
	}
	return true;
}

string_type cache_log_subscriber::get_name () const
{
	return CACHE_LOG_NAME;
}

template<class itType>
rx_result cache_log_subscriber::read_log(const log_query_type& query, log_events_type& result, itType start_it, itType end_it) const
{
	if (start_it != end_it)
	{
		uint32_t count = 0;
		itType it = end_it;

		do
		{
			it--;
			if (it->second.is_included(query))
			{
				result.data.emplace_back(it->second);
				if (query.count)
				{
					count++;
					if (count >= query.count)
						break;
				}
			}
		} while (it != start_it);
	}

	return true;
}
// Class rx_platform::log::file_log_subscriber 

file_log_subscriber::file_log_subscriber (const string_type& path, log_event_type level)
      : level_(level),
        path_(path),
        file_(nullptr)
{
	file_ = fopen(path.c_str(), "wb");
}


file_log_subscriber::~file_log_subscriber()
{
	if (file_)
		fclose(file_);
}



void file_log_subscriber::log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when)
{
	if (file_ && event_type >= level_)
	{
		char buff[STACK_LOG_SIZE];

		int ret = snprintf(buff, STACK_LOG_SIZE, "%s %s@%s \t%s \t%s:%s\r\n",
			when.get_string().c_str(),
			event_type_to_string(event_type),
			library.c_str(),
			user.c_str(),
			source.c_str(),
			message.c_str());
		if (ret < STACK_LOG_SIZE)
		{
			fwrite(buff, ret, 1, file_);
			fflush(file_);
		}
	}
}

string_type file_log_subscriber::get_name () const
{
	return "file";
}


} // namespace log
} // namespace rx_platform

