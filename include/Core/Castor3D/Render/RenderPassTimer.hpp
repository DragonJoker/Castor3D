/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPassTimer_H___
#define ___C3D_RenderPassTimer_H___

#include "RenderModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace castor3d
{
	class RenderPassTimerBlock
	{
	public:
		C3D_API explicit RenderPassTimerBlock( RenderPassTimer & timer );
		C3D_API RenderPassTimerBlock( RenderPassTimerBlock && rhs )noexcept;
		C3D_API RenderPassTimerBlock & operator=( RenderPassTimerBlock && rhs )noexcept;
		C3D_API RenderPassTimerBlock( RenderPassTimerBlock const & ) = delete;
		C3D_API RenderPassTimerBlock & operator=( RenderPassTimerBlock const & ) = delete;
		C3D_API ~RenderPassTimerBlock();

		inline RenderPassTimer * operator->()const
		{
			return m_timer;
		}

	private:
		RenderPassTimer * m_timer;
	};

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
		C3D_API RenderPassTimer( RenderDevice const & device
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
		C3D_API void beginPass( ashes::CommandBuffer const & cmd
			, uint32_t passIndex = 0u )const;
		/**
		 *\~english
		 *\brief		Writes the timestamp for the end of the pass.
		 *\~french
		 *\brief		Ecrit le timestamp pour la fin de la passe.
		 */
		C3D_API void endPass( ashes::CommandBuffer const & cmd
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
		RenderDevice const & m_device;
		uint32_t m_passesCount;
		castor::String m_category;
		castor::PreciseTimer m_cpuTimer;
		castor::Nanoseconds m_cpuTime;
		castor::Nanoseconds m_gpuTime;
		ashes::QueryPoolPtr m_timerQuery;
		std::vector< bool > m_startedPasses;
	};
}

#endif
