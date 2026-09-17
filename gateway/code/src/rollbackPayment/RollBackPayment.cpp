#include "../../inc/rollbackPayment/RollBackPayment.h"
#include "../../inc/auth/AuthContext.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

void RollBackPayment::rollback(const std::string &paymentUid, const std::shared_ptr<CircuitBreaker> &breaker)
{
	Poco::Net::HTTPResponse resp;
	Poco::Net::HTTPRequest req;
	req.setMethod("DELETE");
	req.setURI("/payment?payment_uid=" + paymentUid);
	req.set("Authorization", "Bearer " + AuthContext::token());
	breaker->send(req, resp);
}
