/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MotionBlurPostEffect___
#define ___C3D_MotionBlurPostEffect___

#include "LinearMotionBlurParsers.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/Viewport.hpp>

#include <ShaderAST/Shader.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

namespace motion_blur
{
	class PostEffect
		: public c3d::PostEffect
	{
	public:
		PostEffect( c3d::RenderTarget & renderTarget
			, c3d::RenderSystem & renderSystem
			, c3d::Parameters const & parameters );
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
		static c3d::String Type;
		static c3d::MbString Name;

	private:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = Clock::time_point;
		Configuration m_configuration;
		c3d::UniformBufferOffsetT< Configuration > m_ubo;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		TimePoint m_saved;
		bool m_fpsScale{ true };
	};
}

#endif
