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
		_queueMutex.lock();
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
					_queueMutex.unlock();
					std::this_thread::sleep_for(std::chrono::seconds(10));
					_queueMutex.lock();
				}
			}
		}
		_queueMutex.unlock();
	}
}

void QueueManager::addRequest(const std::function<void()> &request)
{
	std::cout << "ЕЕЕ, работа)\n";
	_queueMutex.lock();
	_queue.push(request);
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
