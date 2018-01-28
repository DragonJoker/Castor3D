/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPassTimer_H___
#define ___C3D_RenderPassTimer_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

namespace castor3d
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
		: public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	category	The render pass category.
		 *\param[in]	name		The timer name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	category	La catégorie de la passe de rendu.
		 *\param[in]	name		Le nom du timer.
		 */
		C3D_API RenderPassTimer( Engine & engine
			, castor::String const & category
			, castor::String const & name );
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
		C3D_API void start( renderer::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Stops the timers.
		 *\~french
		 *\brief
		 *\brief		Arrête les timers.
		 */
		C3D_API void stop( renderer::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Reset the timer's times.
		 *\~french
		 *\brief
		 *\brief		Réinitialise les temps du timer.
		 */
		C3D_API void reset();
		/**
		 *\~english
		 *\return		The CPU time.
		 *\~french
		 *\return		Le temps CPU.
		 */
		inline castor::Nanoseconds getCpuTime()const
		{
			return m_cpuTime;
		}
		/**
		 *\~english
		 *\return		The CPU time.
		 *\~french
		 *\return		Le temps CPU.
		 */
		inline castor::Nanoseconds getGpuTime()const
		{
			return m_gpuTime;
		}
		/**
		 *\~english
		 *\return		The render pass category.
		 *\~french
		 *\return		La categorie de la passe de rendu.
		 */
		inline castor::String const & getCategory()const
		{
			return m_category;
		}

	private:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The render pass category.
		//!\~french		La categorie de la passe de rendu.
		castor::String m_category;
		//!\~english	The CPU timer.
		//!\~french		Le timer CPU.
		castor::PreciseTimer m_cpuTimer;
		//!\~english	The CPU time.
		//!\~french		Le temps CPU.
		castor::Nanoseconds m_cpuTime;
		//!\~english	The GPU time.
		//!\~french		Le temps GPU.
		castor::Nanoseconds m_gpuTime;
		//!\~english	The GPU time elapsed queries.
		//!\~french		Les requêtes GPU de temps écoulé.
		std::array< uint32_t, 2 > m_timerQuery;
		//!\~english	The active query index.
		//!\~french		L'index de la requête active.
		uint32_t m_queryIndex = 0;
	};
}

#endif
