/*
See LICENSE file in root folder
*/
#ifndef ___CU_ThreadPool_H___
#define ___CU_ThreadPool_H___

#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Multithreading/WorkerThread.hpp"

namespace castor
{
	class ThreadPool
		: public NonMovable
	{
		using WorkerPtr = castor::RawUniquePtr< WorkerThread >;
		using WorkerArray = Vector< WorkerPtr >;

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
		CU_API bool waitAll( Milliseconds const & timeout )const;
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
		size_t getCount()const noexcept
		{
			return m_count;
		}

	private:
		WorkerThread & doReserveWorker();
		void doFreeWorker( WorkerThread const & worker );

	private:
		size_t const m_count;
		WorkerArray m_available;
		WorkerArray m_busy;
		mutable castor::Mutex m_mutex;
		Vector< WorkerThread::OnEnded::connection > m_endConnections;
	};
}

#endif
