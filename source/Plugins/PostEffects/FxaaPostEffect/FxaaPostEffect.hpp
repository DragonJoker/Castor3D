/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FxaaPostEffect_H___
#define ___C3D_FxaaPostEffect_H___

#include "FxaaUbo.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/Passes/RenderQuad.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Render/Viewport.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ShaderAST/Shader.hpp>

namespace fxaa
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		~PostEffect()override;
		static castor3d::PostEffectUPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
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
		void doCleanup( castor3d::RenderDevice const & device ) override;
		/**
		 *\copydoc		castor3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( castor3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::StringStream & file, castor::String const & tabs ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor::ChangeTracked< float > m_subpixShift{ { 1.0f / 4.0f } };
		castor::ChangeTracked< float > m_spanMax{ { 8.0f } };
		castor::ChangeTracked< float > m_reduceMul{ { 1.0f / 8.0f } };
		castor3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		FxaaUbo m_fxaaUbo;
		crg::FramePass * m_pass{};
	};
}

#endif
