#ifndef __HEALTHCONTROLLER_H__
#define __HEALTHCONTROLLER_H__

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include <Poco/Net/HTTPRequestHandler.h>

class HealthController : public Poco::Net::HTTPRequestHandler
{
	public:
		HealthController() = default;
		explicit HealthController(const HealthController &) = delete;
		HealthController(HealthController &&) = delete;
		~HealthController() = default;

		virtual void handleRequest(Poco::Net::HTTPServerRequest &req,
						Poco::Net::HTTPServerResponse &resp) override;
};

#endif
