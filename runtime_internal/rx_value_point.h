

/****************************************************************************
*
*  runtime_internal\rx_value_point.h
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


#ifndef rx_value_point_h
#define rx_value_point_h 1


#include "system/server/rx_server.h"


namespace sys_runtime {
namespace data_source {
class data_controler;

} // namespace data_source
} // namespace sys_runtime




namespace sys_runtime {

namespace data_source {
typedef uint64_t value_handle_type;
struct value_handle_extended
{
	uint16_t source;
	uint16_t subscription;
	runtime_handle_t item;

	value_handle_type make_handle() const;
	static value_handle_extended fill_from_handle(value_handle_type handle);
};

enum value_point_state
{
	value_point_not_connected = 0,
	value_point_connected_simple = 1,
	value_point_connected = 2
};





class value_point 
{

	typedef std::vector<rx_value> tag_variables_type;
	typedef std::map<value_handle_type, int> tag_handles_type;

  public:

      void connect (const string_type& path, uint32_t rate, std::function<void(const rx_value&)> callback, data_controler* controler = nullptr, char* buffer = nullptr);

      void disconnect (data_controler* controler = nullptr);

      void calculate (char* token_buff);

      void value_changed (const rx_value& val);


  protected:

  private:


      void level0 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level05 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level07 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level1 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level2 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level3 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level4 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level5 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void level6 (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void primitive (rx_value& result, char*& prog, char*& token, char& tok_type, char*& expres);

      void get_token (char*& prog, char*& token, char& tok_type, char*& expres);

      void put_back (char*& prog, char*& token, char& tok_type, char*& expres);

      void unary (char o, rx_value& r, char*& prog, char*& token, char& tok_type, char*& expres);

      void arith (char o, rx_value& r, rx_value& h, char*& prog, char*& token, char& tok_type, char*& expres);

      void get_expression (rx_value& result, char* tok);

      void parse_and_connect (const char* path, char* tbuff, const rx_time& now, data_controler* controler);


      value_point_state state_;

      uint32_t rate_;

      tag_variables_type tag_variables_;

      tag_handles_type tag_handles_;

      string_type expression_;

      std::function<void(const rx_value&)> callback_;


};


} // namespace data_source
} // namespace sys_runtime



#endif
