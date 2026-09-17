#ifndef __KAFKAPRODUCER_H__
#define __KAFKAPRODUCER_H__

#include <string>

// Fire-and-forget publisher used to send domain events (booking created /
// canceled) to the statistics service via Kafka. Failures are only logged
// and never interrupt the caller's request flow.
class KafkaProducer
{
	public:
		static void publish(const std::string &topic, const std::string &jsonPayload);
};

#endif
