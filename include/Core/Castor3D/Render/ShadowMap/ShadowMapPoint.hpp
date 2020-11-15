/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapPoint_H___
#define ___C3D_ShadowMapPoint_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class ShadowMapPoint
		: public ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène.
		 */
		C3D_API explicit ShadowMapPoint( Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPoint();
		/**
		 *\copydoc		castor3d::ShadowMap::cpuUpdate
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMap::gpuUpdate
		 */
		C3D_API void update( GpuUpdater & updater )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		TextureUnit const & getTexture()const
		{
			return m_result[SmTexture::eVariance];
		}
		/**@}*/

	private:
		void doInitialiseFramebuffers( RenderDevice const & device );
		/**
		 *\copydoc		castor3d::ShadowMap::doInitialise
		 */
		void doInitialise( RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::ShadowMap::doCleanup
		 */
		void doCleanup( RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::ShadowMap::doRender
		 */
		ashes::Semaphore const & doRender( RenderDevice const & device
			, ashes::Semaphore const & toWait
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMap::isUpToDate
		 */
		bool isUpToDate( uint32_t index )const override;

	private:
		struct FrameBuffer
		{
			std::array< ashes::ImageView, size_t( SmTexture::eCount ) > views;
			ashes::ImageView varianceView;
			ashes::FrameBufferPtr frameBuffer;
			CommandsSemaphore blurCommands{ nullptr, nullptr };
		};
		struct PassData
		{
			ashes::CommandBufferPtr commandBuffer;
			std::array< FrameBuffer, 6u > frameBuffers;
			ashes::SemaphorePtr finished;
			ShadowType shadowType;
			std::array< ashes::ImageView, size_t( SmTexture::eCount ) > views;
		};
		std::vector< PassData > m_passesData;
		std::unique_ptr< GaussianBlur > m_blur;
	};
}

#endif
