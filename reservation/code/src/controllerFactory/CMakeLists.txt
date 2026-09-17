add_library(controllerFactory SHARED  ./ControllerFactory.cpp)
target_link_libraries(controllerFactory auth)
target_link_libraries(controllerFactory ${CMAKE_SOURCE_DIR}/libs/libPocoNet.so)
