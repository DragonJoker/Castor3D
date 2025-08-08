/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BloomPostEffect___
#define ___C3D_BloomPostEffect___

#include "BloomPostEffect/BlurPass.hpp"
#include "BloomPostEffect/CombinePass.hpp"
#include "BloomPostEffect/HiPass.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace Bloom
{
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
		void setParameters( c3d::Parameters parameters )override;

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
		c3d::Texture m_blurXImg;
		c3d::Texture m_blurYImg;
		c3d::RawUniquePtr< CombinePass > m_combinePass;
		c3d::RawUniquePtr< HiPass > m_hiPass;
		c3d::RawUniquePtr< BlurPass > m_blurXPass;
		c3d::RawUniquePtr< BlurPass > m_blurYPass;
		uint32_t m_blurKernelSize;
		uint32_t m_blurPassesCount;
	};
}

#endif
