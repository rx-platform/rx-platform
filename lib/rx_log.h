

/****************************************************************************
*
*  lib\rx_log.h
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


#ifndef rx_log_h
#define rx_log_h 1


//////////////////////////////////////////////////////////////////////////////
// log macros defined here

#define RX_LOG_CONFIG_NAME "log"
#define RX_LOG_SELF_PRIORITY 0x8000
#define RELEASE_CODE_INFO ""

/* if*/
#ifdef _DEBUG

#define LOG_CODE_INFO ___code_name.c_str()
#define LOG_CODE_PREFIX { std::string ___code_name(_rx_func_);___code_name+="@" __FILE__"\r\n";
#define LOG_CODE_POSTFIX  ; }

#else
#define LOG_CODE_INFO RELEASE_CODE_INFO
#define LOG_CODE_PREFIX 
#define LOG_CODE_POSTFIX 
#endif


#define RX_LOG_TEST(msg,ev)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::info_log_event, RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, RX_LOG_SELF_PRIORITY,LOG_CODE_INFO,ev, msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_INFO(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::info_log_event, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_WARNING(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::warning_log_event, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_ERROR(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::error_log_event, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
#define RX_LOG_DEBUG(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::debug_log_event, lib, src, lvl,LOG_CODE_INFO,nullptr, msg\
) LOG_CODE_POSTFIX
#define RX_TRACE(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx::log::log_object::instance().log_event_fast(rx::log::trace_log_event, lib, src, lvl,LOG_CODE_INFO,nullptr,msg)\
 LOG_CODE_POSTFIX
//////////////////////////////////////////////////////////////////////////////

// rx_job
#include "lib/rx_job.h"
// rx_thread
#include "lib/rx_thread.h"
// rx_lock
#include "lib/rx_lock.h"
// rx_ptr
#include "lib/rx_ptr.h"



namespace rx {

namespace log {
enum log_event_type
{
	info_log_event=1,
	warning_log_event=2,
	error_log_event=3,
	debug_log_event=4,
	trace_log_event=5
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
};

typedef std::vector<log_event_data> log_events_type;

struct log_query_type
{
	rx_time start_time;
	rx_time stop_time;
};






class log_subscriber : public pointers::interface_object  
{
	DECLARE_INTERFACE_PTR(log_subscriber);

  public:
      log_subscriber();

      virtual ~log_subscriber();


      virtual void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when) = 0;


  protected:

  private:


};






class stream_log_subscriber : public log_subscriber, 
                              	publ
{
	DECLARE_REFERENCE_PTR(stream_log_subscriber);
	DECLARE_DERIVED_FROM_INTERFACE;

  public:
      stream_log_subscriber (std::ostream* stream);

      virtual ~stream_log_subscriber();


      void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when);


  protected:

  private:


      std::ostream& _stream;


};






class cache_log_subscriber : public log_subscriber, 
                             	publi
{
	DECLARE_REFERENCE_PTR(cache_log_subscriber);
	DECLARE_DERIVED_FROM_INTERFACE;
	typedef std::multimap<rx_time,log_event_data> events_cache_type;

  public:
      cache_log_subscriber (size_t max_size);

      virtual ~cache_log_subscriber();


      void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when);

      bool read_log (const log_query_type& query, log_events_type& result);


  protected:

  private:


      size_t _max_size;

      events_cache_type _events_cache;

      size_t _current_size;

      locks::lockable _cache_lock;


};






class log_object : public locks::lockable  
{
	typedef std::set<log_subscriber::smart_ptr> subscribers_type;

  public:

      static log_object& instance ();

      void log_event_fast (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, locks::event* sync_event, const char* message);

      void log_event (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, locks::event* sync_event, const char* message, ... );

      void register_subscriber (log_subscriber::smart_ptr who);

      void unregister_subscriber (log_subscriber::smart_ptr who);

      void deinitialize ();

      bool start (std::ostream& out, bool test, size_t log_cache_size = 0x100, int priority = RX_PRIORITY_IDLE);

      bool read_cache (const log_query_type& query, log_events_type& result);


  protected:

  private:
      log_object();

      log_object(const log_object &right);

      virtual ~log_object();

      log_object & operator=(const log_object &right);


      void sync_log_event (log_event_type event_type, const char* library, const char* source, uint16_t level, const char* code, const char* message, locks::event* sync_event, rx_time when);



      static log_object *g_object;

      subscribers_type _subscribers;

      threads::physical_job_thread _worker;

      rx_reference<cache_log_subscriber> _cache;


    friend class log_event_job;
};







class log_event_job : public jobs::job  
{
	DECLARE_REFERENCE_PTR(log_event_job);

  public:
      log_event_job (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, locks::event* sync_event, rx_time when = rx_time::now());

      virtual ~log_event_job();


      void process ();


  protected:

  private:


      locks::event *_sync_event;


      log_event_type _event_type;

      string_type _library;

      string_type _source;

      string_type _code;

      int _level;

      string_type _message;

      rx_time _when;


};


} // namespace log
} // namespace rx



#endif
