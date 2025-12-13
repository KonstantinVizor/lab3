#include "../../inc/repositories/HotelRepository.h"
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/Net/NetException.h>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../inc/models/ReservationWithHotel.h"
#include "Poco/JSON/Array.h"
#include "Poco/Net/HTTPServerResponse.h"

HotelRepository::HotelRepository(const std::shared_ptr<CircuitBreaker> &breaker) :
								_breaker(breaker)
{}

PaginationResponce HotelRepository::getHotelAll(uint32_t offset, uint32_t limit)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	PaginationResponce result;
	std::string json = "";
	std::string tmp;
	req.setURI("/hotel/all?page=" + std::to_string(offset) + "&size=" + std::to_string(limit));
	req.setMethod("GET");
	std::istream *stream = _breaker->send(req, resp);
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
	while (*stream >> tmp)
		json += (tmp + ' ');
	result.fromJson(json);
	return result;
}

HotelResponce HotelRepository::getHotelByUid(const std::string &uid)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	HotelResponce result;
	std::string json = "";
	std::string tmp;
	req.setURI("/hotel?hotel_uid=" + uid);
	req.setMethod("GET");
	std::istream *stream = _breaker->send(req, resp);
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
	while (*stream >> tmp)
		json += (tmp + ' ');
	result.fromJson(json);
	return result;
}

std::vector<ReservationWithHotel> HotelRepository::getReservationsByUsername(const std::string &username)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	std::vector<ReservationWithHotel> result;
	std::string json = "", tmp;
	req.setURI("/reservation");
	req.set("X-User-Name", username);
	req.setMethod("GET");
	std::istream *stream = _breaker->send(req, resp);
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
	while (*stream >> tmp)
		json += (tmp + ' ');
	Poco::JSON::Parser parser;
	Poco::JSON::Array::Ptr arr = parser.parse(json).extract<Poco::JSON::Array::Ptr>();
	result = std::vector<ReservationWithHotel>(arr->size());
	for (uint32_t i = 0; i < arr->size(); i++)
		result[i].fromJson(arr->get(i).toString(), true);
	return result;
}

#include <iostream>

ReservationWithHotel HotelRepository::createReservation(const std::string &username, const CreateReservationRequest &request)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	ReservationWithHotel model;
	std::string json = "", tmp;
	req.setURI("/reservation");
	req.setMethod("POST");
	req.set("X-User-Name", username);
	req.setContentType("application/json");
	req.setContentLength(request.toJson().size());
	std::istream *stream = _breaker->send(req, resp, request.toJson());
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
	while (*stream >> tmp)
		json += (tmp + ' ');
	model.fromJson(json);
	return model;
}

void HotelRepository::updateReservation(const std::string &uid, const ReservationWithHotel &res)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	req.setURI("/reservation?reservation_uid=" + uid);
	req.setMethod("PATCH");
	req.setContentType("application/json");
	req.setContentLength(res.toJson().size());
	_breaker->send(req, resp, res.toJson());
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
}

void HotelRepository::deleteReservation(const std::string &uid)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	req.setURI("/reservation?reservation_uid=" + uid);
	req.setMethod("DELETE");
	req.setContentType("application/json");
	_breaker->send(req, resp);
	if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
		throw std::runtime_error("Unavailable");
}
