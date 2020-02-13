

/****************************************************************************
*
*  lib\rx_log.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_log_h
#define rx_log_h 1


//////////////////////////////////////////////////////////////////////////////
// log macros defined here

#define RX_LOG_CONFIG_NAME "log"
#define RX_LOG_SELF_PRIORITY 9999
#define RELEASE_CODE_INFO ""

/* if*/
#ifdef RX_LOG_SOURCE_INFO

#define LOG_CODE_INFO ___code_name.c_str()
#define LOG_CODE_PREFIX { std::string ___code_name(_rx_func_);___code_name+="@" __FILE__"\r\n";
#define LOG_CODE_POSTFIX  ; }

#else
#define LOG_CODE_INFO RELEASE_CODE_INFO
#define LOG_CODE_PREFIX 
#define LOG_CODE_POSTFIX 
#endif

typedef std::function<void(void)> log_callback_func_t;

#define RX_LOG_TEST(msg,cb)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::log_event_type::trace, RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, 0,LOG_CODE_INFO,cb,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_INFO(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::log_event_type::info, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_WARNING(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::log_event_type::warning, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_ERROR(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::log_event_type::error, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_CRITICAL(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::log_event_type::critical, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_DEBUG(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::log_event_type::debug, lib, src, lvl,LOG_CODE_INFO,nullptr, msg\
) LOG_CODE_POSTFIX
#define RX_TRACE(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::log_event_type::trace, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
//////////////////////////////////////////////////////////////////////////////

// rx_lock
#include "lib/rx_lock.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_job
#include "lib/rx_job.h"
// rx_thread
#include "lib/rx_thread.h"



namespace rx {

namespace log {
enum class log_event_type
{
	debug = 0,
	trace = 1,
	info = 2,
	warning = 3,
	error = 4,
	critical = 5
};
const char* event_type_to_string(log_event_type type);


struct log_event_data
{
	log_event_type event_type;
	string_type library;
	string_type source;
	uint16_t level;
	string_type code;
	string_type message;
	rx_time when;

	void dump_to_stream(std::ostream& stream) const;
	void dump_to_stream_simple(std::ostream& stream) const;
};

typedef std::vector<log_event_data> log_events_type;

enum class rx_log_query_type
{
	debug_level = 0,
	trace_level = 1,
	normal_level = 2,
	warining_level = 3,
	error_level = 4,
};

struct log_query_type
{
	rx_time start_time;
	rx_time stop_time;
	rx_log_query_type type;
};






class log_subscriber : public pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(log_subscriber);

  public:
      log_subscriber();

      ~log_subscriber();


      virtual void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when) = 0;


  protected:

  private:


};






class stream_log_subscriber : public log_subscriber  
{
	DECLARE_REFERENCE_PTR(stream_log_subscriber);

  public:
      stream_log_subscriber (std::ostream* stream);

      ~stream_log_subscriber();


      void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when);


  protected:

  private:


      std::ostream& stream_;


};






class cache_log_subscriber : public log_subscriber  
{
	DECLARE_REFERENCE_PTR(cache_log_subscriber);
	typedef std::multimap<rx_time,log_event_data> events_cache_type;

  public:
      cache_log_subscriber (size_t max_size);

      ~cache_log_subscriber();


      void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when);

      bool read_log (const log_query_type& query, log_events_type& result);


  protected:

  private:


      size_t max_size_;

      events_cache_type events_cache_;

      size_t current_size_;

      locks::lockable cache_lock_;


};






class log_object : public locks::lockable  
{
	typedef std::set<log_subscriber::smart_ptr> subscribers_type;

  public:

      static log_object& instance ();

      void log_event_fast (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, log_callback_func_t callback, const string_type& message);

      void log_event (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, log_callback_func_t callback, const char* message, ... );

      void register_subscriber (log_subscriber::smart_ptr who);

      void unregister_subscriber (log_subscriber::smart_ptr who);

      void deinitialize ();

      rx_result start (bool test, size_t log_cache_size = 0x100, int priority = RX_PRIORITY_IDLE);

      bool read_cache (const log_query_type& query, log_events_type& result);


  protected:

  private:
      log_object();

      log_object(const log_object &right);

      virtual ~log_object();

      log_object & operator=(const log_object &right);


      void sync_log_event (log_event_type event_type, const char* library, const char* source, uint16_t level, const char* code, const char* message, log_callback_func_t callback, rx_time when);



      static log_object *g_object;

      subscribers_type subscribers_;

      threads::physical_job_thread worker_;

      rx_reference<cache_log_subscriber> cache_;


    friend class log_event_job;
};







class log_event_job : public jobs::job  
{
	DECLARE_REFERENCE_PTR(log_event_job);

  public:
      log_event_job (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, log_callback_func_t callback, rx_time when = rx_time::now());

      ~log_event_job();


      void process ();


  protected:

  private:


      log_event_type event_type_;

      string_type library_;

      string_type source_;

      string_type code_;

      int level_;

      string_type message_;

      rx_time when_;

      log_callback_func_t callback_;


};


} // namespace log
} // namespace rx



#endif
