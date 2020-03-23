/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FxaaPostEffect_H___
#define ___C3D_FxaaPostEffect_H___

#include "FxaaUbo.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Render/ToTexture/RenderQuad.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ShaderAST/Shader.hpp>

namespace fxaa
{
	class RenderQuad
		: public castor3d::RenderQuad
	{
	public:
		explicit RenderQuad( castor3d::RenderSystem & renderSystem
			, castor::Size const & size );
		void update( float subpixShift
			, float spanMax
			, float reduceMul );

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;

	private:
		FxaaUbo m_fxaaUbo;
	};

	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		/**
		 *\copydoc		castor3d::PostEffect::update
		 */
		void update( castor::Nanoseconds const & elapsedTime )override;
		/**
		 *\copydoc		castor3d::PostEffect::accept
		 */
		void accept( castor3d::PipelineVisitorBase & visitor )override;

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		bool doInitialise( castor3d::RenderPassTimer const & timer ) override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup() override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & file, castor::String const & tabs ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor::ChangeTracked< float > m_subpixShift{ { 1.0f / 4.0f } };
		castor::ChangeTracked< float > m_spanMax{ { 8.0f } };
		castor::ChangeTracked< float > m_reduceMul{ { 1.0f / 8.0f } };
		castor3d::SamplerSPtr m_sampler;
		castor3d::PostEffectSurface m_surface;
		ashes::RenderPassPtr m_renderPass;
		std::unique_ptr< RenderQuad > m_fxaaQuad;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif
