

/****************************************************************************
*
*  system\hosting\rx_yaml.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#include "pch.h"


// rx_yaml
#include "system/hosting/rx_yaml.h"



namespace rx_platform {

namespace hosting {
enum yaml_parsing_state
{
	searching_key,
	extracting_key,
	searching_assigment,
	searching_value,
	extracting_value,
	parsing_done
};

// Class rx_platform::hosting::simplified_yaml_reader 


rx_result simplified_yaml_reader::parse_configuration (const string_type& input_data, std::map<string_type, string_type>& config_values)
{
	std::istringstream stream(input_data);
	string_type line;
	string_type key;
	string_type value;
	int level;
	int line_number = -1;
	while (std::getline(stream, line))
	{
        line_number++;
		if (line.empty() || line[0] == '#')
			continue;
		auto result = parse_yaml_line(line, key, value, line_number, level);
		if (!result)
			return result;
		if (!key.empty())
			config_values.emplace(key, value);
	}
	return true;
}

rx_result simplified_yaml_reader::parse_yaml_line (const string_type& line, string_type& key, string_type& value, const int line_number, int& level)
{

	static const string_type delimeters(" \t\r\n");
	auto line_size = line.size();
	size_t idx_key_start = 0;
	size_t idx_key_stop = 0;
	size_t idx_value_start = 0;
	size_t idx_value_stop = 0;
	yaml_parsing_state state = searching_key;
	size_t idx = 0;
	while (idx < line_size && state != yaml_parsing_state::parsing_done)
	{
		char one = line[idx];
		if (one == '#')
		{
			break;// rest of the line is comment, break;
		}
		switch (state)
		{
		case yaml_parsing_state::searching_key:
		{
			if (delimeters.find(one) == string_type::npos)
			{
				state = yaml_parsing_state::extracting_key;
				idx_key_start = idx;
				continue;
			}
			else if (one == '-')
			{
				return "YAML parser does not support structures!\r\nError at position "s
					+ std::to_string(line_number) + "," + std::to_string(idx);
			}
			else
			{
				level++;
				idx++;
			}
			break;
		}
		case yaml_parsing_state::extracting_key:
		{			
			if (!rx_platform::rx_is_valid_name_character(one) && one != '.')
			{
				state = yaml_parsing_state::searching_assigment;
				idx_key_stop = idx;
				continue;
			}
			else
			{
				idx++;
			}
			break;
		}
		case yaml_parsing_state::searching_assigment:
		{
			if (one == ':')
			{
				state = yaml_parsing_state::searching_value;
				idx++;
				continue;
			}
			else
			{
				idx++;
			}
			break;
		}
		case yaml_parsing_state::searching_value:
		{
			if (delimeters.find(one) == string_type::npos)
			{
				state = yaml_parsing_state::parsing_done;
				idx_value_start = idx;
				// delete padding delimiters if it is there
				idx_value_stop = line_size - 1;
				while (idx_value_stop >= idx_value_start && delimeters.find(line[idx_value_stop]) != string_type::npos)
					idx_value_stop--;

				continue;
			}
			else
			{
				idx++;
			}
			break;
		}
		default:
			RX_ASSERT(false);// shouldn't be here
		}
	}
	if (state == yaml_parsing_state::searching_assigment)
		return "No assignment (:) symbol!";
	if (idx_key_start == 0 && idx_key_stop == 0 && idx_value_start == 0)
		return true;// empty line
	if (idx_key_start < idx_key_stop)
		key = line.substr(idx_key_start, idx_key_stop - idx_key_start);
	else
		return "Something really went wrong with key! :(";
	if (idx_value_start != 0 && idx_value_stop != 0)
	{
		if (idx_key_stop < idx_value_start && idx_value_start <= idx_value_stop)
			value = line.substr(idx_value_start, idx_value_stop - idx_value_start + 1);
		else
			return "Something really went wrong with value! :(";
		if (value.size() >= 2 && *value.begin() == '"' && *value.rbegin() == '"')
			value = value.substr(1, value.size() - 2);
	}
	else
		value.clear();
	return true;
}


} // namespace hosting
} // namespace rx_platform

