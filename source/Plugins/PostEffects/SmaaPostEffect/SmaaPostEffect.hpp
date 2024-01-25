/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SmaaPostEffect_H___
#define ___C3D_SmaaPostEffect_H___

#include "SmaaPostEffect/EdgeDetection.hpp"
#include "SmaaPostEffect/BlendingWeightCalculation.hpp"
#include "SmaaPostEffect/NeighbourhoodBlending.hpp"
#include "SmaaPostEffect/Reproject.hpp"
#include "SmaaPostEffect/SmaaUbo.hpp"

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
		static castor3d::PostEffectUPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & parameters );
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::ConfigurationVisitorBase & visitor )override;
		/**
		 *\copydoc		castor3d::PostEffect::setParameters
		 */
		void setParameters( castor3d::Parameters parameters )override;

		crg::FramePass const & getPass()const override
		{
			CU_Require( m_pass );
			return *m_pass;
		}

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		bool doInitialise( castor3d::RenderDevice const & device
			, castor3d::Texture const & source
			, castor3d::Texture const & target
			, crg::FramePass const & previousPass )override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup( castor3d::RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( castor3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::StringStream & file, castor::String const & tabs )override;

		crg::ImageViewId const * doGetPredicationTexture();
		crg::ImageViewId const * doGetVelocityView();

	public:
		static castor::String Type;
		static castor::MbString Name;

	private:
		SmaaConfig m_config;
		uint32_t m_frameIndex{ 0u };
		SmaaUbo m_ubo;
		castor3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;

		castor::RawUniquePtr< EdgeDetection > m_edgeDetection;
		castor::RawUniquePtr< BlendingWeightCalculation > m_blendingWeightCalculation;
		castor::RawUniquePtr< NeighbourhoodBlending > m_neighbourhoodBlending;
		castor::RawUniquePtr< Reproject > m_reproject;

		crg::FramePass const * m_pass{};

		uint32_t m_subsamplePassIndex{};
	};
}

#endif
