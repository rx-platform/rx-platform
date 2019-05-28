

/****************************************************************************
*
*  runtime_internal\rx_runtime_internal.h
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


#ifndef rx_runtime_internal_h
#define rx_runtime_internal_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "rx_runtime_algorithms.h"


namespace sys_runtime {






class platform_runtime_manager 
{
	typedef std::map<rx_node_id, rx_application_ptr> applications_type;
	friend class algorithms::application_algorithms;

  public:

      static platform_runtime_manager& instance ();

	  template<class typeT>
	  rx_result init_runtime(typename typeT::RTypePtr what, runtime::runtime_init_context& ctx)
	  {
		  auto result = algorithms::init_runtime<typeT>(what, ctx);
		  return result;
	  }
  protected:

  private:


      applications_type applications_;


};


} // namespace sys_runtime



#endif
