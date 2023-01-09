

/****************************************************************************
*
*  system\hosting\rx_yaml.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_yaml_h
#define rx_yaml_h 1



// rx_host
#include "system/hosting/rx_host.h"



namespace rx_platform {

namespace hosting {





class simplified_yaml_reader : public configuration_reader  
{

  public:

      rx_result parse_configuration (const string_type& input_data, std::map<string_type, string_type>& config_values);


  protected:

  private:

      rx_result parse_yaml_line (const string_type& line, string_type& key, string_type& value, const int line_number, int& level);



};


} // namespace hosting
} // namespace rx_platform



#endif
