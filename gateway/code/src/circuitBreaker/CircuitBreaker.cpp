#include "../../inc/circuitBreaker/CircuitBreaker.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/NetException.h"
#include <chrono>
#include <istream>
#include <iostream>
#include <thread>

CircuitBreaker::CircuitBreaker() :
			_state(StateType::CLOSE),
			_timeout(10)
{
	_knockKnockRequest.setURI("/manage/health");
	_knockKnockRequest.setMethod("GET");
}

void CircuitBreaker::_knockKnockMethod()
{
	Poco::Net::HTTPResponse resp;
	while (_state != StateType::CLOSE)
	{
		std::this_thread::sleep_for(std::chrono::seconds(_timeout));
		_state = StateType::HALF_OPEN;
		try
		{
			_client.sendRequest(_knockKnockRequest);
			_client.receiveResponse(resp);
			if (resp.getStatus() == Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK)
				_state = StateType::CLOSE;
			else
				_state = StateType::OPEN;
		}
		catch (Poco::Net::NetException &e)
		{
			std::cerr << "Снова отвалился\n";
			_state = StateType::OPEN;
		}
	}
}

CircuitBreaker& CircuitBreaker::createClient(const std::string &host, uint16_t port)
{
	_client.setHost(host);
	_client.setPort(port);
	return *this;
}

std::istream* CircuitBreaker::send(Poco::Net::HTTPRequest &req, Poco::Net::HTTPResponse &resp, const std::string &json)
{
	std::istream *resultStream = NULL;
	if (_state == StateType::CLOSE)
	{
		try
		{
			_client.sendRequest(req) << json;
			resultStream = &_client.receiveResponse(resp);
		}
		catch (Poco::Net::NetException &e)
		{
			std::cerr << e.displayText() << "\n";
			std::cerr << "АААААА, отвалилось\n";
			_state = StateType::OPEN;
			if (!_knockKnockThread.joinable())
			{
				_knockKnockThread = std::thread(&CircuitBreaker::_knockKnockMethod, this);
				_knockKnockThread.detach();
			}
			resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE);
			resp.setReason("Service Unavailable");
		}
	}
	else
	{
		std::cerr << "Не пущу\n";
		resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE);
		resp.setReason("Service Unavailable");
	}
	return resultStream;
}
