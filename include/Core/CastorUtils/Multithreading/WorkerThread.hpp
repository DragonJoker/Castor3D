/*
See LICENSE file in root folder
*/
#ifndef ___CU_WorkerThread_H___
#define ___CU_WorkerThread_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"
#include "CastorUtils/Design/Signal.hpp"

#include <thread>
#include <functional>
#include <atomic>
#include <mutex>

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		02/12/2016
	\~english
	\brief		Implementation of a worker thread to place in a thread pool.
	\~french
	\brief		Implàmentation d'un thread de travail à placer dans un pool de threads.
	*/
	class WorkerThread
	{
	public:
		using Job = std::function< void() >;
		using OnEnded = Signal< std::function< void( WorkerThread & ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API WorkerThread();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~WorkerThread()noexcept;
		/**
		 *\~english
		 *\brief		Runs the given job.
		 *\param[in]	job	The job.
		 *\~french
		 *\brief		Lance la tâche donnée.
		 *\param[in]	job	La tâche.
		 */
		CU_API void feed( Job job );
		/**
		 *\~english
		 *\return		\p true if the job is ended.
		 *\~french
		 *\return		\p true si la tâche est terminàe.
		 */
		CU_API bool isEnded()const;
		/**
		 *\~english
		 *\brief		Waits for the job end for a given time.
		 *\param[in]	timeout	The maximum time to wait.
		 *\return		\p true if the task is ended.
		 *\~french
		 *\brief		Attend la fin de la tâche pour un temps donné.
		 *\param[in]	timeout	Le temps maximal à attendre.
		 *\return		\p true si la tâche est terminée.
		 */
		CU_API bool wait( castor::Milliseconds const & timeout )const;
		/**
		 *\~english
		 *\return		The signal raised when the worker has ended his job.
		 *\~french
		 *\return		Le signal lancé quand le worker a fini sa tâche.
		 */
		OnEnded onEnded;

	private:
		/**
		 *\~english
		 *\return		The thread loop.
		 *\~french
		 *\return		La boucle du thread.
		 */
		void doRun();

	private:
		std::unique_ptr< std::thread > m_thread;
		std::mutex m_mutex;
		std::atomic_bool m_start{ false };
		std::atomic_bool m_terminate{ false };
		Job m_currentJob;
		
	};
}

#endif
