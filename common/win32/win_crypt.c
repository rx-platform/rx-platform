

/****************************************************************************
*
*  common\win32\win_crypt.c
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




HCRYPTPROV hcrypt = 0;


ALG_ID algorithm_from_int(int alg)
{
	switch (alg)
	{
	case RX_SYMETRIC_AES128:
		return CALG_AES_128;
		break;
	case RX_SYMETRIC_AES192:
		return CALG_AES_192;
		break;
	case RX_SYMETRIC_AES256:
		return CALG_AES_256;
		break;
	case RX_HASH_SHA256:
		return CALG_SHA_256;
		break;
	case RX_HASH_SHA1:
		return CALG_SHA1;
		break;
	default:
		// invalid algorithm
		return 0;
	}
}

crypt_key_t rx_crypt_create_symetric_key(const void* data, size_t size, int alg, int mode)
{
	HCRYPTKEY hkey = 0;

	struct keyBlob
	{
		BLOBHEADER hdr;
		DWORD keySize;
		BYTE bytes[0x80];
	} blob;

	blob.hdr.bType = PLAINTEXTKEYBLOB;
	blob.hdr.bVersion = CUR_BLOB_VERSION;
	blob.hdr.reserved = 0;
	blob.hdr.aiKeyAlg = algorithm_from_int(alg);

	blob.keySize = (DWORD)size;
	memcpy(blob.bytes, data, size);

	DWORD err = 0;

	BOOL ret = CryptImportKey(hcrypt, (BYTE*)&blob, (DWORD)sizeof(blob), 0, 0, &hkey);
	if (ret)
	{
		DWORD dwMode = mode;
		ret = CryptSetKeyParam(hkey, KP_MODE, (BYTE*)&dwMode, 0);
		if (ret)
		{
			return hkey;
		}
		else
			CryptDestroyKey(hkey);
	}
	else
		err = GetLastError();
	return 0;
}

int rx_crypt_set_IV(crypt_key_t key, const void* data)
{
	if (CryptSetKeyParam(key, KP_IV, data, 0))
		return RX_OK;
	else
		return RX_ERROR;

}

void rx_crypt_destroy_key(crypt_key_t key)
{
	CryptDestroyKey(key);
}

int rx_crypt_decrypt(crypt_key_t key, const void* chiper, void* plain, size_t* size)
{
	memcpy(plain, chiper, *size);
	DWORD dwsize = (DWORD)(*size);
	if (CryptDecrypt(key, 0, FALSE, 0, plain, &dwsize))
	{
		*size = dwsize;
		return RX_OK;
	}
	return RX_ERROR;
}

int rx_crypt_gen_random(void* buffer, size_t size)
{
	if (CryptGenRandom(hcrypt, (DWORD)size, (BYTE*)buffer))
		return RX_OK;
	else
		return RX_ERROR;
}


crypt_hash_t rx_crypt_create_hash(crypt_key_t key, int alg)
{
	HCRYPTHASH hhash = 0;
	HMAC_INFO info;
	DWORD err = 0;

	BOOL ret = CryptCreateHash(hcrypt, CALG_HMAC, key, 0, &hhash);
	if (ret)
	{
		ZeroMemory(&info, sizeof(info));
		info.HashAlgid = algorithm_from_int(alg);
		ret = CryptSetHashParam(hhash, HP_HMAC_INFO, (BYTE*)&info, 0);
		if (ret)
		{
			return hhash;
		}
		else
		{
			err = GetLastError();
			CryptDestroyHash(hhash);
		}
	}
	else
	{
		err = GetLastError();
	}
	return 0;
}
void rx_crypt_destroy_hash(crypt_hash_t hash)
{
	CryptDestroyHash(hash);
}
int rx_crypt_hash_data(crypt_hash_t hhash, const void* buffer, size_t size)
{
	BOOL ret = CryptHashData(hhash, (BYTE*)buffer, (DWORD)size, 0);
	if (ret)
	{
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}

int rx_crypt_get_hash(crypt_hash_t hhash, void* buffer, size_t* size)
{
	DWORD len = (DWORD)*size;
	BOOL ret = CryptGetHashParam(hhash, HP_HASHVAL, (BYTE*)buffer, &len, 0);
	if (ret)
	{
		*size = len;
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}


