/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapSpot_H___
#define ___C3D_ShadowMapSpot_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include <ashespp/Image/ImageView.hpp>

namespace castor3d
{
	class ShadowMapSpot
		: public ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	scene	La scène.
		 */
		ShadowMapSpot( Engine & engine
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ShadowMapSpot();
		/**
		 *\copydoc		castor3d::ShadowMap::cpuUpdate
		 */
		void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMap::gpuUpdate
		 */
		void update( GpuUpdater & updater )override;

	private:
		/**
		 *\copydoc		castor3d::ShadowMap::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::ShadowMap::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::ShadowMap::doRender
		 */
		ashes::Semaphore const & doRender( ashes::Semaphore const & toWait
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMap::isUpToDate
		 */
		bool isUpToDate( uint32_t index )const override;

	public:
		static VkFormat constexpr VarianceFormat = VK_FORMAT_R32G32_SFLOAT;
		static VkFormat constexpr LinearDepthFormat = VK_FORMAT_R32_SFLOAT;
		static VkFormat RawDepthFormat;

	private:
		struct PassData
		{
			ashes::CommandBufferPtr commandBuffer;
			ashes::FrameBufferPtr frameBuffer;
			ashes::SemaphorePtr finished;
			std::unique_ptr< GaussianBlur > blur;
			CommandsSemaphore blurCommands;
		};
		std::vector< PassData > m_passesData;
	};
}

#endif
