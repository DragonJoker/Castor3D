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
#ifndef ___CU_WorkerThread_H___
#define ___CU_WorkerThread_H___

#include "CastorUtilsPrerequisites.hpp"
#include "Design/Signal.hpp"

#include <thread>
#include <functional>
#include <atomic>
#include <mutex>

namespace Castor
{
	/*!
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
		 *\param[in]	p_job	The job.
		 *\~french
		 *\brief		Lance la tâche donnée.
		 *\param[in]	p_job	La tâche.
		 */
		CU_API void Feed( Job p_job );
		/**
		 *\~english
		 *\return		\p true if the job is ended.
		 *\~french
		 *\return		\p true si la tâche est terminàe.
		 */
		CU_API bool IsEnded()const;
		/**
		 *\~english
		 *\brief		Waits for the job end for a given time.
		 *\param[in]	p_timeout	The maximum time to wait.
		 *\return		\p true if the task is ended.
		 *\~french
		 *\brief		Attend la fin de la tâche pour un temps donné.
		 *\param[in]	p_timeout	Le temps maximal à attendre.
		 *\return		\p true si la tâche est terminée.
		 */
		CU_API bool Wait( Castor::Milliseconds const & p_timeout )const;
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
		void DoRun();

	private:
		std::unique_ptr< std::thread > m_thread;
		std::mutex m_mutex;
		std::atomic_bool m_start{ false };
		std::atomic_bool m_terminate{ false };
		Job m_currentJob;
		
	};
}

#endif
