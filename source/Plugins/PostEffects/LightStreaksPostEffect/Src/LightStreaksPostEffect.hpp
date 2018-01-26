/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaksPostEffect___
#define ___C3D_LightStreaksPostEffect___

#include "KawaseUbo.hpp"

#include <Mesh/Buffer/ParticleDeclaration.hpp>
#include <Miscellaneous/GaussianBlur.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>

namespace light_streaks
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & p_params );
		virtual ~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		/**
		 *\copydoc		castor3d::PostEffect::Initialise
		 */
		virtual bool initialise();
		/**
		 *\copydoc		castor3d::PostEffect::Cleanup
		 */
		virtual void cleanup();
		/**
		 *\copydoc		castor3d::PostEffect::Apply
		 */
		virtual bool apply( castor3d::FrameBuffer & framebuffer );

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		virtual bool doWriteInto( castor::TextFile & file );

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const CombineMapScene;
		static castor::String const CombineMapKawase1;
		static castor::String const CombineMapKawase2;
		static castor::String const CombineMapKawase3;
		static castor::String const CombineMapKawase4;
		static constexpr uint32_t Count{ 4u };
		using SurfaceArray = std::array< PostEffectSurface, Count >;

	private:
		void doHiPassFilter( castor3d::TextureLayout const & origin );
		void doKawaseBlur( castor3d::TextureLayout const & origin
			, castor3d::FrameBuffer & framebuffer );
		void doCombine( castor3d::TextureLayout const & origin );
		castor3d::SamplerSPtr doCreateSampler( bool linear );
		bool doInitialiseHiPassProgram();
		bool doInitialiseKawaseProgram();
		bool doInitialiseCombineProgram();

		castor3d::Viewport m_viewport;
		castor3d::SamplerSPtr m_linearSampler;
		castor3d::SamplerSPtr m_nearestSampler;

		castor3d::RenderPipelineUPtr m_hiPassPipeline;
		castor3d::PushUniform1sSPtr m_hiPassMapDiffuse;

		castor3d::RenderPipelineUPtr m_kawasePipeline;
		KawaseUbo m_kawaseUbo;

		castor3d::MatrixUbo m_matrixUbo;
		castor3d::RenderPipelineUPtr m_combinePipeline;

		castor3d::ParticleDeclaration m_declaration;
		std::array< castor3d::BufferElementGroupSPtr, 6 > m_vertices;
		castor3d::VertexBufferSPtr m_vertexBuffer;
		castor3d::GeometryBuffersSPtr m_geometryBuffers;
		castor::real m_buffer[12];
		SurfaceArray m_hiPassSurfaces;
		SurfaceArray m_kawaseSurfaces;
		PostEffectSurface m_combineSurface;
	};
}

#endif
