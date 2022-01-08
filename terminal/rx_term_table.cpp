

/****************************************************************************
*
*  terminal\rx_term_table.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#include "pch.h"


// rx_term_table
#include "terminal/rx_term_table.h"

namespace rx
{



void rx_dump_large_row(rx_row_type row, std::ostream& out, size_t console_width)
{
	if (row.empty())
		return;

	const size_t col_diff = 2;

	size_t count = row.size();

	std::vector<size_t> widths(count);
	for (size_t i = 0; i < count; i++)
	{
		widths[i] = row[i].value.size();
	}
	// first try to get how many columns do we need
	size_t columns = count + 1;
	std::vector<size_t> column_widths;
	size_t total_width = 1000000000ull;// i guess will be enough

	while (total_width > console_width)
	{
		columns--;
		column_widths.assign(columns, 0);
		size_t idx = 0;
		while (idx < count)
		{
			for (size_t i = 0; i < columns; i++)
			{
				size_t one_idx = idx + i;

				if (one_idx >= count)
					break;
				if (column_widths[i] < widths[one_idx])
				{
					column_widths[i] = widths[one_idx];
				}
			}
			idx += columns;
		}
		total_width = 0;
		for (size_t i = 0; i < columns; i++)
			total_width += (column_widths[i] + col_diff);
	}

	bool first = true;
	size_t idx = 0;
	while (idx < count)
	{
		if (first)
			first = false;
		else
			out << "\r\n";

		for (size_t i = 0; i < columns; i++)
		{
			size_t one_idx = idx + i;
			if (one_idx >= count)
				break;

			string_type rest(column_widths[i] + col_diff - row[one_idx].value.size(), ' ');

			if (!row[one_idx].prefix.empty())
				out << row[one_idx].prefix;
			out << row[one_idx].value;
			if (!row[one_idx].postfix.empty())
				out << row[one_idx].postfix;
			out << rest;
		}
		idx += columns;
	}

	out << "\r\n";
}

void rx_dump_table(const rx_table_type& table, std::ostream& out, bool column_names, bool dot_lines)
{
	if (table.empty())
		return;

	const size_t col_diff = 2;

	size_t columns_number = 0;
	for (const auto& row : table)
	{
		if (columns_number == 0)
			columns_number = row.size();
		else
		{
			if (!row.empty())
			{// we allow empty rows
				if (columns_number != row.size())
				{
					out << "Error in table format\r\n";
					RX_ASSERT(false);
					return;
				}
			}
		}
	}

	// o.k. we checked now so let's calculate columns width
	std::vector<size_t> widths(columns_number);
	for (const auto& row : table)
	{
		if (!row.empty())
		{
			for (size_t i = 0; i < columns_number; i++)
			{
				if (row[i].value.size() > widths[i])
					widths[i] = row[i].value.size();
			}
		}
	}
	char empty_char = dot_lines ? '.' : ' ';
	// now we have all widths
	bool first = true;
	for (const auto& row : table)
	{
		if (!first)
			out << "\r\n";
		if (!row.empty())
		{
			for (size_t i = 0; i < columns_number; i++)
			{
				string_type rest(widths[i] + col_diff - row[i].value.size(),
					i == columns_number - 1 || first || row[i].value.empty()
					? ' ' : empty_char);
				if (!row[i].prefix.empty())
					out << row[i].prefix;
				out << row[i].value;
				if (!row[i].postfix.empty())
					out << row[i].postfix;
				out << rest;
			}
		}
		if (first)
			first = false;
		if (column_names)
		{
			out << "\r\n";
			size_t total_width = 0;
			for (size_t i = 0; i < columns_number; i++)
				total_width += (widths[i] + col_diff);
			string_type rest(total_width, '=');
			out << rest;
			column_names = false;
		}
	}

	out << "\r\n";
}


} // namespace rx


