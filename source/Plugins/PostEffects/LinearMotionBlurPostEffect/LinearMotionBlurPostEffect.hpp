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
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & parameters );
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
		static castor::String Type;
		static castor::String Name;

	private:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = Clock::time_point;
		Configuration m_configuration;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass * m_pass{};
		TimePoint m_saved;
		bool m_fpsScale{ true };
	};
}

#endif
