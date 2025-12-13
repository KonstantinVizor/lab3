#include "../../inc/controllers/HealthController.h"
#include "Poco/Net/HTTPServerResponse.h"

void HealthController::handleRequest([[maybe_unused]] Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
{
	resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK);
	resp.send();
}
