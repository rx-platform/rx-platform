

/****************************************************************************
*
*  system\storage_base\rx_storage.h
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


#ifndef rx_storage_h
#define rx_storage_h 1



#define RX_CODE_STORAGE_NAME "<code>"
#define RX_OTHERS_STORAGE_NAME "<other>"

#include "platform_api/rx_abi.h"
#include "system/rx_platform_typedefs.h"

// rx_meta_data
#include "lib/rx_meta_data.h"
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


typedef rx::pointers::reference<rx_internal::terminal::commands::server_command> server_command_base_ptr;

enum class rx_storage_item_type
{
    none,
    type,
    instance,
    runtime
};


namespace storage_base {





class rx_storage_item 
{

  public:
      rx_storage_item (rx_storage_item_type storage_type = rx_storage_item_type::none);

      virtual ~rx_storage_item();


      virtual rx_result open_for_read () = 0;

      virtual base_meta_reader& read_stream () = 0;

      virtual rx_result close_read () = 0;

      virtual rx_result open_for_write () = 0;

      virtual base_meta_writer& write_stream () = 0;

      virtual rx_result commit_write () = 0;

      virtual rx_result delete_item () = 0;

      virtual const string_type& get_item_reference () const = 0;

      virtual string_type get_item_path () const = 0;

      virtual bool preprocess_meta_data (meta_data& data) = 0;

      virtual bool is_read_only () const;


      const rx_storage_item_type& get_storage_type () const
      {
        return storage_type_;
      }


	  rx_storage_item() = delete;
	  rx_storage_item(const rx_storage_item&) = delete;
	  rx_storage_item(rx_storage_item&&) = delete;
	  rx_storage_item& operator=(const rx_storage_item&) = delete;
	  rx_storage_item& operator=(rx_storage_item&&) = delete;
  protected:

  private:


      rx_storage_item_type storage_type_;


};






class rx_code_storage_item : public rx_storage_item  
{

  public:
      rx_code_storage_item();


      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      rx_result close_read ();

      rx_result commit_write ();

      const string_type& get_item_reference () const;

      rx_result delete_item ();

      bool preprocess_meta_data (meta_data& data);

      string_type get_item_path () const;


  protected:

  private:


};






class rx_plugin_storage_item : public rx_storage_item  
{

  public:
      rx_plugin_storage_item();


      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      rx_result close_read ();

      rx_result commit_write ();

      const string_type& get_item_reference () const;

      rx_result delete_item ();

      bool preprocess_meta_data (meta_data& data);

      string_type get_item_path () const;


  protected:

  private:


};






class rx_roles_storage_item 
{

  public:
      rx_roles_storage_item();

      virtual ~rx_roles_storage_item();


      virtual rx_result open_for_read () = 0;

      virtual base_meta_reader& read_stream () = 0;

      virtual rx_result close_read () = 0;

      virtual rx_result open_for_write () = 0;

      virtual base_meta_writer& write_stream () = 0;

      virtual rx_result commit_write () = 0;

      virtual rx_result delete_item () = 0;


  protected:

  private:


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

      virtual rx_result list_storage (std::vector<rx_storage_item_ptr>& items) = 0;

      virtual rx_result list_storage_roles (std::vector<rx_roles_storage_item_ptr>& items);

      virtual bool is_valid_storage () const = 0;

      virtual rx_result_with<rx_storage_item_ptr> get_item_storage (const meta_data& data, rx_item_type type) = 0;

      virtual rx_result_with<rx_storage_item_ptr> get_runtime_storage (const meta_data& data, rx_item_type type) = 0;

      virtual string_type get_storage_reference () = 0;

      virtual void preprocess_meta_data (meta_data& data) = 0;


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






class rx_code_storage : public rx_platform_storage  
{
    DECLARE_REFERENCE_PTR(rx_code_storage);

  public:
      rx_code_storage();


      string_type get_storage_info ();

      rx_result list_storage (std::vector<rx_storage_item_ptr>& items);

      bool is_valid_storage () const;

      rx_result_with<rx_storage_item_ptr> get_item_storage (const meta_data& data, rx_item_type type);

      rx_result_with<rx_storage_item_ptr> get_runtime_storage (const meta_data& data, rx_item_type type);

      string_type get_storage_reference ();

      void preprocess_meta_data (meta_data& data);


  protected:

  private:


};

rx_result split_storage_reference(const string_type full_ref, string_type& type, string_type& reference);





class rx_storage_connection : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(rx_storage_connection);
    typedef std::map<string_type, rx_platform_storage::smart_ptr> initialized_storages_type;

  public:

      virtual string_type get_storage_reference () const = 0;

      rx_result_with<rx_storage_ptr> get_storage (const string_type& name, hosting::rx_platform_host* host);

      virtual rx_result init_connection (const string_type& storage_reference, hosting::rx_platform_host* host);

      virtual rx_result deinit_connection ();

      virtual string_type get_storage_info () const = 0;

      std::vector<std::pair<string_type, string_type> > get_mounted_storages () const;

      rx_result list_storage_roles (std::vector<rx_roles_storage_item_ptr>& items);


  protected:

      virtual rx_result_with<rx_storage_ptr> get_and_init_storage (const string_type& name, hosting::rx_platform_host* host) = 0;


  private:


      initialized_storages_type initialized_storages_;


};







class rx_platform_storage_type 
{

  public:
      rx_platform_storage_type();

      virtual ~rx_platform_storage_type();


      virtual string_type get_storage_info () = 0;

      virtual rx_storage_connection::smart_ptr construct_storage_connection () = 0;

      virtual string_type get_reference_prefix () const = 0;


  protected:

  private:


};






class rx_empty_storage : public rx_platform_storage  
{

  public:

      string_type get_storage_info ();

      rx_result init_storage (const string_type& storage_reference, hosting::rx_platform_host* host);

      rx_result list_storage (std::vector<rx_storage_item_ptr>& items);

      bool is_valid_storage () const;

      rx_result_with<rx_storage_item_ptr> get_item_storage (const meta_data& data, rx_item_type type);

      rx_result_with<rx_storage_item_ptr> get_runtime_storage (const meta_data& data, rx_item_type type);

      string_type get_storage_reference ();

      void preprocess_meta_data (meta_data& data);


  protected:

  private:


};






class rx_empty_storage_connection : public rx_storage_connection  
{

  public:

      string_type get_storage_reference () const;

      string_type get_storage_info () const;


  protected:

      rx_result_with<rx_storage_ptr> get_and_init_storage (const string_type& name, hosting::rx_platform_host* host);


  private:


};






class rx_plugin_storage : public rx_platform_storage  
{
    DECLARE_REFERENCE_PTR(rx_plugin_storage);

  public:
      rx_plugin_storage (plugin_storage_struct* impl);

      ~rx_plugin_storage();


      string_type get_storage_info ();

      rx_result list_storage (std::vector<rx_storage_item_ptr>& items);

      bool is_valid_storage () const;

      rx_result_with<rx_storage_item_ptr> get_item_storage (const meta_data& data, rx_item_type type);

      rx_result_with<rx_storage_item_ptr> get_runtime_storage (const meta_data& data, rx_item_type type);

      string_type get_storage_reference ();

      rx_result init_storage (const string_type& name, const string_type& ref);

      void preprocess_meta_data (meta_data& data);


  protected:

  private:


      plugin_storage_struct* impl_;


};






class rx_plugin_storage_connection : public rx_storage_connection  
{

  public:
      rx_plugin_storage_connection (rx_storage_constructor_t construct_func);


      string_type get_storage_reference () const;

      string_type get_storage_info () const;

      rx_result init_connection (const string_type& storage_reference, hosting::rx_platform_host* host);


  protected:

      rx_result_with<rx_storage_ptr> get_and_init_storage (const string_type& name, hosting::rx_platform_host* host);


  private:


      rx_storage_constructor_t constructor_;

      string_type reference_;


};






class rx_plugin_storage_type : public rx_platform_storage_type  
{

  public:
      rx_plugin_storage_type (const string_type& prefix, rx_storage_constructor_t construct_func);


      string_type get_storage_info ();

      rx_storage_connection::smart_ptr construct_storage_connection ();

      string_type get_reference_prefix () const;


  protected:

  private:


      string_type prefix_;

      rx_storage_constructor_t constructor_;


};






class rx_others_storage : public rx_platform_storage  
{

  public:
      rx_others_storage();


      string_type get_storage_info ();

      rx_result list_storage (std::vector<rx_storage_item_ptr>& items);

      bool is_valid_storage () const;

      rx_result_with<rx_storage_item_ptr> get_item_storage (const meta_data& data, rx_item_type type);

      rx_result_with<rx_storage_item_ptr> get_runtime_storage (const meta_data& data, rx_item_type type);

      string_type get_storage_reference ();

      void preprocess_meta_data (meta_data& data);


  protected:

  private:


};






class rx_other_storage_item : public rx_storage_item  
{

  public:
      rx_other_storage_item();


      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read ();

      rx_result open_for_write ();

      rx_result close_read ();

      rx_result commit_write ();

      const string_type& get_item_reference () const;

      rx_result delete_item ();

      bool preprocess_meta_data (meta_data& data);

      string_type get_item_path () const;

      bool is_read_only () const;


      rx::meta_data meta_info;


  protected:

  private:


};


} // namespace storage_base
} // namespace rx_platform



#endif
