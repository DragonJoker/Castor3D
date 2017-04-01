#include "ShadowMap.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPass.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMap::ShadowMap( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::Initialise()
	{
		bool l_return = true;

		if ( !m_frameBuffer )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			l_return = m_frameBuffer->Create();
			auto l_size = DoGetSize();

			if ( l_return )
			{
				l_return = m_frameBuffer->Initialise( l_size );
			}

			if ( l_return )
			{
				DoInitialise();
			}

			m_frameBuffer->Bind();
			m_frameBuffer->SetDrawBuffers( FrameBuffer::AttachArray{} );
			m_frameBuffer->Unbind();
		}

		return l_return;
	}

	void ShadowMap::Cleanup()
	{
		for ( auto & l_it : m_passes )
		{
			l_it.second->Cleanup();
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

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void ShadowMap::AddLight( Light & p_light )
	{
		auto l_pass = DoCreatePass( p_light );
		auto l_size = DoGetSize();
		GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
			, [l_pass, l_size]()
			{
				l_pass->Initialise( l_size );
			} ) );
		m_passes.emplace( &p_light, l_pass );
	}

	void ShadowMap::UpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( p_textureFlags, TextureChannel( uint16_t( TextureChannel::eAll )
			& ~uint16_t( TextureChannel::eOpacity ) ) );
		DoUpdateFlags( p_textureFlags
			, p_programFlags
			, p_sceneFlags );
	}

	String ShadowMap::GetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		return DoGetVertexShaderSource( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
	}

	String ShadowMap::GetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return DoGetGeometryShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	String ShadowMap::GetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return DoGetPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	String ShadowMap::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Vertex inputs
		auto position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
		auto texture = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.GetAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( l_writer.GetBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		UBO_SKINNING( l_writer, p_programFlags );
		UBO_MORPHING( l_writer, p_programFlags );

		// Outputs
		auto vtx_position = l_writer.GetOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.GetLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_v3Texture = l_writer.GetLocale( cuT( "l_v3Texture" ), texture );
			auto l_mtxModel = l_writer.GetLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				auto l_mtxBoneTransform = l_writer.GetLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
				l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
				l_mtxModel = c3d_mtxModel * l_mtxBoneTransform;
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_mtxModel = transform;
			}
			else
			{
				l_mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto l_time = l_writer.GetLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
				l_v3Texture = l_v3Texture * l_writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = l_v3Texture;
			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_position = l_v4Vertex.xyz();
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * c3d_mtxView * l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String ShadowMap::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return String{};
	}
}
