

/****************************************************************************
*
*  common\gnu\linux_crypt.c
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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

#include "common/rx_common.h"



crypt_key_t rx_crypt_create_symetric_key(const void* data, size_t size, int alg, int mode)
{
	return 0;
}
int rx_crypt_set_IV(crypt_key_t key, const void* data)
{
	return RX_ERROR;
}
void rx_crypt_destroy_key(crypt_key_t key)
{

}
int rx_crypt_decrypt(crypt_key_t key, const void* chiper, void* plain, size_t* size)
{
	return RX_ERROR;
}

int rx_crypt_gen_random(void* buffer, size_t size)
{
    int rnd = open("/dev/urandom", O_RDONLY);
    if(rnd < 0)
        return RX_ERROR;

    ssize_t result = read(rnd, buffer, size);
    if(result < 0)
    {
        return RX_ERROR;
    }

	return RX_OK;
}

crypt_hash_t rx_crypt_create_hash(crypt_key_t key, int alg)
{
	return 0;
}
void rx_crypt_destroy_hash(crypt_hash_t hash)
{

}
int rx_crypt_hash_data(crypt_hash_t hhash, const void* buffer, size_t size)
{
	return RX_ERROR;
}
int rx_crypt_get_hash(crypt_hash_t hhash, void* buffer, size_t* size)
{
	return RX_ERROR;
}


