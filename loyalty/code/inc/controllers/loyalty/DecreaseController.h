#ifndef __LOYALITYDECREASECONTROLLER_H__
#define __LOYALITYDECREASECONTROLLER_H__

#include <Poco/Net/HTTPRequestHandler.h>
#include <memory>
#include "../../../inc/repositories/LoyaltyRepository.h"

namespace Loyalty
{
class DecreaseController : public Poco::Net::HTTPRequestHandler
{
	private:
		std::shared_ptr<LoyaltyRepository> _loyaltyRepository;

	public:
		DecreaseController() = default;
		explicit DecreaseController(const DecreaseController &) = delete;
		DecreaseController(DecreaseController &&) = delete;
		~DecreaseController() = default;
	
		DecreaseController(const std::shared_ptr<LoyaltyRepository> loyaltyRepository);

		virtual void handleRequest(Poco::Net::HTTPServerRequest &req,
						Poco::Net::HTTPServerResponse &resp) override;
};
};

#endif
