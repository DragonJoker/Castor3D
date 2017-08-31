#include "CombinePass.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Miscellaneous/SsaoConfig.hpp"
#include "PBR/IblTextures.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Fog.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Technique/Opaque/LightPass.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		VertexBufferSPtr doCreateVbo( Engine & engine )
		{
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & renderSystem = *engine.getRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		GeometryBuffersSPtr doCreateVao( Engine & engine
			, ShaderProgram & program
			, VertexBuffer & vbo )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto result = renderSystem.createGeometryBuffers( Topology::eTriangles
				, program );
			result->initialise( { vbo }, nullptr );
			return result;
		}

		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_GPINFO( writer );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetLegacyPixelProgram( Engine & engine
			, FogType fogType
			, bool hasSsao )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto c3d_mapReflection = writer.declUniform< Sampler2D >( cuT( "c3d_mapReflection" ) );
			auto c3d_mapRefraction = writer.declUniform< Sampler2D >( cuT( "c3d_mapRefraction" ) );
			auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
			auto c3d_mapSsao = writer.declUniform< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao );
			auto c3d_mapPostLight = writer.declUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();
			utils.declareCalcWSPosition();

			declareDecodeMaterial( writer );

			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ambient = writer.declLocale( cuT( "ambient" )
						, c3d_ambientLight.xyz() );
					auto postLight = writer.declLocale( cuT( "light" )
						, texture( c3d_mapPostLight, vtx_texture ).xyz() );
					auto data1 = writer.declLocale( cuT( "data1" )
						, texture( c3d_mapData1, vtx_texture ) );
					auto data4 = writer.declLocale( cuT( "data4" )
						, texture( c3d_mapData4, vtx_texture ) );
					auto emissive = writer.declLocale( cuT( "emissive" )
						, data4.xyz() );
					auto flags = writer.declLocale( cuT( "flags" )
						, data1.w() );
					auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
						, 0_i );
					auto receiver = writer.declLocale( cuT( "receiver" )
						, 0_i );
					auto reflection = writer.declLocale( cuT( "reflection" )
						, 0_i );
					auto refraction = writer.declLocale( cuT( "refraction" )
						, 0_i );
					decodeMaterial( writer
						, flags
						, receiver
						, reflection
						, refraction
						, envMapIndex );
					auto data2 = writer.declLocale( cuT( "data2" )
						, texture( c3d_mapData2, vtx_texture ) );
					auto data3 = writer.declLocale( cuT( "data3" )
						, texture( c3d_mapData3, vtx_texture ) );
					auto occlusion = writer.declLocale( cuT( "occlusion" )
						, data3.a() );
					auto diffuse = writer.declLocale( cuT( "diffuse" )
						, data2.xyz() );

					IF( writer, "envMapIndex >= 1 && reflection != 0" )
					{
						auto reflect = writer.declLocale( cuT( "reflect" )
							, texture( c3d_mapReflection, vtx_texture ).xyz() );
						diffuse = occlusion * reflect;
						ambient = vec3( 0.0_f );
					}
					ELSE
					{
						if ( hasSsao )
						{
							ambient *= texture( c3d_mapSsao, vtx_texture ).r();
						}

						ambient *= diffuse;
						diffuse = postLight;
					}
					FI;

					IF( writer, "envMapIndex >= 1 && refraction != 0" )
					{
						auto refract = writer.declLocale( cuT( "refract" )
							, texture( c3d_mapRefraction, vtx_texture ) );

						IF( writer, reflection != 0_i )
						{
							ambient = mix( refract.xyz(), diffuse, refract.w() );
							diffuse = vec3( 0.0_f );
						}
						ELSE
						{
							ambient = refract.xyz();
							diffuse = vec3( 0.0_f );
						}
						FI;
					}
					FI;

					pxl_fragColor = vec4( diffuse + emissive + ambient, 1.0 );

					if ( fogType != FogType::eDisabled )
					{
						auto position = writer.declLocale( cuT( "position" )
							, utils.calcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						fog.applyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.finalise();
		}

		glsl::Shader doGetPbrPixelProgram( Engine & engine
			, FogType fogType
			, bool hasSsao )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto c3d_mapReflection = writer.declUniform< Sampler2D >( cuT( "c3d_mapReflection" ) );
			auto c3d_mapRefraction = writer.declUniform< Sampler2D >( cuT( "c3d_mapRefraction" ) );
			auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
			auto c3d_mapSsao = writer.declUniform< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao );
			auto c3d_mapPostLight = writer.declUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto c3d_mapIrradiance = writer.declUniform< SamplerCube >( ShaderProgram::MapIrradiance );
			auto c3d_mapPrefiltered = writer.declUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
			auto c3d_mapBrdf = writer.declUniform< Sampler2D >( ShaderProgram::MapBrdf );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();
			utils.declareCalcWSPosition();

			declareDecodeMaterial( writer );

			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ambient = writer.declLocale( cuT( "ambient" )
						, vec3( 0.0_f ) );
					auto light = writer.declLocale( cuT( "light" )
						, texture( c3d_mapPostLight, vtx_texture ).xyz() );
					auto data1 = writer.declLocale( cuT( "data1" )
						, texture( c3d_mapData1, vtx_texture ) );
					auto data4 = writer.declLocale( cuT( "data4" )
						, texture( c3d_mapData4, vtx_texture ) );
					auto emissive = writer.declLocale( cuT( "emissive" )
						, data4.xyz() );
					auto flags = writer.declLocale( cuT( "flags" )
						, data1.w() );
					auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
						, 0_i );
					auto receiver = writer.declLocale( cuT( "receiver" )
						, 0_i );
					auto reflection = writer.declLocale( cuT( "reflection" )
						, 0_i );
					auto refraction = writer.declLocale( cuT( "refraction" )
						, 0_i );
					decodeMaterial( writer
						, flags
						, receiver
						, reflection
						, refraction
						, envMapIndex );
					auto data3 = writer.declLocale( cuT( "data3" )
						, texture( c3d_mapData3, vtx_texture ) );
					auto occlusion = writer.declLocale( cuT( "occlusion" )
						, data3.a() );

					if ( hasSsao )
					{
						occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
					}

					ambient = texture( c3d_mapReflection, vtx_texture ).xyz();
					ambient *= c3d_ambientLight.xyz() * occlusion;

					IF( writer, envMapIndex >= 1_i )
					{
						auto refract = writer.declLocale( cuT( "refract" )
							, texture( c3d_mapRefraction, vtx_texture ) );

						IF( writer, refraction != 0_i )
						{
							IF( writer, reflection != 0_i )
							{
								ambient = mix( refract.xyz(), ambient, refract.w() );
							}
							ELSE
							{
								ambient = refract.xyz();
							}
							FI;
						}
						ELSEIF( writer, refract != vec4( -1.0_f ) )
						{
							ambient = mix( refract.xyz(), ambient, refract.w() );
						}
						FI;
					}
					ELSE
					{
						ambient = texture( c3d_mapReflection, vtx_texture ).xyz();
						ambient *= c3d_ambientLight.xyz() * occlusion;
						auto refract = writer.declLocale( cuT( "refract" )
							, texture( c3d_mapRefraction, vtx_texture ) );

						IF( writer, refract != vec4( -1.0_f ) )
						{
							ambient = mix( refract.xyz(), ambient, refract.w() );
						}
						FI;
					}
					FI;

					pxl_fragColor = vec4( light + emissive + ambient, 1.0 );

					if ( fogType != FogType::eDisabled )
					{
						auto position = writer.declLocale( cuT( "position" )
							, utils.calcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						fog.applyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.finalise();
		}

		ShaderProgramSPtr doCreateProgram( Engine & engine
			, FogType fogType
			, bool hasSsao
			, bool isPbr )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto vtx = doGetVertexProgram( engine );
			auto pxl = isPbr
				? doGetPbrPixelProgram( engine, fogType, hasSsao )
				: doGetLegacyPixelProgram( engine, fogType, hasSsao );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );

			program->createUniform< UniformType::eSampler >( cuT( "c3d_mapReflection" ), ShaderType::ePixel )->setValue( 0u );
			program->createUniform< UniformType::eSampler >( cuT( "c3d_mapRefraction" ), ShaderType::ePixel )->setValue( 1u );
			program->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eDepth ), ShaderType::ePixel )->setValue( 2u );
			program->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData1 ), ShaderType::ePixel )->setValue( 3u );
			program->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData2 ), ShaderType::ePixel )->setValue( 4u );
			program->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData3 ), ShaderType::ePixel )->setValue( 5u );
			program->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData4 ), ShaderType::ePixel )->setValue( 6u );

			if ( hasSsao )
			{
				program->createUniform< UniformType::eSampler >( cuT( "c3d_mapSsao" ), ShaderType::ePixel )->setValue( 7u );
			}

			program->createUniform< UniformType::eSampler >( cuT( "c3d_mapPostLight" ), ShaderType::ePixel )->setValue( 8u );

			if ( isPbr )
			{
				program->createUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance, ShaderType::ePixel )->setValue( 9u );
				program->createUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered, ShaderType::ePixel )->setValue( 10u );
				program->createUniform< UniformType::eSampler >( ShaderProgram::MapBrdf, ShaderType::ePixel )->setValue( 11u );
			}

			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		RenderPipelineUPtr doCreateRenderPipeline( Engine & engine
			, ShaderProgram & program
			, MatrixUbo & matrixUbo
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfo )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			BlendState blState;
			auto pipeline = engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, std::move( blState )
				, MultisampleState{}
				, program
				, PipelineFlags{} );
			pipeline->addUniformBuffer( matrixUbo.getUbo() );
			pipeline->addUniformBuffer( sceneUbo.getUbo() );
			pipeline->addUniformBuffer( gpInfo.getUbo() );
			return pipeline;
		}
	}

	//*********************************************************************************************

	CombineProgram::CombineProgram( Engine & engine
		, VertexBuffer & vbo
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfo
		, bool hasSsao
		, bool isPbr
		, FogType fogType )
		: m_program{ doCreateProgram( engine, fogType, hasSsao, isPbr ) }
		, m_geometryBuffers{ doCreateVao( engine, *m_program, vbo ) }
		, m_pipeline{ doCreateRenderPipeline( engine, *m_program, matrixUbo, sceneUbo, gpInfo ) }
	{
	}

	CombineProgram::~CombineProgram()
	{
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void CombineProgram::render( VertexBuffer const & vertexBuffer )const
	{
		m_pipeline->apply();
		m_geometryBuffers->draw( 6u, 0u );
	}

	//*********************************************************************************************

	CombinePass::CombinePass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, SsaoConfig const & config )
		: m_size{ size }
		, m_viewport{ engine }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_matrixUbo{ engine }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_programs
		{
			{
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, false, FogType::eDisabled },
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, false, FogType::eLinear },
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, false, FogType::eExponential },
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, false, FogType::eSquaredExponential },
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, true, FogType::eDisabled },
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, true, FogType::eLinear },
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, true, FogType::eExponential },
				CombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, true, FogType::eSquaredExponential }
			}
		}
		, m_ssaoEnabled{ config.m_enabled }
		, m_ssao{ engine, size, config, gpInfoUbo }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Combine" ) ) }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();
		m_matrixUbo.update( m_viewport.getProjection() );
	}

	CombinePass::~CombinePass()
	{
		m_matrixUbo.getUbo().cleanup();
		m_vertexBuffer->cleanup();
	}

	void CombinePass::render( GeometryPassResult const & gp
		, TextureUnit const & light
		, TextureUnit const & reflection
		, TextureUnit const & refraction
		, Fog const & fog
		, FrameBuffer const & frameBuffer
		, RenderInfo & info )
	{
		TextureUnit const * ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.render( gp, info );
			ssao = &m_ssao.getResult();
		}

		m_timer->start();
		frameBuffer.bind( FrameBufferTarget::eDraw );
		frameBuffer.setDrawBuffers();
		frameBuffer.clear( BufferComponent::eColour );

		m_viewport.apply();
		uint32_t index{ 0u };
		reflection.getTexture()->bind( index );
		reflection.getSampler()->bind( index );
		++index;
		refraction.getTexture()->bind( index );
		refraction.getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eDepth )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData1 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData2 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData2 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData3 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData3 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData4 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->bind( index );
		++index;

		if ( m_ssaoEnabled )
		{
			ssao->getSampler()->bind( index );
			ssao->getTexture()->bind( index );
		}

		++index;
		light.getTexture()->bind( index );
		light.getSampler()->bind( index );

		m_programs[size_t( fog.getType() )].render( *m_vertexBuffer );

		light.getTexture()->unbind( index );
		light.getSampler()->unbind( index );
		--index;

		if ( m_ssaoEnabled )
		{
			ssao->getSampler()->unbind( index );
			ssao->getTexture()->unbind( index );
		}

		--index;
		gp[size_t( DsTexture::eData4 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData3 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData3 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData2 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData2 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData1 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eDepth )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->unbind( index );
		--index;
		refraction.getTexture()->bind( index );
		refraction.getSampler()->bind( index );
		--index;
		reflection.getTexture()->bind( index );
		reflection.getSampler()->bind( index );
		ENSURE( index == 0u );

		m_timer->stop();
	}

	void CombinePass::render( GeometryPassResult const & gp
		, TextureUnit const & light
		, TextureUnit const & reflection
		, TextureUnit const & refraction
		, IblTextures const & ibl
		, Fog const & fog
		, FrameBuffer const & frameBuffer
		, RenderInfo & info )
	{
		TextureUnit const * ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.render( gp, info );
			ssao = &m_ssao.getResult();
		}

		m_timer->start();
		frameBuffer.bind( FrameBufferTarget::eDraw );
		frameBuffer.setDrawBuffers();

		m_viewport.apply();
		uint32_t index{ 0u };
		reflection.getTexture()->bind( index );
		reflection.getSampler()->bind( index );
		++index;
		refraction.getTexture()->bind( index );
		refraction.getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eDepth )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData1 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData2 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData2 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData3 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData3 )]->getSampler()->bind( index );
		++index;
		gp[size_t( DsTexture::eData4 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->bind( index );
		++index;

		if ( m_ssaoEnabled )
		{
			ssao->getSampler()->bind( index );
			ssao->getTexture()->bind( index );
		}

		++index;
		light.getTexture()->bind( index );
		light.getSampler()->bind( index );
		++index;
		ibl.getIrradiance().getTexture()->bind( index );
		ibl.getIrradiance().getSampler()->bind( index );
		++index;
		ibl.getPrefilteredEnvironment().getTexture()->bind( index );
		ibl.getPrefilteredEnvironment().getSampler()->bind( index );
		++index;
		ibl.getPrefilteredBrdf().getTexture()->bind( index );
		ibl.getPrefilteredBrdf().getSampler()->bind( index );

		m_programs[size_t( fog.getType() ) + size_t( FogType::eCount )].render( *m_vertexBuffer );

		ibl.getPrefilteredBrdf().getSampler()->unbind( index );
		ibl.getPrefilteredBrdf().getTexture()->unbind( index );
		--index;
		ibl.getPrefilteredEnvironment().getSampler()->unbind( index );
		ibl.getPrefilteredEnvironment().getTexture()->unbind( index );
		--index;
		ibl.getIrradiance().getSampler()->unbind( index );
		ibl.getIrradiance().getTexture()->unbind( index );
		--index;
		light.getSampler()->unbind( index );
		light.getTexture()->unbind( index );
		--index;

		if ( m_ssaoEnabled )
		{
			ssao->getSampler()->unbind( index );
			ssao->getTexture()->unbind( index );
		}

		--index;
		gp[size_t( DsTexture::eData4 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData3 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData3 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData2 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData2 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eData1 )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->unbind( index );
		--index;
		gp[size_t( DsTexture::eDepth )]->getTexture()->unbind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->unbind( index );
		--index;
		refraction.getTexture()->unbind( index );
		refraction.getSampler()->unbind( index );
		--index;
		reflection.getTexture()->unbind( index );
		reflection.getSampler()->unbind( index );
		ENSURE( index == 0u );

		m_timer->stop();
	}

	//*********************************************************************************************
}
