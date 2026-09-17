#ifndef __HOTEL_REPOSITORY_H__
#define __HOTEL_REPOSITORY_H__


#include <memory>
#include <string>
#include <cstdint>
#include "../models/PaginationResponce.h"
#include "../models/ReservationWithHotel.h"
#include "../models/CreateReservationRequest.h"
#include "../models/HotelResponce.h"
#include "../circuitBreaker/CircuitBreaker.h"

class HotelRepository
{
	private:
		std::shared_ptr<CircuitBreaker> _breaker;
	
	public:
		~HotelRepository() = default;
		HotelRepository(const HotelRepository &) = delete;
		HotelRepository(HotelRepository &&) = delete;
		HotelRepository(const std::shared_ptr<CircuitBreaker> &breaker);

		PaginationResponce getHotelAll(uint32_t page, uint32_t size);
		HotelResponce getHotelByUid(const std::string &uid);
		std::vector<ReservationWithHotel> getReservationsByUsername(const std::string &username);
		ReservationWithHotel createReservation(const std::string &username, const CreateReservationRequest &request);
		void updateReservation(const std::string &uid, const ReservationWithHotel &res);
		void deleteReservation(const std::string &uid);
};

#endif
