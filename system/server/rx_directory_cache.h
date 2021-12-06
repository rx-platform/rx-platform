

/****************************************************************************
*
*  system\server\rx_directory_cache.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_directory_cache_h
#define rx_directory_cache_h 1



// rx_ns_resolver
#include "system/server/rx_ns_resolver.h"
// rx_ns
#include "system/server/rx_ns.h"



namespace rx_platform {

namespace ns {






class rx_directory_cache 
{
    typedef std::map<rx_directory_ptr, std::set<rx_directory_ptr> > sub_items_cache_type;
    typedef std::map<string_type, rx_directory_ptr> cache_type;

  public:

      rx_result_with<rx_directory_ptr> add_directory (const string_type& dir_path, rx_storage_ptr storage = rx_storage_ptr::null_ptr);

      rx_result_with<rx_directory_ptr> add_directory (rx_directory_ptr parent, const string_type& dir_name, rx_storage_ptr storage = rx_storage_ptr::null_ptr);

      rx_result remove_directory (const string_type& dir_path);

      rx_directory_ptr get_directory (const string_type& path) const;

      rx_directory_ptr get_sub_directory (rx_directory_ptr whose, const string_type& name) const;

      bool get_sub_directories (rx_directory_ptr whose, std::vector<rx_directory_ptr>& items, const char* c_pattern = nullptr) const;

      static rx_directory_cache& instance ();

      rx_directory_ptr get_root () const;

      rx_result insert_cached_item (const string_type& name, const rx_namespace_item& item);

      rx_result remove_cached_item (const string_type& name, const rx_namespace_item& item);

      rx_namespace_item get_cached_item (const string_type& name) const;

      void clear_cache ();


  protected:

  private:
      rx_directory_cache();

      ~rx_directory_cache() = default;
      rx_directory_cache(const rx_directory_cache&) = delete;
      rx_directory_cache(rx_directory_cache&&) = delete;
      rx_directory_cache& operator=(const rx_directory_cache&) = delete;
      rx_directory_cache& operator=(rx_directory_cache&&) = delete;

      rx_names_cache names_cache_;


      cache_type cache_;

      sub_items_cache_type sub_items_cache_;

      locks::slim_lock cache_lock_;

      rx_directory_ptr root_;


};


} // namespace ns
} // namespace rx_platform



#endif
