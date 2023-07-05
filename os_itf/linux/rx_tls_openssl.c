

/****************************************************************************
*
*  os_itf\linux\rx_tls_openssl.c
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



#include "os_itf/linux/rx_linux.h"
#include "os_itf/rx_ositf.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPENSSL TLS implementation


#define DEFAULT_BUF_SIZE 64

enum sslstatus { SSLSTATUS_OK, SSLSTATUS_WANT_IO, SSLSTATUS_FAIL };

static enum sslstatus get_sslstatus(SSL* ssl, int n)
{
	switch (SSL_get_error(ssl, n))
	{
	case SSL_ERROR_NONE:
		return SSLSTATUS_OK;
	case SSL_ERROR_WANT_WRITE:
		return SSLSTATUS_WANT_IO;
	case SSL_ERROR_WANT_READ:
		return SSLSTATUS_WANT_IO;
	case SSL_ERROR_ZERO_RETURN:
	case SSL_ERROR_SYSCALL:
	default:
		return SSLSTATUS_FAIL;
	}
}

SSL_CTX* g_ssl_ctx = NULL;



void handle_error(const char* file, int lineno, const char* msg) {
    fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
    ERR_print_errors_fp(stderr);
    exit(1);
}

#define int_error(msg) handle_error(__FILE__, __LINE__, msg)


int rx_aquire_cert_credentials(rx_cred_t* cred, struct rx_time_struct_t* life_time, rx_certificate_t* cert)
{

    /* create the SSL server context */
    cred->ssl_ctx = g_ssl_ctx;


    cred->buffer_size = 0x4000;
	return RX_OK;
}


int rx_init_auth_context(rx_auth_context_t* ctx)
{

	memzero(ctx, sizeof(rx_auth_context_t));


	return RX_OK;
}
int rx_is_auth_context_complete(rx_auth_context_t* ctx)
{
    return ctx->init_finished;
}
int rx_deinit_auth_context(rx_auth_context_t* ctx)
{
	if (ctx->ssl)
		SSL_free(ctx->ssl);   /* free the SSL object and its BIO's */
	return RX_OK;
}



int rx_accept_credentials(rx_cred_t* cred, rx_auth_context_t* ctx, const void* data, size_t size, void* out_data, size_t* out_size)
{
	int n = 0;

	if(ctx->ssl == NULL)
	{
        ctx->rbio = BIO_new(BIO_s_mem());
        ctx->wbio = BIO_new(BIO_s_mem());

        ctx->ssl = SSL_new(cred->ssl_ctx);

        SSL_set_accept_state(ctx->ssl); /* sets ssl to work in server mode. */
        SSL_set_bio(ctx->ssl, ctx->rbio, ctx->wbio);


/* Load certificate and private key files, and check consistency  */
        int err;
        err = SSL_use_certificate_file(ctx->ssl, "server.crt", SSL_FILETYPE_PEM);
        if (err != 1)
            int_error("SSL_CTX_use_certificate_file failed");


        /* Indicate the key file to be used */
        err = SSL_use_PrivateKey_file(ctx->ssl, "server.key", SSL_FILETYPE_PEM);
        if (err != 1)
            int_error("SSL_CTX_use_PrivateKey_file failed");


        /* Make sure the key and certificate file match. */
        if (SSL_check_private_key(ctx->ssl) != 1)
            int_error("SSL_CTX_check_private_key failed");

	}

	const uint8_t* pin_buffer = data;
	int to_read = (int)size;


	while (to_read > 0)
	{
		n = BIO_write(ctx->rbio, pin_buffer, to_read);
		if (n < 0)
		{
            printf("Ispao Accept 1 \r\n");
			return RX_ERROR;
        }
		if (n > 0)
		{
			to_read -= n;
			pin_buffer += n;

			n = SSL_accept(ctx->ssl);

			int sserr = SSL_get_error(ctx->ssl, n);

			enum sslstatus status = get_sslstatus(ctx->ssl, n);

			uint8_t* pbuffer = out_data;

			size_t out_count = 0;

			/* Did SSL request to write bytes? */
			if (n==1 || status == SSLSTATUS_WANT_IO)
			{
				do {
					n = BIO_read(ctx->wbio, pbuffer + out_count, 0x100);
					if (n > 0)
					{
						out_count += n;
					}
					else if (!BIO_should_retry(ctx->wbio))
					{
                        printf("Ispao Accept 2 \r\n");
						return RX_ERROR;
					}
				} while (n > 0);
			}



			if (status == SSLSTATUS_FAIL)
            {
                printf("Ispao Accept 3 : %d \r\n", sserr);
                return RX_ERROR;
            }

            *out_size=out_count;
		}
	}


	if (SSL_is_init_finished(ctx->ssl))
	{
		ctx->init_finished = 1;
	}


	return RX_OK;
}

int rx_decrypt_message(rx_auth_context_t* ctx, const void* data, size_t size, void* out_data, size_t* out_size)
{
    int n = 0;


	const uint8_t* pin_buffer = data;
	int to_read = (int)size;

    size_t out_count = 0;
    uint8_t* pbuffer = out_data;

	while (to_read > 0)
	{
		n = BIO_write(ctx->rbio, pin_buffer, to_read);
		if (n < 0)
        {
            printf("Ispao Decryipt \r\n");
            return RX_ERROR;
        }
		if (n > 0)
		{
			to_read -= n;
			pin_buffer += n;



			do
			{
                n=SSL_read(ctx->ssl, pbuffer + out_count, 0x100);
                if(n>0)
                {
                    out_count+=n;
                }
			} while(n>0);

        }
	}

    *out_size=out_count;


	return RX_OK;
}
int rx_encrypt_message(rx_auth_context_t* ctx, const void* data, size_t size, size_t* processed_count, void* out_data, size_t* out_size)
{
	int n = 0;


	size=min(size, 16384);


	const uint8_t* pin_buffer = data;
	int to_write = (int)size;

    size_t out_count = 0;
    uint8_t* pbuffer = out_data;

	while (to_write > 0)
	{
		n = SSL_write(ctx->ssl, pin_buffer, to_write);
		if (n < 0)
        {
            printf("Ispao Encrypt \r\n");
            return RX_ERROR;
        }
		if (n > 0)
		{
			to_write -= n;
			pin_buffer += n;


			do
			{
                n=BIO_read(ctx->wbio, pbuffer + out_count, 0x100);
                if(n>0)
                {
                    out_count+=n;
                }
			} while(n>0);

        }
	}

    *out_size=out_count;
    *processed_count=size;


	return RX_OK;
}
int rx_release_credentials(rx_cred_t* cred)
{
	return RX_OK;
}

int rx_get_peer_certificate(rx_auth_context_t* ctx, rx_certificate_t* cert)
{
	return RX_ERROR;
}



