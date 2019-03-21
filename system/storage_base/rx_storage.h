

/****************************************************************************
*
*  system\storage_base\rx_storage.h
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_storage_h
#define rx_storage_h 1



// rx_ptr
#include "lib/rx_ptr.h"


#include "system/runtime/rx_objbase.h"
#include "system/meta/rx_obj_types.h"
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
namespace storage_base
{
class rx_storage_item;
}
typedef std::unique_ptr<storage_base::rx_storage_item> rx_storage_item_ptr;

namespace storage_base {





class rx_storage_item 
{

  public:
      rx_storage_item (const string_type& path, const string_type& name, const string_type& serialization_type);

      virtual ~rx_storage_item();


      virtual rx_result open_for_read () = 0;

      virtual rx_result open_for_write () = 0;

      virtual rx_result close () = 0;

      virtual base_meta_reader& read_stream () = 0;

      virtual base_meta_writer& write_stream () = 0;

      virtual rx_result delete_item () = 0;


      const string_type& get_path () const
      {
        return path_;
      }


      const string_type& get_name () const
      {
        return name_;
      }


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


      string_type path_;

      string_type name_;

      string_type serialization_type_;


};







class rx_platform_storage : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_storage);

  public:
      rx_platform_storage();

      ~rx_platform_storage();


      virtual string_type get_storage_info () = 0;

      virtual rx_result init_storage (const string_type& storage_reference);

      virtual rx_result deinit_storage ();

      virtual rx_result list_storage (std::vector<rx_storage_item_ptr>& items) = 0;

      virtual string_type get_storage_reference () = 0;

      virtual bool is_valid_storage () const = 0;

      virtual rx_result_with<rx_storage_item_ptr> get_storage_item (const string_type& path) = 0;


  protected:

  private:


};


} // namespace storage_base
} // namespace rx_platform



#endif
