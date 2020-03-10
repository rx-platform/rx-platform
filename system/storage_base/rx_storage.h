

/****************************************************************************
*
*  system\storage_base\rx_storage.h
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


#ifndef rx_storage_h
#define rx_storage_h 1



// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace hosting {
class rx_platform_host;

} // namespace hosting
} // namespace rx_platform


#include "lib/rx_ser_lib.h"
namespace rx_internal
{
namespace terminal
{
namespace commands
{
class server_command;
}
}
}

namespace rx_platform
{
struct configuration_data_t;
namespace ns
{
class rx_platform_item;
}
namespace meta
{
class meta_data;
}
}
/////////////////////////////////////////////////////////////
// logging macros for storage library
#define STORAGE_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Storage",src,lvl,msg)
#define STORAGE_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Storage",src,lvl,msg)
#define STORAGE_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Storage",src,lvl,msg)
#define STORAGE_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Storage",src,lvl,msg)
#define STORAGE_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Storage",src,lvl,msg)
#define STORAGE_LOG_TRACE(src,lvl,msg) RX_TRACE("Storage",src,lvl,msg)


namespace rx_platform {
namespace storage_base
{
class rx_storage_item;
class rx_platform_storage;
}
typedef std::unique_ptr<storage_base::rx_storage_item> rx_storage_item_ptr;
typedef rx_reference<storage_base::rx_platform_storage> rx_storage_ptr;

namespace ns
{
class rx_platform_directory;
}

typedef rx::pointers::reference<rx_internal::terminal::commands::server_command> server_command_base_ptr;
typedef std::unique_ptr<rx_platform::ns::rx_platform_item> platform_item_ptr;
typedef rx::pointers::reference<ns::rx_platform_directory> rx_directory_ptr;
typedef std::vector<rx_directory_ptr> platform_directories_type;


namespace storage_base {





class rx_storage_item 
{

  public:
      rx_storage_item (const string_type& serialization_type);

      virtual ~rx_storage_item();


      virtual rx_result open_for_read () = 0;

      virtual rx_result open_for_write () = 0;

      virtual void close () = 0;

      virtual base_meta_reader& read_stream () = 0;

      virtual base_meta_writer& write_stream () = 0;

      virtual rx_result delete_item () = 0;

      virtual const string_type& get_item_reference () const = 0;


      const string_type& get_serialization_type () const
      {
        return serialization_type_;
      }


	  rx_storage_item() = delete;
	  rx_storage_item(const rx_storage_item&) = delete;
	  rx_storage_item(rx_storage_item&&) = delete;
	  rx_storage_item& operator=(const rx_storage_item&) = delete;
	  rx_storage_item& operator=(rx_storage_item&&) = delete;
  protected:

  private:


      string_type serialization_type_;


};


enum rx_storage_type
{
	invalid_storage = 0,
	system_storage,
	user_storage,
	extern_storage,
	test_storage
};





class rx_platform_storage : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_storage);

  public:
      rx_platform_storage();

      ~rx_platform_storage();


      virtual string_type get_storage_info () = 0;

      virtual rx_result init_storage (const string_type& storage_reference, hosting::rx_platform_host* host);

      virtual void deinit_storage ();

      virtual rx_result list_storage (std::vector<rx_storage_item_ptr>& items) = 0;

      virtual bool is_valid_storage () const = 0;

      virtual rx_result_with<rx_storage_item_ptr> get_item_storage (const meta::meta_data& data) = 0;

      virtual string_type get_storage_reference () = 0;


      const string_type& get_base_path () const
      {
        return base_path_;
      }

      void set_base_path (const string_type& value)
      {
        base_path_ = value;
      }



  protected:

  private:


      string_type base_path_;


};






class rx_platform_storage_holder 
{
	typedef std::map<string_type, rx_platform_storage::smart_ptr> initialized_storages_type;

  public:
      rx_platform_storage_holder();

      virtual ~rx_platform_storage_holder();


      virtual string_type get_storage_info () = 0;

      virtual rx_result init_storage (const string_type& storage_reference, hosting::rx_platform_host* host) = 0;

      void deinit_storage ();

      virtual string_type get_storage_reference () = 0;

      rx_result_with<rx_storage_ptr> get_storage (const string_type& name, hosting::rx_platform_host* host);


  protected:

      virtual rx_result_with<rx_storage_ptr> get_and_init_storage (const string_type& name, hosting::rx_platform_host* host) = 0;


  private:


      initialized_storages_type initialized_storages_;


};






class rx_code_storage_item : public rx_storage_item  
{

  public:
      rx_code_storage_item();


      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      void close ();

      const string_type& get_item_reference () const;

      rx_result delete_item ();


  protected:

  private:


};






class rx_code_storage : public rx_platform_storage  
{
    DECLARE_REFERENCE_PTR(rx_code_storage);

  public:
      rx_code_storage();


      string_type get_storage_info ();

      rx_result list_storage (std::vector<rx_storage_item_ptr>& items);

      bool is_valid_storage () const;

      rx_result_with<rx_storage_item_ptr> get_item_storage (const meta::meta_data& data);

      string_type get_storage_reference ();


  protected:

  private:


};


} // namespace storage_base
} // namespace rx_platform



#endif
