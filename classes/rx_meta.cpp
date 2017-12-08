

/****************************************************************************
*
*  classes\rx_meta.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"


// rx_meta
#include "classes/rx_meta.h"



namespace model {

// Class model::internal_classes_manager 

internal_classes_manager::internal_classes_manager()
{
}


internal_classes_manager::~internal_classes_manager()
{
}



internal_classes_manager& internal_classes_manager::instance ()
{
	static internal_classes_manager g_instance;
	return g_instance;//ROOT of CLASSES!!!! $$$ Importanat Object Here!!!
}


// Class model::relations_hash_data 

relations_hash_data::relations_hash_data()
{
}


relations_hash_data::~relations_hash_data()
{
}



void relations_hash_data::add_to_hash_data (const rx_node_id& new_id, const rx_node_id& first_backward)
{
	bool first = true;
	/*
	std::vector<rx_node_id> super_array;
	auto itf = _backward_hash.find(first_backward);
	if (itf != _backward_hash.end())
	{
		super_array.reserve(itf->second->size() + 1);
		super_array.push_back(first_backward);
		for (auto ite : (*itf->second))
			super_array.push_back(ite);
	}
	else
		super_array.push_back(first_backward);

	for (auto one : super_array)
	{
		if (first)
		{// this is first stuff do it once
			first = false;
			// backward update
			auto it_elem = _first_backward_hash.find(new_id);
			if (it_elem == _first_backward_hash.end())
			{
				auto it = _first_backward_hash.emplace(new_id, std::make_unique<relation_elements_type>());
				it.first->second->insert(one);
			}
			else
				it_elem->second->insert(one);

			// forward update
			it_elem = _first_forward_hash.find(one);
			if (it_elem == _first_forward_hash.end())
			{
				auto it = _first_forward_hash.emplace(new_id, std::make_unique<relation_elements_type>());
				it.first->second->insert(new_id);
				temp = new relation_elements_type;
				m_first_forward_hash.insert(relation_map_type::value_type(*it, temp));
			}
			else
				temp = it_elem->second;
			temp->insert(new_id);
		}
		// backward update
		relation_elements_type* temp;
		relation_map_iterator it_elem = m_backward_hash.find(new_id);
		if (it_elem == m_backward_hash.end())
		{
			temp = new relation_elements_type;
			m_backward_hash.insert(relation_map_type::value_type(new_id, temp));
		}
		else
			temp = it_elem->second;
		temp->insert(*it);

		// forward update
		it_elem = m_forward_hash.find(*it);
		if (it_elem == m_forward_hash.end())
		{
			temp = new relation_elements_type;
			m_forward_hash.insert(relation_map_type::value_type(*it, temp));
		}
		else
			temp = it_elem->second;
		temp->insert(new_id);
	}*/
}

void relations_hash_data::remove_from_hash_data (const rx_node_id& id)
{
}

void relations_hash_data::change_hash_data (const rx_node_id& id, const rx_node_id& first_backward_old, const rx_node_id& first_backward_new)
{
}

bool relations_hash_data::is_backward_from (const rx_node_id& id, const rx_node_id& parent)
{
	return false;
}

void relations_hash_data::get_full_forward (const rx_node_id& id, std::vector< rx_node_id>& result)
{
}

void relations_hash_data::get_full_backward (const rx_node_id& id, std::vector< rx_node_id>& result)
{
}


} // namespace model

