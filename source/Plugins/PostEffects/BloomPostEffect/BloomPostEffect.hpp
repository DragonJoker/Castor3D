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
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & param );
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
			, castor3d::RenderPassTimer const & timer
			, crg::FramePass const & previousPass )override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup( castor3d::RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::StringStream & file, castor::String const & tabs )override;

	public:
		static castor::String const Type;
		static castor::String const Name;

	private:
		crg::ImageId m_blurImg;
		crg::ImageViewIdArray m_blurViews;
		crg::FramePass const * m_pass{};
		std::unique_ptr< CombinePass > m_combinePass;
		std::unique_ptr< HiPass > m_hiPass;
		std::unique_ptr< BlurPass > m_blurXPass;
		std::unique_ptr< BlurPass > m_blurYPass;
		uint32_t m_blurKernelSize;
		uint32_t m_blurPassesCount;
	};
}

#endif
