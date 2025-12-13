#include "../inc/controllers/ReservationDeleteByUsernameNUidController.h"
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include "../inc/models/ReservationWithHotel.h"
#include "../../inc/uri/Uri.h"

ReservationDeleteByUsernameController::ReservationDeleteByUsernameController(const std::shared_ptr<HotelRepository> &hotelRepository,
										const std::shared_ptr<LoyaltyRepository> &loyaltyRepository,
										const std::shared_ptr<PaymentRepository> &paymentRepository,
										const std::shared_ptr<QueueManager> &qManager,
										const std::shared_ptr<CircuitBreaker> &loyaltyBreaker) :
															Poco::Net::HTTPRequestHandler(),
															_hotelRepository(hotelRepository),
															_loyaltyRepository(loyaltyRepository),
															_paymentRepository(paymentRepository),
															_qManager(qManager),
															_loyaltyBreaker(loyaltyBreaker)
{}

void ReservationDeleteByUsernameController::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
{
	Uri uri(req.getURI());
	std::string username = req.get("X-User-Name");
	std::string uid = uri.getPathFragment(uri.getPathSize() - 1);
	std::vector<ReservationWithHotel> reservations;
	try
	{
		reservations = _hotelRepository->getReservationsByUsername(username);
	}
	catch (...)
	{
		resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE);
		resp.setReason("Service Unavailable");
		resp.send();
		return;
	}
	ReservationWithHotel reservation;
	bool flag = true;
	for (uint32_t i = 0; flag && i < reservations.size(); i++)
		if (reservations[i].getReservationUid() == uid)
		{
			flag = false;
			reservation = reservations[i];
		}
	if (flag)
	{
		resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NOT_FOUND);
		resp.setReason("Not Found");
		resp.setContentType("application/json");
		resp.send() << "{\"message\":\"string\"}";
	}
	else
	{
		reservation.setStatus(ReservationWithHotel::Status::CANCELED);
		try
		{
			_hotelRepository->updateReservation(uid, reservation);
		}
		catch (...)
		{
			resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE);
			resp.setReason("Service Unavailable");
			resp.send();
			return;
		}
		PaymentInfo pinfo = _paymentRepository->getByUid(reservation.getPaymentUid());
		pinfo.setStatus(PaymentInfo::Status::CANCELED);
		try
		{
			_paymentRepository->updateByUid(reservation.getPaymentUid(), PaymentModel().setPrice(pinfo.getPrice())
													.setStatus(pinfo.getStatus())
													.setPaymentUid(reservation.getPaymentUid()));
		}
		catch (...)
		{
			resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE);
			resp.setReason("Service Unavailable");
			resp.send();
			return;
		}
		try
		{
			_loyaltyRepository->decrease(username);
		}
		catch (...)
		{
			_qManager->addRequest([this, username](){ this->_loyaltyRepository->decrease(username); });
		}
		resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NO_CONTENT);
		resp.setReason("No Content");
		resp.send();
	}
}
