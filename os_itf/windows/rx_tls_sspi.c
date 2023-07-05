

/****************************************************************************
*
*  os_itf\windows\rx_tls_sspi.c
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





#include "rx_win.h"

#include "os_itf/rx_ositf.h"
#include "common/rx_common.h"

#include <authz.h>

extern uint32_t tlc_max_token;

#define SPP_NAME ((LPSTR)"negotiate")

#define SEC_SUCCESS(Status) ((Status) >= 0)

 int rx_aquire_ntlm_credentials(rx_cred_t* cred, struct rx_time_struct_t* life_time, const char* user, const char* domain, const char* pass)
{
	return RX_ERROR;
}

int rx_aquire_cert_credentials(rx_cred_t* cred, struct rx_time_struct_t* life_time, rx_certificate_t* cert)
{

	TimeStamp lt;
	lt.QuadPart = 0;


	SCHANNEL_CRED cert_cred;
	memzero(&cert_cred, sizeof(cert_cred));
	cert_cred.dwVersion = SCHANNEL_CRED_VERSION;
	cert_cred.dwFlags = SCH_CRED_AUTO_CRED_VALIDATION;
	cert_cred.cCreds = 1;
	cert_cred.paCred = &cert->ctx;
	cert_cred.grbitEnabledProtocols = SP_PROT_TLS1_1_SERVER | SP_PROT_TLS1_2_SERVER | SP_PROT_TLS1_3_SERVER | SCH_USE_STRONG_CRYPTO;

	SECURITY_STATUS ss = AcquireCredentialsHandleA(NULL, SCHANNEL_NAME_A, SECPKG_CRED_BOTH, NULL, &cert_cred, NULL, NULL, &cred->handle, &lt);
	cred->buffer_size = tlc_max_token;

	if (ss == SEC_E_OK)
	{
		life_time->t_value = lt.QuadPart;
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}



int rx_init_auth_context(rx_auth_context_t* ctx)
{
	memzero(ctx, sizeof(rx_auth_context_t));
	return RX_OK;
}
int rx_is_auth_context_complete(rx_auth_context_t* ctx)
{
	return ctx->has_context;
}
int rx_deinit_auth_context(rx_auth_context_t* ctx)
{
	if (ctx->has_handle)
	{
		ctx->has_handle = 0;
		return SEC_SUCCESS(DeleteSecurityContext(&ctx->handle)) ? RX_OK : RX_ERROR;
	}
	else
	{
		return RX_OK;
	}
}

void handle_name(char* buff, const char* full_name)
{
	char* buff_temp_ptr = buff;

	const char* temp_ptr = strstr(full_name, "CN=");
	if (temp_ptr == NULL)
		temp_ptr = full_name;
	else
		temp_ptr += 3;

	strcpy(buff_temp_ptr, temp_ptr);

	while (*buff_temp_ptr != '\0'
		&& *buff_temp_ptr != ' '
		&& *buff_temp_ptr != '\t'
		&& *buff_temp_ptr != '\r'
		&& *buff_temp_ptr != '\n'
		)
	{
		buff_temp_ptr++;
	}
	*buff_temp_ptr = '\0';
}

int rx_accept_credentials(rx_cred_t* cred, rx_auth_context_t* ctx, const void* data, size_t size, void* out_data, size_t* out_size)
{
	SECURITY_STATUS    ss;
	SecBufferDesc      outSecBufDesc;
	SecBuffer          outSecBuf[1];
	SecBufferDesc      inSecBufDesc;
	SecBuffer          inSecBuf[1];

	// Prepare output buffer
	//
	outSecBufDesc.ulVersion = 0;
	outSecBufDesc.cBuffers = 1;
	outSecBufDesc.pBuffers = outSecBuf;
	outSecBuf[0].cbBuffer = 0;
	outSecBuf[0].BufferType = SECBUFFER_TOKEN;
	outSecBuf[0].pvBuffer = NULL;

	inSecBufDesc.ulVersion = 0;
	inSecBufDesc.cBuffers = 1;
	inSecBufDesc.pBuffers = inSecBuf;
	inSecBuf[0].cbBuffer = (DWORD)size;
	inSecBuf[0].BufferType = SECBUFFER_TOKEN;
	inSecBuf[0].pvBuffer = (void*)data;

	DWORD m_ctxtAttr;
	TimeStamp m_ctxtLifetime;

	DWORD dwSSPIFlags = ASC_REQ_SEQUENCE_DETECT |
		ASC_REQ_REPLAY_DETECT |
		ASC_REQ_CONFIDENTIALITY |
		ASC_REQ_EXTENDED_ERROR |
		ASC_REQ_ALLOCATE_MEMORY |
		ASC_REQ_STREAM |
		ASC_REQ_MUTUAL_AUTH;

	ss = AcceptSecurityContext(
		&cred->handle,
		ctx->has_handle ? &ctx->handle : NULL,
		&inSecBufDesc,
		// context requirements
		dwSSPIFlags, //ASC_REQ_CONNECTION,
		SECURITY_NETWORK_DREP, //SECURITY_NATIVE_DREP
		&ctx->handle,
		&outSecBufDesc,
		&m_ctxtAttr,
		&m_ctxtLifetime);

	printf("\r\n*** Accept sec handle called result = 0x%08x\r\n", ss);

	if (!SEC_SUCCESS(ss))
	{
		return RX_ERROR;
	}

	ctx->has_handle = TRUE;
	ctx->has_context = !((SEC_I_CONTINUE_NEEDED == ss) ||
		(SEC_I_COMPLETE_AND_CONTINUE == ss));

	*out_size = outSecBuf[0].cbBuffer;
	if (*out_size && outSecBuf[0].pvBuffer != out_data)
	{
		memmove(out_data, outSecBuf[0].pvBuffer, *out_size);
	}

	if (ctx->has_context)
	{
		SecPkgCredentials_NamesA names;

		names.sUserName = NULL;// ;
		*cred->name_buffer = '\0';
		auto ssret = QueryCredentialsAttributesA(&cred->handle, SECPKG_CRED_ATTR_NAMES, &names);
		if (ssret == SEC_E_OK && names.sUserName)
		{
			handle_name(cred->name_buffer, names.sUserName);
			printf("\r\n*** My User is: %s\r\n", cred->name_buffer);
		}

		names.sUserName = NULL;
		*ctx->name_buffer = '\0';
		ssret = QueryContextAttributesA(&ctx->handle, SECPKG_CRED_ATTR_NAMES, &names);
		if (ssret == SEC_E_OK && names.sUserName)
		{
			handle_name(ctx->name_buffer, names.sUserName);
			printf("\r\n*** His User is: %s\r\n", ctx->name_buffer);
		}
		SecPkgContext_SubjectAttributes subj;
		subj.AttributeInfo = NULL;
		PCERT_CONTEXT remote_ctx = NULL;
		ssret = QueryContextAttributesA(&ctx->handle, SECPKG_ATTR_REMOTE_CERT_CONTEXT, &remote_ctx);
		if (ssret == SEC_E_OK && remote_ctx)
		{
			BYTE pdata[0x100];
			DWORD cdata = sizeof(pdata);
			if (CertGetCertificateContextProperty(remote_ctx, CERT_HASH_PROP_ID, pdata, &cdata))
			{
				printf("\r\nHis hash is:");
				for (DWORD i = 0; i < cdata; i++)
				{
					printf("%02x", (int)pdata[i]);
				}
				printf("\r\n");
			}
		}
		
	}

	return RX_OK;

}

int rx_get_peer_certificate(rx_auth_context_t* ctx, rx_certificate_t* cert)
{
	PCERT_CONTEXT remote_ctx = NULL;
	SECURITY_STATUS ssret = QueryContextAttributesA(&ctx->handle, SECPKG_ATTR_REMOTE_CERT_CONTEXT, &remote_ctx);
	if (ssret == SEC_E_OK && remote_ctx)
	{
		cert->ctx = remote_ctx;
		return RX_OK;
	}
	else
	{
		rx_init_certificate_struct(cert);
		return RX_ERROR;
	}
}

int rx_decrypt_message(rx_auth_context_t* ctx, const void* data, size_t size, void* out_data, size_t* out_size)
{
	if (!ctx->has_context)
		return RX_ERROR;

	SECURITY_STATUS    ss;
	SecBufferDesc      outSecBufDesc;
	SecBuffer          outSecBuf[4];

	outSecBufDesc.ulVersion = 0;
	outSecBufDesc.cBuffers = sizeof(outSecBuf) / sizeof(outSecBuf[0]);
	outSecBufDesc.pBuffers = outSecBuf;
	memzero(outSecBuf, sizeof(outSecBuf));
	outSecBuf[0].cbBuffer = (DWORD)size;
	outSecBuf[0].BufferType = SECBUFFER_DATA;
	outSecBuf[0].pvBuffer = out_data;
	memcpy(out_data, data, size);

	DWORD fqop = 0;

	ss = DecryptMessage(&ctx->handle, &outSecBufDesc, 0, &fqop);
	if (!SEC_SUCCESS(ss))
	{
		return RX_ERROR;
	}

	SecBuffer* pDataBuffer = NULL;
	SecBuffer* pExtraBuffer = NULL;
	for (int i = 1; i < 4; i++)
	{
		if (pDataBuffer == NULL && outSecBuf[i].BufferType == SECBUFFER_DATA)
		{
			pDataBuffer = &outSecBuf[i];
		}
		if (pExtraBuffer == NULL && outSecBuf[i].BufferType == SECBUFFER_EXTRA)
		{
			pExtraBuffer = &outSecBuf[i];
		}
	}

	// Display or otherwise process the decrypted data.
	if (pDataBuffer && pDataBuffer->cbBuffer > 0)
	{
		*out_size = pDataBuffer->cbBuffer;
		memmove(out_data, pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
	}
	RX_ASSERT(pExtraBuffer == NULL);

	return RX_OK;

}
int rx_encrypt_message(rx_auth_context_t* ctx, const void* data, size_t size, size_t* processed_count, void* out_data, size_t* out_size)
{
	if (!ctx->has_context)
		return RX_ERROR;

	SECURITY_STATUS    ss;
	SecBufferDesc      outSecBufDesc;
	SecBuffer          outSecBuf[4];

	SecPkgContext_StreamSizes sizes;
	QueryContextAttributesA(&ctx->handle, SECPKG_ATTR_STREAM_SIZES, &sizes);

	size_t to_process = min(size, sizes.cbMaximumMessage);

	*processed_count = to_process;

	uint8_t* user_ptr = (uint8_t*)out_data + sizes.cbHeader;
	memcpy(user_ptr, data, to_process);

	size_t padding = to_process % sizes.cbBlockSize;
	/*for (size_t i = 0; i < padding; i++)
		user_ptr[to_process + i] = 0;
	to_process += padding;*/

	outSecBufDesc.ulVersion = 0;
	outSecBufDesc.cBuffers = sizeof(outSecBuf) / sizeof(outSecBuf[0]);
	outSecBufDesc.pBuffers = outSecBuf;
	memzero(outSecBuf, sizeof(outSecBuf));
	outSecBuf[0].BufferType = SECBUFFER_STREAM_HEADER;
	outSecBuf[0].cbBuffer = sizes.cbHeader;
	outSecBuf[0].pvBuffer = out_data;
	outSecBuf[1].BufferType = SECBUFFER_DATA;
	outSecBuf[1].cbBuffer = (uint32_t)to_process;
	outSecBuf[1].pvBuffer = user_ptr;
	outSecBuf[2].BufferType = SECBUFFER_STREAM_TRAILER;
	outSecBuf[2].cbBuffer = sizes.cbTrailer + (uint32_t)padding + 0x100;
	outSecBuf[2].pvBuffer = user_ptr + to_process;

	unsigned long seqNum = 0;

	ss = EncryptMessage(&ctx->handle, 0, &outSecBufDesc, seqNum);
	if (!SEC_SUCCESS(ss))
	{
		return RX_ERROR;
	}

	size_t OutBufferLen = outSecBuf[0].cbBuffer + outSecBuf[1].cbBuffer + outSecBuf[2].cbBuffer;

	// Display or otherwise process the decrypted data.
	if (OutBufferLen > 0)
	{
		*out_size = OutBufferLen;
	}

	return RX_OK;
}
int rx_release_credentials(rx_cred_t* cred)
{
	FreeCredentialsHandle(&cred->handle);
	return RX_OK;
}



