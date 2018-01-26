/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FxaaPostEffect_H___
#define ___C3D_FxaaPostEffect_H___

#include "FxaaUbo.hpp"

#include <Mesh/Buffer/ParticleDeclaration.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>

namespace fxaa
{
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
		bool apply( castor3d::FrameBuffer & p_framebuffer ) override;

		inline void setSubpixShift( float p_value )
		{
			m_subpixShift = p_value;
		}

		inline void setMaxSpan( float p_value )
		{
			m_spanMax = p_value;
		}

		inline void setReduceMul( float p_value )
		{
			m_reduceMul = p_value;
		}

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & p_file ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::SamplerSPtr m_sampler;
		castor3d::RenderPipelineSPtr m_pipeline;
		PostEffectSurface m_surface;
		castor3d::MatrixUbo m_matrixUbo;
		FxaaUbo m_fxaaUbo;
		float m_subpixShift{ 1.0f / 4.0f };
		float m_spanMax{ 8.0f };
		float m_reduceMul{ 1.0f / 8.0f };
	};
}

#endif
