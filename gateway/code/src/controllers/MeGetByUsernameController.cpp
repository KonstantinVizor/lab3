#include "../../inc/controllers/MeGetByUsernameController.h"
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "Poco/Net/HTTPRequestHandler.h"
#include "../../inc/models/ReservationResponse.h"
#include "../../inc/models/ReservationWithHotel.h"
#include "../../inc/models/LoyaltyInfo.h"
#include "../../inc/models/PaymentInfo.h"
#include "../../inc/models/UserInfoResponse.h"
#include <vector>

MeGetByUsernameController::MeGetByUsernameController(const std::shared_ptr<HotelRepository> &hotelRepository,
							const std::shared_ptr<LoyaltyRepository> &loyaltyRepository,
							const std::shared_ptr<PaymentRepository> &paymentRepository) :
												Poco::Net::HTTPRequestHandler(),
												_hotelRepository(hotelRepository),
												_loyaltyRepository(loyaltyRepository),
												_paymentRepository(paymentRepository)
{}

void MeGetByUsernameController::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
{
	std::string username = req.get("X-User-Name");
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
	std::vector<ReservationResponse> ress;
	PaymentInfo payment;
	UserInfoResponse result;
	LoyaltyInfo loyalty;
	try
	{
		loyalty = _loyaltyRepository->getByUsername(username);
	}
	catch (...)
	{
		loyalty.setstatus("")
			.setDiscount(0)
			.setReservationCount(0);
	}
	for (ReservationWithHotel &reservation : reservations)
	{
		try
		{
			payment = _paymentRepository->getByUid(reservation.getPaymentUid());
		}
		catch (...)
		{
			payment.setPrice(0)
				.setStatus("");
		}
		ress.push_back(ReservationResponse().setEndDate(reservation.getEndDate())
							.setHotel(reservation.getHotel())
							.setStatus(reservation.getStatus())
							.setPayment(payment)
							.setStartDate(reservation.getStartDate())
							.setReservationUid(reservation.getReservationUid()));
	}
	result.setReservations(ress)
		.setLoyalty(loyalty);
	resp.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_OK);
	resp.setContentType("application/json");
	resp.send() << result.toJson();
}
