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
#include <Shader/ShaderProgram.hpp>
#include <Shader/PassBuffer/LegacyPassBuffer.hpp>
#include <Shader/PassBuffer/MetallicRoughnessPassBuffer.hpp>
#include <Shader/PassBuffer/SpecularGlossinessPassBuffer.hpp>
#include <Shader/Shaders/GlslMaterial.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Buffer/GlBuffer.hpp>
#include <Graphics/PixelBuffer.hpp>

#include <GlslSource.hpp>

using namespace castor;
using namespace castor3d;

#ifdef max
#	undef max
#endif

#define Glsl( x ) cuT( "#version 430\n\n" ) cuT( #x )

namespace Testing
{
	namespace
	{
		glsl::Shader doCreateVtxShader( Engine & engine )
		{
			using namespace glsl;
			auto writer = engine.getRenderSystem()->createGlslWriter();
			auto projection = writer.declUniform< Mat4 >( cuT( "projection" ) );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );
			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					gl_Position = projection * vec4( position.xy(), 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doCreateLegacyPixelShader( Engine & engine )
		{
			using namespace glsl;
			auto writer = engine.getRenderSystem()->createGlslWriter();
			shader::LegacyMaterials materials{ writer };
			materials.declare();
			uint32_t index = 0;
			auto out_c3dOutput1 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput1" ), index++ );
			auto out_c3dOutput2 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput2" ), index++ );
			auto out_c3dOutput3 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput3" ), index++ );
			auto out_c3dOutput4 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput4" ), index++ );
			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto material = writer.declLocale( cuT( "material" )
						, materials.getMaterial( 0_i ) );
					out_c3dOutput1 = vec4( material.m_diffuse()
						, material.m_emissive() );
					out_c3dOutput2 = vec4( material.m_specular()
						, material.m_ambient() );
					out_c3dOutput3 = vec4( material.m_shininess()
						, material.m_alphaRef()
						, material.m_exposure()
						, material.m_gamma() );
					out_c3dOutput4 = vec4( material.m_hasReflection()
						, material.m_hasRefraction()
						, material.m_refractionRatio()
						, 0.0_f );
					out_c3dOutput1 = vec4( 1.0_f );
					out_c3dOutput2 = vec4( 1.0_f );
					out_c3dOutput3 = vec4( 1.0_f );
					out_c3dOutput4 = vec4( 1.0_f );
				} );
			return writer.finalise();
		}

		glsl::Shader doCreateMetallicRoughnessPixelShader( Engine & engine )
		{
			using namespace glsl;
			auto writer = engine.getRenderSystem()->createGlslWriter();
			shader::PbrMRMaterials materials{ writer };
			materials.declare();
			uint32_t index = 0;
			auto out_c3dOutput1 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput1" ), index++ );
			auto out_c3dOutput2 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput2" ), index++ );
			auto out_c3dOutput3 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput3" ), index++ );
			auto out_c3dOutput4 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput4" ), index++ );
			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto material = writer.declLocale( cuT( "material" )
						, materials.getMaterial( 0_i ) );
					out_c3dOutput1 = vec4( material.m_albedo()
						, material.m_emissive() );
					out_c3dOutput2 = vec4( material.m_roughness()
						, material.m_metallic()
						, 0.0_f
						, 0.0_f );
					out_c3dOutput3 = vec4( 0.0_f
						, material.m_alphaRef()
						, material.m_exposure()
						, material.m_gamma() );
					out_c3dOutput4 = vec4( material.m_hasReflection()
						, material.m_hasRefraction()
						, material.m_refractionRatio()
						, 0.0_f );
				} );
			return writer.finalise();
		}

		glsl::Shader doCreateSpecularGlossinessPixelShader( Engine & engine )
		{
			using namespace glsl;
			auto writer = engine.getRenderSystem()->createGlslWriter();
			shader::PbrSGMaterials materials{ writer };
			materials.declare();
			uint32_t index = 0;
			auto out_c3dOutput1 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput1" ), index++ );
			auto out_c3dOutput2 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput2" ), index++ );
			auto out_c3dOutput3 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput3" ), index++ );
			auto out_c3dOutput4 = writer.declFragData< Vec4 >( cuT( "out_c3dOutput4" ), index++ );
			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto material = writer.declLocale( cuT( "material" )
						, materials.getMaterial( 0_i ) );
					out_c3dOutput1 = vec4( material.m_diffuse()
						, material.m_emissive() );
					out_c3dOutput2 = vec4( material.m_specular()
						, material.m_glossiness() );
					out_c3dOutput3 = vec4( 0.0_f
						, material.m_alphaRef()
						, material.m_exposure()
						, material.m_gamma() );
					out_c3dOutput4 = vec4( material.m_hasReflection()
						, material.m_hasRefraction()
						, material.m_refractionRatio()
						, 0.0_f );
				} );
			return writer.finalise();
		}

		struct Result
		{
		public:
			Result( Engine & engine )
				: output1{ doCreateTexture( engine ) }
				, output2{ doCreateTexture( engine ) }
				, output3{ doCreateTexture( engine ) }
				, output4{ doCreateTexture( engine ) }
				, fbo{ engine.getRenderSystem()->createFrameBuffer() }
			{
				fbo->initialise();
				output1Attach = fbo->createAttachment( output1.getTexture() );
				output2Attach = fbo->createAttachment( output2.getTexture() );
				output3Attach = fbo->createAttachment( output3.getTexture() );
				output4Attach = fbo->createAttachment( output4.getTexture() );
				fbo->bind();
				fbo->attach( AttachmentPoint::eColour, 0u, output1Attach, TextureType::eTwoDimensions );
				fbo->attach( AttachmentPoint::eColour, 1u, output2Attach, TextureType::eTwoDimensions );
				fbo->attach( AttachmentPoint::eColour, 2u, output3Attach, TextureType::eTwoDimensions );
				fbo->attach( AttachmentPoint::eColour, 3u, output4Attach, TextureType::eTwoDimensions );
				fbo->unbind();
				fbo->setClearColour( Colour::fromComponents( 0.0f, 0.0f, 0.0f, 0.0f ) );
			}

			~Result()
			{
				fbo->bind();
				fbo->detachAll();
				fbo->unbind();
				fbo->cleanup();
				fbo.reset();
				output1Attach.reset();
				output2Attach.reset();
				output3Attach.reset();
				output4Attach.reset();
				output1.cleanup();
				output2.cleanup();
				output3.cleanup();
				output4.cleanup();
			}

		private:
			TextureUnit doCreateTexture( Engine & engine )
			{
				auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
					, AccessType::eRead
					, AccessType::eWrite
					, PixelFormat::eRGBA32F
					, Size{ 1, 1 } );
				texture->getImage().initialiseSource();
				TextureUnit unit{ engine };
				unit.setTexture( texture );
				unit.initialise();
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
				, glsl::Shader const & vtx
				, glsl::Shader const & pxl )
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

				viewport.initialise();
				auto & renderSystem = *engine.getRenderSystem();

				program = engine.getRenderSystem()->createShaderProgram();
				program->createObject( ShaderType::eVertex );
				program->createObject( ShaderType::ePixel );
				program->setSource( ShaderType::eVertex, vtx );
				program->setSource( ShaderType::eVertex, pxl );
				auto projection = program->createUniform< UniformType::eMat4x4f >( cuT( "projection" ), ShaderType::ePixel );
				program->initialise();

				vertexBuffer = std::make_shared< VertexBuffer >( *renderSystem.getEngine()
					, declaration );
				vertexBuffer->resize( uint32_t( arrayVertex.size()
					* declaration.stride() ) );
				vertexBuffer->linkCoords( arrayVertex.begin()
					, arrayVertex.end() );
				vertexBuffer->initialise( BufferAccessType::eStatic
					, BufferAccessNature::eDraw );
				geometryBuffers = renderSystem.createGeometryBuffers( Topology::eTriangles
					, *program );
				geometryBuffers->initialise( { *vertexBuffer }, nullptr );

				DepthStencilState dsState;
				dsState.setDepthTest( false );
				dsState.setDepthMask( WritingMask::eZero );
				pipeline = renderSystem.createRenderPipeline( std::move( dsState )
					, RasteriserState{}
					, BlendState{}
					, MultisampleState{}
					, *program
					, PipelineFlags{} );

				viewport.resize( Size{ 1, 1 } );
				viewport.initialise();
				viewport.apply();

				projection->setValue( viewport.getProjection() );
			}

			~Pipeline()
			{
				pipeline->cleanup();
				pipeline.reset();
				vertexBuffer->cleanup();
				vertexBuffer.reset();
				geometryBuffers->cleanup();
				geometryBuffers.reset();
				viewport.cleanup();

				for ( auto & vertex : arrayVertex )
				{
					vertex.reset();
				}
			}

			ShaderProgramSPtr program;
			RenderPipelineUPtr pipeline;
			Viewport viewport;
			std::array< real, 6 * 2 > bufferVertex;
			castor3d::BufferDeclaration declaration;
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

	void GlPassBufferTest::doRegisterTests()
	{
		doRegisterTest( "GlPassBufferTest::Legacy", std::bind( &GlPassBufferTest::Legacy, this ) );
	}

	void GlPassBufferTest::Legacy()
	{
		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		auto material = m_engine.getMaterialCache().create( cuT( "Test" ), MaterialType::eLegacy );
		material->createPass();
		auto pass = material->getTypedPass< MaterialType::eLegacy >( 0u );
		pass->setDiffuse( Colour::fromComponents( 0.0f, 0.1f, 0.2f, 1.0f ) );
		pass->setSpecular( Colour::fromComponents( 0.3f, 0.4f, 0.5f, 1.0f ) );
		pass->setShininess( 0.6f );
		pass->setAmbient( 0.7f );
		pass->setEmissive( 0.8f );
		pass->setOpacity( 0.9f );
		pass->setAlphaValue( 1.0f );
		pass->setRefractionRatio( 1.1f );
		material->initialise();

		Result result{ m_engine };
		Pipeline pipeline{ m_engine
			, doCreateVtxShader( m_engine )
			, doCreateLegacyPixelShader( m_engine ) };

		LegacyPassBuffer passBuffer{ m_engine, shader::MaxMaterialsCount };
		passBuffer.addPass( *pass );
		passBuffer.update();
		passBuffer.bind();

		result.fbo->bind();
		result.fbo->setDrawBuffers();
		result.fbo->clear( BufferComponent::eColour );
		pipeline.pipeline->apply();
		pipeline.geometryBuffers->draw( 6u, 0u );
		result.fbo->unbind();
		m_engine.getRenderSystem()->getMainContext()->memoryBarrier( MemoryBarrier::eFrameBuffer );

		m_engine.getRenderSystem()->getMainContext()->swapBuffers();

		auto buffer1 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output1.getTexture()->getImage().getBuffer() );
		auto buffer2 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output2.getTexture()->getImage().getBuffer() );
		auto buffer3 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output3.getTexture()->getImage().getBuffer() );
		auto buffer4 = std::static_pointer_cast< PxBuffer< PixelFormat::eRGBA32F > >( result.output4.getTexture()->getImage().getBuffer() );

		result.output1Attach->download( Position{}, *buffer1 );
		result.output2Attach->download( Position{}, *buffer2 );
		result.output3Attach->download( Position{}, *buffer3 );
		result.output4Attach->download( Position{}, *buffer4 );
		result.fbo->unbind();

		auto pixel1 = *buffer1->begin();
		auto pixel2 = *buffer2->begin();
		auto pixel3 = *buffer3->begin();
		auto pixel4 = *buffer4->begin();

		material->cleanup();
		m_engine.getMaterialCache().remove( material->getName() );
	}
}
