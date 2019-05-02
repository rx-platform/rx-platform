

/****************************************************************************
*
*  model\rx_model_algorithms.h
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


#ifndef rx_model_algorithms_h
#define rx_model_algorithms_h 1



// rx_meta_internals
#include "model/rx_meta_internals.h"



namespace model {

namespace algorithms {






template <class typeT>
class types_model_algorithm 
{

  public:

      static void check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_object_ptr ref);

      static void create_type (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_object_ptr ref);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes);

      static void delete_type (const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_object_ptr ref);


  protected:

  private:

      static type_check_context check_type_sync (const string_type& name, rx_directory_ptr dir);

      static rx_result delete_type_sync (const string_type& name, rx_directory_ptr dir);



};






template <class typeT>
class simple_types_model_algorithm 
{

  public:

      static void check_type (const string_type& name, rx_directory_ptr dir, std::function<void(type_check_context)> callback, rx_object_ptr ref);

      static void create_type (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback, rx_object_ptr ref);

      static rx_result_with<typename typeT::smart_ptr> create_type_sync (const string_type& name, const string_type& base_name, typename typeT::smart_ptr prototype, rx_directory_ptr dir, namespace_item_attributes attributes);

      static void delete_type (const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_object_ptr ref);


  protected:

  private:

      static type_check_context check_type_sync (const string_type& name, rx_directory_ptr dir);

      static rx_result delete_type_sync (const string_type& name, rx_directory_ptr dir);



};






template <class typeT>
class runtime_model_algorithm 
{

  public:

      static void delete_runtime (const string_type& name, rx_directory_ptr dir, std::function<void(rx_result)> callback, rx_object_ptr ref);

      static void create_runtime (const string_type& name, const string_type& type_name, data::runtime_values_data* init_data, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_object_ptr ref);

      static rx_result_with<typename typeT::RTypePtr> create_runtime_sync (const string_type& name, const string_type& type_name, data::runtime_values_data* init_data, rx_directory_ptr dir, namespace_item_attributes attributes);

      static void create_prototype (const string_type& name, const string_type& type_name, rx_directory_ptr dir, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_object_ptr ref);


  protected:

  private:

      static rx_result delete_runtime_sync (const string_type& name, rx_directory_ptr dir);

      static rx_result_with<typename typeT::RTypePtr> create_prototype_sync (const string_type& name, const string_type& type_name, rx_directory_ptr dir, namespace_item_attributes attributes);



};


} // namespace algorithms
} // namespace model



#endif
