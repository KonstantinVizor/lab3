#include "../../inc/repositories/PaymentRepository.h"
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <exception>
#include <memory>
#include "../../inc/uri/Uri.h"

PaymentRepository::PaymentRepository(const std::shared_ptr<CircuitBreaker> &breaker) :
							_breaker(breaker)
{}

PaymentInfo PaymentRepository::getByUid(const std::string &uid)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	PaymentModel model;
	PaymentInfo info;
	std::string json = "", tmp;
	req.setURI("/payment?payment_uid=" + uid);
	req.setMethod("GET");
	std::istream *stream = _breaker->send(req, resp);
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
	while (*stream >> tmp)
		json += tmp;
	model.fromJson(json, true);
	info.setStatus(model.getStatus())
		.setPrice(model.getPrice());
	return info;
}

void PaymentRepository::updateByUid(const std::string &uid, const PaymentModel &model)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	req.setURI("/payment?payment_uid=" + uid);
	req.setContentType("application/json");
	req.setContentLength(model.toJson().size());
	req.setMethod("PATCH");
	_breaker->send(req, resp, model.toJson());
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
}

#include <iostream>

std::string PaymentRepository::create(const PaymentModel &model)
{
	try
	{
		Poco::Net::HTTPResponse resp;
		Poco::Net::HTTPRequest req;
		req.setURI("/payment");
		req.setMethod("POST");
		req.setContentType("application/json");
		req.setContentLength(model.toJson().size());
		std::cout << "\t" << __LINE__ << "\n";
		_breaker->send(req, resp, model.toJson());
		std::cout << "\t" << __LINE__ << "\n";
		if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
			throw std::runtime_error("Unavailable");
		std::cout << "\t" << __LINE__ << "\n";
		std::cout << resp.getStatus() << " " << resp.getReason() << "\n";
		Uri uri(resp.get("location"));
		return uri.getValue("payment_uid");
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << "\n";
	}
	return "";
}
