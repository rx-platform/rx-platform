

/****************************************************************************
*
*  terminal\rx_term_table.h
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


#ifndef rx_term_table_h
#define rx_term_table_h 1




namespace rx
{


struct rx_table_cell_struct
{
	rx_table_cell_struct(const string_type& vvalue)
		: value(vvalue)
	{
	}
	rx_table_cell_struct(const string_type& vvalue, const string_type& vprefix, const string_type& vpostfix)
		: prefix(vprefix), value(vvalue), postfix(vpostfix)
	{
	}
	rx_table_cell_struct(const rx_table_cell_struct& right) = default;
	rx_table_cell_struct(rx_table_cell_struct&& right) noexcept = default;
	string_type prefix;
	string_type value;
	string_type postfix;
};

typedef std::vector<rx_table_cell_struct> rx_row_type;
typedef std::vector<rx_row_type> rx_table_type;

void rx_dump_large_row(rx_row_type row, std::ostream& out, size_t console_width);
void rx_dump_table(const rx_table_type& table, std::ostream& out, bool column_names, bool dot_lines);

} // namespace rx




#endif
