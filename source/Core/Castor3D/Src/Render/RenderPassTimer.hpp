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
#ifndef ___C3D_RenderPassTimer_H___
#define ___C3D_RenderPassTimer_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		24/07/2017
	\~english
	\brief		Class that holds data needed to compute a render pass times.
	\~french
	\brief		Classe englobant les données nécessaires au calcul des temps d'une passe de rendu.
	*/
	class RenderPassTimer
		: public Castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The timer name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom du timer.
		 */
		C3D_API RenderPassTimer( Engine & engine
			, Castor::String const & name );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderPassTimer();
		/**
		 *\~english
		 *\brief		Starts the timers.
		 *\~french
		 *\brief		Démarre les timers.
		 */
		C3D_API void Start();
		/**
		 *\~english
		 *\brief		Stops the timers.
		 *\~french
		 *\brief
		 *\brief		Arrête les timers.
		 */
		C3D_API void Stop();
		/**
		 *\~english
		 *\return		The CPU time.
		 *\~french
		 *\return		Le temps CPU.
		 */
		inline Castor::Nanoseconds GetCpuTime()const
		{
			return m_cpuTime;
		}
		/**
		 *\~english
		 *\return		The CPU time.
		 *\~french
		 *\return		Le temps CPU.
		 */
		inline Castor::Nanoseconds GetGpuTime()const
		{
			return m_gpuTime;
		}

	private:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The CPU timer.
		//!\~french		Le timer CPU.
		Castor::PreciseTimer m_cpuTimer;
		//!\~english	The CPU time.
		//!\~french		Le temps CPU.
		Castor::Nanoseconds m_cpuTime;
		//!\~english	The GPU time.
		//!\~french		Le temps GPU.
		Castor::Nanoseconds m_gpuTime;
		//!\~english	The GPU time elapsed queries.
		//!\~french		Les requêtes GPU de temps écoulé.
		std::array< GpuQueryUPtr, 2 > m_timerQuery;
		//!\~english	The active query index.
		//!\~french		L'index de la requête active.
		uint32_t m_queryIndex = 0;
	};
}

#endif
