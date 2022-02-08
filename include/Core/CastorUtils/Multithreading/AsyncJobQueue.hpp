/*
See LICENSE file in root folder
*/
#ifndef ___CU_AsyncJobQueue_H___
#define ___CU_AsyncJobQueue_H___

#include "CastorUtils/Multithreading/ThreadPool.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

namespace castor
{
	class AsyncJobQueue
	{
	public:
		using Job = std::function< void() >;
		using JobArray = std::vector< Job >;

	public:
		/**
		 *\~english
		 *\brief		Constructor, initialises the pool with given threads count.
		 *\param[in]	count	The threads count.
		 *\~french
		 *\brief		Constructeur, initialise le pool au nombre de threads donné.
		 *\param[in]	count	Le nombre de threads du pool.
		 */
		CU_API explicit AsyncJobQueue( size_t count );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~AsyncJobQueue()noexcept;
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
		CU_API void pushJob( Job job );
		/**
		 *\~english
		 *\brief		Waits for all the jobs to be run.
		 *\~french
		 *\brief		Attend que tous les jobs soient terminés.
		 */
		CU_API void waitAll();
		/**
		 *\~english
		 *\brief		Waits for all the jobs to be run, prevents any new job push, just discards them.
		 *\~french
		 *\brief		Attend que tous les jobs soient terminés, prévient tout nouveau job, iles juste jeté.
		 */
		CU_API void finish();

	private:
		void doRun();
		Job doPopJob();
		bool doCheckEnded();

	private:
		std::atomic_bool m_ended;
		JobArray m_pending;
		ThreadPool m_pool;
		std::mutex m_mutex;
		std::thread m_worker;
	};
}

#endif
