#include "../../inc/repositories/LoyaltyRepository.h"
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <memory>

LoyaltyRepository::LoyaltyRepository(const std::shared_ptr<CircuitBreaker> &breaker) :
									_breaker(breaker)
{}

LoyaltyInfo LoyaltyRepository::getByUsername(const std::string &username)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	LoyaltyModel model;
	LoyaltyInfo info;
	std::string json = "", tmp;
	req.setURI("/loyalty");
	req.set("X-User-Name", username);
	req.setMethod("GET");
	std::istream *stream = _breaker->send(req, resp);
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
	while (*stream >> tmp)
		json += tmp;
	model.fromJson(json, true);
	info = LoyaltyInfo(model);
	return info;
}

#include <iostream>

void LoyaltyRepository::updateByUsername(const std::string &username, const LoyaltyModel &model)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	std::string uri = "/loyalty";
	req.setURI(uri);
	req.set("X-User-Name", username);
	req.setMethod("PATCH");
	req.setContentType("application/json");
	req.setContentLength(model.toJson().size());
	_breaker->send(req, resp, model.toJson());
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
}
