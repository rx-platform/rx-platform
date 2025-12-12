

/****************************************************************************
*
*  system\server\rx_namespaces.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef rx_namespaces_h
#define rx_namespaces_h 1




#include "lib/rx_lock.h"


namespace rx_platform {

namespace ns {





class rx_namespace_array 
{
    typedef std::map<uint16_t, string_type> namespaces_type;
    typedef std::map<string_type, uint16_t> rev_namespaces_type;
	typedef std::map<string_type, string_type> aliases_type;

  public:
      rx_namespace_array();

      ~rx_namespace_array();


      rx_result_with<uint16_t> add_namespace (const string_type& name, const string_type& alias = "");

      string_type namespace_lookup (uint16_t id) const;

      string_array get_namespaces ();

      rx_result_with<uint16_t> get_namespace_id (const string_type& name, bool check_alias = false);


  protected:

  private:


      locks::slim_lock namespaces_lock_;

      namespaces_type namespaces_;

      rev_namespaces_type rev_namespaces_;

      aliases_type aliases_;


    friend class rx_namespace_translator;
};







class rx_namespace_translator 
{

  public:
      rx_namespace_translator();

      rx_namespace_translator (rx_namespace_array* mine);

      rx_namespace_translator (const rx_namespace_array* pair, rx_namespace_array* mine);

      ~rx_namespace_translator();


      rx_result_with<uint16_t> translate_namespace (uint16_t in, const rx_namespace_array* pair = nullptr);


  protected:

  private:


      rx_namespace_array *mine_;

      const rx_namespace_array* pair_;


};


} // namespace ns
} // namespace rx_platform



#endif
