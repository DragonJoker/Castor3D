/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FxaaPostEffect_H___
#define ___C3D_FxaaPostEffect_H___

#include "FxaaUbo.hpp"

#include <PostEffect/PostEffect.hpp>
#include <RenderToTexture/RenderQuad.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>

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
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;

	private:
		struct Configuration
		{
			castor::Point2f m_renderSize;
			float m_subpixShift;
			float m_spanMax;
			float m_reduceMul;
		};
		castor::Size m_size;
		renderer::UniformBufferPtr< Configuration > m_configUbo;
	};

	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & p_renderTarget, castor3d::RenderSystem & renderSystem, castor3d::Parameters const & p_param );
		~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & p_renderTarget, castor3d::RenderSystem & renderSystem, castor3d::Parameters const & p_param );
		/**
		 *\copydoc		castor3d::PostEffect::Initialise
		 */
		bool initialise() override;
		/**
		 *\copydoc		castor3d::PostEffect::Cleanup
		 */
		void cleanup() override;
		/**
		 *\copydoc		castor3d::PostEffect::Apply
		 */
		bool apply() override;

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & p_file ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		float m_subpixShift{ 1.0f / 4.0f };
		float m_spanMax{ 8.0f };
		float m_reduceMul{ 1.0f / 8.0f };
		castor3d::SamplerSPtr m_sampler;
		PostEffectSurface m_surface;
		renderer::RenderPassPtr m_renderPass;
		std::unique_ptr< RenderQuad > m_fxaaQuad;
		std::unique_ptr< RenderQuad > m_resultQuad;
	};
}

#endif
