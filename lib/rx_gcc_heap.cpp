

/****************************************************************************
*
*  lib\rx_heap.cpp
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





#include <new>

extern "C++"
{


#ifndef _DEBUG


_GLIBCXX_NODISCARD void* operator new(std::size_t sz) _GLIBCXX_THROW(std::bad_alloc)
{
    return rx_heap_alloc(sz);
}
_GLIBCXX_NODISCARD void* operator new[](std::size_t sz) _GLIBCXX_THROW(std::bad_alloc)
{
    return rx_heap_alloc(sz);
}
void operator delete(void* p) _GLIBCXX_USE_NOEXCEPT
{
    rx_heap_free(p);
}
void operator delete[](void* p) _GLIBCXX_USE_NOEXCEPT
{
    rx_heap_free(p);
}


_GLIBCXX_NODISCARD void* operator new(std::size_t sz, const std::nothrow_t&) _GLIBCXX_USE_NOEXCEPT
{
    return rx_heap_alloc(sz);
}
_GLIBCXX_NODISCARD void* operator new[](std::size_t sz, const std::nothrow_t&) _GLIBCXX_USE_NOEXCEPT
{
    return rx_heap_alloc(sz);
}
void operator delete(void* p, const std::nothrow_t&) _GLIBCXX_USE_NOEXCEPT
{
    rx_heap_free(p);
}
void operator delete[](void* p, const std::nothrow_t&) _GLIBCXX_USE_NOEXCEPT
{
    rx_heap_free(p);
}

#endif // _DEBUG

}


