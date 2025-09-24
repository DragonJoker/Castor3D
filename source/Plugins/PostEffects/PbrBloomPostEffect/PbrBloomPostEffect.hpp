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
		: public c3d::PostEffect
	{
	public:
		PostEffect( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & param );
		static c3d::PostEffectUPtr create( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & param );
		/**
		 *\copydoc		c3d::PostEffect::accept
		 */
		void accept( c3d::ConfigurationVisitorBase & visitor )override;
		/**
		 *\copydoc		c3d::PostEffect::setParameters
		 */
		void setParameters( c3d::Parameters parameters )final;

	private:
		/**
		*\copydoc		c3d::PostEffect::doInitialise
		*/
		bool doInitialise( c3d::RenderDevice const & device
			, c3d::Texture const & source
			, c3d::Texture & target )override;
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

	public:
		static c3d::String const Type;
		static c3d::MbString const Name;

	private:
		c3d::Extent3D m_extent;
		c3d::Vector< c3d::Texture > m_downSampled;
		c3d::Vector< c3d::Texture > m_upSampled;
		c3d::RawUniquePtr< DownsamplePass > m_downsamplePass;
		c3d::RawUniquePtr< UpsamplePass > m_upsamplePass;
		c3d::RawUniquePtr< CombinePass > m_combinePass;
		c3d::UniformBufferOffsetT< c3d::Point2f > m_ubo;
		uint32_t m_blurRadius{ BaseBlurRadius };
		float m_bloomStrength{ BaseBloomStrength };
		uint32_t m_duPassesCount{ BaseFilterCount };
	};
}

#endif
