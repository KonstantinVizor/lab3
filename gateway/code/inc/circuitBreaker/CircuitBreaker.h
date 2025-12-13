#ifndef __CIRCUITBREAKER_H__
#define __CIRCUITBREAKER_H__

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <thread>
#include <string>

class CircuitBreaker
{
	public:
		enum class StateType
		{
			OPEN,
			CLOSE,
			HALF_OPEN
		};

	private:
		StateType _state;
		std::thread _knockKnockThread;
		Poco::Net::HTTPRequest _knockKnockRequest;
		int _timeout;
		Poco::Net::HTTPClientSession _client;

		void _knockKnockMethod();

	public:
		CircuitBreaker();
		CircuitBreaker& createClient(const std::string &host, uint16_t port);
		std::istream* send(Poco::Net::HTTPRequest &req, Poco::Net::HTTPResponse &resp, const std::string &json = "");
};

#endif
