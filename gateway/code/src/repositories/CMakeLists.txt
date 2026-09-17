add_library(repositories SHARED ./PaymentRepository.cpp ./HotelRepository.cpp ./LoyaltyRepository.cpp)
target_link_libraries(repositories auth)
