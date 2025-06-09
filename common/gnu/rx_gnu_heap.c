

/****************************************************************************
*
*  common\gnu\rx_gnu_heap.c
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

#include "../rx_common.h"



volatile int g_init_locks = 0;
void enter_init_spin_lock()
{
	int32_t temp = __atomic_add_fetch(&g_init_locks, 1, __ATOMIC_SEQ_CST);
	while (temp > 1)
	{
		__atomic_add_fetch(&g_init_locks, -1, __ATOMIC_SEQ_CST);
		temp = __atomic_add_fetch(&g_init_locks, 1, __ATOMIC_SEQ_CST);
	}
}

void exit_init_spin_lock()
{
	__atomic_add_fetch(&g_init_locks, -1, __ATOMIC_SEQ_CST);
}



