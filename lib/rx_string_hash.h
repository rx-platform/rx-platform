

/****************************************************************************
*
*  lib\rx_string_hash.h
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


#ifndef rx_string_hash_h
#define rx_string_hash_h 1


#include "rx_hashes.h"
#include "rx_lib.h"
#include "rx_const_size_vector.h"




namespace rx {

namespace rx_hashes {





class rx_string_hash 
{
	struct hash_item_t
	{
		const char* str_ptr;
		bool operator<(const hash_item_t& right) const noexcept
		{
			return (*str_ptr) < (*right.str_ptr);
		}
	};
	typedef std::map<hash_item_t, hash_handle_t> strings_hash_type;
	typedef std::map<hash_handle_t, string_type*> inv_strings_hash_type;
	typedef std::vector<string_type> values_type;

  public:
      ~rx_string_hash();


      static rx_string_hash& instance ();


  protected:

  private:
      rx_string_hash();

	  rx_string_hash(const rx_string_hash &right) = delete;
	  rx_string_hash & operator=(const rx_string_hash &right) = delete;

	  rx_string_hash(rx_string_hash &&right) = delete;
	  rx_string_hash & operator=(rx_string_hash &&right) = delete;

      strings_hash_type hash_;

      inv_strings_hash_type inv_hash_;

      values_type values_;


    friend struct hashed_string_type;
};







struct hashed_string_type 
{

      hashed_string_type (const string_type& right);

      hashed_string_type (const char* right);

  public:

	  hashed_string_type(string_type&& right) noexcept;

	  hashed_string_type() = default;
	  ~hashed_string_type() = default;
	  hashed_string_type(const hashed_string_type&) = default;
	  hashed_string_type(hashed_string_type&&) = default;
	  hashed_string_type& operator=(const hashed_string_type&) = default;
	  hashed_string_type& operator=(hashed_string_type&&) = default;
  protected:

  private:


      hash_handle_t handle_;

      static std::atomic<hash_handle_t> g_new_handle_;


};


} // namespace rx_hashes
} // namespace rx



#endif
