/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GrayScalePostEffect___
#define ___C3D_GrayScalePostEffect___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ShaderAST/Shader.hpp>

namespace grayscale
{
	class PostEffect
		: public castor3d::PostEffect
	{
	private:
		class Quad
			: public castor3d::RenderQuad
		{
		public:
			Quad( castor3d::RenderSystem & renderSystem
				, castor3d::UniformBuffer< castor::Point3f > const & configUbo );

		private:
			void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet );

		private:
			castor3d::UniformBuffer< castor::Point3f > const & m_configUbo;
		};

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
		castor3d::PostEffectSurface m_surface;
		ashes::RenderPassPtr m_renderPass;
		castor3d::UniformBufferUPtr< castor::Point3f > m_configUbo;
		std::unique_ptr< Quad > m_quad;
		castor::ChangeTracked< castor::Point3f > m_factors{ castor::Point3f{ 0.2126f, 0.7152f, 0.0722f } };
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif
