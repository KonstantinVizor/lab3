add_library(loyaltyControllers SHARED ./CreateController.cpp ./DeleteByUsernameController.cpp ./GetAllController.cpp ./GetByUsernameController.cpp ./UpdateByUsernameController.cpp ./DecreaseController.cpp)
target_link_libraries(loyaltyControllers auth)
