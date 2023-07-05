

/****************************************************************************
*
*  common\win32\win_crypt.c
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




HCERTSTORE hcert_store = 0;

RX_COMMON_API int rx_init_certificate_struct(rx_certificate_t* cert)
{
	memzero(cert, sizeof(rx_certificate_t));
	return RX_OK;
}

RX_COMMON_API int rx_open_certificate_from_bytes(rx_certificate_t* cert, const uint8_t* data, size_t size)
{
	PCCERT_CONTEXT ctx = CertCreateCertificateContext(X509_ASN_ENCODING, data, (DWORD)size);
	if (ctx)
	{
		cert->ctx = ctx;
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}
RX_COMMON_API int rx_open_certificate_from_thumb(rx_certificate_t* cert, const char* store, const uint8_t* thumb, size_t size)
{

	PCCERT_CONTEXT pCertContext = NULL;

	memzero(cert, sizeof(rx_certificate_t));

	if (hcert_store == NULL)
		return RX_ERROR;


	CRYPT_HASH_BLOB hash = { (DWORD)size, (uint8_t*)thumb };

	pCertContext = CertFindCertificateInStore(hcert_store,
		X509_ASN_ENCODING,
		0,
		CERT_FIND_HASH, //CERT_FIND_CERT_ID, //CERT_FIND_SUBJECT_STR,
		&hash, //&id, //L"dusan.r.ciric",
		NULL);

	if (pCertContext == NULL)
	{
		return RX_ERROR;
	}

	cert->ctx = pCertContext;

	return RX_OK;

}

RX_COMMON_API int rx_certificate_get_principal_name(const rx_certificate_t* cert, string_value_struct* data)
{
	char buff2[0x100];
	DWORD cc = CertGetNameStringA(cert->ctx, CERT_NAME_UPN_TYPE, 0, 0, NULL, 0);
	if (cc > 0 && CertGetNameStringA(cert->ctx, CERT_NAME_UPN_TYPE, 0, 0, buff2, cc) > 0 && buff2[0] != '\0')
	{
		return rx_init_string_value_struct(data, buff2, -1);
	}
	DWORD type = CERT_X500_NAME_STR| CERT_NAME_STR_REVERSE_FLAG;//szOID_DOMAIN_COMPONENT
	cc = CertGetNameStringA(cert->ctx, CERT_NAME_RDN_TYPE, CERT_NAME_ISSUER_FLAG, &type, NULL, 0);
	if (CertGetNameStringA(cert->ctx, CERT_NAME_RDN_TYPE, CERT_NAME_ISSUER_FLAG, &type , buff2, cc) > 0 && buff2[0] != '\0')
	{
		char buffdc[0x100];
		char* dcptr = buff2;
		char* result_ptr = NULL;
		char* temp_ptr;
		while (dcptr != NULL)
		{
			dcptr = strstr(dcptr, "DC=");
			if (dcptr)
			{
				dcptr += 3;
				temp_ptr = dcptr;
				dcptr = strstr(dcptr, ", ");
				size_t comp_size = 0;
				if (dcptr)
				{
					comp_size = dcptr - temp_ptr;
					dcptr += 2;
				}
				else
				{
					comp_size = strlen(temp_ptr);
				}
				if (comp_size > 0)
				{
					if (result_ptr != NULL)
					{
						*result_ptr = '.';
						result_ptr++;
					}
					else
					{
						result_ptr = buffdc;
					}
					memcpy(result_ptr, temp_ptr, comp_size);
					result_ptr += comp_size;
				}
			}
		}
		if (result_ptr)
		{
			*result_ptr = '\0';
		}
		else
		{
			cc = CertGetNameStringA(cert->ctx, CERT_NAME_DNS_TYPE, CERT_NAME_ISSUER_FLAG, 0, NULL, 0);
			if (CertGetNameStringA(cert->ctx, CERT_NAME_DNS_TYPE, CERT_NAME_ISSUER_FLAG, 0, buffdc, cc) > 0 && buffdc[0] != '\0')
			{
			}
			else
			{
				return RX_ERROR;
			}
		}

		cc = CertGetNameStringA(cert->ctx, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, NULL, 0);
		if (CertGetNameStringA(cert->ctx, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, buff2, cc) > 0 && buff2[0] != '\0')
		{
			strcat(buff2, "@");
			strcat(buff2, buffdc);
			return rx_init_string_value_struct(data, buff2, -1);
		}
		else
		{
			return RX_ERROR;
		}
		return rx_init_string_value_struct(data, buff2, -1);
	}
	return RX_ERROR;
}


RX_COMMON_API int rx_certificate_get_thumbprint(const rx_certificate_t* cert, bytes_value_struct* data)
{
	BYTE pdata[0x100];
	DWORD cdata = sizeof(pdata);

	if (CertGetCertificateContextProperty(cert->ctx, CERT_HASH_PROP_ID, pdata, &cdata))
	{
		return rx_init_bytes_value_struct(data, pdata, cdata);
	}
	return RX_ERROR;
}
RX_COMMON_API int rx_certificate_get_bytes(const rx_certificate_t* cert, bytes_value_struct* data)
{
	return rx_init_bytes_value_struct(data, cert->ctx->pbCertEncoded, cert->ctx->cbCertEncoded);
}
RX_COMMON_API int rx_is_valid_certificate(const rx_certificate_t* cert)
{
	return cert->ctx != NULL ? 1 : 0;
}
RX_COMMON_API int rx_close_certificate(rx_certificate_t* cert)
{
	int ret = CertFreeCertificateContext(cert->ctx);
	if (ret)
	{
		memzero(cert, sizeof(rx_certificate_t));
	}
	return ret != 0 ? RX_OK : RX_ERROR;
}



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


