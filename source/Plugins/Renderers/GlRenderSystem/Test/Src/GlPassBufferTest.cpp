#include "GlPassBufferTest.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Material/LegacyPass.hpp>
#include <Material/MetallicRoughnessPbrPass.hpp>
#include <Material/SpecularGlossinessPbrPass.hpp>
#include <Miscellaneous/ComputePipeline.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/LegacyPassBuffer.hpp>
#include <Shader/MetallicRoughnessPassBuffer.hpp>
#include <Shader/SpecularGlossinessPassBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Buffer/GlBuffer.hpp>
#include <Graphics/PixelBuffer.hpp>

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>

using namespace Castor;
using namespace Castor3D;

#ifdef max
#	undef max
#endif

#define Glsl( x ) cuT( "#version 430\n\n" ) cuT( #x )

namespace Testing
{
	namespace
	{
		GLSL::Shader DoCreateVtxShader( Engine & engine )
		{
			using namespace GLSL;
			auto writer = engine.GetRenderSystem()->CreateGlslWriter();
			auto projection = writer.DeclUniform< Mat4 >( cuT( "projection" ) );
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );
			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					gl_Position = projection * vec4( position.xy(), 0.0, 1.0 );
				} );
			return writer.Finalise();
		}

		GLSL::Shader DoCreateLegacyPixelShader( Engine & engine )
		{
			using namespace GLSL;
			auto writer = engine.GetRenderSystem()->CreateGlslWriter();
			LegacyMaterials materials{ writer };
			materials.Declare();
			uint32_t index = 0;
			auto out_c3dOutput1 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput1" ), index++ );
			auto out_c3dOutput2 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput2" ), index++ );
			auto out_c3dOutput3 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput3" ), index++ );
			auto out_c3dOutput4 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput4" ), index++ );
			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					out_c3dOutput1 = vec4( materials.GetDiffuse( 0_i )
						, materials.GetEmissive( 0_i ) );
					out_c3dOutput2 = vec4( materials.GetSpecular( 0_i )
						, materials.GetAmbient( 0_i ) );
					out_c3dOutput3 = vec4( materials.GetShininess( 0_i )
						, materials.GetAlphaRef( 0_i )
						, materials.GetExposure( 0_i )
						, materials.GetGamma( 0_i ) );
					out_c3dOutput4 = vec4( materials.GetReflection( 0_i )
						, materials.GetRefraction( 0_i )
						, materials.GetRefractionRatio( 0_i )
						, 0.0_f );
					out_c3dOutput1 = vec4( 1.0_f );
					out_c3dOutput2 = vec4( 1.0_f );
					out_c3dOutput3 = vec4( 1.0_f );
					out_c3dOutput4 = vec4( 1.0_f );
				} );
			return writer.Finalise();
		}

		GLSL::Shader DoCreateMetallicRoughnessPixelShader( Engine & engine )
		{
			using namespace GLSL;
			auto writer = engine.GetRenderSystem()->CreateGlslWriter();
			PbrMRMaterials materials{ writer };
			materials.Declare();
			uint32_t index = 0;
			auto out_c3dOutput1 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput1" ), index++ );
			auto out_c3dOutput2 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput2" ), index++ );
			auto out_c3dOutput3 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput3" ), index++ );
			auto out_c3dOutput4 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput4" ), index++ );
			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					out_c3dOutput1 = vec4( materials.GetDiffuse( 0_i )
						, materials.GetEmissive( 0_i ) );
					out_c3dOutput2 = vec4( materials.GetRoughness( 0_i )
						, materials.GetMetallic( 0_i )
						, 0.0_f
						, 0.0_f );
					out_c3dOutput3 = vec4( 0.0_f
						, materials.GetAlphaRef( 0_i )
						, materials.GetExposure( 0_i )
						, materials.GetGamma( 0_i ) );
					out_c3dOutput4 = vec4( materials.GetReflection( 0_i )
						, materials.GetRefraction( 0_i )
						, materials.GetRefractionRatio( 0_i )
						, 0.0_f );
				} );
			return writer.Finalise();
		}

		GLSL::Shader DoCreateSpecularGlossinessPixelShader( Engine & engine )
		{
			using namespace GLSL;
			auto writer = engine.GetRenderSystem()->CreateGlslWriter();
			PbrSGMaterials materials{ writer };
			materials.Declare();
			uint32_t index = 0;
			auto out_c3dOutput1 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput1" ), index++ );
			auto out_c3dOutput2 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput2" ), index++ );
			auto out_c3dOutput3 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput3" ), index++ );
			auto out_c3dOutput4 = writer.DeclFragData< Vec4 >( cuT( "out_c3dOutput4" ), index++ );
			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					out_c3dOutput1 = vec4( materials.GetDiffuse( 0_i )
						, materials.GetEmissive( 0_i ) );
					out_c3dOutput2 = vec4( materials.GetSpecular( 0_i )
						, materials.GetGlossiness( 0_i ) );
					out_c3dOutput3 = vec4( 0.0_f
						, materials.GetAlphaRef( 0_i )
						, materials.GetExposure( 0_i )
						, materials.GetGamma( 0_i ) );
					out_c3dOutput4 = vec4( materials.GetReflection( 0_i )
						, materials.GetRefraction( 0_i )
						, materials.GetRefractionRatio( 0_i )
						, 0.0_f );
				} );
			return writer.Finalise();
		}

		struct Result
		{
		public:
			Result( Engine & engine )
				: output1{ DoCreateTexture( engine ) }
				, output2{ DoCreateTexture( engine ) }
				, output3{ DoCreateTexture( engine ) }
				, output4{ DoCreateTexture( engine ) }
				, fbo{ engine.GetRenderSystem()->CreateFrameBuffer() }
			{
				fbo->Create();
				fbo->Initialise( Size{ 1, 1 } );
				output1Attach = fbo->CreateAttachment( output1.GetTexture() );
				output2Attach = fbo->CreateAttachment( output2.GetTexture() );
				output3Attach = fbo->CreateAttachment( output3.GetTexture() );
				output4Attach = fbo->CreateAttachment( output4.GetTexture() );
				fbo->Bind();
				fbo->Attach( AttachmentPoint::eColour, 0u, output1Attach, TextureType::eTwoDimensions );
				fbo->Attach( AttachmentPoint::eColour, 1u, output2Attach, TextureType::eTwoDimensions );
				fbo->Attach( AttachmentPoint::eColour, 2u, output3Attach, TextureType::eTwoDimensions );
				fbo->Attach( AttachmentPoint::eColour, 3u, output4Attach, TextureType::eTwoDimensions );
				fbo->Unbind();
				fbo->SetClearColour( Colour::from_components( 0.0f, 0.0f, 0.0f, 0.0f ) );
			}

			~Result()
			{
				fbo->Bind();
				fbo->DetachAll();
				fbo->Unbind();
				fbo->Cleanup();
				fbo->Destroy();
				fbo.reset();
				output1Attach.reset();
				output2Attach.reset();
				output3Attach.reset();
				output4Attach.reset();
				output1.Cleanup();
				output2.Cleanup();
				output3.Cleanup();
				output4.Cleanup();
			}

		private:
			TextureUnit DoCreateTexture( Engine & engine )
			{
				auto texture = engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
					, AccessType::eRead
					, AccessType::eWrite
					, PixelFormat::eRGBA32F
					, Size{ 1, 1 } );
				texture->GetImage().InitialiseSource();
				TextureUnit unit{ engine };
				unit.SetTexture( texture );
				unit.Initialise();
				return unit;
			}

		public:
			TextureUnit output1;
			TextureUnit output2;
			TextureUnit output3;
			TextureUnit output4;
			FrameBufferSPtr fbo;
			TextureAttachmentSPtr output1Attach;
			TextureAttachmentSPtr output2Attach;
			TextureAttachmentSPtr output3Attach;
			TextureAttachmentSPtr output4Attach;
		};

		struct Pipeline
		{
			Pipeline( Engine & engine
				, GLSL::Shader const & vtx
				, GLSL::Shader const & pxl )
				: viewport{ engine }
				, bufferVertex
				{
					{
						0, 0,
						1, 1,
						0, 1,
						0, 0,
						1, 0,
						1, 1
					}
				}
				, declaration
				{
					{
						BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 }
					}
				}
			{
				uint32_t i = 0;

				for ( auto & vertex : arrayVertex )
				{
					vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( bufferVertex.data() )[i++ * declaration.stride()] );
				}

				viewport.Initialise();
				auto & renderSystem = *engine.GetRenderSystem();

				program = engine.GetRenderSystem()->CreateShaderProgram();
				program->CreateObject( ShaderType::eVertex );
				program->CreateObject( ShaderType::ePixel );
				program->SetSource( ShaderType::eVertex, vtx );
				program->SetSource( ShaderType::eVertex, pxl );
				auto projection = program->CreateUniform< UniformType::eMat4x4f >( cuT( "projection" ), ShaderType::ePixel );
				program->Initialise();

				vertexBuffer = std::make_shared< VertexBuffer >( *renderSystem.GetEngine()
					, declaration );
				vertexBuffer->Resize( uint32_t( arrayVertex.size()
					* declaration.stride() ) );
				vertexBuffer->LinkCoords( arrayVertex.begin()
					, arrayVertex.end() );
				vertexBuffer->Initialise( BufferAccessType::eStatic
					, BufferAccessNature::eDraw );
				geometryBuffers = renderSystem.CreateGeometryBuffers( Topology::eTriangles
					, *program );
				geometryBuffers->Initialise( { *vertexBuffer }, nullptr );

				DepthStencilState dsState;
				dsState.SetDepthTest( false );
				dsState.SetDepthMask( WritingMask::eZero );
				pipeline = renderSystem.CreateRenderPipeline( std::move( dsState )
					, RasteriserState{}
					, BlendState{}
					, MultisampleState{}
					, *program
					, PipelineFlags{} );

				viewport.Resize( Size{ 1, 1 } );
				viewport.Initialise();
				viewport.Apply();

				projection->SetValue( viewport.GetProjection() );
			}

			~Pipeline()
			{
				pipeline->Cleanup();
				pipeline.reset();
				vertexBuffer->Cleanup();
				vertexBuffer.reset();
				geometryBuffers->Cleanup();
				geometryBuffers.reset();
				viewport.Cleanup();

				for ( auto & vertex : arrayVertex )
				{
					vertex.reset();
				}
			}

			ShaderProgramSPtr program;
			RenderPipelineUPtr pipeline;
			Viewport viewport;
			std::array< real, 6 * 2 > bufferVertex;
			Castor3D::BufferDeclaration declaration;
			std::array< BufferElementGroupSPtr, 6 > arrayVertex;
			VertexBufferSPtr vertexBuffer;
			GeometryBuffersSPtr geometryBuffers;
		};
	}

	GlPassBufferTest::GlPassBufferTest( Engine & engine )
		: GlTestCase{ "GlPassBufferTest", engine }
	{
	}

	GlPassBufferTest::~GlPassBufferTest()
	{
	}

	void GlPassBufferTest::DoRegisterTests()
	{
		DoRegisterTest( "GlPassBufferTest::Legacy", std::bind( &GlPassBufferTest::Legacy, this ) );
	}

	void GlPassBufferTest::Legacy()
	{
		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		auto material = m_engine.GetMaterialCache().Create( cuT( "Test" ), MaterialType::eLegacy );
		material->CreatePass();
		auto pass = material->GetTypedPass< MaterialType::eLegacy >( 0u );
		pass->SetDiffuse( Colour::from_components( 0.0f, 0.1f, 0.2f, 1.0f ) );
		pass->SetSpecular( Colour::from_components( 0.3f, 0.4f, 0.5f, 1.0f ) );
		pass->SetShininess( 0.6f );
		pass->SetAmbient( 0.7f );
		pass->SetEmissive( 0.8f );
		pass->SetOpacity( 0.9f );
		pass->SetAlphaValue( 1.0f );
		pass->SetRefractionRatio( 1.1f );
		material->Initialise();

		Result result{ m_engine };
		Pipeline pipeline{ m_engine
			, DoCreateVtxShader( m_engine )
			, DoCreateLegacyPixelShader( m_engine ) };

		LegacyPassBuffer passBuffer{ m_engine, GLSL::MaxMaterialsCount };
		passBuffer.AddPass( *pass );
		passBuffer.Update();
		passBuffer.Bind();

		result.fbo->Bind();
		result.fbo->SetDrawBuffers();
		result.fbo->Clear( BufferComponent::eColour );
		pipeline.pipeline->Apply();
		pipeline.geometryBuffers->Draw( 6u, 0u );
		result.fbo->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->Barrier( MemoryBarrier::eFrameBuffer );

		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		auto buffer1 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output1.GetTexture()->GetImage().GetBuffer() );
		auto buffer2 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output2.GetTexture()->GetImage().GetBuffer() );
		auto buffer3 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output3.GetTexture()->GetImage().GetBuffer() );
		auto buffer4 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output4.GetTexture()->GetImage().GetBuffer() );

		result.output1Attach->Download( Position{}, *buffer1 );
		result.output2Attach->Download( Position{}, *buffer2 );
		result.output3Attach->Download( Position{}, *buffer3 );
		result.output4Attach->Download( Position{}, *buffer4 );
		result.fbo->Unbind();

		auto pixel1 = *buffer1->begin();
		auto pixel2 = *buffer2->begin();
		auto pixel3 = *buffer3->begin();
		auto pixel4 = *buffer4->begin();

		material->Cleanup();
		m_engine.GetMaterialCache().Remove( material->GetName() );
	}
}
