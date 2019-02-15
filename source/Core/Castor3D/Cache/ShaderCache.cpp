#include "Castor3D/Cache/ShaderCache.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/CleanupEvent.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Material/Pass.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ShaderWriter/Source.hpp>

#include <Ashes/Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		uint64_t makeKey( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc
			, bool invertNormals )
		{
			return ( uint64_t( passFlags ) << 56 ) // Pass flags on 8 bits
				   | ( uint64_t( textureFlags ) << 44 ) // Texture flags on 12 bits
				   | ( uint64_t( programFlags ) << 28 ) // Program flags on 16 bits
				   | ( uint64_t( sceneFlags ) << 20 ) // SceneFlags on 8 bits
				   | ( uint64_t( alphaFunc ) << 12 ) // Alpha func on 8 bits
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
		auto lock = makeUniqueLock( m_mutex );

		for ( auto & program : m_arrayPrograms )
		{
			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [&program]()
				{
					program->cleanup();
				} ) );
		}
	}

	void ShaderProgramCache::clear()
	{
		auto lock = makeUniqueLock( m_mutex );
		m_mapBillboards.clear();
		m_mapAutogenerated.clear();
		m_arrayPrograms.clear();
	}

	ShaderProgramSPtr ShaderProgramCache::getNewProgram( bool initialise )
	{
		return doAddProgram( std::make_shared< ShaderProgram >( *getEngine()->getRenderSystem() ), initialise );
	}

	ShaderProgramSPtr ShaderProgramCache::getAutomaticProgram( RenderPass const & renderPass
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc
		, bool invertNormals )
	{
		if ( checkFlag( programFlags, ProgramFlag::eBillboards ) )
		{
			uint64_t key = makeKey( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, false );
			auto lock = makeUniqueLock( m_mutex );
			auto const & it = m_mapBillboards.find( key );

			if ( it != m_mapBillboards.end() )
			{
				return it->second;
			}
			else
			{
				auto result = doCreateBillboardProgram( renderPass
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc );
				CU_Require( result );
				return doAddBillboardProgram( std::move( result )
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc );
			}
		}
		else
		{
			uint64_t key = makeKey( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, invertNormals );
			auto lock = makeUniqueLock( m_mutex );
			auto it = m_mapAutogenerated.find( key );

			if ( it != m_mapAutogenerated.end() )
			{
				return it->second;
			}
			else
			{
				auto result = doCreateAutomaticProgram( renderPass
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc
					, invertNormals );
				CU_Require( result );
				return doAddAutomaticProgram( std::move( result )
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc
					, invertNormals );
			}
		}
	}

	ShaderProgramSPtr ShaderProgramCache::doAddProgram( ShaderProgramSPtr program
		, bool initialise )
	{
		auto lock = makeUniqueLock( m_mutex );
		m_arrayPrograms.push_back( program );

		if ( initialise )
		{
			getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
				, [program]()
				{
					program->initialise();
				} ) );
		}

		return m_arrayPrograms.back();
	}

	ShaderProgramSPtr ShaderProgramCache::doCreateAutomaticProgram( RenderPass const & renderPass
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc
		, bool invertNormals )const
	{
		ashes::ShaderStageFlags matrixUboShaderMask = ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment;
		ShaderProgramSPtr result = std::make_shared< ShaderProgram >( *getEngine()->getRenderSystem() );
		result->setSource( ashes::ShaderStageFlag::eVertex
			, renderPass.getVertexShaderSource( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, invertNormals ) );
		result->setSource( ashes::ShaderStageFlag::eFragment
			, renderPass.getPixelShaderSource( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc ) );
		auto geometry = renderPass.getGeometryShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );

		if ( geometry )
		{
			addFlag( matrixUboShaderMask, ashes::ShaderStageFlag::eGeometry );
			result->setSource( ashes::ShaderStageFlag::eGeometry
				, std::move( geometry ) );
		}

		return result;
	}

	ShaderProgramSPtr ShaderProgramCache::doAddAutomaticProgram( ShaderProgramSPtr program
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc
		, bool invertNormals )
	{
		uint64_t key = makeKey( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc
			, invertNormals );
		auto lock = makeUniqueLock( m_mutex );
		auto const & it = m_mapAutogenerated.find( key );

		if ( it == m_mapAutogenerated.end() )
		{
			m_mapAutogenerated.insert( { key, program } );
			return doAddProgram( std::move( program ), true );
		}

		return it->second;
	}

	ShaderProgramSPtr ShaderProgramCache::doCreateBillboardProgram( RenderPass const & renderPass
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		ShaderProgramSPtr result = std::make_shared< ShaderProgram >( *getEngine()->getRenderSystem() );
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec4 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );
			auto center = writer.declInput< Vec3 >( cuT( "center" ), 2u );
			auto in = writer.getIn();
			UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
			UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
			UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
			UBO_BILLBOARD( writer, BillboardUbo::BindingPoint, 0 );

			// Shader outputs
			auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" )
				, RenderPass::VertexOutputs::WorldPositionLocation );
			auto vtx_curPosition = writer.declOutput< Vec3 >( cuT( "vtx_curPosition" )
				, RenderPass::VertexOutputs::CurPositionLocation );
			auto vtx_prvPosition = writer.declOutput< Vec3 >( cuT( "vtx_prvPosition" )
				, RenderPass::VertexOutputs::PrvPositionLocation );
			auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" )
				, RenderPass::VertexOutputs::NormalLocation );
			auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" )
				, RenderPass::VertexOutputs::TangentLocation );
			auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" )
				, RenderPass::VertexOutputs::BitangentLocation );
			auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
				, RenderPass::VertexOutputs::TextureLocation );
			auto vtx_instance = writer.declOutput< UInt >( cuT( "vtx_instance" )
				, RenderPass::VertexOutputs::InstanceLocation );
			auto vtx_material = writer.declOutput< UInt >( cuT( "vtx_material" )
				, RenderPass::VertexOutputs::MaterialLocation );
			auto out = writer.getOut();

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto curBbcenter = writer.declLocale( cuT( "curBbcenter" )
						, writer.paren( c3d_curMtxModel * vec4( center, 1.0_f ) ).xyz() );
					auto prvBbcenter = writer.declLocale( cuT( "prvBbcenter" )
						, writer.paren( c3d_prvMtxModel * vec4( center, 1.0_f ) ).xyz() );
					auto curToCamera = writer.declLocale( cuT( "curToCamera" )
						, c3d_cameraPosition.xyz() - curBbcenter );
					curToCamera.y() = 0.0_f;
					curToCamera = normalize( curToCamera );
					auto right = writer.declLocale( cuT( "right" )
						, vec3( c3d_curView[0][0], c3d_curView[1][0], c3d_curView[2][0] ) );
					auto up = writer.declLocale( cuT( "up" )
						, vec3( c3d_curView[0][1], c3d_curView[1][1], c3d_curView[2][1] ) );

					if ( !checkFlag( programFlags, ProgramFlag::eSpherical ) )
					{
						right = normalize( vec3( right.x(), 0.0, right.z() ) );
						up = vec3( 0.0_f, 1.0f, 0.0f );
					}

					vtx_material = writer.cast< UInt >( c3d_materialIndex );
					vtx_normal = curToCamera;
					vtx_tangent = up;
					vtx_bitangent = right;

					auto width = writer.declLocale( cuT( "width" ), c3d_dimensions.x() );
					auto height = writer.declLocale( cuT( "height" ), c3d_dimensions.y() );

					if ( checkFlag( programFlags, ProgramFlag::eFixedSize ) )
					{
						width = c3d_dimensions.x() / c3d_clipInfo.x();
						height = c3d_dimensions.y() / c3d_clipInfo.y();
					}

					vtx_worldPosition = curBbcenter
						+ right * position.x() * width
						+ up * position.y() * height;
					auto prvPosition = writer.declLocale( cuT( "prvPosition" )
						, vec4( prvBbcenter + right * position.x() * width + up * position.y() * height, 1.0 ) );

					vtx_texture = vec3( uv, 0.0 );
					vtx_instance = writer.cast< UInt >( in.gl_InstanceID );
					auto curPosition = writer.declLocale( cuT( "curPosition" )
						, c3d_curView * vec4( vtx_worldPosition, 1.0 ) );
					prvPosition = c3d_prvView * vec4( prvPosition );
					curPosition = c3d_projection * curPosition;
					prvPosition = c3d_projection * prvPosition;

					// Convert the jitter from non-homogeneous coordiantes to homogeneous
					// coordinates and add it:
					// (note that for providing the jitter in non-homogeneous projection space,
					//  pixel coordinates (screen space) need to multiplied by two in the C++
					//  code)
					curPosition.xy() -= c3d_jitter * curPosition.w();
					prvPosition.xy() -= c3d_jitter * prvPosition.w();
					out.gl_out.gl_Position = curPosition;

					vtx_curPosition = curPosition.xyw();
					vtx_prvPosition = prvPosition.xyw();
					// Positions in projection space are in [-1, 1] range, while texture
					// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
					// the scale (and flip the y axis):
					vtx_curPosition.xy() *= vec2( 0.5_f, -0.5_f );
					vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5_f );
				} );

			auto & vtxShader = writer.getShader();
			result->setSource( ashes::ShaderStageFlag::eVertex, std::make_unique< sdw::Shader >( std::move( vtxShader ) ) );
		}

		auto pxlShader = renderPass.getPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
		result->setSource( ashes::ShaderStageFlag::eFragment, std::move( pxlShader ) );
		return result;
	}

	ShaderProgramSPtr ShaderProgramCache::doAddBillboardProgram( ShaderProgramSPtr program
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )
	{
		uint64_t key = makeKey( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc
			, false );
		auto lock = makeUniqueLock( m_mutex );
		auto const & it = m_mapBillboards.find( key );

		if ( it == m_mapBillboards.end() )
		{
			m_mapBillboards.insert( { key, program } );
			return doAddProgram( std::move( program ), true );
		}

		return it->second;
	}
}
