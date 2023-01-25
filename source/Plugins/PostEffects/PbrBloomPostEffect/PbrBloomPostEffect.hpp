/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrBloomPostEffect___
#define ___C3D_PbrBloomPostEffect___

#include "PbrBloomPostEffect/CombinePass.hpp"
#include "PbrBloomPostEffect/DownsamplePass.hpp"
#include "PbrBloomPostEffect/UpsamplePass.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace PbrBloom
{
	static uint32_t constexpr BaseBlurRadius = 1u;
	static float constexpr BaseBloomStrength = 0.2f;
	static uint32_t constexpr BaseFilterCount = 5u;

	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;
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

	public:
		static castor::String const Type;
		static castor::String const Name;

	private:
		crg::ImageId m_intermediateImg;
		crg::FramePass const * m_pass{};
		std::unique_ptr< DownsamplePass > m_downsamplePass;
		std::unique_ptr< UpsamplePass > m_upsamplePass;
		std::unique_ptr< CombinePass > m_combinePass;
		castor3d::UniformBufferOffsetT< castor::Point2f > m_ubo;
		uint32_t m_blurRadius;
		float m_bloomStrength;
		uint32_t m_duPassesCount;
		VkExtent3D m_extent{};
	};
}

#endif
