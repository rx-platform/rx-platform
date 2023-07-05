

/****************************************************************************
*
*  model\rx_model_dependencies.h
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


#ifndef rx_model_dependencies_h
#define rx_model_dependencies_h 1



// rx_ptr
#include "lib/rx_ptr.h"

#include "system/meta/rx_meta_attr_algorithm.h"
#include "system/meta/rx_queries.h"
#include "system/runtime/rx_runtime_helpers.h"
#include "system/meta/rx_obj_types.h"
#include "system/meta/rx_runtime_data.h"
using namespace rx_platform::meta::runtime_data;


namespace rx_internal {

namespace model {

namespace transactions {





class dependency_cache 
{
    typedef std::set<rx_node_id> cached_items;
    typedef std::map<rx_node_id, cached_items> cache_type;

  public:

      void add_dependency (const rx_node_id& id, const rx_node_id& from);

      void remove_item (const rx_node_id& id);

      rx_result fill_dependents (const rx_node_id& id, std::set<rx_node_id>& results);

      dependency_cache() = default;
      ~dependency_cache() = default;
      dependency_cache(const dependency_cache&) = delete;
      dependency_cache(dependency_cache&&) = delete;
      dependency_cache& operator=(const dependency_cache&) = delete;
      dependency_cache& operator=(dependency_cache&&) = delete;
  protected:

  private:

      void add_single_dependency (const rx_node_id& id, const rx_node_id& from);



      cache_type depend_of_;

      cache_type depend_from_;


};






class local_dependecy_builder : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(local_dependecy_builder);

    template<typename T>
    struct item_creation_data
    {
        T item;
        bool create;
        bool remove;
        bool save_result;
    };

    typedef std::map<string_type, item_creation_data<object_runtime_data> > objects_type;
    typedef std::map<string_type, item_creation_data<domain_runtime_data> > domains_type;
    typedef std::map<string_type, item_creation_data<port_runtime_data> > ports_type;
    typedef std::map<string_type, item_creation_data<application_runtime_data> > applications_type;

    typedef std::map<string_type, item_creation_data<rx_object_type_ptr> > object_types_type;
    typedef std::map<string_type, item_creation_data<rx_port_type_ptr> > port_types_type;
    typedef std::map<string_type, item_creation_data<rx_domain_type_ptr> > domain_types_type;
    typedef std::map<string_type, item_creation_data<rx_application_type_ptr> > application_types_type;
    ;
    typedef std::map<string_type, item_creation_data<struct_type_ptr> > struct_types_type;
    typedef std::map<string_type, item_creation_data<variable_type_ptr> > variable_types_type;

    enum class builder_phase
    {
        idle                = 0,
        deleting_objects    = 1,
        deleting_ports      = 2,
        deleting_domains    = 3,
        deleting_apps       = 4,
        deleting_types      = 5,
        deleting_directories = 6,
        building_directories = 7,
        building_types       = 8,
        building_runtimes   = 9,
        done                = 10
    };

    struct builder_state
    {
        builder_phase phase;
        objects_type::iterator objects_it;
        ports_type::iterator ports_it;
        domains_type::iterator domains_it;
        applications_type::iterator applications_it;
    };

    typedef std::set<string_type> directory_list_type;

    struct item_data
    {
        rx_item_type type;
    };

    typedef std::map<string_type, std::map<rx_node_id, item_data> > directories_chache_type;


  public:
      ~local_dependecy_builder();


      void add_runtime (const object_runtime_data& what, bool remove, bool create, bool save);

      void add_runtime (const domain_runtime_data& what, bool remove, bool create, bool save);

      void add_runtime (const port_runtime_data& what, bool remove, bool create, bool save);

      void add_runtime (const application_runtime_data& what, bool remove, bool create, bool save);

      void add_type (rx_application_type_ptr what, bool remove, bool create, bool save);

      void add_type (rx_domain_type_ptr what, bool remove, bool create, bool save);

      void add_type (rx_port_type_ptr what, bool remove, bool create, bool save);

      void add_type (rx_object_type_ptr what, bool remove, bool create, bool save);

      void add_sub_type (struct_type_ptr what, bool remove, bool create, bool save);

      void add_sub_type (variable_type_ptr what, bool remove, bool create, bool save);

      rx_result add_query_result (const api::query_result_detail& what, bool remove, bool create, bool save);

      void add_config_part (const meta::config_part_container& part);

      rx_result apply_items (rx_result_callback&& callback);


      const std::vector<rx_object_ptr>& get_built_objects () const
      {
        return built_objects_;
      }


      const std::vector<rx_domain_ptr>& get_built_domains () const
      {
        return built_domains_;
      }


      const std::vector<rx_port_ptr>& get_built_ports () const
      {
        return built_ports_;
      }


      const std::vector<rx_application_ptr>& get_built_apps () const
      {
        return built_apps_;
      }


      const std::vector<rx_object_type_ptr>& get_built_object_types () const
      {
        return built_object_types_;
      }


      const std::vector<rx_domain_type_ptr>& get_built_domain_types () const
      {
        return built_domain_types_;
      }


      const std::vector<rx_port_type_ptr>& get_built_port_types () const
      {
        return built_port_types_;
      }


      const std::vector<rx_application_type_ptr>& get_built_app_types () const
      {
        return built_app_types_;
      }


      const std::vector<struct_type_ptr>& get_built_struct_types () const
      {
        return built_struct_types_;
      }


      const std::vector<variable_type_ptr>& get_built_variable_types () const
      {
        return built_variable_types_;
      }


      template<typename typeT>
      typename typeT::RTypePtr extract_single_result();
  protected:

  private:

      rx_result delete_types ();

      rx_result delete_objects ();

      rx_result delete_ports ();

      rx_result delete_domains ();

      rx_result delete_apps ();

      rx_result delete_directories ();

      rx_result build_directories ();

      rx_result build_types ();

      rx_result build_runtimes ();

      void process (rx_result&& result);

      rx_result consolidate_meta_data (meta_data& new_data, const meta_data& old_data);

      rx_result consolidate_meta_data (meta_data& data);



      objects_type objects_;

      domains_type domains_;

      ports_type ports_;

      applications_type applications_;

      directory_list_type dirs_to_delete_;

      directory_list_type dirs_to_create_;

      builder_state state_;

      rx_result_callback callback_;

      std::vector<rx_object_ptr> built_objects_;

      std::vector<rx_domain_ptr> built_domains_;

      std::vector<rx_port_ptr> built_ports_;

      std::vector<rx_application_ptr> built_apps_;

      object_types_type object_types_;

      domain_types_type domain_types_;

      port_types_type port_types_;

      application_types_type application_types_;

      std::vector<rx_object_type_ptr> built_object_types_;

      std::vector<rx_domain_type_ptr> built_domain_types_;

      std::vector<rx_port_type_ptr> built_port_types_;

      std::vector<rx_application_type_ptr> built_app_types_;

      struct_types_type struct_types_;

      variable_types_type variable_types_;

      std::vector<struct_type_ptr> built_struct_types_;

      std::vector<variable_type_ptr> built_variable_types_;


      template<typename typeT>
      using container_t = std::map<string_type, item_creation_data<typename typeT::smart_ptr> >;

      template<typename typeT>
      using rt_container_t = std::map<string_type, item_creation_data<typename typeT::instance_data_t> >;

      template<typename T>
      void do_consolidate_for_item(T& container);

      template<typename T, typename dataT>
      void add_runtime_concrete(T& container, const dataT& what, bool remove, bool create, bool save);

      template<typename T, typename dataT>
      void add_type_concrete(T& container, const dataT& what, bool remove, bool create, bool save);

      template<typename typeT>
      rx_result add_query_runtime_concrete(rt_container_t<typeT>& container, const api::query_result_detail& what, bool remove, bool create, bool save);

      template<typename typeT>
      rx_result add_query_type_concrete(container_t<typeT>&, const api::query_result_detail& what, bool remove, bool create, bool save);

      template<typename typeT>
      rx_result add_query_simple_type_concrete(container_t<typeT>&, const api::query_result_detail& what, bool remove, bool create, bool save);

      template<typename typeT>
      rx_result create_types(container_t<typeT>& data, std::vector<typename typeT::smart_ptr>& built);
      template<typename typeT>
      rx_result delete_types(container_t<typeT>& data);

      template<typename typeT>
      rx_result create_simple_types(container_t<typeT>& data, std::vector<typename typeT::smart_ptr>& built);
      template<typename typeT>
      rx_result delete_simple_types(container_t<typeT>& data);

      template<typename T>
      void do_consolidate_for_types(T& container);
};


} // namespace transactions
} // namespace model
} // namespace rx_internal



#endif
