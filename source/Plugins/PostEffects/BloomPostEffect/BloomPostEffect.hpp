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
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
		static castor3d::PostEffectUPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
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

	public:
		static castor::String const Type;
		static castor::MbString const Name;

	private:
		crg::ImageId m_blurImg;
		crg::ImageViewIdArray m_blurViews;
		crg::FramePass const * m_pass{};
		castor::RawUniquePtr< CombinePass > m_combinePass;
		castor::RawUniquePtr< HiPass > m_hiPass;
		castor::RawUniquePtr< BlurPass > m_blurXPass;
		castor::RawUniquePtr< BlurPass > m_blurYPass;
		uint32_t m_blurKernelSize;
		uint32_t m_blurPassesCount;
	};
}

#endif
