/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FxaaPostEffect_H___
#define ___C3D_FxaaPostEffect_H___

#include "DepthNormalEdgeDetection.hpp"
#include "DrawEdgesUbo.hpp"
#include "ObjectIDEdgeDetection.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/Passes/RenderQuad.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Render/Viewport.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ShaderAST/Shader.hpp>

namespace draw_edges
{
	class PostEffect
		: public c3d::PostEffect
	{
	public:
		PostEffect( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & params );
		~PostEffect()override;
		static c3d::PostEffectUPtr create( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & params );
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
		void doCleanup( c3d::RenderDevice const & device ) override;
		/**
		 *\copydoc		c3d::PostEffect::doCpuUpdate
		 */
		void doCpuUpdate( c3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		c3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( c3d::StringStream & file, c3d::String const & tabs ) override;

	public:
		static const c3d::MbString Name;
		static const c3d::String Type;
		static const c3d::String NormalDepthWidth;
		static const c3d::String ObjectWidth;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::RawUniquePtr< DepthNormalEdgeDetection > m_depthNormal;
		c3d::RawUniquePtr< ObjectIDEdgeDetection > m_objectID;
		DrawEdgesUbo m_ubo;
		DrawEdgesUboConfiguration m_config;
	};
}

#endif
