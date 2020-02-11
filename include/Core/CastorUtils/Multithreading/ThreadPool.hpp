/*
See LICENSE file in root folder
*/
#ifndef ___CU_ThreadPool_H___
#define ___CU_ThreadPool_H___

#include "CastorUtils/Multithreading/WorkerThread.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Thread pool implementation, using WorkerThreads.
	\~french
	\brief		Implémentation de pool de thread, utilisant des WorkerThread.
	*/
	class ThreadPool
	{
		using WorkerPtr = std::unique_ptr< WorkerThread >;
		using WorkerArray = std::vector< WorkerPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor, initialises the pool with given threads count.
		 *\param[in]	count	The threads count.
		 *\~french
		 *\brief		Constructeur, initialise le pool au nombre de threads donné.
		 *\param[in]	count	Le nombre de threads du pool.
		 */
		CU_API explicit ThreadPool( size_t count );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~ThreadPool()noexcept;
		/**
		 *\~english
		 *\return		\p true if all the threads are busy.
		 *\~french
		 *\return		\p true si tous les threads sont occupés.
		 */
		CU_API bool isEmpty()const;
		/**
		 *\~english
		 *\return		\p true if all the threads are idle.
		 *\~french
		 *\return		\p true si tous les threads sont inoccupés.
		 */
		CU_API bool isFull()const;
		/**
		 *\~english
		 *\brief		Waits for all the threads to finish their job.
		 *\param[in]	timeout	The maximum time to wait.
		 *\return		\p true if all threads have finished.
		 *\~french
		 *\brief		Attend que tous les threads aient terminé leur tâche.
		 *\param[in]	timeout	Le temps d'attente maximum.
		 *\return		\p true Si tous les threads on terminé.
		 */
		CU_API bool waitAll( castor::Milliseconds const & timeout )const;
		/**
		 *\~english
		 *\brief		Feeds a worker thread with the given job.
		 *\remarks		If no thread is available, waits for one to finish its job.
		 *\param[in]	job	The job.
		 *\~french
		 *\brief		donne à un thread de travail la tâche donnée.
		 *\remarks		Si aucun thread n'est disponible, attend que l'un d'eux finisse sa tâche.
		 *\param[in]	job	La tâche.
		 */
		CU_API void pushJob( WorkerThread::Job job );
		/**
		 *\~english
		 *\return		The threads count.
		 *\~french
		 *\return		Le nombre de threads.
		 */
		inline size_t getCount()const
		{
			return m_count;
		}

	private:
		WorkerThread & doReserveWorker();
		void doFreeWorker( WorkerThread & worker );

	private:
		size_t const m_count;
		WorkerArray m_available;
		WorkerArray m_busy;
		mutable std::mutex m_mutex;
		std::vector< WorkerThread::OnEnded::connection > m_endConnections;
	};
}

#endif
