#include "../../inc/kafka/KafkaProducer.h"

#include <librdkafka/rdkafka.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>

namespace
{
	rd_kafka_t *createProducer()
	{
		char errstr[512];
		rd_kafka_conf_t *conf = rd_kafka_conf_new();

		const char *brokers = std::getenv("KAFKA_BROKERS");
		if (!brokers)
			brokers = "kafka:9092";

		rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr, sizeof(errstr));

		rd_kafka_t *rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
		if (!rk)
			std::cerr << "KafkaProducer: failed to create producer: " << errstr << std::endl;

		return rk;
	}

	rd_kafka_t *producer()
	{
		static rd_kafka_t *rk = createProducer();
		return rk;
	}

	std::mutex &producerMutex()
	{
		static std::mutex m;
		return m;
	}
}

void KafkaProducer::publish(const std::string &topic, const std::string &jsonPayload)
{
	std::lock_guard<std::mutex> lock(producerMutex());

	rd_kafka_t *rk = producer();
	if (!rk)
		return;

	rd_kafka_resp_err_t err = rd_kafka_producev(
		rk,
		RD_KAFKA_V_TOPIC(topic.c_str()),
		RD_KAFKA_V_VALUE(const_cast<char *>(jsonPayload.data()), jsonPayload.size()),
		RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
		RD_KAFKA_V_END);

	if (err)
		std::cerr << "KafkaProducer: failed to publish to " << topic << ": " << rd_kafka_err2str(err) << std::endl;

	rd_kafka_poll(rk, 0);
}
