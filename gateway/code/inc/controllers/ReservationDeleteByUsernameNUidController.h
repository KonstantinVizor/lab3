#ifndef __RESERVATIONDELETEBYUSERNAMECONTROLLER_H__
#define __RESERVATIONDELETEBYUSERNAMECONTROLLER_H__

#include <Poco/Net/HTTPRequestHandler.h>
#include <memory>
#include "../../inc/repositories/HotelRepository.h"
#include "../../inc/repositories/LoyaltyRepository.h"
#include "../../inc/repositories/PaymentRepository.h"
#include "../../inc/queue/QueueManager.h"
#include "../../inc/circuitBreaker/CircuitBreaker.h"

class ReservationDeleteByUsernameController : public Poco::Net::HTTPRequestHandler
{
	private:
		std::shared_ptr<HotelRepository> _hotelRepository;
		std::shared_ptr<LoyaltyRepository> _loyaltyRepository;
		std::shared_ptr<PaymentRepository> _paymentRepository;
		std::shared_ptr<QueueManager> _qManager;
		std::shared_ptr<CircuitBreaker> _loyaltyBreaker;

	public:
		ReservationDeleteByUsernameController() = default;
		explicit ReservationDeleteByUsernameController(const ReservationDeleteByUsernameController &) = delete;
		ReservationDeleteByUsernameController(ReservationDeleteByUsernameController &&) = delete;
		~ReservationDeleteByUsernameController() = default;
	
		ReservationDeleteByUsernameController(const std::shared_ptr<HotelRepository> &hotelRepository,
							const std::shared_ptr<LoyaltyRepository> &loyaltyRepository,
							const std::shared_ptr<PaymentRepository> &paymentRepository,
							const std::shared_ptr<QueueManager> &qManager,
							const std::shared_ptr<CircuitBreaker> &loayltyBreaker);

		virtual void handleRequest(Poco::Net::HTTPServerRequest &req,
						Poco::Net::HTTPServerResponse &resp) override;
};

#endif
