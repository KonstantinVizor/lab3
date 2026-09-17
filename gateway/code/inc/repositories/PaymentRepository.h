#ifndef __PAYMENT_REPOSITORY_H__
#define __PAYMENT_REPOSITORY_H__

#include <memory>
#include <string>
#include "../models/PaymentInfo.h"
#include "../models/PaymentModel.h"
#include "../circuitBreaker/CircuitBreaker.h"

class PaymentRepository
{
	private:
		std::shared_ptr<CircuitBreaker> _breaker;
	
	public:
		~PaymentRepository() = default;
		PaymentRepository(const PaymentRepository &) = delete;
		PaymentRepository(PaymentRepository &&) = delete;
		PaymentRepository(const std::shared_ptr<CircuitBreaker> &breaker);

		PaymentInfo getByUid(const std::string &uid);
		void updateByUid(const std::string &uid, const PaymentModel &model);
		std::string create(const PaymentModel &model);

};

#endif
