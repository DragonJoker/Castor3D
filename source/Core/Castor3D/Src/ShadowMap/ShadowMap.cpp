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

using namespace Castor;

namespace Castor3D
{
	ShadowMap::ShadowMap( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::Initialise()
	{
		bool result = true;

		if ( !m_frameBuffer )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			result = m_frameBuffer->Create();
			auto size = DoGetSize();

			if ( result )
			{
				result = m_frameBuffer->Initialise( size );
			}

			if ( result )
			{
				DoInitialise();
			}

			m_frameBuffer->Bind();
			m_frameBuffer->SetDrawBuffers( FrameBuffer::AttachArray{} );
			m_frameBuffer->Unbind();
		}

		return result;
	}

	void ShadowMap::Cleanup()
	{
		for ( auto & it : m_passes )
		{
			it.second->Cleanup();
		}

		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind();
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();

			DoCleanup();

			m_frameBuffer->Cleanup();
			m_frameBuffer->Destroy();
			m_frameBuffer.reset();
		}

		for ( auto buffer : m_geometryBuffers )
		{
			buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void ShadowMap::AddLight( Light & light )
	{
		auto pass = DoCreatePass( light );
		auto size = DoGetSize();
		GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
			, [pass, size]()
			{
				pass->Initialise( size );
			} ) );
		m_passes.emplace( &light, pass );
	}

	void ShadowMap::UpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		RemFlag( programFlags, ProgramFlag::eLighting );
		RemFlag( programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( textureFlags, TextureChannel( uint16_t( TextureChannel::eAll )
			& ~uint16_t( TextureChannel::eOpacity ) ) );
		DoUpdateFlags( textureFlags
			, programFlags
			, sceneFlags );
	}

	GLSL::Shader ShadowMap::GetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return DoGetVertexShaderSource( textureFlags, programFlags, sceneFlags, invertNormals );
	}

	GLSL::Shader ShadowMap::GetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return DoGetGeometryShaderSource( textureFlags
			, programFlags
			, sceneFlags );
	}

	GLSL::Shader ShadowMap::GetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return DoGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader ShadowMap::DoGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace GLSL;
		auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Vertex inputs
		auto position = writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto texture = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.DeclBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		SkinningUbo::Declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

		// Outputs
		auto vtx_position = writer.DeclOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_texture = writer.DeclOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.DeclOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.DeclLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto v3Texture = writer.DeclLocale( cuT( "v3Texture" ), texture );
			auto mtxModel = writer.DeclLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( CheckFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::ComputeTransform( writer, programFlags );
			}
			else if ( CheckFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.DeclLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				v3Texture = v3Texture * writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * c3d_mtxView * v4Vertex;
		};

		writer.ImplementFunction< void >( cuT( "main" ), main );
		return writer.Finalise();
	}

	GLSL::Shader ShadowMap::DoGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return GLSL::Shader{};
	}

	void ShadowMap::DoApplyAlphaFunc( GLSL::GlslWriter & writer
		, ComparisonFunc alphaFunc
		, GLSL::Float const & alpha
		, GLSL::Int const & material
		, GLSL::Materials const & materials )
	{
		using namespace GLSL;

		switch ( alphaFunc )
		{
		case ComparisonFunc::eLess:
			IF( writer, alpha >= materials.GetAlphaRef( material ) )
			{
				writer.Discard();
			}
			FI;
			break;

		case ComparisonFunc::eLEqual:
			IF( writer, alpha > materials.GetAlphaRef( material ) )
			{
				writer.Discard();
			}
			FI;
			break;

		case ComparisonFunc::eEqual:
			IF( writer, alpha != materials.GetAlphaRef( material ) )
			{
				writer.Discard();
			}
			FI;
			break;

		case ComparisonFunc::eNEqual:
			IF( writer, alpha == materials.GetAlphaRef( material ) )
			{
				writer.Discard();
			}
			FI;
			break;

		case ComparisonFunc::eGEqual:
			IF( writer, alpha < materials.GetAlphaRef( material ) )
			{
				writer.Discard();
			}
			FI;
			break;

		case ComparisonFunc::eGreater:
			IF( writer, alpha <= materials.GetAlphaRef( material ) )
			{
				writer.Discard();
			}
			FI;
			break;

		default:
			IF( writer, alpha <= 0.2 )
			{
				writer.Discard();
			}
			FI;
			break;
		}
	}}
