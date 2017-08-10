#include "ShadowMap.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPass.hpp"

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>

using namespace castor;

namespace castor3d
{
	ShadowMap::ShadowMap( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::initialise()
	{
		bool result = true;

		if ( !m_frameBuffer )
		{
			m_frameBuffer = getEngine()->getRenderSystem()->createFrameBuffer();
			result = m_frameBuffer->create();
			auto size = doGetSize();

			if ( result )
			{
				result = m_frameBuffer->initialise( size );
			}

			if ( result )
			{
				doInitialise();
			}

			m_frameBuffer->bind();
			m_frameBuffer->setDrawBuffers( FrameBuffer::AttachArray{} );
			m_frameBuffer->unbind();
		}

		return result;
	}

	void ShadowMap::cleanup()
	{
		for ( auto & it : m_passes )
		{
			it.second->cleanup();
		}

		if ( m_frameBuffer )
		{
			m_frameBuffer->bind();
			m_frameBuffer->detachAll();
			m_frameBuffer->unbind();

			doCleanup();

			m_frameBuffer->cleanup();
			m_frameBuffer->destroy();
			m_frameBuffer.reset();
		}

		for ( auto buffer : m_geometryBuffers )
		{
			buffer->cleanup();
		}

		m_geometryBuffers.clear();
	}

	void ShadowMap::addLight( Light & light )
	{
		auto pass = doCreatePass( light );
		auto size = doGetSize();
		getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [pass, size]()
			{
				pass->initialise( size );
			} ) );
		m_passes.emplace( &light, pass );
	}

	void ShadowMap::updateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( programFlags, ProgramFlag::eAlphaBlending );
		remFlag( textureFlags, TextureChannel( uint16_t( TextureChannel::eAll )
			& ~uint16_t( TextureChannel::eOpacity ) ) );
		doUpdateFlags( textureFlags
			, programFlags
			, sceneFlags );
	}

	GLSL::Shader ShadowMap::getVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return doGetVertexShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	GLSL::Shader ShadowMap::getGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return doGetGeometryShaderSource( textureFlags
			, programFlags
			, sceneFlags );
	}

	GLSL::Shader ShadowMap::getPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return doGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader ShadowMap::doGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace GLSL;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( ShaderProgram::Position );
		auto texture = writer.declAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( ShaderProgram::Weights0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( ShaderProgram::Weights1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( ShaderProgram::Transform, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( ShaderProgram::Position2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( ShaderProgram::Texture2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

		// Outputs
		auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" ), texture );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( writer, programFlags );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				v3Texture = v3Texture * writer.paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * c3d_mtxView * v4Vertex;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	GLSL::Shader ShadowMap::doGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return GLSL::Shader{};
	}

	void ShadowMap::doApplyAlphaFunc( GLSL::GlslWriter & writer
		, ComparisonFunc alphaFunc
		, GLSL::Float const & alpha
		, GLSL::Int const & material
		, GLSL::Materials const & materials )
	{
		using namespace GLSL;

		switch ( alphaFunc )
		{
		case ComparisonFunc::eLess:
			IF( writer, alpha >= materials.getAlphaRef( material ) )
			{
				writer.discard();
			}
			FI;
			break;

		case ComparisonFunc::eLEqual:
			IF( writer, alpha > materials.getAlphaRef( material ) )
			{
				writer.discard();
			}
			FI;
			break;

		case ComparisonFunc::eEqual:
			IF( writer, alpha != materials.getAlphaRef( material ) )
			{
				writer.discard();
			}
			FI;
			break;

		case ComparisonFunc::eNEqual:
			IF( writer, alpha == materials.getAlphaRef( material ) )
			{
				writer.discard();
			}
			FI;
			break;

		case ComparisonFunc::eGEqual:
			IF( writer, alpha < materials.getAlphaRef( material ) )
			{
				writer.discard();
			}
			FI;
			break;

		case ComparisonFunc::eGreater:
			IF( writer, alpha <= materials.getAlphaRef( material ) )
			{
				writer.discard();
			}
			FI;
			break;

		default:
			IF( writer, alpha <= 0.2 )
			{
				writer.discard();
			}
			FI;
			break;
		}
	}}
