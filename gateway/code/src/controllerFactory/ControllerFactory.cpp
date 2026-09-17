#include "../../inc/controllerFactory/ControllerFactory.h"
#include "../../inc/auth/JwtAuth.h"
#include "../../inc/auth/AuthContext.h"
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <regex>

namespace
{

void setCorsHeaders(Poco::Net::HTTPServerResponse &response)
{
	response.set("Access-Control-Allow-Origin", "*");
	response.set("Access-Control-Allow-Headers", "Authorization, Content-Type");
	response.set("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
}

class CorsPreflightHandler : public Poco::Net::HTTPRequestHandler
{
	public:
		void handleRequest(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &response) override
		{
			setCorsHeaders(response);
			response.setStatus(Poco::Net::HTTPServerResponse::HTTP_NO_CONTENT);
			response.send();
		}
};

class UnauthorizedHandler : public Poco::Net::HTTPRequestHandler
{
	public:
		void handleRequest(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &response) override
		{
			response.setStatus(Poco::Net::HTTPServerResponse::HTTP_UNAUTHORIZED);
			response.setContentType("application/json");
			response.send() << "{\"message\":\"unauthorized\"}";
		}
};

class CorsWrapperHandler : public Poco::Net::HTTPRequestHandler
{
	private:
		Poco::Net::HTTPRequestHandler *_inner;
	public:
		explicit CorsWrapperHandler(Poco::Net::HTTPRequestHandler *inner) : _inner(inner) {}
		~CorsWrapperHandler() override { delete _inner; }
		void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) override
		{
			setCorsHeaders(resp);
			if (_inner)
				_inner->handleRequest(req, resp);
		}
};

}

ControllerFactory::ControllerFactory()
{
	_regexMap = {{R"((\d{1,}))", std::regex(R"(\{i\})")},
			{R"(.*)", std::regex(R"(\{s\})")}};
}

ControllerFactory& ControllerFactory::registerHandler(const std::string &path, const std::string &method,
							const HandlerCreator &creator, bool requireAuth)
{
	std::string resultPath = path;
	for (RegexMap::value_type &pair : _regexMap)
		resultPath = std::regex_replace(resultPath, pair.second, pair.first);
	_creators.push_back({resultPath, method, creator, requireAuth});
	return *this;
}

ControllerFactory& ControllerFactory::registerDefaultHandler(const HandlerCreator &creator)
{
	_defaultCreator = creator;
	return *this;
}

ControllerFactory::HandlerPtr ControllerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &req)
{
	AuthContext::clear();

	if (req.getMethod() == "OPTIONS")
		return new CorsPreflightHandler();

	std::string path = "";
	for (int i = 0; i < static_cast<int>(req.getURI().size()) &&
			req.getURI()[i] != '?' && req.getURI()[i] != '#'; i++)
		path += req.getURI()[i];

	bool found = false;
	HandlerCreator matchedCreator;
	bool requireAuth = false;

	for (HandlerCreatorMap::value_type &tuple : _creators)
		if (std::regex_match(path, std::regex(std::get<0>(tuple))) &&
				req.getMethod() == std::get<1>(tuple))
		{
			found = true;
			matchedCreator = std::get<2>(tuple);
			requireAuth = std::get<3>(tuple);
		}

	if (!found)
		return new CorsWrapperHandler(_defaultCreator());

	if (requireAuth)
	{
		AuthClaims claims = JwtAuth::validate(req.get("Authorization", ""));
		if (!claims.valid)
			return new CorsWrapperHandler(new UnauthorizedHandler());
		AuthContext::set(claims.username, claims.role, claims.token);
	}

	return new CorsWrapperHandler(matchedCreator());
}
