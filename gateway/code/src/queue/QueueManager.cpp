#include "../../inc/queue/QueueManager.h"
#include <chrono>
#include <thread>
#include <iostream>

void QueueManager::_process()
{
	bool sended;
	std::cout << "Работаем\n";
	while (_running)
	{
		std::unique_lock<std::mutex> lock(_queueMutex);
		_queueCv.wait(lock, [this] { return !_queue.empty() || !_running; });
		if (_queue.size() > 0)
		{
			std::cout << "Здесь что-то есть\n";
			sended = false;
			while (!sended)
			{
				try
				{
					_queue.front()();
					std::cout << "ЕЕЕ\n";
					_queue.pop();
					sended = true;
				}
				catch (...)
				{
					std::cout << "Не получилось\n";
					lock.unlock();
					std::this_thread::sleep_for(std::chrono::seconds(10));
					lock.lock();
				}
			}
		}
	}
}

void QueueManager::addRequest(const std::function<void()> &request)
{
	std::cout << "ЕЕЕ, работа)\n";
	{
		std::lock_guard<std::mutex> lock(_queueMutex);
		_queue.push(request);
	}
	_queueCv.notify_one();
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
	_queueCv.notify_all();
}
