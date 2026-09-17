#include "../../inc/auth/AuthContext.h"

namespace
{
	thread_local std::string tUsername;
	thread_local std::string tRole;
	thread_local std::string tToken;
}

void AuthContext::set(const std::string &username, const std::string &role, const std::string &token)
{
	tUsername = username;
	tRole = role;
	tToken = token;
}

void AuthContext::clear()
{
	tUsername.clear();
	tRole.clear();
	tToken.clear();
}

const std::string &AuthContext::username() { return tUsername; }
const std::string &AuthContext::role() { return tRole; }
const std::string &AuthContext::token() { return tToken; }
