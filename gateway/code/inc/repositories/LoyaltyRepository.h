#ifndef __LOYALTY_REPOSITORY_H__
#define __LOYALTY_REPOSITORY_H__

#include <memory>
#include <string>
#include "../models/LoyaltyInfo.h"
#include "../models/LoyaltyModel.h"
#include "../circuitBreaker/CircuitBreaker.h"

class LoyaltyRepository
{
	private:
		std::shared_ptr<CircuitBreaker> _breaker;
	
	public:
		~LoyaltyRepository() = default;
		LoyaltyRepository(const LoyaltyRepository &) = delete;
		LoyaltyRepository(LoyaltyRepository &&) = delete;
		LoyaltyRepository(const std::shared_ptr<CircuitBreaker> &breaker);

		LoyaltyInfo getByUsername(const std::string &username);
		void updateByUsername(const std::string &username, const LoyaltyModel &model);
		void decrease(const std::string &username);

};

#endif
