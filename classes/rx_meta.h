

/****************************************************************************
*
*  classes\rx_meta.h
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


#ifndef rx_meta_h
#define rx_meta_h 1






namespace model {





class internal_classes_manager 
{

  public:
      virtual ~internal_classes_manager();


      static internal_classes_manager& instance ();


  protected:

  private:
      internal_classes_manager();



};






class relations_hash_data 
{
	relations_hash_data(const relations_hash_data&) = delete;
	relations_hash_data(relations_hash_data&&) = delete;
	void operator=(const relations_hash_data&) = delete;
	void operator=(relations_hash_data&&) = delete;

	typedef std::set<rx_node_id> relation_elements_type;
	// this here is pointer type so we don't have copying of whole set justy pointer
	typedef std::map<rx_node_id, std::unique_ptr<relation_elements_type> > relation_map_type;


  public:
      relations_hash_data();

      virtual ~relations_hash_data();


      void add_to_hash_data (const rx_node_id& new_id, const rx_node_id& first_backward);

      void remove_from_hash_data (const rx_node_id& id);

      void change_hash_data (const rx_node_id& id, const rx_node_id& first_backward_old, const rx_node_id& first_backward_new);

      bool is_backward_from (const rx_node_id& id, const rx_node_id& parent);

      void get_full_forward (const rx_node_id& id, std::vector< rx_node_id>& result);

      void get_full_backward (const rx_node_id& id, std::vector< rx_node_id>& result);


  protected:

  private:


      relation_map_type _forward_hash;

      relation_map_type _backward_hash;

      relation_map_type _first_forward_hash;

      relation_map_type _first_backward_hash;


};


} // namespace model



#endif
