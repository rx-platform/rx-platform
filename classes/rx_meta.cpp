

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


} // namespace model
