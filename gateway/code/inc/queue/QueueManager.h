#ifndef __QUEUEMANAGER_H__
#define __QUEUEMANAGER_H__

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <Poco/Net/HTTPRequest.h>

class QueueManager
{
	private:
		std::queue<std::function<void()>> _queue;
		std::mutex _queueMutex;
		std::thread mainThread;
		bool _running;

		void _process();

	public:
		void addRequest(const std::function<void()> &request);
		void start();
		void stop();
};

#endif
