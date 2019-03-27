#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H

#include <queue>

extern "C" {
#include "pthread.h"
}


template <typename T>
class BlockQueue
{
public:
	BlockQueue();
	explicit BlockQueue(size_t size);
	~BlockQueue();

	T get();
	void put(T data);
	size_t size();
	void clear();

private:
	std::queue<T> m_queue;
	size_t m_size;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_getCondition;
	pthread_cond_t m_putCondition;
};


template <typename T>
BlockQueue<T>::BlockQueue() : m_size(128)
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_getCondition, NULL);
	pthread_cond_init(&m_putCondition, NULL);
}

template <typename T>
BlockQueue<T>::BlockQueue(size_t size) : m_size(size)
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_getCondition, NULL);
	pthread_cond_init(&m_putCondition, NULL);
}

template <typename T>
BlockQueue<T>::~BlockQueue() {
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_getCondition);
	pthread_cond_destroy(&m_putCondition);
}

template <typename T>
T BlockQueue<T>::get()
{
	pthread_mutex_lock(&m_mutex);
	while (m_queue.empty())
	{
		pthread_cond_wait(&m_getCondition, &m_mutex);
	}

	T data = m_queue.front();
	m_queue.pop();

	pthread_cond_signal(&m_putCondition);
	pthread_mutex_unlock(&m_mutex);
	return data;
}

template <typename T>
void BlockQueue<T>::put(T data)
{
	pthread_mutex_lock(&m_mutex);
	while (m_queue.size() >= m_size)
	{
		pthread_cond_wait(&m_putCondition, &m_mutex);
	}

	m_queue.push(data);

	pthread_cond_signal(&m_getCondition);
	pthread_mutex_unlock(&m_mutex);
}

template<typename T>
size_t BlockQueue<T>::size()
{
	size_t size = 0;
	pthread_mutex_lock(&m_mutex);
	size = m_queue.size();
	pthread_mutex_unlock(&m_mutex);
	return size;
}

template<typename T>
void BlockQueue<T>::clear()
{
	pthread_mutex_lock(&m_mutex);
	while (!m_queue.empty())
	{
		m_queue.pop();
	}
	pthread_mutex_unlock(&m_mutex);
}

#endif//BLOCKQUEUE_H