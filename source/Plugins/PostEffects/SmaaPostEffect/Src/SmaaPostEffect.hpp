/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SmaaPostEffect_H___
#define ___C3D_SmaaPostEffect_H___

#include "EdgeDetection.hpp"
#include "BlendingWeightCalculation.hpp"
#include "NeighbourhoodBlending.hpp"
#include "Reproject.hpp"

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

		void doInitialiseSubsampleIndices();
		void doInitialiseEdgeDetection();
		void doInitialiseBlendingWeightCalculation();
		void doInitialiseNeighbourhoodBlending();
		void doInitialiseReproject();
		void doBuildCommandBuffers( castor3d::RenderPassTimer const & timer );
		castor3d::CommandsSemaphoreArray doBuildCommandBuffer( castor3d::RenderPassTimer const & timer
			, uint32_t index );

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
		uint32_t m_curIndex{ 0u };

		std::unique_ptr< EdgeDetection > m_edgeDetection;
		std::unique_ptr< BlendingWeightCalculation > m_blendingWeightCalculation;
		std::unique_ptr< NeighbourhoodBlending > m_neighbourhoodBlending;
		std::vector< std::unique_ptr< Reproject > > m_reproject;
		std::vector< castor3d::CommandsSemaphoreArray > m_commandBuffers;
		renderer::TextureView const * m_srgbTexture{ nullptr };
		renderer::TextureView const * m_hdrTexture{ nullptr };
	};
}

#endif
