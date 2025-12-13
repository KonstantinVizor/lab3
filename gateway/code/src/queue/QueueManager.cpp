#include "../../inc/queue/QueueManager.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPServerResponse.h"
#include <chrono>
#include <memory>
#include <thread>

void QueueManager::_process()
{
	std::shared_ptr<CircuitBreaker> breaker;
	bool sended;
	Poco::Net::HTTPClientSession client;
	Poco::Net::HTTPRequest req;
	Poco::Net::HTTPResponse resp;
	while (_running)
	{
		_queueMutex.lock();
		if (_queue.size() > 0)
		{
			sended = false;
			while (!sended)
			{
				std::tie(req, breaker) = _queue.front();
				breaker->send(req, resp);
				if (resp.getStatus() != Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_SERVICE_UNAVAILABLE)
				{
					_queue.pop();
					sended = true;
				}
				else
				{
					_queueMutex.unlock();
					std::this_thread::sleep_for(std::chrono::seconds(10));
					_queueMutex.lock();
				}
			}
		}
		_queueMutex.unlock();
	}
}

void QueueManager::addRequest(const Poco::Net::HTTPRequest &request, const std::shared_ptr<CircuitBreaker> &breaker)
{
	_queueMutex.lock();
	_queue.push({request, breaker});
	_queueMutex.unlock();
}

void QueueManager::start()
{
	_running = true;
	mainThread = std::thread(&QueueManager::_process, this);
	mainThread.detach();
}

void QueueManager::stop()
{
	_running = false;
}
