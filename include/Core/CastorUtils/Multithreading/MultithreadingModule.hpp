/*
See LICENSE file in root folder
*/
#ifndef ___CU_MultithreadingModule_HPP___
#define ___CU_MultithreadingModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/**@name Multithreading */
	//@{
	/**
	*\~english
	*\brief
	*	Atomic operators based spin lock implementation.
	*\remarks
	*	Uses the same interface as std::mutex.
	*\~french
	*\brief
	*	Implémentation de spin lock basée sur les opérations atomiques.
	*\remarks
	*	Utilise la même interface que std::mutex.
	*/
	class SpinMutex;
	/**
	*\~english
	*\brief
	*	Thread pool implementation, using WorkerThreads.
	*\~french
	*\brief
	*	Implémentation de pool de thread, utilisant des WorkerThread.
	*/
	class ThreadPool;
	/**
	*\~english
	*\brief
	*	Implementation of a worker thread to place in a thread pool.
	*\~french
	*\brief
	*	Implàmentation d'un thread de travail à placer dans un pool de threads.
	*/
	class WorkerThread;
	//@}
}

#endif
