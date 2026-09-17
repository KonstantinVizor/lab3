#ifndef __JWKSCLIENT_H__
#define __JWKSCLIENT_H__

#include <openssl/evp.h>

// Fetches the Identity Provider's JWKS exactly once (with a few retries
// in case the IdP is still starting up) and caches the resulting RSA
// public key in memory. No request is made to the Identity Provider
// per incoming request afterwards.
class JwksClient
{
	public:
		static EVP_PKEY *getKey();
};

#endif
