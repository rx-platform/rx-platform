

/****************************************************************************
*
*  common\gnu\linux_crypt.c
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






#include "pch.h"

#include "common/rx_common.h"


extern SSL_CTX* g_ssl_ctx;

extern EVP_MD* g_sha1_evp;
extern EVP_MD* g_sha256_evp;

/*
const EVP_MD* algorithm_from_int(int alg)
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
		return EVP_sha256();
		break;
	case RX_HASH_SHA1:
		return EVP_sha1();
		break;
	default:
		// invalid algorithm
		return 0;
	}
}
*/

const EVP_MD* hash_algorithm_from_int(int alg)
{
	switch (alg)
	{
	case RX_HASH_SHA256:
		return g_sha256_evp;
		break;
	case RX_HASH_SHA1:
		return g_sha1_evp;
		break;
	default:
		// invalid algorithm
		return 0;
	}
}


RX_COMMON_API int rx_init_certificate_struct(rx_certificate_t* cert)
{
	return RX_ERROR;
}
RX_COMMON_API int rx_open_certificate_from_bytes(rx_certificate_t* cert, const uint8_t* data, size_t size)
{
	return RX_ERROR;
}
RX_COMMON_API int rx_open_certificate_from_thumb(rx_certificate_t* cert, const char* store, const uint8_t* thumb, size_t size)
{
	return RX_ERROR;
}

RX_COMMON_API int rx_certificate_get_principal_name(const rx_certificate_t* cert, string_value_struct* data)
{
	return RX_ERROR;
}
RX_COMMON_API int rx_is_valid_certificate(const rx_certificate_t* cert)
{
	return RX_ERROR;
}
RX_COMMON_API int rx_certificate_get_thumbprint(const rx_certificate_t* cert, bytes_value_struct* data)
{
	return RX_ERROR;
}
RX_COMMON_API int rx_certificate_get_bytes(const rx_certificate_t* cert, bytes_value_struct* data)
{
	return RX_ERROR;
}
RX_COMMON_API int rx_close_certificate(rx_certificate_t* cert)
{
	return RX_ERROR;
}





crypt_key_t rx_crypt_create_symetric_key(const void* data, size_t size, int alg, int mode)
{
	return 0;
}
RX_COMMON_API crypt_key_t rx_crypt_open_public_from_pem(const char* data)
{
    BIO* bio = BIO_new_mem_buf((const void*)data, -1);
    if (!bio)
        return NULL;

    // Try PKCS#1 format first ("-----BEGIN RSA PUBLIC KEY-----")
    RSA* rsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
    if (!rsa)
    {
        // Fall back to X.509 SubjectPublicKeyInfo format ("-----BEGIN PUBLIC KEY-----")
        BIO_reset(bio);
        EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
        if (pkey)
        {
            rsa = EVP_PKEY_get1_RSA(pkey);
            EVP_PKEY_free(pkey);
        }
    }
    BIO_free(bio);

    if (!rsa)
        return NULL;

    // DER-encode the RSA public key (PKCS#1 format) to store in the crypt_key_t struct
    int der_len = i2d_RSAPublicKey(rsa, NULL);
    if (der_len <= 0)
    {
        RSA_free(rsa);
        return NULL;
    }

    crypt_key_t result = (crypt_key_t)malloc(sizeof(struct _crypt_key_t));
    if (!result)
    {
        RSA_free(rsa);
        return NULL;
    }

    result->data = (uint8_t*)malloc((size_t)der_len);
    if (!result->data)
    {
        RSA_free(rsa);
        free(result);
        return NULL;
    }

    uint8_t* p = result->data;
    i2d_RSAPublicKey(rsa, &p);
    result->size = (uint32_t)der_len;
    RSA_free(rsa);

    return result;
}
int rx_crypt_set_IV(crypt_key_t key, const void* data)
{
	return RX_ERROR;
}
void rx_crypt_destroy_key(crypt_key_t key)
{
    if (key)
    {
        if (key->data)
            free(key->data);
        free(key);
    }
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
    const EVP_MD* algorithm = hash_algorithm_from_int(alg);
    if(!algorithm)
        return 0;

    crypt_hash_t ret = EVP_MD_CTX_new();
    if(ret==NULL)
        return 0;
    void* pkey = NULL;
    int key_len = 0;
    if(key!=NULL)
    {
        pkey=key->data;
        key_len=key->size;
    }
    int result = EVP_DigestInit(ret, algorithm);
    if(result!=1)
    {
        int err = ERR_get_error();

        EVP_MD_CTX_free(ret);
        return 0;
    }
	return ret;
}
void rx_crypt_destroy_hash(crypt_hash_t hash)
{
    EVP_MD_CTX_free(hash);
}
int rx_crypt_hash_data(crypt_hash_t hhash, const void* buffer, size_t size)
{
    if(EVP_DigestUpdate(hhash, buffer, size)!=1)
    {
        return RX_ERROR;
    }
	return RX_OK;
}
int rx_crypt_get_hash(crypt_hash_t hhash, void* buffer, size_t* size)
{
    if(EVP_DigestFinal(hhash, buffer, size)!=1)
        return RX_ERROR;

	return RX_OK;
}
int rx_crypt_rsa_verify_sha256(crypt_key_t key, const void* content, size_t content_size, const void* sig, size_t sig_size)
{
    if (!key || !key->data || key->size == 0)
        return RX_ERROR;

    // Reconstruct the RSA key from the stored DER-encoded PKCS#1 blob
    const uint8_t* p = key->data;
    RSA* rsa = d2i_RSAPublicKey(NULL, &p, (long)key->size);
    if (!rsa)
        return RX_ERROR;

    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey)
    {
        RSA_free(rsa);
        return RX_ERROR;
    }
    // EVP_PKEY_assign_RSA transfers ownership of rsa to pkey
    if (!EVP_PKEY_assign_RSA(pkey, rsa))
    {
        EVP_PKEY_free(pkey);
        RSA_free(rsa);
        return RX_ERROR;
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        EVP_PKEY_free(pkey);
        return RX_ERROR;
    }

    int ret = RX_ERROR;
    if (EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey) == 1
        && EVP_DigestVerifyUpdate(ctx, content, content_size) == 1
        && EVP_DigestVerifyFinal(ctx, (const unsigned char*)sig, sig_size) == 1)
    {
        ret = RX_OK;
    }

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);  // also frees rsa via assigned ownership
    return ret;
}



