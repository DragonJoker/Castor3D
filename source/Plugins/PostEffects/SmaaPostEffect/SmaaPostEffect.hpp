/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SmaaPostEffect_H___
#define ___C3D_SmaaPostEffect_H___

#include "SmaaPostEffect/EdgeDetection.hpp"
#include "SmaaPostEffect/BlendingWeightCalculation.hpp"
#include "SmaaPostEffect/NeighbourhoodBlending.hpp"
#include "SmaaPostEffect/Reproject.hpp"

#include <Castor3D/PostEffect/PostEffect.hpp>

namespace smaa
{
	castor::String getName( Mode mode );
	castor::String getName( Preset preset );
	castor::String getName( EdgeDetectionType detection );

	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & parameters );
		~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & parameters );
		/**
		*\copydoc		castor3d::PostEffect::update
		*/
		void update( castor::Nanoseconds const & elapsedTime )override;
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		bool doInitialise( castor3d::RenderPassTimer const & timer )override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & file, castor::String const & tabs )override;

		ashes::Image const * doGetPredicationTexture();
		ashes::ImageView const * doGetVelocityView();
		void doBuildCommandBuffers( castor3d::RenderPassTimer const & timer );
		castor3d::CommandsSemaphoreArray doBuildCommandBuffer( castor3d::RenderPassTimer const & timer
			, uint32_t index );

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		SmaaConfig m_config;
		uint32_t m_frameIndex{ 0u };

		std::unique_ptr< EdgeDetection > m_edgeDetection;
		std::unique_ptr< BlendingWeightCalculation > m_blendingWeightCalculation;
		std::unique_ptr< NeighbourhoodBlending > m_neighbourhoodBlending;
		std::vector< std::unique_ptr< Reproject > > m_reproject;
		std::vector< castor3d::CommandsSemaphoreArray > m_commandBuffers;
		// sRGB view.
		ashes::ImageView const * m_srgbTextureView{ nullptr };
		// Gamma view.
		ashes::ImageView const * m_hdrTextureView{ nullptr };
		castor3d::TextureLayout * m_smaaResult{ nullptr };

		ashes::PipelineShaderStageCreateInfoArray m_copyProgram;
		ashes::RenderPassPtr m_copyRenderPass;
		ashes::FrameBufferPtr m_copyFrameBuffer;
		std::vector< castor3d::RenderQuadSPtr > m_copyQuads;
	};
}

#endif
