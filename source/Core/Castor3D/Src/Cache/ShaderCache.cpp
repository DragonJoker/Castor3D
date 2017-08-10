#include "ShaderCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderPass.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		uint64_t MakeKey( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals )
		{
			return ( uint64_t( textureFlags ) << 52 ) // Texture flags on 12 bits
				   | ( uint64_t( programFlags ) << 36 ) // Program flags on 16 bits
				   | ( uint64_t( sceneFlags ) << 28 ) // SceneFlags on 8 bits
				   | ( uint64_t( alphaFunc ) << 20 ) // Alpha func on 8 bits
				   | ( uint64_t( invertNormals ? 0x01 : 0x00 ) );
		}
	}

	ShaderProgramCache::ShaderProgramCache( Engine & engine )
		: OwnedBy< Engine >( engine )
	{
	}

	ShaderProgramCache::~ShaderProgramCache()
	{
	}

	void ShaderProgramCache::cleanup()
	{
		for ( auto program : m_arrayPrograms )
		{
			getEngine()->postEvent( MakeCleanupEvent( *program ) );
		}
	}

	void ShaderProgramCache::clear()
	{
		m_mapBillboards.clear();
		m_mapAutogenerated.clear();
		m_arrayPrograms.clear();
	}

	ShaderProgramSPtr ShaderProgramCache::getNewProgram( bool p_initialise )
	{
		ShaderProgramSPtr result = getEngine()->getRenderSystem()->createShaderProgram();

		if ( result )
		{
			doAddProgram( result, p_initialise );
		}

		return result;
	}

	ShaderProgramSPtr ShaderProgramCache::getAutomaticProgram( RenderPass const & p_renderPass
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc
		, bool invertNormals )
	{
		ShaderProgramSPtr result;

		if ( checkFlag( programFlags, ProgramFlag::eBillboards ) )
		{
			uint64_t key = MakeKey( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, false );
			auto const & it = m_mapBillboards.find( key );

			if ( it != m_mapBillboards.end() )
			{
				result = it->second.lock();
			}
			else
			{
				result = doCreateBillboardProgram( p_renderPass
					, textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc );

				if ( result )
				{
					doAddBillboardProgram( result
						, textureFlags
						, programFlags
						, sceneFlags
						, alphaFunc );
				}
			}
		}
		else
		{
			uint64_t key = MakeKey( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, invertNormals );
			ShaderProgramWPtrUInt64MapConstIt it = m_mapAutogenerated.find( key );

			if ( it != m_mapAutogenerated.end() )
			{
				result = it->second.lock();
			}
			else
			{
				result = doCreateAutomaticProgram( p_renderPass
					, textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc
					, invertNormals );

				if ( result )
				{
					doAddAutomaticProgram( result
						, textureFlags
						, programFlags
						, sceneFlags
						, alphaFunc
						, invertNormals );
				}
			}
		}

		return result;
	}

	void ShaderProgramCache::createTextureVariables(
		ShaderProgram & p_shader,
		TextureChannels const & textureFlags,
		ProgramFlags const & programFlags )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eLighting ) )
		{
			p_shader.createUniform< UniformType::eSampler >( ShaderProgram::Lights, ShaderType::ePixel )->setValue( Pass::LightBufferIndex );
		}

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapNormal, ShaderType::ePixel );
		}

		if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
		{
			p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapEmissive, ShaderType::ePixel );
		}

		if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
		{
			p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapOpacity, ShaderType::ePixel );
		}

		if ( checkFlag( textureFlags, TextureChannel::eHeight ) )
		{
			p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapHeight, ShaderType::ePixel );
		}

		if ( checkFlag( programFlags, ProgramFlag::ePbrMetallicRoughness ) )
		{
			if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapAlbedo, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eMetallic ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapMetallic, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eRoughness ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapRoughness, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapAmbientOcclusion, ShaderType::ePixel );
			}

			p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel );
		}
		else if ( checkFlag( programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
		{
			if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapSpecular, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapGloss, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapAmbientOcclusion, ShaderType::ePixel );
			}

			p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel );
		}
		else
		{
			if ( checkFlag( textureFlags, TextureChannel::eReflection )
				|| checkFlag( textureFlags, TextureChannel::eRefraction ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapSpecular, ShaderType::ePixel );
			}

			if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
			{
				p_shader.createUniform< UniformType::eSampler >( ShaderProgram::MapGloss, ShaderType::ePixel );
			}
		}
	}

	void ShaderProgramCache::doAddProgram( ShaderProgramSPtr p_program, bool p_initialise )
	{
		m_arrayPrograms.push_back( p_program );

		if ( p_initialise )
		{
			if ( getEngine()->getRenderSystem()->getCurrentContext() )
			{
				p_program->initialise();
			}
			else
			{
				getEngine()->postEvent( MakeInitialiseEvent( *p_program ) );
			}
		}
	}

	ShaderProgramSPtr ShaderProgramCache::doCreateAutomaticProgram( RenderPass const & p_renderPass
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc
		, bool invertNormals )const
	{
		ShaderProgramSPtr result = getEngine()->getRenderSystem()->createShaderProgram();

		if ( result )
		{
			ShaderTypeFlags matrixUboShaderMask = ShaderTypeFlag::eVertex | ShaderTypeFlag::ePixel;
			result->createObject( ShaderType::eVertex );
			result->createObject( ShaderType::ePixel );
			result->setSource( ShaderType::eVertex, p_renderPass.getVertexShaderSource( textureFlags, programFlags, sceneFlags, invertNormals ) );
			result->setSource( ShaderType::ePixel, p_renderPass.getPixelShaderSource( textureFlags, programFlags, sceneFlags, alphaFunc ) );
			auto geometry = p_renderPass.getGeometryShaderSource( textureFlags, programFlags, sceneFlags );

			if ( !geometry.getSource().empty() )
			{
				addFlag( matrixUboShaderMask, ShaderTypeFlag::eGeometry );
				result->createObject( ShaderType::eGeometry );
				result->setSource( ShaderType::eGeometry, geometry );
			}

			createTextureVariables( *result, textureFlags, programFlags );
		}

		return result;
	}

	void ShaderProgramCache::doAddAutomaticProgram( ShaderProgramSPtr p_program
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc
		, bool invertNormals )
	{
		uint64_t key = MakeKey( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc
			, invertNormals );
		auto const & it = m_mapAutogenerated.find( key );

		if ( it == m_mapAutogenerated.end() )
		{
			m_mapAutogenerated.insert( { key, p_program } );
			doAddProgram( p_program, true );
		}
	}

	ShaderProgramSPtr ShaderProgramCache::doCreateBillboardProgram( RenderPass const & p_renderPass
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		ShaderProgramSPtr result = getEngine()->getRenderSystem()->createShaderProgram();

		if ( result )
		{
			GLSL::Shader strVtxShader;
			{
				using namespace GLSL;
				auto writer = renderSystem.createGlslWriter();

				// Shader inputs
				auto position = writer.declAttribute< Vec4 >( ShaderProgram::Position );
				auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );
				auto center = writer.declAttribute< Vec3 >( cuT( "center" ) );
				auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );
				auto gl_VertexID( writer.declBuiltin< Int >( cuT( "gl_VertexID" ) ) );
				UBO_MATRIX( writer );
				UBO_MODEL_MATRIX( writer );
				UBO_SCENE( writer );
				UBO_MODEL( writer );
				UBO_BILLBOARD( writer );

				// Shader outputs
				auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ) );
				auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" ) );
				auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" ) );
				auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" ) );
				auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
				auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" ) );
				auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ) );
				auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

				writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					auto bbcenter = writer.declLocale( cuT( "bbcenter" ), writer.paren( c3d_mtxModel * vec4( center, 1.0 ) ).xyz() );
					auto toCamera = writer.declLocale( cuT( "toCamera" ), c3d_v3CameraPosition - bbcenter );
					toCamera.y() = 0.0_f;
					toCamera = normalize( toCamera );
					auto right = writer.declLocale( cuT( "right" ), vec3( c3d_mtxView[0][0], c3d_mtxView[1][0], c3d_mtxView[2][0] ) );
					auto up = writer.declLocale( cuT( "up" ), vec3( c3d_mtxView[0][1], c3d_mtxView[1][1], c3d_mtxView[2][1] ) );

					if ( !checkFlag( programFlags, ProgramFlag::eSpherical ) )
					{
						right = normalize( vec3( right.x(), 0.0, right.z() ) );
						up = vec3( 0.0_f, 1.0f, 0.0f );
					}

					vtx_material = c3d_materialIndex;
					vtx_normal = toCamera;
					vtx_tangent = up;
					vtx_bitangent = right;

					auto width = writer.declLocale( cuT( "width" ), c3d_v2iDimensions.x() );
					auto height = writer.declLocale( cuT( "height" ), c3d_v2iDimensions.y() );

					if ( checkFlag( programFlags, ProgramFlag::eFixedSize ) )
					{
						width = c3d_v2iDimensions.x() / c3d_v2iWindowSize.x();
						height = c3d_v2iDimensions.y() / c3d_v2iWindowSize.y();
					}

					vtx_position = bbcenter
						+ right * position.x() * width
						+ up * position.y() * height;

					vtx_texture = vec3( texture, 0.0 );
					vtx_instance = gl_InstanceID;
					auto wvPosition = writer.declLocale( cuT( "wvPosition" ), writer.paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
					gl_Position = c3d_mtxProjection * vec4( wvPosition, 1.0 );
				} );

				strVtxShader = writer.finalise();
			}

			GLSL::Shader strPxlShader = p_renderPass.getPixelShaderSource( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );

			result->createObject( ShaderType::eVertex );
			result->createObject( ShaderType::ePixel );
			result->setSource( ShaderType::eVertex, strVtxShader );
			result->setSource( ShaderType::ePixel, strPxlShader );

			createTextureVariables( *result, textureFlags, programFlags );
		}

		return result;
	}

	void ShaderProgramCache::doAddBillboardProgram( ShaderProgramSPtr p_program
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )
	{
		uint64_t key = MakeKey( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc
			, false );
		auto const & it = m_mapBillboards.find( key );

		if ( it == m_mapBillboards.end() )
		{
			m_mapBillboards.insert( { key, p_program } );
			doAddProgram( p_program, true );
		}
	}
}
