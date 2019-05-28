

/****************************************************************************
*
*  storage\rx_storage_policy.h
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


#ifndef rx_storage_policy_h
#define rx_storage_policy_h 1




#include "system/storage_base/rx_storage.h"
#define RX_JSON_FILE_EXTESION "json"
#define RX_BINARY_FILE_EXTESION "rxbin"


namespace storage {

namespace storage_policy {





class file_path_addresing_policy 
{
	typedef std::map<string_type, string_type> items_cache_type;

  public:

      string_type get_file_path (const meta::meta_data& data, const string_type& root, const string_type& base);

      void add_file_path (const meta::meta_data& data, const string_type& path);


  protected:

  private:


      locks::slim_lock cache_lock_;

      items_cache_type items_cache_;


};


} // namespace storage_policy
} // namespace storage



#endif
