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
	c3d::String getName( Mode mode );
	c3d::String getName( Preset preset );
	c3d::String getName( EdgeDetectionType detection );

	class PostEffect
		: public c3d::PostEffect
	{
	public:
		PostEffect( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & parameters );
		static c3d::PostEffectUPtr create( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & parameters );
		/**
		 *\copydoc		c3d::PostEffect::accept
		 */
		void accept( c3d::ConfigurationVisitorBase & visitor )override;
		/**
		 *\copydoc		c3d::PostEffect::setParameters
		 */
		void setParameters( c3d::Parameters parameters )override;

		crg::FramePass const & getPass()const override
		{
			CU_Require( m_pass );
			return *m_pass;
		}

	private:
		/**
		*\copydoc		c3d::PostEffect::doInitialise
		*/
		bool doInitialise( c3d::RenderDevice const & device
			, c3d::Texture const & source
			, c3d::Texture const & target
			, crg::FramePass const & previousPass )override;
		/**
		*\copydoc		c3d::PostEffect::doCleanup
		*/
		void doCleanup( c3d::RenderDevice const & device )override;
		/**
		 *\copydoc		c3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( c3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		c3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( c3d::StringStream & file, c3d::String const & tabs )override;

		crg::ImageViewId const * doGetPredicationTexture();
		crg::ImageViewId const * doGetVelocityView();

	public:
		static c3d::String Type;
		static c3d::MbString Name;

	private:
		SmaaConfig m_config;
		uint32_t m_frameIndex{ 0u };
		SmaaUbo m_ubo;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;

		c3d::RawUniquePtr< EdgeDetection > m_edgeDetection;
		c3d::RawUniquePtr< BlendingWeightCalculation > m_blendingWeightCalculation;
		c3d::RawUniquePtr< NeighbourhoodBlending > m_neighbourhoodBlending;
		c3d::RawUniquePtr< Reproject > m_reproject;

		crg::FramePass const * m_pass{};

		uint32_t m_subsamplePassIndex{};
	};
}

#endif
