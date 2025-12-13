#ifndef __QUEUEMANAGER_H__
#define __QUEUEMANAGER_H__

#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <Poco/Net/HTTPRequest.h>
#include "../circuitBreaker/CircuitBreaker.h"

class QueueManager
{
	private:
		std::queue<std::tuple<Poco::Net::HTTPRequest, std::shared_ptr<CircuitBreaker>>> _queue;
		std::mutex _queueMutex;
		std::thread mainThread;
		bool _running;

		void _process();

	public:
		void addRequest(const Poco::Net::HTTPRequest &request, const std::shared_ptr<CircuitBreaker> &breaker);
		void start();
		void stop();
};

#endif
