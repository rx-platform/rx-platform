

/****************************************************************************
*
*  lib\rx_crypto.cpp
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


// rx_crypto
#include "lib/rx_crypto.h"



namespace rx {

namespace crypto {

// Class rx::crypto::cryptography_key 

cryptography_key::cryptography_key()
      : hkey_(0)
{
}


cryptography_key::~cryptography_key()
{
	if (hkey_)
		rx_crypt_destroy_key(hkey_);
}



bool cryptography_key::is_valid () const
{
	return hkey_ != 0;
}


// Class rx::crypto::cryptography_hash 

cryptography_hash::cryptography_hash()
      : hhash_(0)
{
}


cryptography_hash::~cryptography_hash()
{
	if (hhash_)
		rx_crypt_destroy_hash(hhash_);
}



bool cryptography_hash::is_valid () const
{
	return hhash_ != 0;
}


// Class rx::crypto::symetric_key 

symetric_key::symetric_key (const void* data, size_t size, int alg, int mode)
{
	hkey_ = rx_crypt_create_symetric_key(data, size, alg, mode);
}


symetric_key::~symetric_key()
{
}



} // namespace crypto
} // namespace rx

