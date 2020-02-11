/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BloomPostEffect___
#define ___C3D_BloomPostEffect___

#include "BloomPostEffect/BlurPass.hpp"
#include "BloomPostEffect/CombinePass.hpp"
#include "BloomPostEffect/HiPass.hpp"

#include <Castor3D/PostEffect/PostEffect.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

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
		bool doWriteInto( castor::TextFile & file, castor::String const & tabs )override;

	public:
		static castor::String const Type;
		static castor::String const Name;

	private:
		castor3d::SamplerSPtr doCreateSampler( bool linear );
		bool doBuildCommandBuffer( castor3d::RenderPassTimer const & timer );

	private:
		ashes::VertexBufferPtr< castor3d::NonTexturedQuad > m_vertexBuffer;
		castor3d::TextureLayoutSPtr m_blurTexture;
		std::unique_ptr< CombinePass > m_combinePass;
		std::unique_ptr< HiPass > m_hiPass;
		std::unique_ptr< BlurPass > m_blurXPass;
		std::unique_ptr< BlurPass > m_blurYPass;
		uint32_t m_blurKernelSize;
		uint32_t m_blurPassesCount;
	};
}

#endif
