#ifndef __ROLLBACKPAYMENT_H__
#define __ROLLBACKPAYMENT_H__

#include <memory>
#include "../circuitBreaker/CircuitBreaker.h"

class RollBackPayment
{
	public:
		static void rollback(const std::string &paymentUid, const std::shared_ptr<CircuitBreaker> &breaker);
};

#endif
