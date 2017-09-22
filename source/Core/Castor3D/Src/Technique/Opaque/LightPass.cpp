#include "LightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Shader/Ubos/ModelMatrixUbo.hpp>
#include <Shader/Ubos/SceneUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslShadow.hpp"
#include "Shader/Shaders/GlslPhongLighting.hpp"
#include "Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Shader/Shaders/GlslSubsurfaceScattering.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//************************************************************************************************

	String getTextureName( DsTexture texture )
	{
		static std::array< String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapDepth" ),
				cuT( "c3d_mapData1" ),
				cuT( "c3d_mapData2" ),
				cuT( "c3d_mapData3" ),
				cuT( "c3d_mapData4" ),
			}
		};

		return Values[size_t( texture )];
	}

	PixelFormat getTextureFormat( DsTexture texture )
	{
		static std::array< PixelFormat, size_t( DsTexture::eCount ) > Values
		{
			{
				PixelFormat::eD24S8,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
			}
		};

		return Values[size_t( texture )];
	}

	AttachmentPoint getTextureAttachmentPoint( DsTexture texture )
	{
		static std::array< AttachmentPoint, size_t( DsTexture::eCount ) > Values
		{
			{
				AttachmentPoint::eDepthStencil,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
			}
		};

		return Values[size_t( texture )];
	}

	uint32_t getTextureAttachmentIndex( DsTexture texture )
	{
		static std::array< uint32_t, size_t( DsTexture::eCount ) > Values
		{
			{
				0,
				0,
				1,
				2,
				3,
			}
		};

		return Values[size_t( texture )];
	}

	float getMaxDistance( LightCategory const & light
		, Point3f const & attenuation )
	{
		constexpr float threshold = 0.000001f;
		auto constant = std::abs( attenuation[0] );
		auto linear = std::abs( attenuation[1] );
		auto quadratic = std::abs( attenuation[2] );
		float result = std::numeric_limits< float >::max();

		if ( constant >= threshold
			|| linear >= threshold
			|| quadratic >= threshold )
		{
			float maxChannel = std::max( std::max( light.getColour()[0]
				, light.getColour()[1] )
				, light.getColour()[2] );
			result = 256.0f * maxChannel * light.getDiffuseIntensity();

			if ( quadratic >= threshold )
			{
				if ( linear < threshold )
				{
					REQUIRE( result >= constant );
					result = sqrtf( ( result - constant ) / quadratic );
				}
				else
				{
					auto delta = linear * linear - 4 * quadratic * ( constant - result );
					REQUIRE( delta >= 0 );
					result = ( -linear + sqrtf( delta ) ) / ( 2 * quadratic );
				}
			}
			else if ( linear >= threshold )
			{
				result = ( result - constant ) / linear;
			}
			else
			{
				result = 4000.0f;
			}
		}
		else
		{
			Logger::logError( cuT( "Light's attenuation is set to (0.0, 0.0, 0.0), which results in infinite litten distance, not representable." ) );
		}

		return result;
	}

	float getMaxDistance( LightCategory const & light
		, Point3f const & attenuation
		, float max )
	{
		return std::min( max, getMaxDistance( light, attenuation ) );
	}

	void declareEncodeMaterial( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		using glsl::operator<<;
		writer.implementFunction< Void >( cuT( "encodeMaterial" )
			, [&]( Int const & receiver
				, Int const & reflection
				, Int const & refraction
				, Int const & envMapIndex
				, Int const & materialId
				, Float encoded )
			{
				auto flags = writer.declLocale( cuT( "flags" )
					, writer.paren( materialId << 8 )
						+ writer.paren( receiver << 7 )
						+ writer.paren( refraction << 6 )
						+ writer.paren( reflection << 5 )
						+ writer.paren( envMapIndex ) );
				encoded = writer.cast< Float >( flags );
			}, InInt{ &writer, cuT( "receiver" ) }
			, InInt{ &writer, cuT( "reflection" ) }
			, InInt{ &writer, cuT( "refraction" ) }
			, InInt{ &writer, cuT( "envMapIndex" ) }
			, InInt{ &writer, cuT( "materialId" ) }
			, OutFloat{ &writer, cuT( "encoded" ) } );
	}
	
	void declareDecodeMaterial( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		writer.implementFunction< Void >( cuT( "decodeMaterial" )
			, [&]( Float const & encoded
				, Int receiver
				, Int reflection
				, Int refraction
				, Int envMapIndex
				, Int materialId )
			{
				auto flags = writer.declLocale( cuT( "flags" ), writer.cast< Int >( encoded ) );
				materialId = flags >> 8;
				flags -= writer.paren( materialId << 8 );
				receiver = flags >> 7;
				flags -= writer.paren( receiver << 7 );
				refraction = flags >> 6;
				flags -= writer.paren( refraction << 6 );
				reflection = flags >> 5;
				flags -= writer.paren( reflection << 5 );
				envMapIndex = flags;
			}, InFloat{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) }
			, OutInt{ &writer, cuT( "reflection" ) }
			, OutInt{ &writer, cuT( "refraction" ) }
			, OutInt{ &writer, cuT( "envMapIndex" ) }
			, OutInt{ &writer, cuT( "materialId" ) } );
	}

	void declareDecodeReceiver( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		writer.implementFunction< Void >( cuT( "decodeReceiver" )
			, [&]( Int const & encoded
				, Int receiver )
			{
				auto flags = writer.declLocale( cuT( "flags" ), encoded );
				receiver = flags >> 7;
			}, InInt{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) } );
	}

	void declareDecodeReceiverAndID( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		writer.implementFunction< Void >( cuT( "decodeReceiverAndID" )
			, [&]( Int const & encoded
				, Int receiver
				, Int materialId )
			{
				auto flags = writer.declLocale( cuT( "flags" ), encoded );
				materialId = flags >> 8;
				flags -= writer.paren( materialId << 8 );
				receiver = flags >> 7;
			}, InInt{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) }
			, OutInt{ &writer, cuT( "materialId" ) } );
	}

	void encodeMaterial( glsl::GlslWriter & writer
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex
		, glsl::Int const & materialId
		, glsl::Float const & encoded )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "encodeMaterial" )
			, InInt{ receiver }
			, InInt{ reflection }
			, InInt{ refraction }
			, InInt{ envMapIndex }
			, InInt{ materialId }
			, OutFloat{ encoded } );
		writer << endi;
	}

	void decodeMaterial( glsl::GlslWriter & writer
		, glsl::Float const & encoded
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex
		, glsl::Int const & materialId )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "decodeMaterial" )
			, InFloat{ encoded }
			, OutInt{ receiver }
			, OutInt{ reflection }
			, OutInt{ refraction }
			, OutInt{ envMapIndex }
			, OutInt{ materialId } );
		writer << endi;
	}

	void decodeReceiver( glsl::GlslWriter & writer
		, glsl::Int & encoded
		, glsl::Int const & receiver )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "decodeReceiver" )
			, InInt{ encoded }
			, OutInt{ receiver } );
		writer << endi;
	}

	void decodeReceiverAndID( glsl::GlslWriter & writer
		, glsl::Int & encoded
		, glsl::Int const & receiver
		, glsl::Int const & materialId )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "decodeReceiverAndID" )
			, InInt{ encoded }
			, OutInt{ receiver }
			, OutInt{ materialId } );
		writer << endi;
	}

	//************************************************************************************************

	LightPass::Program::Program( Engine & engine
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl )
	{
		auto & renderSystem = *engine.getRenderSystem();

		m_program = engine.getShaderProgramCache().getNewProgram( false );
		m_program->createObject( ShaderType::eVertex );
		m_program->createObject( ShaderType::ePixel );
		m_program->setSource( ShaderType::eVertex, vtx );
		m_program->setSource( ShaderType::ePixel, pxl );

		m_lightColour = m_program->createUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_colour" ), ShaderType::ePixel );

		for ( int i = 0; i < int( DsTexture::eCount ); i++ )
		{
			m_program->createUniform< UniformType::eSampler >( getTextureName( DsTexture( i ) ), ShaderType::ePixel )->setValue( MinTextureIndex + i );
		}
	}

	LightPass::Program::~Program()
	{
		m_firstPipeline->cleanup();
		m_blendPipeline->cleanup();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_lightColour = nullptr;
		m_program.reset();
	}

	void LightPass::Program::initialise( VertexBuffer & vbo
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, UniformBuffer & gpInfoUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		m_program->initialise();

		m_firstPipeline = doCreatePipeline( false );
		m_firstPipeline->addUniformBuffer( matrixUbo.getUbo() );
		m_firstPipeline->addUniformBuffer( sceneUbo.getUbo() );
		m_firstPipeline->addUniformBuffer( gpInfoUbo );

		m_blendPipeline = doCreatePipeline( true );
		m_blendPipeline->addUniformBuffer( matrixUbo.getUbo() );
		m_blendPipeline->addUniformBuffer( sceneUbo.getUbo() );
		m_blendPipeline->addUniformBuffer( gpInfoUbo );

		if ( modelMatrixUbo )
		{
			m_firstPipeline->addUniformBuffer( modelMatrixUbo->getUbo() );
			m_blendPipeline->addUniformBuffer( modelMatrixUbo->getUbo() );
		}

		m_geometryBuffers = m_program->getRenderSystem()->createGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->initialise( { vbo }, nullptr );
	}

	void LightPass::Program::cleanup()
	{
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_program->cleanup();
	}

	void LightPass::Program::bind( Light const & light )
	{
		doBind( light );
	}

	void LightPass::Program::render( Size const & size
		, Point3f const & colour
		, uint32_t count
		, bool first
		, uint32_t offset )const
	{
		m_lightColour->setValue( colour );

		if ( first )
		{
			m_firstPipeline->apply();
		}
		else
		{
			m_blendPipeline->apply();
		}

		m_geometryBuffers->draw( count, offset );
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & engine
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: m_engine{ engine }
		, m_shadows{ hasShadows }
		, m_matrixUbo{ engine }
		, m_frameBuffer{ frameBuffer }
		, m_depthAttach{ depthAttach }
		, m_gpInfoUbo{ gpInfoUbo }
	{
	}

	void LightPass::render( Size const & size
		, GeometryPassResult const & gp
		, Light const & light
		, Camera const & camera
		, bool first
		, ShadowMap * shadowMapOpt )
	{
		doUpdate( size
			, light
			, camera );

		m_program->bind( light );

		doRender( size
			, gp
			, light.getColour()
			, first );
	}

	void LightPass::doInitialise( Scene const & scene
		, LightType type
		, VertexBuffer & vbo
		, SceneUbo & sceneUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		SceneFlags sceneFlags{ scene.getFlags() };

		if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness )
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetPbrMRPixelShaderSource( sceneFlags, type ) );
		}
		else if ( scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetPbrSGPixelShaderSource( sceneFlags, type ) );
		}
		else
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetLegacyPixelShaderSource( sceneFlags, type ) );
		}

		m_program->initialise( vbo
			, m_matrixUbo
			, sceneUbo
			, m_gpInfoUbo.getUbo()
			, modelMatrixUbo );
	}

	void LightPass::doCleanup()
	{
		m_program->cleanup();
		m_program.reset();
		m_matrixUbo.getUbo().cleanup();
	}

	void LightPass::doRender( castor::Size const & size
		, GeometryPassResult const & gp
		, Point3f const & colour
		, bool first )
	{
		m_frameBuffer.bind( FrameBufferTarget::eDraw );
		m_depthAttach.attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.setDrawBuffers();
		gp[size_t( DsTexture::eDepth )]->bind();
		gp[size_t( DsTexture::eData1 )]->bind();
		gp[size_t( DsTexture::eData2 )]->bind();
		gp[size_t( DsTexture::eData3 )]->bind();
		gp[size_t( DsTexture::eData4 )]->bind();

		m_program->render( size
			, colour
			, getCount()
			, first
			, m_offset );

		gp[size_t( DsTexture::eData4 )]->unbind();
		gp[size_t( DsTexture::eData3 )]->unbind();
		gp[size_t( DsTexture::eData2 )]->unbind();
		gp[size_t( DsTexture::eData1 )]->unbind();
		gp[size_t( DsTexture::eDepth )]->unbind();
		m_frameBuffer.unbind();
	}
	
	glsl::Shader LightPass::doGetLegacyPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto shadowType = getShadowType( sceneFlags );

		if ( type == LightType::ePoint && shadowType != ShadowType::eNone )
		{
			auto c3d_lightFarPlane = writer.declUniform< Float >( cuT( "c3d_lightFarPlane" ) );
		}

		// Shader outputs
		auto pxl_diffuse = writer.declFragData< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declFragData< Vec3 >( cuT( "pxl_specular" ), 1 );

		// Utility functions
		auto lighting = shader::legacy::createLightingModel( writer
			, type
			, m_shadows ? getShadowType( sceneFlags ) : ShadowType::eNone );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		declareDecodeReceiverAndID( writer );
		shader::LegacyMaterials materials{ writer };
		materials.declare();
		shader::SubsurfaceScattering sss{ writer
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( type );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.calcTexCoord() );
			auto data1 = writer.declLocale( cuT( "data1" )
				, texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, texture( c3d_mapData4, texCoord ) );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, 0_i );
			decodeReceiverAndID( writer, flags, shadowReceiver, materialId );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, data2.xyz() );
			auto shininess = writer.declLocale( cuT( "shininess" )
				, data2.w() );
			auto specular = writer.declLocale( cuT( "specular" )
				, data3.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition );
			auto depth = writer.declLocale( cuT( "depth" )
				, texture( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto translucency = writer.declLocale( cuT( "translucency" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.getBuiltin< shader::DirectionalLight >( cuT( "light" ) );
					lighting->computeDirectionalLight( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye );
				}
				break;

			case LightType::ePoint:
				{
					auto light = writer.getBuiltin< shader::PointLight >( cuT( "light" ) );
					lighting->computeOnePointLight( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye );
				}
				break;

			case LightType::eSpot:
				{
					auto light = writer.getBuiltin< shader::SpotLight >( cuT( "light" ) );
					lighting->computeOneSpotLight( light
						, eye
						, shininess
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye );
				}
				break;
			}

			pxl_diffuse = lightDiffuse * diffuse;
			pxl_specular = lightSpecular * specular;
		} );

		return writer.finalise();
	}
	
	glsl::Shader LightPass::doGetPbrMRPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto shadowType = getShadowType( sceneFlags );

		if ( type == LightType::ePoint && shadowType != ShadowType::eNone )
		{
			auto c3d_lightFarPlane = writer.declUniform< Float >( cuT( "c3d_lightFarPlane" ) );
		}

		// Shader outputs
		auto pxl_diffuse = writer.declFragData< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declFragData< Vec3 >( cuT( "pxl_specular" ), 1 );

		// Utility functions
		auto lighting = shader::pbr::mr::createLightingModel( writer
			, type
			, m_shadows ? shadowType : ShadowType::eNone );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		declareDecodeReceiverAndID ( writer );
		shader::PbrMRMaterials materials{ writer };
		materials.declare();
		shader::SubsurfaceScattering sss{ writer
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( type );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.calcTexCoord() );
			auto data1 = writer.declLocale( cuT( "data1" )
				, texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, texture( c3d_mapData4, texCoord ) );
			auto metallic = writer.declLocale( cuT( "metallic" )
				, data3.r() );
			auto roughness = writer.declLocale( cuT( "roughness" )
				, data3.g() );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, 0_i );
			decodeReceiverAndID( writer
				, flags
				, shadowReceiver
				, materialId );
			auto albedo = writer.declLocale( cuT( "albedo" )
				, data2.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition );
			auto depth = writer.declLocale( cuT( "depth" )
				, texture( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto translucency = writer.declLocale( cuT( "translucency" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.getBuiltin< shader::DirectionalLight >( cuT( "light" ) );
					lighting->computeDirectionalLight( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye
						, albedo
						, metallic );
				}
				break;

			case LightType::ePoint:
				{
					auto light = writer.getBuiltin< shader::PointLight >( cuT( "light" ) );
					lighting->computeOnePointLight( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye
						, albedo
						, metallic );
				}
				break;

			case LightType::eSpot:
				{
					auto light = writer.getBuiltin< shader::SpotLight >( cuT( "light" ) );
					lighting->computeOneSpotLight( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye
						, albedo
						, metallic );
				}
				break;
			}

			pxl_diffuse = lightDiffuse;
			pxl_specular = lightSpecular;
		} );

		return writer.finalise();
	}
	
	glsl::Shader LightPass::doGetPbrSGPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto shadowType = getShadowType( sceneFlags );

		if ( type == LightType::ePoint && shadowType != ShadowType::eNone )
		{
			auto c3d_lightFarPlane = writer.declUniform< Float >( cuT( "c3d_lightFarPlane" ) );
		}

		// Utility functions
		auto lighting = shader::pbr::sg::createLightingModel( writer
			, type
			, m_shadows ? getShadowType( sceneFlags ) : ShadowType::eNone );
		shader::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		shader::PbrSGMaterials materials{ writer };
		materials.declare();
		declareDecodeReceiverAndID( writer );
		shader::SubsurfaceScattering sss{ writer
			, m_shadows && shadowType != ShadowType::eNone };
		sss.declare( type );

		// Shader outputs
		auto pxl_diffuse = writer.declFragData< Vec3 >( cuT( "pxl_diffuse" ), 0 );
		auto pxl_specular = writer.declFragData< Vec3 >( cuT( "pxl_specular" ), 1 );
		auto pxl_backLit = writer.declFragData< Vec3 >( cuT( "pxl_backLit" ), 1 );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, utils.calcTexCoord() );
			auto data1 = writer.declLocale( cuT( "data1" )
				, texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" )
				, texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" )
				, texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" )
				, texture( c3d_mapData4, texCoord ) );
			auto specular = writer.declLocale( cuT( "specular" )
				, data3.rgb() );
			auto glossiness = writer.declLocale( cuT( "glossiness" )
				, data2.a() );
			auto flags = writer.declLocale( cuT( "flags" )
				, writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" )
				, 0_i );
			auto materialId = writer.declLocale( cuT( "materialId" )
				, 0_i );
			decodeReceiverAndID( writer
				, flags
				, shadowReceiver
				, materialId );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, data2.xyz() );
			auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
				, vec3( 0.0_f ) );
			auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
				, vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" )
				, c3d_cameraPosition );
			auto depth = writer.declLocale( cuT( "depth" )
				, texture( c3d_mapDepth, texCoord, 0.0_f ).x() );
			auto wsPosition = writer.declLocale( cuT( "wsPosition" )
				, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" )
				, data1.xyz() );
			auto translucency = writer.declLocale( cuT( "translucency" )
				, data4.w() );
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( materialId ) );

			shader::OutputComponents output{ lightDiffuse, lightSpecular };

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.getBuiltin< shader::DirectionalLight >( cuT( "light" ) );
					lighting->computeDirectionalLight( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye
						, specular );
				}
				break;

			case LightType::ePoint:
				{
					auto light = writer.getBuiltin< shader::PointLight >( cuT( "light" ) );
					lighting->computeOnePointLight( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye
						, specular );
				}
				break;

			case LightType::eSpot:
				{
					auto light = writer.getBuiltin< shader::SpotLight >( cuT( "light" ) );
					lighting->computeOneSpotLight( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, shader::FragmentInput( wsPosition, wsNormal )
						, output );
					lightDiffuse += sss.computeOne( *lighting
						, material
						, light
						, texCoord
						, wsPosition
						, wsNormal
						, translucency
						, eye
						, specular );
				}
				break;
			}

			pxl_diffuse = lightDiffuse;
			pxl_specular = lightSpecular;
		} );

		return writer.finalise();
	}

	//************************************************************************************************
}
