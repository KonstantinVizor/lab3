#ifndef __JWTAUTH_H__
#define __JWTAUTH_H__

#include <string>

struct AuthClaims
{
	bool valid = false;
	std::string username;
	std::string role;
	std::string token;
};

// Validates a bearer JWT locally using the RSA public key cached by
// JwksClient (RS256 signature + exp check). No network call here.
class JwtAuth
{
	public:
		static AuthClaims validate(const std::string &authorizationHeader);
};

#endif
