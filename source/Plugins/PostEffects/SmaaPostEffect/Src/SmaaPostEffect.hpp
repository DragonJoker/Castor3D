/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SmaaPostEffect_H___
#define ___C3D_SmaaPostEffect_H___

#include "ColourEdgeDetection.hpp"
#include "DepthEdgeDetection.hpp"
#include "LumaEdgeDetection.hpp"
#include "BlendingWeightCalculation.hpp"
#include "NeighbourhoodBlending.hpp"
#include "Reproject.hpp"

#include <PostEffect/PostEffect.hpp>

namespace smaa
{
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
		bool doWriteInto( castor::TextFile & file )override;

		renderer::Texture const * doGetPredicationTexture();
		renderer::TextureView const * doGetVelocityView();
		void doBuildCommandBuffers( castor3d::RenderPassTimer const & timer );
		castor3d::CommandsSemaphoreArray doBuildCommandBuffer( castor3d::RenderPassTimer const & timer
			, uint32_t index );

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::SamplerSPtr m_pointSampler;
		castor3d::SamplerSPtr m_linearSampler;
		SmaaConfig m_config;
		uint32_t m_curIndex{ 0u };

		std::unique_ptr< DepthEdgeDetection > m_depthEdgeDetection;
		std::unique_ptr< ColourEdgeDetection > m_colourEdgeDetection;
		std::unique_ptr< LumaEdgeDetection > m_lumaEdgeDetection;
		std::unique_ptr< BlendingWeightCalculation > m_blendingWeightCalculation;
		std::unique_ptr< NeighbourhoodBlending > m_neighbourhoodBlending;
		std::vector< std::unique_ptr< Reproject > > m_reproject;
		std::vector< castor3d::CommandsSemaphoreArray > m_commandBuffers;
		renderer::TextureView const * m_srgbTextureView{ nullptr };
		renderer::TextureView const * m_hdrTextureView{ nullptr };
		castor3d::TextureLayout * m_smaaResult{ nullptr };

		renderer::ShaderStageStateArray m_copyProgram;
		renderer::RenderPassPtr m_copyRenderPass;
		renderer::FrameBufferPtr m_copyFrameBuffer;
		std::vector< castor3d::RenderQuadSPtr > m_copyQuads;
	};
}

#endif
