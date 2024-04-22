

/****************************************************************************
*
*  system\server\rx_log.h
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


#ifndef rx_log_h
#define rx_log_h 1


#include "rx_log_macros.h"

typedef std::function<void(void)> log_callback_func_t;

// rx_lock
#include "lib/rx_lock.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_job
#include "system/threads/rx_job.h"
// rx_thread
#include "system/threads/rx_thread.h"



namespace rx_platform {

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

enum rx_log_query_type
{
    rx_log_debug_level = 0,
    rx_log_trace_level = 1,
    rx_log_normal_level = 2,
    rx_log_warining_level = 3,
    rx_log_error_level = 4,

    // other that can be masked
    rx_log_level_mask = 0x7,
    rx_log_acceding = 0x80
};

struct log_query_type
{
    rx_time start_time;
    rx_time stop_time;
    rx_log_query_type type;
    string_type pattern;
    string_type library;
    uint32_t count;
};

struct log_event_data
{
	log_event_type event_type;
	string_type library;
	string_type source;
	uint16_t level;
	string_type code;
	string_type message;
	rx_time when;
    string_type user;

	void dump_to_stream(std::ostream& stream) const;
	void dump_to_stream_simple(std::ostream& stream) const;
    bool is_included(log_query_type query) const;
};

struct log_events_type
{
    bool succeeded = false;
    std::vector<log_event_data> data;
    operator bool() const
    {
        return succeeded;
    }
};







class log_subscriber : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(log_subscriber);

  public:
      log_subscriber();

      ~log_subscriber();


      virtual void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when) = 0;

      virtual string_type get_name () const = 0;

      virtual rx_result read_log (log_query_type query, log_events_type& result) const;


  protected:

  private:


};






class log_object : public rx::locks::lockable  
{
	typedef std::map<string_type, log_subscriber::smart_ptr> subscribers_type;

  public:

      static log_object& instance ();

      void log_event_fast (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, log_callback_func_t callback, const string_type& message);

      void log_event (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, log_callback_func_t callback, const char* message, ... );

      void register_subscriber (log_subscriber::smart_ptr who);

      void unregister_subscriber (log_subscriber::smart_ptr who);

      void deinitialize ();

      rx_result start (bool test, std::vector<log_subscriber::smart_ptr>& subscribers, int priority = RX_PRIORITY_IDLE);

      bool read_log (const string_type& log, const log_query_type& query, std::function<void(rx_result_with<log_events_type>&&)> callback) const;


  protected:

  private:
      log_object();

      log_object(const log_object &right);

      virtual ~log_object();

      log_object & operator=(const log_object &right);


      void sync_log_event (log_event_type event_type, const char* library, const char* source, uint16_t level, const char* user, const char* code, const char* message, log_callback_func_t callback, rx_time when);



      static log_object *g_object;

      subscribers_type subscribers_;

      threads::physical_job_thread worker_;


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

      string_type user_;


};






class stream_log_subscriber : public log_subscriber  
{
	DECLARE_REFERENCE_PTR(stream_log_subscriber);

  public:
      stream_log_subscriber (std::ostream* stream);

      ~stream_log_subscriber();


      void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when);

      string_type get_name () const;

      virtual rx_result read_log (const log_query_type& query, log_events_type& result);


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


      void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when);

      rx_result read_log (log_query_type query, log_events_type& result) const;

      string_type get_name () const;


  protected:

  private:
      template<class itType>
      rx_result read_log(const log_query_type& query, log_events_type& result, itType start_it, itType end_it) const;

      size_t max_size_;

      events_cache_type events_cache_;

      size_t current_size_;

      locks::slim_lock cache_lock_;


};






class file_log_subscriber : public log_subscriber  
{

  public:
      file_log_subscriber (const string_type& path, log_event_type level = log_event_type::trace);

      ~file_log_subscriber();


      void log_event (log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when);

      string_type get_name () const;


  protected:

  private:


      log_event_type level_;

      string_type path_;

      FILE* file_;


};


} // namespace log
} // namespace rx_platform



#endif
