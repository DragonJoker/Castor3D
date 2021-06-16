/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SmaaPostEffect_H___
#define ___C3D_SmaaPostEffect_H___

#include "SmaaPostEffect/EdgeDetection.hpp"
#include "SmaaPostEffect/BlendingWeightCalculation.hpp"
#include "SmaaPostEffect/NeighbourhoodBlending.hpp"
#include "SmaaPostEffect/Reproject.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

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
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & parameters );
		/**
		*\copydoc		castor3d::PostEffect::update
		*/
		void update( castor3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;

		crg::FramePass const & getPass()const override
		{
			CU_Require( m_pass );
			return *m_pass;
		}

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		crg::ImageViewId const * doInitialise( castor3d::RenderDevice const & device
			, crg::FramePass const & previousPass )override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup( castor3d::RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::StringStream & file, castor::String const & tabs )override;

		crg::ImageViewId const * doGetPredicationTexture();
		crg::ImageViewId const * doGetVelocityView();

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		SmaaConfig m_config;
		uint32_t m_frameIndex{ 0u };
		ashes::PipelineShaderStageCreateInfoArray m_stages;

		std::unique_ptr< EdgeDetection > m_edgeDetection;
		std::unique_ptr< BlendingWeightCalculation > m_blendingWeightCalculation;
		std::unique_ptr< NeighbourhoodBlending > m_neighbourhoodBlending;
		std::unique_ptr< Reproject > m_reproject;
		// sRGB view.
		crg::ImageViewId const * m_srgbTextureView{ nullptr };
		// Gamma view.
		crg::ImageViewId const * m_hdrTextureView{ nullptr };
		crg::ImageViewId * m_smaaResult{ nullptr };

		crg::FramePass const * m_pass{};
	};
}

#endif
