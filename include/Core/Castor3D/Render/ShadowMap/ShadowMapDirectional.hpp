/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapDirectional_H___
#define ___C3D_ShadowMapDirectional_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include <ashespp/Image/ImageView.hpp>

namespace castor3d
{
	class ShadowMapDirectional
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
		C3D_API explicit ShadowMapDirectional( Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapDirectional();
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		C3D_API void update( GpuUpdater & updater )override;

	private:
		void doInitialiseFramebuffers( RenderDevice const & device );
		void doInitialise( RenderDevice const & device )override;
		void doCleanup( RenderDevice const & device )override;
		ashes::Semaphore const & doRender( RenderDevice const & device
			, ashes::Semaphore const & toWait
			, uint32_t index )override;
		bool isUpToDate( uint32_t index )const override;

	public:
		static VkFormat constexpr VarianceFormat = VK_FORMAT_R32G32_SFLOAT;
		static VkFormat constexpr LinearDepthFormat = VK_FORMAT_R32_SFLOAT;

	private:
		struct FrameBuffer
		{
			ashes::FrameBufferPtr frameBuffer;
			ashes::ImageView depthView;
			ashes::ImageView linearView;
			ashes::ImageView varianceView;
			ashes::ImageView positionView;
			ashes::ImageView fluxView;
			CommandsSemaphore blurCommands{ nullptr, nullptr };
		};
		ashes::CommandBufferPtr m_commandBuffer;
		CameraSPtr m_camera;
		std::unique_ptr< GaussianBlur > m_blur;
		std::vector< FrameBuffer > m_frameBuffers;
		ShadowType m_shadowType{ ShadowType::eRaw };
		uint32_t m_cascades;
	};
}

#endif
