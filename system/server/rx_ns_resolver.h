

/****************************************************************************
*
*  system\server\rx_ns_resolver.h
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


#ifndef rx_ns_resolver_h
#define rx_ns_resolver_h 1



// rx_ns
#include "system/server/rx_ns.h"



namespace rx_platform {

namespace ns {






class rx_names_cache 
{
    typedef std::map<string_type, std::list<rx_namespace_item> > name_items_hash_type;

  public:
      rx_names_cache();


      rx_namespace_item get_cached_item (const string_type& name) const;

      rx_result insert_cached_item (const string_type& name, const rx_namespace_item& item);

      static bool should_cache (const platform_item_ptr& item);

      static bool should_cache (const rx_namespace_item& item);

      rx_result removed_cached_item (const string_type& name, const rx_node_id& id);

      void clear ();


  protected:

  private:


      name_items_hash_type name_items_hash_;

      locks::slim_lock lock_;


};







class rx_directory_resolver 
{
    struct resolver_data
    {
        string_type path;
    };
    typedef std::vector<resolver_data> directories_type;
    rx_directory_resolver(const rx_directory_resolver&) = delete;
    rx_directory_resolver(rx_directory_resolver&&) = delete;
    rx_directory_resolver& operator=(const rx_directory_resolver&) = delete;
    rx_directory_resolver& operator=(rx_directory_resolver&&) = delete;

  public:
      rx_directory_resolver();

      rx_directory_resolver (rx_directory_resolver* parent);


      rx_namespace_item resolve_item (const string_type& path) const;

      rx_directory_ptr resolve_directory (const string_type& path) const;

      void add_paths (std::initializer_list<string_type> paths);

      ~rx_directory_resolver() = default;
  protected:

  private:


      rx_directory_resolver *parent_;


      directories_type directories_;


};


} // namespace ns
} // namespace rx_platform



#endif
