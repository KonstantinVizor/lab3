#ifndef __AUTHCONTEXT_H__
#define __AUTHCONTEXT_H__

#include <string>

// Populated by ControllerFactory for the duration of a single request,
// on the same thread that runs the matched handler (Poco calls
// createRequestHandler() and handleRequest() synchronously on one
// pooled thread per connection), so thread_local storage is safe here.
class AuthContext
{
	public:
		static void set(const std::string &username, const std::string &role, const std::string &token);
		static void clear();
		static const std::string &username();
		static const std::string &role();
		static const std::string &token();
};

#endif
