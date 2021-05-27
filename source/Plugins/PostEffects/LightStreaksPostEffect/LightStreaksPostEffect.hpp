/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaksPostEffect___
#define ___C3D_LightStreaksPostEffect___

#include "CombinePass.hpp"
#include "HiPass.hpp"
#include "KawasePass.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace light_streaks
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
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
		static uint32_t const Count = 4u;

	private:
		crg::ImageId m_hiImage;
		crg::ImageViewIdArray m_hiViews;
		crg::ImageId m_kawaseImage;
		crg::ImageViewIdArray m_kawaseViews;
		KawaseUbo m_kawaseUbo;
		crg::FramePass const * m_pass{};
		std::unique_ptr< HiPass > m_hiPass;
		std::unique_ptr< KawasePass > m_kawasePass;
		std::unique_ptr< CombinePass > m_combinePass;
	};
}

#endif
