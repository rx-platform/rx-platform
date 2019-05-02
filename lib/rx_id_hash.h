

/****************************************************************************
*
*  lib\rx_id_hash.h
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


#ifndef rx_id_hash_h
#define rx_id_hash_h 1


#include "rx_hashes.h"
#include "rx_lib.h"




namespace rx {

namespace rx_hashes {





class rx_node_id_hash 
{
	typedef std::map<hash_handle_t, rx_node_id> ids_hash_type;

  public:
      ~rx_node_id_hash();


      static rx_node_id_hash& instance ();


  protected:

  private:
      rx_node_id_hash();

	  rx_node_id_hash(const rx_node_id_hash &right) = delete;
	  rx_node_id_hash & operator=(const rx_node_id_hash &right) = delete;

	  rx_node_id_hash(rx_node_id_hash &&right) = delete;
	  rx_node_id_hash & operator=(rx_node_id_hash &&right) = delete;

      ids_hash_type hash_;


};


} // namespace rx_hashes
} // namespace rx



#endif
