

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



// rx_thread
#include "lib/rx_thread.h"
// rx_construct
#include "system/constructors/rx_construct.h"

#include "system/meta/rx_classes.h"
using namespace rx_platform::meta;


namespace model {





class internal_classes_manager 
{

  public:
      virtual ~internal_classes_manager();


      static internal_classes_manager& instance ();

      uint32_t initialize (hosting::rx_platform_host* host, meta_data_t& data);

      uint32_t deinitialize ();

      uint32_t start (hosting::rx_platform_host* host, const meta_data_t& data);

      uint32_t stop ();

	  template<class T>
	  struct get_arg_data
	  {
		  T data;
	  };
	  //template<class T>
	  //void get_class(std::function<void(T))
	  //{
		 // //_worker.append();
	  //}

  protected:

  private:
      internal_classes_manager();



      rx::threads::physical_job_thread _worker;


};






class relations_hash_data 
{
	relations_hash_data(const relations_hash_data&) = delete;
	relations_hash_data(relations_hash_data&&) = delete;
	void operator=(const relations_hash_data&) = delete;
	void operator=(relations_hash_data&&) = delete;

	// these are mostly static data, so we keep it ordered to find quickly
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






template <class typeT>
class type_hash 
{
	type_hash(const type_hash&) = delete;
	type_hash(type_hash&&) = delete;
	void operator=(const type_hash&) = delete;
	void operator=(type_hash&&) = delete;

	typedef typename typeT::RType::smart_ptr RType;
	typedef typename typeT::smart_ptr Tptr;
	typedef typename constructors::object_constructor_base<RType,RType> constructorType;

	typedef typename std::map<rx_node_id, RType> registered_objects_type;
	typedef typename std::map<rx_node_id, Tptr> registered_classes_type;
	typedef typename std::map<rx_node_id, constructorType* > object_constructors_type;

  public:
      type_hash();

      virtual ~type_hash();


  protected:

  private:


      object_constructors_type _object_constructors;

      relations_hash_data _hash;


      registered_objects_type _registered_objects;

      registered_classes_type _registered_classes;


};


// Parameterized Class model::type_hash 

template <class typeT>
type_hash<typeT>::type_hash()
{
}


template <class typeT>
type_hash<typeT>::~type_hash()
{
}



} // namespace model



#endif
