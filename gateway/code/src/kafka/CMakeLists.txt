add_library(kafka SHARED ./KafkaProducer.cpp)
target_link_libraries(kafka rdkafka)
