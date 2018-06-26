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
	\version	0.11.0
	\date		26/06/2018
	\~english
	\brief		Allows stopping a RenderPassTimer when an instance of this class goes out of scope.
	\~french
	\brief		Permet d'arrêter un RenderPassTimer lorsqu'une instance de cette classe sort du scope courant.
	*/
	class RenderPassTimerBlock
	{
	public:
		C3D_API RenderPassTimerBlock( RenderPassTimer & timer );
		C3D_API RenderPassTimerBlock( RenderPassTimerBlock && rhs );
		C3D_API RenderPassTimerBlock & operator=( RenderPassTimerBlock && rhs );
		C3D_API RenderPassTimerBlock( RenderPassTimerBlock const & ) = delete;
		C3D_API RenderPassTimerBlock & operator=( RenderPassTimerBlock const & ) = delete;
		C3D_API ~RenderPassTimerBlock();

	private:
		RenderPassTimer * m_timer;
	};
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
		friend class RenderPassTimerBlock;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	category	The render pass category.
		 *\param[in]	name		The timer name.
		 *\param[in]	passesCount	The number of render passes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	category	La catégorie de la passe de rendu.
		 *\param[in]	name		Le nom du timer.
		 *\param[in]	passesCount	Le nombre de passes de rendu.
		 */
		C3D_API RenderPassTimer( Engine & engine
			, castor::String const & category
			, castor::String const & name
			, uint32_t passesCount = 1u );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderPassTimer();
		/**
		 *\~english
		 *\brief		Starts the CPU timer, resets GPU time.
		 *\~french
		 *\brief		Démarre le timer CPU, réinitialise le temps GPU.
		 */
		C3D_API RenderPassTimerBlock start();
		/**
		 *\~english
		 *\brief		Notifies the given pass render.
		 *\~french
		 *\brief		Notifie le rendu de la passe donnée.
		 */
		C3D_API void notifyPassRender( uint32_t passIndex = 0u );
		/**
		 *\~english
		 *\brief		Reset the timer's times.
		 *\~french
		 *\brief		Réinitialise les temps du timer.
		 */
		C3D_API void reset();
		/**
		 *\~english
		 *\brief		Writes the timestamp for the beginning of the pass.
		 *\~french
		 *\brief		Ecrit le timestamp pour le début de la passe.
		 */
		C3D_API void beginPass( renderer::CommandBuffer const & cmd
			, uint32_t passIndex = 0u )const;
		/**
		 *\~english
		 *\brief		Writes the timestamp for the end of the pass.
		 *\~french
		 *\brief		Ecrit le timestamp pour la fin de la passe.
		 */
		C3D_API void endPass( renderer::CommandBuffer const & cmd
			, uint32_t passIndex = 0u )const;
		/**
		 *\~english
		 *\brief		Retrieves GPU time from the query.
		 *\~french
		 *\brief		Récupère le temps GPU depuis la requête.
		 */
		C3D_API void retrieveGpuTime();
		/**
		 *\~english
		 *\brief		Updates the passes count to the given value.
		 *\~french
		 *\brief		Met à jour le nombre de passes à la valeur donnée.
		 */
		C3D_API void updateCount( uint32_t count );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline castor::Nanoseconds getCpuTime()const
		{
			return m_cpuTime;
		}

		inline castor::Nanoseconds getGpuTime()const
		{
			return m_gpuTime;
		}

		inline uint32_t getCount()const
		{
			return m_passesCount;
		}

		inline castor::String const & getCategory()const
		{
			return m_category;
		}
		/**@}*/

	private:
		/**
		 *\~english
		 *\brief		Stops the CPU timer.
		 *\~french
		 *\brief		Arrête le timer CPU.
		 */
		void stop();

	private:
		Engine & m_engine;
		uint32_t m_passesCount;
		castor::String m_category;
		castor::PreciseTimer m_cpuTimer;
		castor::Nanoseconds m_cpuTime;
		castor::Nanoseconds m_gpuTime;
		renderer::QueryPoolPtr m_timerQuery;
		std::vector< bool > m_startedPasses;
	};
}

#endif
