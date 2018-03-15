/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SmaaPostEffect_H___
#define ___C3D_SmaaPostEffect_H___

#include "SmaaUbo.hpp"

#include <Buffer/ParticleDeclaration.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>

namespace smaa
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		enum class Mode
		{
			e1X,
			eT2X,
			eS2X,
			e4X
		};

		enum class Preset
		{
			eLow,
			eMedium,
			eHigh,
			eUltra,
			eCustom
		};

	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & parameters );
		~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & parameters );
		/**
		 *\copydoc		castor3d::PostEffect::Initialise
		 */
		bool initialise() override;
		/**
		 *\copydoc		castor3d::PostEffect::Cleanup
		 */
		void cleanup() override;
		/**
		 *\copydoc		castor3d::PostEffect::Apply
		 */
		bool apply( castor3d::FrameBuffer & framebuffer ) override;

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & file ) override;

		void doInitialiseSubsampleIndices();
		void doInitialiseEdgeDetection();
		void doInitialiseBlendingWeightCalculation();
		void doInitialiseNeighbourhoodBlending();
		void doInitialiseReproject();

		void doEdgesDetectionPass( uint32_t prvIndex
			, uint32_t curIndex
			, castor3d::TextureLayout const & gammaSrc
			, castor3d::TextureLayout const * depthStencil );
		void doBlendingWeightsCalculationPass( uint32_t prvIndex
			, uint32_t curIndex );
		void doNeighbourhoodBlendingPass( uint32_t prvIndex
			, uint32_t curIndex
			, castor3d::TextureLayout const & gammaSrc );
		void doMainPass( uint32_t prvIndex
			, uint32_t curIndex
			, castor3d::TextureLayout const & hdrSrc
			, castor3d::TextureLayout const & srgbSrc
			, castor3d::TextureLayout const * depthStencil );
		void doReproject( uint32_t prvIndex
			, uint32_t curIndex
			, castor3d::TextureLayout const & gammaSrc );

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::SamplerSPtr m_pointSampler;
		castor3d::SamplerSPtr m_linearSampler;
		Mode m_mode{ Mode::e1X };
		uint32_t m_subsampleIndex{ 0u };
		bool m_reprojection{ false };
		float m_reprojectionWeightScale{ 30.0f };

		// Edge detection.
		PostEffectSurface m_edgeDetectionSurface;
		castor3d::RenderPipelineSPtr m_edgeDetectionPipeline;
		castor3d::TextureLayoutSPtr m_depthBuffer;
		castor3d::TextureAttachmentSPtr m_depthAttach;
		// Blending weight calculation.
		castor3d::TextureUnit m_areaTex;
		castor3d::TextureUnit m_searchTex;
		PostEffectSurface m_blendingWeightCalculationSurface;
		castor3d::RenderPipelineSPtr m_blendingWeightCalculationPipeline;
		castor3d::PushUniform4iSPtr m_subsampleIndicesUniform;
		// Neighbourhood blending.
		std::vector< PostEffectSurface > m_neighbourhoodBlendingSurface;
		castor3d::RenderPipelineSPtr m_neighbourhoodBlendingPipeline;
		// Reproject
		PostEffectSurface m_reprojectSurface;
		castor3d::RenderPipelineSPtr m_reprojectPipeline;

		// Common.
		float m_smaaThreshold{ 0.1f };
		int m_smaaMaxSearchSteps{ 16 };
		int m_smaaMaxSearchStepsDiag{ 8 };
		int m_smaaCornerRounding{ 25 };
		float m_smaaPredicationThreshold{ 0.01f };
		float m_smaaPredicationScale{ 1.0f };
		float m_smaaPredicationStrength{ 0.6f };
		std::array< castor::Point4i, 4u > m_smaaSubsampleIndices
		{
			{
				castor::Point4i{ 0, 0, 0, 0 },
				castor::Point4i{ 0, 0, 0, 0 },
				castor::Point4i{ 0, 0, 0, 0 },
				castor::Point4i{ 0, 0, 0, 0 }
			}
		};
		uint32_t m_maxSubsampleIndices{ 1u };
		std::vector< castor::Point2r > m_jitters;
	};
}

#endif
