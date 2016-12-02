/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CU_ThreadPool_H___
#define ___CU_ThreadPool_H___

#include "WorkerThread.hpp"

#include <mutex>

namespace Castor
{
	/*!
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
		 *\param[in]	p_count	The threads count.
		 *\~french
		 *\brief		Constructeur, initialise le pool au nombre de threads donné.
		 *\param[in]	p_count	Le nombre de threads du pool.
		 */
		CU_API ThreadPool( size_t p_count );
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
		CU_API bool IsEmpty()const;
		/**
		 *\~english
		 *\return		\p true if all the threads are idle.
		 *\~french
		 *\return		\p true si tous les threads sont inoccupés.
		 */
		CU_API bool IsFull()const;
		/**
		 *\~english
		 *\brief		Waits for all the threads to finish their job.
		 *\param[in]	p_timeout	The maximum time to wait.
		 *\return		\p true if all threads have finished.
		 *\~french
		 *\brief		Attend que tous les threads aient terminé leur tâche.
		 *\param[in]	p_timeout	Le temps d'attente maximum.
		 *\return		\p true Si tous les threads on terminé.
		 */
		CU_API bool WaitAll( std::chrono::milliseconds const & p_timeout )const;
		/**
		 *\~english
		 *\brief		Feeds a worker thread with the given job.
		 *\remarks		If no thread is available, waits for one to finish its job.
		 *\param[in]	p_job	The job.
		 *\~french
		 *\brief		Donne à un thread de travail la tâche donnée.
		 *\remarks		Si aucun thread n'est disponible, attend que l'un d'eux finisse sa tâche.
		 *\param[in]	p_job	La tâche.
		 */
		CU_API void PushJob( WorkerThread::Job p_job );

	private:
		WorkerThread & DoReserveWorker();
		void DoFreeWorker( WorkerThread & p_worker );

	private:
		size_t m_count;
		WorkerArray m_available;
		WorkerArray m_busy;
		mutable std::mutex m_mutex;
	};
}

#endif
