add_library(reservationControllers SHARED ./CreateController.cpp ./DeleteByUidController.cpp ./GetAllController.cpp ./GetByUidController.cpp ./UpdateByUidController.cpp ./GetByUsernameController.cpp)
target_link_libraries(reservationControllers auth)
